/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TrigMinBias/TrigVertexCounterHypo.h>
#include <xAODTrigMinBias/TrigVertexCounts.h>

//---------------------------------------------------------------------------------

TrigVertexCounterHypo::TrigVertexCounterHypo(const std::string& name, ISvcLocator* pSvcLocator): 
  HLT::HypoAlgo(name, pSvcLocator),
  m_log(msgSvc(), name),
  m_rejectPileup(false),
  m_required_ntrks(0),
  m_max_ntrks(0),
  m_cutNumVtx(false),
  m_acceptAll(false),
  m_vtxNtrks(),
  m_vtxTrkPtSqSum(),
  m_trigVertexCounts(0){

  declareProperty("RejectPileup", m_rejectPileup = false); 
  declareProperty("Required_ntrks", m_required_ntrks = 3); 
  declareProperty("Max_ntrks", m_max_ntrks = 0); 
  declareProperty("AcceptAll_EF", m_acceptAll = false );
  declareProperty("CutNumVtx", m_cutNumVtx = false );
  declareProperty("Min_Nvtx", m_min_Nvtx = 0); 

  // Monitoring
  declareMonitoredStdContainer("vtxNtrks",m_vtxNtrks); 
  declareMonitoredStdContainer("vtxTrkPtSqSum",m_vtxTrkPtSqSum); 
  declareMonitoredVariable("vtxNvtx", m_vtxNvtx);
  declareMonitoredStdContainer("vtxNtrksAccepted", m_vtxNtrksAccepted); 
  declareMonitoredStdContainer("vtxTrkPtSqSumAccepted", m_vtxTrkPtSqSumAccepted); 
  declareMonitoredVariable("vtxNvtxAccepted",m_vtxNvtxAccepted);
}

//---------------------------------------------------------------------------------

TrigVertexCounterHypo::~TrigVertexCounterHypo(){}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounterHypo::hltInitialize() {
  m_log.setLevel( outputLevel() );

  if (msgLvl() <= MSG::INFO) {
    m_log << MSG::INFO << "Initialising TrigVertexCounterHypo, name = " << name() << endreq;
    m_log << MSG::INFO << "RejectPileup = " << (m_rejectPileup ? "True" : "False") << endreq; 
    m_log << MSG::INFO << "Required_ntrks = " << m_required_ntrks << " #" << endreq;
    m_log << MSG::INFO << "Max_ntrks = " << m_max_ntrks << " #" << endreq;
    m_log << MSG::INFO << "AcceptAll = " << (m_acceptAll ? "True" : "False") << endreq;
    m_log << MSG::INFO << "CutNumVtx = " << (m_cutNumVtx ? "True" : "False") << endreq;
  }

  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounterHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass=false;

  // Clear the monitoring variables
  m_vtxNtrks.clear();
  m_vtxTrkPtSqSum.clear();
  m_vtxNvtx = -1;
  m_vtxNtrksAccepted.clear();
  m_vtxTrkPtSqSumAccepted.clear();
  m_vtxNvtxAccepted = -1;

  if( msgLvl() <= MSG::DEBUG ) {
    m_log << MSG::DEBUG << "Executing this TrigVertexCounterHypo " << name() << endreq;
  }
  
  if( m_acceptAll ){
    pass = true;
    return HLT::OK;
  }
  
  // Retrieve the TrigVertexCounts object stored by the TrigVertexCounter class.
  m_trigVertexCounts = 0;
  HLT::ErrorCode hltStatus = getFeature(outputTE, m_trigVertexCounts, "vertexcounts");
  if(hltStatus != HLT::OK || !m_trigVertexCounts){
    if( msgLvl() <= MSG::WARNING ) m_log << MSG::WARNING << "Failed to retrieve features from TE." << endreq;
    return HLT::OK;
  }
 
  // Get the number of tracks and the pT^2 sum per vertex
  m_vtxNtrks = m_trigVertexCounts->vtxNtrks();
  m_vtxTrkPtSqSum = m_trigVertexCounts->vtxTrkPtSqSum();

  unsigned int nvtx = m_vtxNtrks.size();
  if(nvtx != m_vtxTrkPtSqSum.size()) {
    m_log << MSG::ERROR << "The number of vertices does not agree within the TrigVertexCounts object." << endreq;
    return HLT::OK;
  }
  m_vtxNvtx = nvtx;

  if(m_cutNumVtx){ // select event using the primary vertex multiplicity
    unsigned int num_vtx = 0;
     for(unsigned int ivtx=0; ivtx<nvtx; ivtx++) {
       if(m_vtxNtrks[ivtx]>m_required_ntrks)
	 ++ num_vtx;
     }
     if(num_vtx> m_min_Nvtx){
       pass = true;
       if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event accepted.  Num vertices is :" << num_vtx << "  with at least " << m_required_ntrks << "  tracks" << endreq;
       // Fill the monitoring variables
       m_vtxNtrksAccepted = m_vtxNtrks;
       m_vtxTrkPtSqSumAccepted = m_vtxTrkPtSqSum;
       m_vtxNvtxAccepted = num_vtx; 
       return HLT::OK;
     } else {
       pass = false;
       if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event rejected.  Not enough vertices." << endreq;
       return HLT::OK;
     }
     
  } else { // default for High Multi Track analysis


  unsigned int ivtx, priVtx = 0;
  bool firstVtx = true;
  float maxPtSqSum = 0;

  // Find the primary vertex.  The primary vertex is the vertex with the highest sum pT^2
  for(ivtx=0; ivtx<nvtx; ivtx++) {
    if(firstVtx || m_vtxTrkPtSqSum[ivtx] > maxPtSqSum) {
      maxPtSqSum = m_vtxTrkPtSqSum[ivtx];
      priVtx = ivtx;
      firstVtx = false;
    }
  }

  if(firstVtx) {
    pass = false;
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event rejected.  No primary vertex." << endreq;
    return HLT::OK;
  }

  if(m_rejectPileup) {
    for(ivtx=0; ivtx<nvtx; ivtx++) {
      if(ivtx == priVtx) continue; // Skip the primary vertex
      if(m_vtxNtrks[ivtx] > m_max_ntrks) { // Check the pile up requirement
	pass = false;
	if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event rejected.  Failed pile up requirement." << endreq;
	return HLT::OK;
      }
    }

    // Check if the primary vertex contains enough tracks.
    if(m_vtxNtrks[priVtx] < m_required_ntrks) {
      pass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event rejected.  Primary vertex only has " << m_vtxNtrks[priVtx] << " tracks." << endreq;
      return HLT::OK;
    }

    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG
                                     << "Event accepted.  Primary vertex has " << m_vtxNtrks[priVtx] << " tracks." << endreq; 

    // Fill the monitoring variables
    m_vtxNtrksAccepted.push_back(m_vtxNtrks[priVtx]);
    m_vtxTrkPtSqSumAccepted.push_back(m_vtxTrkPtSqSum[priVtx]); 
    m_vtxNvtxAccepted = 1;

    pass = true;
  }
  else { 
    // Require at least one vertex contains enough tracks.
    for(ivtx=0; ivtx<nvtx; ivtx++) {
      if(m_vtxNtrks[ivtx] >= m_required_ntrks) {
	pass = true;
	
        if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG
					 << "Event accepted, has a vertex with "
					 <<  m_vtxNtrks[ivtx] << " tracks." << endreq;

        // Fill the monitoring variables
        m_vtxNtrksAccepted = m_vtxNtrks;
        m_vtxTrkPtSqSumAccepted = m_vtxTrkPtSqSum;
        m_vtxNvtxAccepted = nvtx; 

	return HLT::OK;
      }
    }

    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event rejected.  No vertex contains enough tracks." << endreq;
    pass = false;
  }

  return HLT::OK;
  }
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounterHypo::hltFinalize() {

  if( msgLvl() <= MSG::INFO )
    m_log << MSG::INFO << " Finalizing this TrigVertexCounterHypo" << name() <<"." << endreq; 
  return HLT::OK;  
}
