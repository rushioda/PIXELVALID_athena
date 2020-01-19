/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigVxSecondary/TrigVxSecondaryCombo.h"
#include "GaudiKernel/MsgStream.h"
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxContainer.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include <sstream>
#include <TLorentzVector.h>
//#include "xAODTracking/Vertex.h"
//#include "xAODTracking/VertexContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthContainers/ConstDataVector.h"

/** @brief Comparison operator for sorting objects of type VxContainer.
 * Enables vertices to be sorted in order of increasing chi2/NDoF
 */
namespace VxSort { 
  class byChi2OverNDoF {
  public:
    inline byChi2OverNDoF() {};
    inline bool operator () (const xAOD::Vertex* Vx1, const xAOD::Vertex* Vx2) const {
      double chi2_1 = Vx1->chiSquared();
      double NDoF_1 = Vx1->numberDoF();
      double chi2_2 = Vx2->chiSquared();
      double NDoF_2 = Vx2->numberDoF();
      return fabs(chi2_1 / NDoF_1) < fabs(chi2_2 / NDoF_2);
    }
  };
} // end of namespace VxSort

namespace InDet {

  TrigVxSecondaryCombo::TrigVxSecondaryCombo(const std::string &name, ISvcLocator *pSvcLocator):
    HLT::ComboAlgo(name, pSvcLocator),
    m_trackJetFinderTool("TrigTrackJetFinderTool",this),
    m_secVertexFinderToolsHandleArray(),
    m_nVxCandidates(0),
    m_nVxContainers(0),
    m_nVxSecVertexInfo(0),
    m_nVxSecVertexInfoContainers(0) {
    
    declareProperty("PriVtxKey",            m_priVtxKey            = "xPrimVx"); //"EFHistoPrmVtx"
    declareProperty("SecVtxKey",            m_secVtxKey            = "SecVtxInfo");
    declareProperty("SecVtxFinderList",     m_secVertexFinderToolsHandleArray); //!< Default = InDetVKalVxInJetTool only
    declareProperty("sortSecVxContainer",   m_sortSecVxContainer);
    declareProperty("T2PrmVtxAtEF",         m_T2PrmVtxAtEF         = false);
    declareProperty("UseBeamSpotFlag",      m_useBeamSpotFlag      = false);
    declareProperty("T2PrmVtxAtEFAlgoId",   m_algo_T2TrkForVtx     = 11);
    declareProperty("UseJetDirection",      m_useJetDirection);

    declareMonitoredVariable("SecVtx_NumTrkTot",      m_secVtx_numTrkTot, AutoClear);
    declareMonitoredVariable("SecVtx_TwoTrkTot",      m_secVtx_twoTrkTot, AutoClear);
    declareMonitoredVariable("SecVtx_NumTrkSV",       m_secVtx_numTrkSV,  AutoClear);
    declareMonitoredVariable("SecVtx_NumPV",          m_secVtx_numPV,     AutoClear);
    declareMonitoredVariable("SecVtx_NumSV",          m_secVtx_numSV,     AutoClear);
    declareMonitoredVariable("SecVtx_Mass",           m_secVtx_mass,      AutoClear);
    declareMonitoredVariable("SecVtx_Energy",         m_secVtx_energy,    AutoClear);
    
    m_jetDirection = new TLorentzVector;
  }
  
  TrigVxSecondaryCombo::~TrigVxSecondaryCombo() {
    delete m_jetDirection;
  }	
	
  //** ----------------------------------------------------------------------------------------------------------------- **//
  
  HLT::ErrorCode TrigVxSecondaryCombo::hltInitialize() {
    
    msg() << MSG::INFO << "TrigVxSecondaryCombo::initialize()" << endreq;

    //* declareProperty overview *//
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "declareProperty review:" << endreq;
      msg() << MSG::DEBUG << " PriVtxKey = "            << m_priVtxKey                       << endreq;
      msg() << MSG::DEBUG << " SecVtxKey = "            << m_secVtxKey                       << endreq;
      msg() << MSG::DEBUG << " SecVtxFinderList = "     << m_secVertexFinderToolsHandleArray << endreq;
      msg() << MSG::DEBUG << " sortSecVxContainer = "   << m_sortSecVxContainer              << endreq;
      msg() << MSG::DEBUG << " T2PrmVtxAtEF = "         << m_T2PrmVtxAtEF                    << endreq;
      msg() << MSG::DEBUG << " UseBeamSpotFlag = "      << m_useBeamSpotFlag                 << endreq;
      msg() << MSG::DEBUG << " T2PrmVtxAtEFAlgoId = "   << m_algo_T2TrkForVtx                << endreq;
      msg() << MSG::DEBUG << " JetDirection = "         << m_useJetDirection                 << endreq;
    }

    msg() << MSG::DEBUG << "Sorting of secondary vertices (in order of increasing chi2/NDoF) ";
    if(m_sortSecVxContainer) msg() << MSG::DEBUG << "ON" << endreq;
    else msg() << MSG::DEBUG << "OFF" << endreq;

    msg() << MSG::DEBUG << "Using beamspot as primary vertex ";
    if(m_T2PrmVtxAtEF) msg() << MSG::DEBUG << "YES" << endreq;
    else msg() << MSG::DEBUG << "NO" << endreq;

    // Get all possible secondary vertex finders
    if(m_secVertexFinderToolsHandleArray.retrieve().isFailure()) {
      msg() << MSG::ERROR
	    << "Failed to retrieve tool array "
	    << m_secVertexFinderToolsHandleArray
	    << endreq;
      
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      msg() << MSG::DEBUG << "Retrieved tool array " << m_secVertexFinderToolsHandleArray << endreq;
    }

    // List the secondary vertex finder tools to be executed
    msg() << MSG::DEBUG << "Using secondary vertex finder tools: " << endreq;
    for(unsigned int i = 0; i < m_secVertexFinderToolsHandleArray.size(); ++i) {
      msg() << MSG::DEBUG << "Tool: " << m_secVertexFinderToolsHandleArray[i] << endreq;
    }

    //* Retrieve TrigTrackJetFinder tool *//
    StatusCode sc = m_trackJetFinderTool.retrieve();
    if(sc.isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } else
      msg() << MSG::DEBUG << "Retrieved tool " << m_trackJetFinderTool << endreq;

    msg() << MSG::DEBUG << "Initialization successful" << endreq;

    return HLT::OK;
  }

  // Execute HLT algorithm
  HLT::ErrorCode TrigVxSecondaryCombo::hltExecute(HLT::TEConstVec& inputTEs, HLT::TriggerElement* outputTE) {

    int outputLevel = msgLvl();

    if(outputLevel <= MSG::DEBUG) {
      msg() << MSG::DEBUG << ">>>>>>>> In execHLTAlgorithm() <<<<<<<<" << endreq;
      msg() << MSG::DEBUG << "Require 2 TEs as input: [0] = jet tracks, [1] = primary vertex" << endreq;
    }

    const HLT::TriggerElement* jetTrackTE = inputTEs[0];
    const HLT::TriggerElement* prmVtxTE   = inputTEs[1];

    if (inputTEs.size() != 2 ) {
      msg() << MSG::ERROR << "Need 2 TEs as input: [0] = jet tracks, [1] = primary vertex" << endreq;
      return HLT::NAV_ERROR;
    }

    const xAOD::TrackParticleContainer* trackContainer;
    if(HLT::OK != getFeature(jetTrackTE, trackContainer)) {
      msg() << MSG::ERROR << "Input track collection could not be retrieved from input TE" << endreq;
      return HLT::NAV_ERROR;
    }

    if(!trackContainer) {
      if(outputLevel <= MSG::DEBUG)
    	msg() << MSG::DEBUG << "Input track collection is null pointer" << endreq;
      return HLT::MISSING_FEATURE; 
    }
    
//    if(trackContainer->size() == 0) {
//      if(outputLevel <= MSG::DEBUG)
//    	msg() << MSG::DEBUG << "Input track collection is empty" << endreq;
//      return HLT::MISSING_FEATURE; 
//    }

    m_secVtx_numTrkTot = trackContainer->size();
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Retrieved input track collection with " << m_secVtx_numTrkTot << " tracks." << endreq;

    const xAOD::VertexContainer* primaryVxContainer;

    if(HLT::OK != getFeature(prmVtxTE, primaryVxContainer, m_priVtxKey)) {
      msg() << MSG::ERROR << "Input primary vertex container could not be retreived from input TE" << endreq;
      return HLT::NAV_ERROR;
    }

    // TEMP: CARLO  xAOD::VertexContainer* xaod = new xAOD::VertexContainer(); 
    // TEMP: CARLO  xAOD::VertexAuxContainer aux;
    // TEMP: CARLO  xaod->setStore( &aux );

    // TEMP: CARLO  xAOD::Vertex* bestFitPriVertex = new xAOD::Vertex();
    xAOD::Vertex* bestFitPriVertex(0);

    // TEMP: CARLO  xaod->push_back(bestFitPriVertex); 

    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Choosing primary vertex candidate (if more than one)" << endreq;

    if(m_T2PrmVtxAtEF) {

       int prmVtxStatusBitMap=0;

       HLT::ErrorCode sc = getPrmVtxForFit(bestFitPriVertex, primaryVxContainer, m_secVtx_numPV, m_xPrmVtx, m_yPrmVtx, m_zPrmVtx, prmVtxStatusBitMap);

       if(sc != HLT::OK) {

	 if(outputLevel <= MSG::DEBUG)
	   msg() << MSG::DEBUG << "Failed to get beamspot primary vertex proxy" << endreq;

	 if(bestFitPriVertex) delete bestFitPriVertex;
          return sc;     
       
       } 
       else {

	 if(outputLevel <= MSG::DEBUG)
	   msg() << MSG::DEBUG << "Successfully found beamspot primary vertex proxy with status bitmap: " << prmVtxStatusBitMap << endreq;
       }
       
       if(m_secVtx_numPV == 0 || !bestFitPriVertex) {

	 if(outputLevel <= MSG::DEBUG) 
	   msg() << MSG::DEBUG << "Primary vertex from T2 failed." << endreq;

	 return HLT::MISSING_FEATURE;
       }
       
       //Only continue if the status flag is ok
       //here one could image to continue if we want to allow a sec vtx w.r.t. a "weird primary vertex"

       if(prmVtxStatusBitMap) {

          msg() << MSG::DEBUG << "Beamspot and T2 z-vertex had issues, the status returned was: " << prmVtxStatusBitMap 
		<< ". Do not try to reconstruct SV" << endreq;

	  if(bestFitPriVertex) delete bestFitPriVertex;
          return HLT::MISSING_FEATURE;
       }
   
    } 
    else {
           
      HLT::ErrorCode sc = getEFPrmVtxForFit(bestFitPriVertex, prmVtxTE ,m_secVtx_numPV, m_xPrmVtx, m_yPrmVtx, m_zPrmVtx);
       
       if(sc!=HLT::OK) {

	 if(outputLevel <= MSG::DEBUG)
	   msg() << MSG::DEBUG << "Failed to get EF primary vertex found" << endreq;

	 if(bestFitPriVertex) delete bestFitPriVertex;
          return sc;

       } 
       else {

	 if(outputLevel <= MSG::DEBUG)
	   msg() << MSG::DEBUG << "Successfully found EF primary vertex" << endreq;
       }

       if(m_secVtx_numPV == 0 || !bestFitPriVertex) {

	 if(outputLevel <= MSG::DEBUG) 
	   msg() << MSG::DEBUG << "No primary vertex found" << endreq;

	 return HLT::MISSING_FEATURE;
       }
    }

    // Should have found a PV by this point.  Now start looking for a SV.

    if (trackContainer->size() < 2) {

       if (msgLvl() <= MSG::DEBUG) 
	 msg() << MSG::DEBUG << "Less than 2 tracks; won't try to reconstruct sv" << endreq;

       // TEMP: CARLO - WAS COMMENTED OUT BEFORE
       if(bestFitPriVertex) delete bestFitPriVertex;
       return HLT::OK;
    }

    m_trackJetFinderTool->clear();    
    m_trackJetFinderTool->inputPrimaryVertexZ(m_zPrmVtx);

    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Loop over tracks and find jet direction" << endreq;
    
    for(unsigned int i = 0; i < m_secVtx_numTrkTot; ++i) {
      
      const xAOD::TrackParticle* track = (*trackContainer)[i];

      // NEED TRACKJETFINDERTOOL TO EVOLVE AND ACCEPT XAOD::TRACKPARTICLES AS INPUT
      // m_trackJetFinderTool->addTrack(track, i);

      if(outputLevel <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "ID: " << std::fixed << std::setw(2) << i
	      << ", eta: "  << std::setw(6) << std::setprecision(3) << track->eta()
	      << ", phi: "  << std::setw(6) << std::setprecision(3) << track->phi()
	      << ", pT: "   << std::setw(6) << std::setprecision(0) << track->pt()
	      << std::setprecision(5) << endreq;
      }
    }
        
    if(m_useJetDirection == 2) {
       
       m_trackJetFinderTool->clear();    
       m_trackJetFinderTool->inputPrimaryVertexZ(m_zPrmVtx);
       
       if(outputLevel <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Loop over tracks and find jet direction (method 2)" << endreq;
       
       for(unsigned int i = 0; i < m_secVtx_numTrkTot; ++i) {
        
	 const xAOD::TrackParticle* track = (*trackContainer)[i];

	 // NEED TRACKJETFINDERTOOL TO EVOLVE AND ACCEPT XAOD::TRACKPARTICLES AS INPUT
	 //m_trackJetFinderTool->addTrack(track, i);
          
	 if(outputLevel <= MSG::DEBUG) {
	   msg() << MSG::DEBUG << "ID: " << std::fixed << std::setw(2) << i
		 << ", eta: "  << std::setw(6) << std::setprecision(3) << track->eta()
		 << ", phi: "  << std::setw(6) << std::setprecision(3) << track->phi()
		 << ", pT: "   << std::setw(6) << std::setprecision(0) << track->pt()
		 << std::setprecision(5) << endreq;
	 }
       }
       
       std::vector<int> tracksTrackJet;
       float etaTrackJet;
       float phiTrackJet;
       
       // Find jet direction
       m_trackJetFinderTool->findJet(tracksTrackJet, etaTrackJet, phiTrackJet);
       
       if(etaTrackJet == -99 || phiTrackJet == -99) {

          if(outputLevel <= MSG::DEBUG)
             msg() << MSG::DEBUG << "Unable to find jet direction" << endreq;

	  if(bestFitPriVertex) delete bestFitPriVertex;
          return HLT::MISSING_FEATURE;
       }
       
       if(outputLevel <= MSG::DEBUG) 
          msg() << MSG::DEBUG << "Calculated jet direction: eta = " << etaTrackJet << ", phi = " << phiTrackJet << endreq;
       
       if(outputLevel <= MSG::DEBUG) 
          msg() << MSG::DEBUG << "Number of tracks used to determine track direction: " << tracksTrackJet.size() << endreq;
       
       if(outputLevel <= MSG::DEBUG) {
          for(unsigned int i = 0; i < tracksTrackJet.size(); ++i) {
             msg() << MSG::DEBUG << "Track ID: " << tracksTrackJet[i] << endreq;
          }
       }
       
       m_jetDirection->SetX(cos(phiTrackJet));
       m_jetDirection->SetY(sin(phiTrackJet));
       m_jetDirection->SetZ(sinh(etaTrackJet));
       
    } 
    else if(m_useJetDirection == 1) {

       if(outputLevel <= MSG::DEBUG)
	 msg() << MSG::DEBUG << "Find jet direction from ROI (method 1)" << endreq;
       
       const TrigRoiDescriptor* roiDescriptor = 0;

       if ( (getFeature(jetTrackTE, roiDescriptor) == HLT::OK) && roiDescriptor ) {
          if (msgLvl() <= MSG::DEBUG) {
             msg() << MSG::DEBUG << "Using inputTE (of jet): " 
                   << "RoI id " << roiDescriptor->roiId()
                   << ", Phi = " <<  roiDescriptor->phi()
                   << ", Eta = " << roiDescriptor->eta() << endreq;
          }
       } 
       else {
          if (msgLvl() <= MSG::DEBUG) 
             msg() <<  MSG::DEBUG << "No valid RoI for this Trigger Element" << endreq;

	  if(bestFitPriVertex) delete bestFitPriVertex;
          return HLT::NAV_ERROR;
       }
       float phiJet = roiDescriptor->phi();
       float etaJet = roiDescriptor->eta();
       
       m_jetDirection->SetX(cos(phiJet));
       m_jetDirection->SetY(sin(phiJet));
       m_jetDirection->SetZ(sinh(etaJet));
       
    }
    
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG
	    << "TLorentzVector direction: eta = " << m_jetDirection->Eta()
	    << ", phi = " << m_jetDirection->Phi() 
	    << endreq;

    // Call all the secondary vertex finders specified in m_secVertexFinderToolsHandleArray
    ToolHandleArray<InDet::ISecVertexInJetFinder>::iterator itSecVtxFinders = m_secVertexFinderToolsHandleArray.begin();
    ToolHandleArray<InDet::ISecVertexInJetFinder>::iterator itSecVtxFindersEnd = m_secVertexFinderToolsHandleArray.end();

    m_secVertexInfoContainer = new Trk::VxSecVertexInfoContainer;
    m_secVertexInfo = 0;

    for(; itSecVtxFinders != itSecVtxFindersEnd; ++itSecVtxFinders) {

      std::string vxAuthor = (*itSecVtxFinders).name();

      if(*itSecVtxFinders != 0) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Running " << vxAuthor << endreq;

	xAOD::Vertex* vertex = new xAOD::Vertex();
	vertex->makePrivateStore();
	vertex->setPosition(bestFitPriVertex->position());
	vertex->setCovariancePosition(bestFitPriVertex->covariancePosition());
	vertex->setFitQuality(bestFitPriVertex->chiSquared(), bestFitPriVertex->numberDoF ()); 

	std::vector<const xAOD::IParticle*> inputIParticles;
	
	xAOD::TrackParticleContainer::const_iterator trackIt     = trackContainer->begin();
	xAOD::TrackParticleContainer::const_iterator lastTrackIt = trackContainer->end();	
	
	for(; trackIt != lastTrackIt; ++trackIt)
	  inputIParticles.push_back(*trackIt);
	
	m_secVertexInfo = (*itSecVtxFinders)->findSecVertex(*vertex, *m_jetDirection, inputIParticles);

	delete vertex;

	if(m_secVertexInfo == 0) {
	  if(outputLevel <= MSG::DEBUG)
	    msg() << MSG::DEBUG << vxAuthor << " returned null pointer (no vertex)" << endreq;
	  continue;
	}
	else {
	  if (m_secVertexInfo->vertices().size()) {
	    m_secVertexInfo->vertices().front()->setVertexType((xAOD::VxType::VertexType)2); 
	    if(outputLevel <= MSG::DEBUG)
	      msg() << MSG::DEBUG<< "SV has z-position = " << m_secVertexInfo->vertices().front()->z() << endreq;
	  }
	  m_secVertexInfoContainer->push_back(const_cast<Trk::VxSecVertexInfo*>(m_secVertexInfo));
	  
	  m_secVertexInfoContainer->back()->getSVOwnership(true);
	  m_nVxSecVertexInfo++;
	}

	if(outputLevel <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << vxAuthor << " returned " << m_secVertexInfo->vertices().size() << " vertices" << endreq;
	}
    
	//* for monitoring *//

	const Trk::VxSecVKalVertexInfo* m_secVKalVertexInfo = dynamic_cast<const Trk::VxSecVKalVertexInfo*>(m_secVertexInfo);
	const std::vector<xAOD::Vertex*> & myVertices = m_secVKalVertexInfo->vertices();
	if(myVertices.size()>0) {
	  m_secVtx_twoTrkTot = m_secVKalVertexInfo->n2trackvertices();
	  m_secVtx_mass      = m_secVKalVertexInfo->mass();
	  m_secVtx_energy    = m_secVKalVertexInfo->energyFraction();

	  std::vector<xAOD::Vertex*>::const_iterator verticesIt  = myVertices.begin();
	  std::vector<xAOD::Vertex*>::const_iterator verticesEnd = myVertices.end();

	  for( ; verticesIt!=verticesEnd ; ++verticesIt) {

	    if(!(*verticesIt)) {
	      msg() << MSG::DEBUG << "Secondary vertex from InDetVKalVxInJet has zero pointer. Skipping this vtx.." << endreq;
	      continue;
	    }
	
	    msg() << MSG::DEBUG << "VxCandidate at ("
		  << (*verticesIt)->position().x() << ","
		  << (*verticesIt)->position().y() << ","
		  << (*verticesIt)->position().z() << endreq;

	    std::vector<Trk::VxTrackAtVertex> m_tracksAtVertex = (*verticesIt)->vxTrackAtVertex();
	  
	    if(m_tracksAtVertex.size())
	      m_secVtx_numTrkSV = m_tracksAtVertex.size();
	  }
	}

	std::vector<xAOD::Vertex*>::const_iterator vertexIt     = m_secVertexInfo->vertices().begin();
	std::vector<xAOD::Vertex*>::const_iterator lastVertexIt = m_secVertexInfo->vertices().end();
      }
    }

    //  should this be inside getPriVtx function?
    // TrigDecisionTool ->ancestor (gets ancestor of feature)  ?

    // Create new ViewContainer of the primary vertex to attach to the TE
    ConstDataVector<xAOD::VertexContainer>* prmVx = new ConstDataVector<xAOD::VertexContainer>(SG::VIEW_ELEMENTS);
    if(bestFitPriVertex) {
      prmVx->clear(SG::VIEW_ELEMENTS); 
      prmVx->push_back(bestFitPriVertex);
    }

    // Attach new PrimaryVertex container to the TriggerElement
    if(HLT::OK != attachFeature(outputTE, prmVx, "T2PrimaryVertex")) {
      msg() << MSG::ERROR << "Could not attach T2PrimaryVertex feature to the TE" << endreq;
      return HLT::NAV_ERROR;
    } 
/*
    // Create new ViewContainer of the jet tracks to attach to the TE
    // Actually, probably this isn't necessary...  
    // at least it doesn't solve the problem I was having before... rubbishness squared
    xAOD::TrackParticleContainer::const_iterator trackIt      = trackContainer->begin();
    xAOD::TrackParticleContainer::const_iterator lastTrackIt  = trackContainer->end();	
    ConstDataVector<xAOD::TrackParticleContainer>* bJetTracks = new ConstDataVector<xAOD::TrackParticleContainer>(SG::VIEW_ELEMENTS);
    bJetTracks->clear(SG::VIEW_ELEMENTS);    
    for(; trackIt != lastTrackIt; ++trackIt) {
      bJetTracks->push_back(*trackIt);
    }

    // Attach JetTracks container to the TriggerElement
    if(HLT::OK != attachFeature(outputTE, bJetTracks, "bJetTracks")) {
      msg() << MSG::ERROR << "Could not attach bjetTracks feature to the TE" << endreq;
      return HLT::NAV_ERROR;
    } 
*/
    // Create dummy xAOD SV output and attach as feature 
    xAOD::VertexAuxContainer trigSecondaryVertexAuxContainer;
    xAOD::VertexContainer* trigSecondaryVertexContainer = new xAOD::VertexContainer();
    trigSecondaryVertexContainer->setStore(&trigSecondaryVertexAuxContainer);
    xAOD::Vertex* newVrt = new xAOD::Vertex();
    trigSecondaryVertexContainer->push_back(newVrt);
    
    if(attachFeature(outputTE, trigSecondaryVertexContainer, "SecondaryVertex") == HLT::OK) {
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::VertexContainer (SecondaryVertex)" << endreq;
    } 
    else {
      if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::VertexContainer (SecondaryVertex)" << endreq;
      return HLT::NAV_ERROR;
    }

    // Attach VxSecVertexInfo container to the TriggerElement
    if(HLT::OK != attachFeature(outputTE, m_secVertexInfoContainer, m_secVtxKey)) {
      msg() << MSG::ERROR << "Could not attach SecVtxInfo feature to the TE" << endreq;
      return HLT::NAV_ERROR;
    } 
    else {
      m_nVxSecVertexInfoContainers++;
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "VxSecVertexInfo containers recorded in StoreGate" << endreq;
    }
    
    if(outputLevel <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "VxContainer size: " << m_secVtx_numSV << endreq;
      msg() << MSG::DEBUG << "VxSecVertexInfoContainer size: " << m_secVertexInfoContainer->size() << endreq;
    }

    // Information about this method invocation
    if(outputLevel <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "~ Execution summary ~" << endreq;
      //msg() << MSG::DEBUG << "Number of VxCandidates so far (ignore - this always prints 0): " << m_nVxCandidates << endreq;
      //msg() << MSG::DEBUG << "Number of VxContainers so far (ignore - this always prints 0): " << m_nVxContainers << endreq;
      msg() << MSG::DEBUG << "Number of VxSecVertexInfo objects so far: " << m_nVxSecVertexInfo << endreq;
      msg() << MSG::DEBUG << "Number of VxSecVertexInfo containers so far: " << m_nVxSecVertexInfoContainers << endreq;
    }

    delete bestFitPriVertex;

    return HLT::OK;
  }

  HLT::ErrorCode TrigVxSecondaryCombo::hltFinalize() {
    
    msg() << MSG::INFO << "TrigVxSecondaryCombo::finalize()" << endreq;
    return HLT::OK;
  }

  HLT::ErrorCode TrigVxSecondaryCombo::getPrmVtxForFit(xAOD::Vertex*& vertex,
						       const xAOD::VertexContainer* prmVtxColl,
						       unsigned int& numPrimaryVertices,
						       float& xPrmVtx, float& yPrmVtx, float& zPrmVtx, int& status ) {
      
    //Retrieve beamspot to use in transverse plane
    // z vertex from T2Histo algorithm
      
    int beamSpotErrorCode = 0;
    //Used internally to communicate special treatment for requester.
    //Instead of returning an error so that one can still continue depending on what the issue was
    //Use bits:
    // ==0: it's good!
    // 1: beamspot status flag is not fulfilled
    // 2: beamspot width too large?
    // 3: beamspot width too small?
      
    float x, y, z, exx, exy, exz, eyy, eyz, ezz;
    exy = eyz = exz = 0; // cross-correlation 0 for now

    if (!prmVtxColl || prmVtxColl->size() == 0) {
	msg() << MSG::ERROR << "Failed to get xAOD::VertexContainer from the trigger element" << endreq;
	return HLT::MISSING_FEATURE;
    }
    
    z = prmVtxColl->at(0)->z();
    ezz = prmVtxColl->at(0)->covariance()[5];  
    
    if ( z ==-200 && ezz==-200 ) {
      msg() << MSG::DEBUG << "xAOD::Vertex was not found" << endreq;
      return HLT::OK;
    }

    float ndof = prmVtxColl->at(0)->numberDoF();
    float chi2 = prmVtxColl->at(0)->chiSquared();
    
    if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG <<"Found T2 z vertex at " << z << " +- " << ezz << "   (chi2/ndof="<<chi2<<"/"<<ndof<<")" << endreq;
    
    //* Retrieve beamspot information *//
    IBeamCondSvc* m_iBeamCondSvc; 
    StatusCode sc = service("BeamCondSvc", m_iBeamCondSvc);
    int beamSpotStatus;
    
    if (sc.isFailure() || m_iBeamCondSvc == 0) {
      x = 0.0;
      y = 0.0;
      z = 0.0;
      exx = 0.0;
      eyy = 0.0;
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. Using origin at ("<< x << "," << y << "," << z << ")" << endreq;
      
      //Put to zero, will be checked below
      beamSpotStatus = 0;
    } 
    else {
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Extracting beamspot parameteres" << endreq;
      } 
      
      Amg::Vector3D m_beamSpot = m_iBeamCondSvc->beamPos();
      x = m_beamSpot.x();
      y = m_beamSpot.y();
      
      //* Apply beam spot correction for tilt *//
      x = x + tan(m_iBeamCondSvc->beamTilt(0)) * (z - m_beamSpot.z());
      y = y + tan(m_iBeamCondSvc->beamTilt(1)) * (z - m_beamSpot.z());
      
      exx = m_iBeamCondSvc->beamSigma(0);
      eyy = m_iBeamCondSvc->beamSigma(1);

      //Beamspot status flag
      int beamSpotBitMap = m_iBeamCondSvc->beamStatus();
      
      //* Check if beam spot is from online algorithms *//
      beamSpotStatus = ((beamSpotBitMap & 0x4) == 0x4);
      
      //* Check if beam spot fit converged *//
      if (beamSpotStatus)
	beamSpotStatus = ((beamSpotBitMap & 0x3) == 0x3);
      
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Beam spot from service: x=" << x << ", y=" << y << ", z=" << m_beamSpot.z() 
	      << ", tiltX=" << m_iBeamCondSvc->beamTilt(0) << ", tiltY=" << m_iBeamCondSvc->beamTilt(1) << ", sigmaX=" << exx << ", sigmaY=" << eyy 
	      << ", sigmaZ=" <<  m_iBeamCondSvc->beamSigma(1) << ", status=" << beamSpotStatus << endreq;
    } 

    if (m_useBeamSpotFlag && !beamSpotStatus) {
      
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Beam spot status flag set to " << beamSpotStatus << ". SV weights are not computed." << endreq;
	msg() << MSG::DEBUG << "Use beam spot flag set to " << m_useBeamSpotFlag << ". SV weights are not computed." << endreq;
      }
      beamSpotErrorCode = beamSpotErrorCode | (1<<1) ; 
    }
      
    AmgSymMatrix(3) err;
    
    err(0,0) = exx;
    err(1,1) = eyy;
    err(2,2) = ezz;
    err(0,1) = err(1,0) = exy; 
    err(0,2) = err(2,0) = exz;
    err(1,2) = err(2,1) = eyz;

    AmgSymMatrix(3) cov = err;

    vertex = new xAOD::Vertex();
    vertex->makePrivateStore(); // does this help?
    vertex->setX(x);
    vertex->setY(y);
    vertex->setZ(z);
    vertex->setCovariancePosition(cov);
    vertex->setFitQuality(chi2, ndof);
 
    xPrmVtx = x;
    yPrmVtx = y;
    zPrmVtx = z;
    numPrimaryVertices = 1; 
    status = beamSpotErrorCode;
    
    return HLT::OK;
  }

  HLT::ErrorCode TrigVxSecondaryCombo::getEFPrmVtxForFit(xAOD::Vertex*& bestFitPriVertex,
							 const HLT::TriggerElement* outputTE,
							 unsigned int& numPrimaryVertices,
							 float& xPrmVtx, float& yPrmVtx, float& zPrmVtx ) {
     
    int outputLevel = msgLvl();
    
    // Navigate from the TriggerElement to get the input primary vertex container
    const xAOD::VertexContainer* primaryVxContainer;
    
    if(HLT::OK != getFeature(outputTE, primaryVxContainer, m_priVtxKey)) {
      msg() << MSG::ERROR << "Input primary vertex container could not be found" << endreq;
      return HLT::NAV_ERROR;
    }

    if(!primaryVxContainer) {
       if(outputLevel <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Input primary vertex container is null pointer" << endreq;
       return HLT::MISSING_FEATURE;
    }
    
    numPrimaryVertices = primaryVxContainer->size();
    
    if(numPrimaryVertices == 0) {
       if(outputLevel <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Input primary vertex container is empty" << endreq;
       return HLT::MISSING_FEATURE; 
    }
    
    if(outputLevel <= MSG::DEBUG)
       msg() << MSG::DEBUG << "Retrieved input primary vertex container with " << numPrimaryVertices << " vertices." << endreq;
    
    // Get the primary vertex, if one has been found
    xAOD::VertexContainer::const_iterator firstPriVx = primaryVxContainer->begin();
    xAOD::VertexContainer::const_iterator lastPriVx  = primaryVxContainer->end();
    
    unsigned int numberOfPriVx = 0;
    
    double Chi2 = 0.0;
    double NDoF = 1.0;
    
    double minFit = 999.0;
    double fit;
    
    // Can there be more than one primary vertex candidate? Unsure,
    // but if this does happen, let's take the one with the best
    // fit...
    for(; firstPriVx != lastPriVx; ++firstPriVx) {
       
       if(outputLevel <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Vertex type: " << (*firstPriVx)->vertexType() << endreq;
       
       // Reject dummy vertices      
       if((*firstPriVx)->vertexType() != xAOD::VxType::PriVtx) { // Trk::PriVtx) {
          if(outputLevel <= MSG::DEBUG)
             msg() << MSG::DEBUG << " -- Not a primary vertex, skipping..." << endreq;
          continue;
       }
       
       numberOfPriVx++;

       Chi2 = (*firstPriVx)->chiSquared();
       NDoF = (*firstPriVx)->numberDoF();
       fit = fabs(Chi2/NDoF);
       
       if(outputLevel <= MSG::DEBUG)
          msg() << MSG::DEBUG << " -- Chi2: " << Chi2 << ", NDoF: " << NDoF << ", Chi2/NDoF: " << fit << endreq;

       // Take lowest Chi2/NDoF
       if(fit < minFit && fit != 0) {
          minFit  = fit;
	  // Set bestFitPriVertex to this vertex
	  bestFitPriVertex = *firstPriVx; 
          xPrmVtx = bestFitPriVertex->x();
          yPrmVtx = bestFitPriVertex->y();
          zPrmVtx = bestFitPriVertex->z();
       }
    }
    
    if(numberOfPriVx == 0 || !bestFitPriVertex) {
       if(outputLevel <= MSG::DEBUG) 
          msg() << MSG::DEBUG << "No primary vertex found" << endreq;
       return HLT::MISSING_FEATURE;
    }
    
    if(outputLevel <= MSG::DEBUG) {
       msg() << MSG::DEBUG << "Number of primary vertices found: " << numberOfPriVx << endreq;
       msg() << MSG::DEBUG << " -- Chi2/NDoF: " << bestFitPriVertex->chiSquared() 
	     << "/" << bestFitPriVertex->numberDoF() << endreq;
       msg() << MSG::DEBUG << " -- x: " << xPrmVtx << ", y: " << yPrmVtx << ", z: " << zPrmVtx << endreq;
    }

    return HLT::OK;
  }


} // end namespace InDet
