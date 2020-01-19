/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************
//
// NAME:     TrigCaloRatioHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
//
// ************************************************************

#include "TrigLongLivedParticlesHypo/TrigCaloRatioHypo.h"

#include "TrigSteeringEvent/PhiHelper.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "CxxUtils/fpcompare.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigCaloRatioHypo::TrigCaloRatioHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("EtCut",       m_etCut = 30*CLHEP::GeV, "cut value forthe jet et"); 
  declareProperty("LogRatioCut", m_logRatioCut = 1.2, "cut value for the jet energy ratio"); 
  declareProperty("PtMinID",     m_ptCut = 2000.0, "minimum track Pt in MeV for the isolation requirement");
  declareProperty("TrackCut",    m_trackCut = 0, "minimum number of tracks for the isolation requirement");
  declareProperty("DeltaR",      m_deltaR=0.2, "radius for the track isolation requirement");
  declareProperty("EtaCut",      m_etaCut = 2.5, "cut value for Eta of the Jet");
  declareProperty("Reversed",    m_reversedCut = false, "reversed cut for collimated photons");
  declareProperty("DoTrackIso",  m_doTrackIso = true, "switch on/off the track isolation requirement");
  declareProperty("AcceptAll",   m_acceptAll=false);

  declareMonitoredStdContainer("JetEt",      m_jetEt);
  declareMonitoredStdContainer("JetEta",     m_jetEta);
  declareMonitoredStdContainer("JetPhi",     m_jetPhi);
  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("logRatio",   m_logRatio);  
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigCaloRatioHypo::~TrigCaloRatioHypo() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigCaloRatioHypo::hltInitialize() {

  msg() << MSG::INFO << "in initialize()" << endreq;

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {  
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " EtCut       = " << m_etCut << endreq; 
    msg() << MSG::DEBUG << " LogRatioCut = " << m_logRatioCut << endreq; 
    msg() << MSG::DEBUG << " PtMinID     = " << m_ptCut << endreq; 
    msg() << MSG::DEBUG << " TrackCut    = " << m_trackCut << endreq; 
    msg() << MSG::DEBUG << " DeltaR      = " << m_deltaR << endreq; 
    msg() << MSG::DEBUG << " EtaCut      = " << m_etaCut << endreq; 
    msg() << MSG::DEBUG << " Reversed    = " << m_reversedCut << endreq; 
    msg() << MSG::DEBUG << " DoTrackIso  = " << m_doTrackIso << endreq; 
    msg() << MSG::DEBUG << " AcceptAll   = " << m_acceptAll << endreq;
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigCaloRatioHypo::hltFinalize() {

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors: "<< m_accepted <<" / "<<m_rejected <<" / "<< m_errors << endreq;
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigCaloRatioHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  m_cutCounter = -1;

  bool passCutJet = false;
  bool passCutTrk = true; //default to true, changes false if m_trackCut tracks are within m_deltaR of the jet axis
  pass=false;

  double jetRatio = -1.;
  m_logRatio = -1.;

  //clear monitoring vectors
  m_jetEt.clear();
  m_jetPhi.clear();
  m_jetEta.clear();

  const xAOD::JetContainer* vectorOfJets = 0;
  HLT::ErrorCode status = getFeature(outputTE, vectorOfJets);

  m_cutCounter = 0;

  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get the xAOD jet container" << endreq;
    m_errors++;
    return HLT::ERROR;
  } else if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Got the xAOD jet container" << endreq;
  
  if (vectorOfJets->size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "The xAOD jet container is empty" << endreq;
    m_errors++;
    return HLT::OK;
  } else if (vectorOfJets->size() != 1) {
    msg() << MSG::ERROR << "More than one xAOD jet, not expected!" << endreq;
    m_errors++;
    return HLT::ERROR;
  } else {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << vectorOfJets->size() << " jet is found" << endreq;
  }

  const xAOD::Jet *jet = vectorOfJets->front();

  double jetEta = jet->eta();
  double jetPhi = jet->phi();

  double jetEt  = jet->p4().Et();
  double jetEMF = jet->getAttribute<float>("EMFrac");

  double zero = 0.;
  double one = 1.;
  if (CxxUtils::fpcompare::greater(jetEMF,zero)){
    if(CxxUtils::fpcompare::greater_equal(jetEMF,one)) jetRatio = -999.;
    else jetRatio = log10(double(1./jetEMF - 1.));
  } else {
    jetRatio = 999; 
  }
  
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " jet with et=" << jetEt << ", eta=" << jetEta << ", phi=" << jetPhi 
	  << ", log-ratio=" << jetRatio << endreq;
  
  if(!m_reversedCut) {
    
    if (jetEt > m_etCut && std::fabs(jetEta) <= m_etaCut) {
      
      m_jetEt.push_back(jetEt/CLHEP::GeV);
      m_jetEta.push_back(jetEta); 
      m_jetPhi.push_back(jetPhi);
      m_logRatio = jetRatio;
      
      passCutJet = true;
    }
    
  } else {
      
    if (jetEt > m_etCut && std::fabs(jetEta) <= m_etaCut && jetRatio <= m_logRatioCut) {
	
      m_jetEt.push_back(jetEt/CLHEP::GeV);
      m_jetEta.push_back(jetEta); 
      m_jetPhi.push_back(jetPhi);
      m_logRatio = jetRatio;
      
      passCutJet = true;
    }
    
  }

  if(m_doTrackIso) {

    bool countTracks=0;

    const xAOD::TrackParticleContainer* vectorOfTracks;
    status = getFeature(outputTE, vectorOfTracks, "InDetTrigTrackingxAODCnv_Tau_IDTrig");
    
    if(status != HLT::OK) {
      msg() << MSG::ERROR << "Failed to get the xAOD track container" << endreq;
      m_errors++;
      return HLT::ERROR;
    } else if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Got the xAOD track container" << endreq;
    
    if(vectorOfTracks->size() == 0) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "The xAOD track container is empty" << endreq;
      passCutTrk = true; //default is true, but just for clarity
    } else {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << vectorOfTracks->size() << " tracks are found" << endreq;
      
      xAOD::TrackParticleContainer::const_iterator
	track     = vectorOfTracks->begin(),
	lasttrack = vectorOfTracks->end();
      
      for(; track !=lasttrack; track++ ) {
	
	float theta   = (*track)->theta();
	float qOverPt = (*track)->qOverP()/TMath::Sin(theta);
	float pT      = (1/qOverPt);
	
	if (fabs(pT) <= m_ptCut) continue;
	
	double phi  = (*track)->phi0();
	double eta  = (*track)->eta();
	
	if(msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << " track with " << "pt=" << pT << ", eta=" << eta << ", phi=" << phi  << endreq;
	
	double deta = fabs(eta-jetEta);	
	double dphi = fabs(HLT::wrapPhi(phi-jetPhi));
	
	double dR = sqrt((deta*deta)+(dphi*dphi));
	if (dR<m_deltaR) {
	  countTracks++;
	}
      }
    }

    if(countTracks>m_trackCut){
      passCutTrk = false;
    }
    else{
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet passed tracking isolation" << endreq; 
    }
  }

  if((passCutJet&&passCutTrk) || m_acceptAll) {

    pass = true;
    m_cutCounter = 1;

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Jet passing calo-ratio requirements" << endreq; 

  } else {
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Jet not passing calo-ratio requirements" << endreq; 
  }

  return HLT::OK;  
}
