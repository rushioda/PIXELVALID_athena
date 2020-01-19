/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinkTrkZmumuTagTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Shimpei Yamamoto (shimpei.yamamoto@cern.ch)

#include "LongLivedParticleDPDMaker/KinkTrkZmumuTagTool.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODMissingET/MissingETContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::KinkTrkZmumuTagTool::KinkTrkZmumuTagTool(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_trigMatchTool("TrigMatchTool/TrigMatchTool"),
  m_trigNames(std::vector<std::string>()),
  m_trigMatchDeltaR(0.1),
  m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
  m_muonSGKey("Muons"),
  m_muonIDKeys(std::vector<std::string>()),
  m_muonPtCut(0),
  m_muonEtaMax(9999),
  m_trackSGKey("MuonSpectrometerTracks"),
  m_trackPtCut(0),
  m_trackEtaMax(9999),
  m_diMuonMassLow(50.),
  m_diMuonMassHigh(-1),
  m_dPhiMax(10),
  m_doOppositeSignReq(false),
  m_sgKeyPrefix("KinkTrk")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("TriggerDecisionTool", m_trigDecisionTool);
  declareProperty("TriggerMatchTool", m_trigMatchTool);
  declareProperty("Triggers", m_trigNames);
  declareProperty("TriggerMatchDeltaR", m_trigMatchDeltaR);
  declareProperty("RequireTriggerMatch", m_doTrigMatch);
  declareProperty("MuonContainerKey", m_muonSGKey);
  declareProperty("MuonIDKeys", m_muonIDKeys);
  declareProperty("MuonPtMin", m_muonPtCut);
  declareProperty("MuonEtaMax", m_muonEtaMax);
  declareProperty("TrackContainerKey", m_trackSGKey);
  declareProperty("TrackPtMin", m_trackPtCut);
  declareProperty("TrackEtaMax", m_trackEtaMax);
  declareProperty("DiMuonMassLow", m_diMuonMassLow);
  declareProperty("DiMuonMassHigh", m_diMuonMassHigh);
  declareProperty("DeltaPhiMax", m_dPhiMax); 
  declareProperty("RequireOppositeSign", m_doOppositeSignReq); 
  declareProperty("StoreGateKeyPrefix", m_sgKeyPrefix);
}
  

// Destructor
DerivationFramework::KinkTrkZmumuTagTool::~KinkTrkZmumuTagTool() {
}  


// Athena initialize and finalize
StatusCode DerivationFramework::KinkTrkZmumuTagTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // trigger decision tool
  if (m_trigNames.size()>0) {
    if (m_trigDecisionTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("TriggerDecisionTool: " << m_trigDecisionTool);

    if (m_trigMatchTool.empty()) {
      ATH_MSG_FATAL("TrigMatchTool is not specified.");
      return StatusCode::FAILURE;
    }
    CHECK(m_trigMatchTool.retrieve());
    ATH_MSG_INFO("TrgMatchTool retrived successfully");
  }

  CHECK(m_muonSelectionTool.retrieve());

  return StatusCode::SUCCESS;
}


StatusCode DerivationFramework::KinkTrkZmumuTagTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}



// Augmentation
StatusCode DerivationFramework::KinkTrkZmumuTagTool::addBranches() const
{
  std::vector<float> *diMuonTrkMass = new std::vector<float>();
  std::vector<float> *probeMuPt = new std::vector<float>();
  std::string sgKey1(m_sgKeyPrefix+"DiMuMass");
  std::string sgKey2(m_sgKeyPrefix+"ProbeMuPt");

  const xAOD::MuonContainer* muons(0);
  ATH_CHECK(evtStore()->retrieve(muons, m_muonSGKey));	

  const xAOD::TrackParticleContainer* mstracks(0);
  ATH_CHECK(evtStore()->retrieve(mstracks, m_trackSGKey));

  for (auto muon: *muons) {
    if (!checkTagMuon(muon)) continue;
    for (auto track: *mstracks) {
      if (!checkMSTrack(track)) continue;
      if (!checkMuonTrackPair(muon, track)) continue;
      diMuonTrkMass->push_back((muon->p4()+track->p4()).M());
      probeMuPt->push_back(track->pt());
    }
  }

  // Writing to SG
  if (evtStore()->contains< float >(sgKey1)) {
    ATH_MSG_ERROR("StoreGate key " << sgKey1 << "already exists.");
    // avoid mem leak
    delete diMuonTrkMass;
    delete probeMuPt;
    return StatusCode::FAILURE;
  }
  CHECK(evtStore()->record(diMuonTrkMass, sgKey1));

  if (evtStore()->contains< float >(sgKey2)) {
    ATH_MSG_ERROR("StoreGate key " << sgKey2 << "already exists.");
    // avoid mem leak
    delete probeMuPt;
    return StatusCode::FAILURE;
  }
  CHECK(evtStore()->record(probeMuPt, sgKey2));

  return StatusCode::SUCCESS;
}


bool DerivationFramework::KinkTrkZmumuTagTool::passTrigger(const std::vector<std::string>& triggers) const
{
  for (unsigned int i=0; i<triggers.size(); i++) {
    if (m_trigDecisionTool->isPassed(triggers[i])) return true;
  }
  return false;
}


bool DerivationFramework::KinkTrkZmumuTagTool::checkTagMuon(const xAOD::Muon *muon) const
{
  if (!passMuonQuality(muon)) return false;
  if (m_doTrigMatch) {
    if (!passMuonTrigMatch(muon, m_trigNames)) return false;
  }
  return true;
}


bool DerivationFramework::KinkTrkZmumuTagTool::checkMSTrack(const xAOD::TrackParticle *track) const
{
  if (!passMSTrackQuality(track)) return false;
  return true;
}


bool DerivationFramework::KinkTrkZmumuTagTool::checkMuonTrackPair(const xAOD::Muon *muon, const xAOD::TrackParticle *track) const
{
  if (fabs(muon->p4().DeltaPhi(track->p4())) > m_dPhiMax) return false;
  if (m_doOppositeSignReq) {
    if (muon->charge()*track->charge() > 0) return false;
  }
  float mass = (muon->p4()+track->p4()).M();
  if (mass < m_diMuonMassLow) return false;
  if (mass > m_diMuonMassHigh) return false;
  return true;
}


bool DerivationFramework::KinkTrkZmumuTagTool::passMuonQuality(const xAOD::Muon *muon) const
{
  if (muon->pt() < m_muonPtCut) return false;
  if (fabs(muon->eta()) > m_muonEtaMax) return false;

  bool passID(false);
  for (unsigned int i=0; i<m_muonIDKeys.size(); i++) {
    int qflag(0);
    if (m_muonIDKeys[i] == "VeryLoose") {
      qflag = xAOD::Muon::VeryLoose;
    } else if (m_muonIDKeys[i] == "Loose") {
      qflag = xAOD::Muon::Loose;
    } else if (m_muonIDKeys[i] == "Medium" ) {
      qflag = xAOD::Muon::Medium;
    } else if (m_muonIDKeys[i] == "Tight") { 
      qflag = xAOD::Muon::Tight;
    } else {
      ATH_MSG_WARNING("Cannot find the muon quality flag " << m_muonIDKeys[i] << ". Use Medium instead.");
      qflag = xAOD::Muon::Medium;
    }

    if ( m_muonSelectionTool->getQuality(*muon) <= qflag ) {
      passID = true;
      break;
    }
  }
  if (!passID) return false;

  return true;
}


bool DerivationFramework::KinkTrkZmumuTagTool::passMSTrackQuality(const xAOD::TrackParticle *track) const
{
  if (track->pt() < m_trackPtCut) return false;
  if (fabs(track->eta()) > m_trackEtaMax) return false;
  return true;
}


// checking the muon trigger matching
bool DerivationFramework::KinkTrkZmumuTagTool::passMuonTrigMatch(const xAOD::Muon *muon, const std::vector<std::string>& triggers) const
{
  for (unsigned int i=0; i<triggers.size(); i++) {
    if (m_trigMatchTool->chainPassedByObject<TrigMatch::TrigMuonEF, xAOD::Muon>(muon, triggers[i], m_trigMatchDeltaR)) return true;
  }
  return false;
}

