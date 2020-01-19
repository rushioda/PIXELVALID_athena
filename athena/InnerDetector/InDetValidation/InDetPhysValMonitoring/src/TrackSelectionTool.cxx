/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetPhysValMonitoring includes
#include "TrackSelectionTool.h"
#include <cmath> // std::fabs

TrackSelectionTool::TrackSelectionTool(const std::string& name) :
  asg::AsgTool(name),
  m_accept("TrackSelection"),
  m_numProcessed(0),
  m_numPassed(0) {
  std::cout << "<TrackSelectionTool::TrackSelectionTool> Entering." << std::endl;

  declareInterface<IAsgSelectionTool>(this);

  // declareProperty( "Property", m_nProperty ); // Example property declaration.

  declareProperty("maxPt", m_maxPt = 1E+06); // [MeV]
  declareProperty("minPt", m_minPt = 500.); // [MeV]
  declareProperty("maxEta", m_maxEta = 5.);
  declareProperty("minEta", m_minEta = -1);
  declareProperty("maxPrimaryImpact", m_maxPrimaryImpact = 300.); // [mm]
  declareProperty("maxZImpact", m_maxZImpact = 1500.); // [mm]
  declareProperty("minPrimaryImpact", m_minPrimaryImpact = -1); // [mm]
  declareProperty("minZImpact", m_minZImpact = -1); // [mm]
  declareProperty("maxSecondaryImpact", m_maxSecondaryImpact = 300.); // [mm]
  declareProperty("minSecondaryPt", m_minSecondaryPt = 500.); // [MeV]
  declareProperty("minClusters", m_minClusters = 7);
  declareProperty("minSiNotShared", m_minSiNotShared = 5);
  declareProperty("maxShared", m_maxShared = 2);
  declareProperty("minPixelHits", m_minPixelHits = 0);
  declareProperty("maxHoles", m_maxHoles = 2);
  declareProperty("maxPixelHoles", m_maxPixelHoles = 1);
  declareProperty("maxSctHoles", m_maxSctHoles = 2);
  declareProperty("maxDoubleHoles", m_maxDoubleHoles = 1);
  declareProperty("radMax", m_radMax = 600.); // [mm]
  declareProperty("nHolesMax", m_nHolesMax = m_maxHoles);
  declareProperty("nHolesGapMax", m_nHolesGapMax = m_maxHoles);
  declareProperty("seedFilterLevel", m_seedFilterLevel = 2);
  declareProperty("maxTRTHighThresholdHits", m_maxTRTHighThresholdHits = -1);
  declareProperty("minTRTHighThresholdHits", m_minTRTHighThresholdHits = -1);
  declareProperty("maxTRTHighThresholdOutliers", m_maxTRTHighThresholdOutliers = -1);
  declareProperty("maxSCTHits", m_maxSCTHits = -1);
  declareProperty("minSCTHits", m_minSCTHits = -1);
  declareProperty("maxTRTOutliers", m_maxTRTOutliers = -1);
  declareProperty("maxBLayerSplitHits", m_maxBLayerSplitHits = -1);
  declareProperty("maxPixelOutliers", m_maxPixelOutliers = -1);

  std::cout << "<TrackSelectionTool::TrackSelectionTool> Exiting." << std::endl;
}

TrackSelectionTool::~TrackSelectionTool() {
}

StatusCode
TrackSelectionTool::initialize() {
  std::cout << "<TrackSelectionTool::initialize> Entering." << std::endl;

  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Initializing " << name() << "...");

  // Define cut names and descriptions.
  m_cuts.clear();

  if (m_maxPt > -1) {
    m_cuts.push_back(std::make_pair("maxPt", "descr."));
  }
  if (m_minPt > -1) {
    m_cuts.push_back(std::make_pair("minPt", "descr."));
  }
  if (m_maxEta > -1) {
    m_cuts.push_back(std::make_pair("maxEta", "descr."));
  }
  if (m_minEta > -1) {
    m_cuts.push_back(std::make_pair("minEta", "descr."));
  }
  if (m_maxPrimaryImpact > -1) {
    m_cuts.push_back(std::make_pair("maxPrimaryImpact", "descr."));
  }
  if (m_maxZImpact > -1) {
    m_cuts.push_back(std::make_pair("maxZImpact", "descr."));
  }
  if (m_minPrimaryImpact > -1) {
    m_cuts.push_back(std::make_pair("minPrimaryImpact", "descr."));
  }
  if (m_minZImpact > -1) {
    m_cuts.push_back(std::make_pair("minZImpact", "descr."));
  }
  if (m_maxSecondaryImpact > -1) {
    m_cuts.push_back(std::make_pair("maxSecondaryImpact", "descr."));
  }
  if (m_minSecondaryPt > -1) {
    m_cuts.push_back(std::make_pair("minSecondaryPt", "descr."));
  }
  if (m_minClusters > -1) {
    m_cuts.push_back(std::make_pair("minClusters", "descr."));
  }
  if (m_minSiNotShared > -1) {
    m_cuts.push_back(std::make_pair("minSiNotShared", "descr."));
  }
  if (m_maxShared > -1) {
    m_cuts.push_back(std::make_pair("maxShared", "descr."));
  }
  if (m_minPixelHits > -1) {
    m_cuts.push_back(std::make_pair("minPixelHits", "descr."));
  }
  if (m_maxHoles > -1) {
    m_cuts.push_back(std::make_pair("maxHoles", "descr."));
  }
  if (m_maxPixelHoles > -1) {
    m_cuts.push_back(std::make_pair("maxPixelHoles", "descr."));
  }
  if (m_maxSctHoles > -1) {
    m_cuts.push_back(std::make_pair("maxSctHoles", "descr."));
  }
  if (m_maxDoubleHoles > -1) {
    m_cuts.push_back(std::make_pair("maxDoubleHoles", "descr."));
  }
  if (m_radMax > -1) {
    m_cuts.push_back(std::make_pair("radMax", "descr."));
  }
  if (m_nHolesMax > -1) {
    m_cuts.push_back(std::make_pair("nHolesMax", "descr."));
  }
  if (m_nHolesGapMax > -1) {
    m_cuts.push_back(std::make_pair("nHolesGapMax", "descr."));
  }
  if (m_seedFilterLevel > -1) {
    m_cuts.push_back(std::make_pair("seedFilterLevel", "descr."));
  }
  if (m_maxTRTHighThresholdHits > -1) {
    m_cuts.push_back(std::make_pair("maxTRTHighThresholdHits", "descr."));
  }
  if (m_minTRTHighThresholdHits > -1) {
    m_cuts.push_back(std::make_pair("minTRTHighThresholdHits", "descr."));
  }
  if (m_maxTRTHighThresholdOutliers > -1) {
    m_cuts.push_back(std::make_pair("maxTRTHighThresholdOutliers", "descr."));
  }
  if (m_maxSCTHits > -1) {
    m_cuts.push_back(std::make_pair("maxSCTHits", "descr."));
  }
  if (m_minSCTHits > -1) {
    m_cuts.push_back(std::make_pair("minSCTHits", "descr."));
  }
  if (m_maxTRTOutliers > -1) {
    m_cuts.push_back(std::make_pair("maxTRTOutliers", "descr."));
  }
  if (m_maxBLayerSplitHits > -1) {
    m_cuts.push_back(std::make_pair("maxBLayerSplitHits", "descr."));
  }
  if (m_maxPixelOutliers > -1) {
    m_cuts.push_back(std::make_pair("maxPixelOutliers", "descr."));
  }

  // Example.
  // if (m_maxEta>-1) m_cuts.push_back(std::make_pair("eta", "Cut on (absolute) particle eta"));

  // Add cuts to the TAccept.
  for (const auto& cut : m_cuts) {
    if (m_accept.addCut(cut.first, cut.second) < 0) {
      ATH_MSG_ERROR("Failed to add cut " << cut.first << " because the TAccept object is full.");
      return StatusCode::FAILURE;
    }
  }

  // Initialise counters.
  m_numPassedCuts.resize(m_accept.getNCuts(), 0);

  std::cout << "<TrackSelectionTool::initialize> Exiting." << std::endl;

  return StatusCode::SUCCESS;
}

const Root::TAccept&
TrackSelectionTool::getTAccept( ) const {
  return m_accept;
}

const Root::TAccept&
TrackSelectionTool::accept(const xAOD::IParticle* p) const {
  /*Is this perhaps supposed to be xAOD::TrackParticle? */

  // Reset the result.
  m_accept.clear();

  // Check if this is a track.
  if (!p) {
    ATH_MSG_ERROR("accept(...) Function received a null pointer");
    return m_accept;
  }
  if (p->type() != xAOD::Type::TrackParticle) {
    ATH_MSG_ERROR("accept(...) Function received a non-TrackParticle");
    return m_accept;
  }

  // Cast it to a track (we have already checked its type so we do not have to dynamic_cast).
  const xAOD::TrackParticle* track = static_cast< const xAOD::TrackParticle* >(p);

  // Let the specific function do the work.
  return accept(track);
}

const Root::TAccept&
TrackSelectionTool::accept(const xAOD::TrackParticle* p) const {
  // Reset the TAccept.
  m_accept.clear();

  uint8_t iBLayerHits(0), iBLayerOutliers(0), iBLayerSplitHits(0), iBLayerSharedHits(0);
  uint8_t iPixHits(0), iPixHoles(0), iPixSharedHits(0), iPixOutliers(0), iPixContribLayers(0), iPixSplitHits(0),
  iPixGangedHits(0);
  uint8_t iSCTHits(0), iSCTHoles(0), iSCTDoubleHoles(0), iSCTSharedHits(0), iSCTOutliers(0);
  // uint8_t iSiHits(0);//unused?
  uint8_t iTRTHits(0), iTRTHTHits(0), iTRTOutliers(0), iTRTHTOutliers(0);

  if (!p->summaryValue(iBLayerHits, xAOD::numberOfBLayerHits)) {
    return m_accept;
  }
  if (!p->summaryValue(iBLayerOutliers, xAOD::numberOfBLayerOutliers)) {
    return m_accept;
  }
  if (!p->summaryValue(iBLayerSharedHits, xAOD::numberOfBLayerSharedHits)) {
    return m_accept;
  }
  if (!p->summaryValue(iBLayerSplitHits, xAOD::numberOfBLayerSplitHits)) {
    return m_accept;
  }

  if (!p->summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    return m_accept;
  }
  if (!p->summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    return m_accept;
  }
  if (!p->summaryValue(iPixOutliers, xAOD::numberOfPixelOutliers)) {
    return m_accept;
  }
  if (!p->summaryValue(iPixContribLayers, xAOD::numberOfContribPixelLayers)) {
    return m_accept;
  }
  if (!p->summaryValue(iPixSharedHits, xAOD::numberOfPixelSharedHits)) {
    return m_accept;
  }
  if (!p->summaryValue(iPixSplitHits, xAOD::numberOfPixelSplitHits)) {
    return m_accept;
  }
  if (!p->summaryValue(iPixGangedHits, xAOD::numberOfGangedPixels)) {
    return m_accept;
  }

  if (!p->summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
    return m_accept;
  }
  if (!p->summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    return m_accept;
  }
  if (!p->summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
    return m_accept;
  }
  if (!p->summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) {
    return m_accept;
  }
  if (!p->summaryValue(iSCTSharedHits, xAOD::numberOfSCTSharedHits)) {
    return m_accept;
  }

  if (!p->summaryValue(iTRTOutliers, xAOD::numberOfTRTOutliers)) {
    return m_accept;
  }
  if (!p->summaryValue(iTRTHTOutliers, xAOD::numberOfTRTHighThresholdOutliers)) {
    return m_accept;
  }
  if (!p->summaryValue(iTRTHits, xAOD::numberOfTRTHits)) {
    return m_accept;
  }
  if (!p->summaryValue(iTRTHTHits, xAOD::numberOfTRTHighThresholdHits)) {
    return m_accept;
  }

  // iSiHits = iPixHits + iSCTHits;

  // Check cuts.
  if (m_maxPt > -1) {
    m_accept.setCutResult("maxPt", p->pt() < m_maxPt);
  }
  if (m_minPt > -1) {
    m_accept.setCutResult("minPt", p->pt() > m_minPt);
  }
  if (m_maxEta > -1) {
    m_accept.setCutResult("maxEta", p->pt() > 1E-07 ? std::fabs(p->eta()) < m_maxEta : false);
  }
  if (m_minEta > -1) {
    m_accept.setCutResult("minEta", p->pt() > 1E-07 ? std::fabs(p->eta()) > m_minEta : false);
  }
  if (m_maxPrimaryImpact > -1) {
    m_accept.setCutResult("maxPrimaryImpact", std::fabs(p->d0()) < m_maxPrimaryImpact);
  }
  if (m_maxZImpact > -1) {
    m_accept.setCutResult("maxZImpact", std::fabs(p->z0()) < m_maxZImpact);
  }
  if (m_minPrimaryImpact > -1) {
    m_accept.setCutResult("minPrimaryImpact", std::fabs(p->d0()) > m_minPrimaryImpact);
  }
  if (m_minZImpact > -1) {
    m_accept.setCutResult("minZImpact", std::fabs(p->z0()) > m_minZImpact);
  }
  if (m_maxSecondaryImpact > -1) {
    m_accept.setCutResult("maxSecondaryImpact", true /* nop */);
  }
  if (m_minSecondaryPt > -1) {
    m_accept.setCutResult("minSecondaryPt", true /* nop */);
  }
  if (m_minClusters > -1) {
    m_accept.setCutResult("minClusters", true /* nop */);
  }
  if (m_minSiNotShared > -1) {
    m_accept.setCutResult("minSiNotShared",
                          (iBLayerHits + iPixHits + iSCTHits - iBLayerSharedHits - iPixSharedHits - iSCTSharedHits) >=
                          m_minSiNotShared);
  }
  if (m_maxShared > -1) {
    m_accept.setCutResult("maxShared", iBLayerSharedHits + iPixSharedHits + iSCTSharedHits <= m_maxShared);
  }
  if (m_minPixelHits > -1) {
    m_accept.setCutResult("minPixelHits", iPixHits >= m_minPixelHits);
  }
  if (m_maxHoles > -1) {
    m_accept.setCutResult("maxHoles", iPixHoles + iSCTHoles <= m_maxHoles);
  }
  if (m_maxPixelHoles > -1) {
    m_accept.setCutResult("maxPixelHoles", iPixHoles <= m_maxPixelHoles);
  }
  if (m_maxSctHoles > -1) {
    m_accept.setCutResult("maxSctHoles", iSCTHoles <= m_maxSctHoles);
  }
  if (m_maxDoubleHoles > -1) {
    m_accept.setCutResult("maxDoubleHoles", iSCTDoubleHoles <= m_maxDoubleHoles);
  }
  if (m_radMax > -1) {
    m_accept.setCutResult("radMax", true /* nop */);
  }
  if (m_nHolesMax > -1) {
    m_accept.setCutResult("nHolesMax", true /* nop */);
  }
  if (m_nHolesGapMax > -1) {
    m_accept.setCutResult("nHolesGapMax", true /* nop */);
  }
  if (m_seedFilterLevel > -1) {
    m_accept.setCutResult("seedFilterLevel", true /* nop */);
  }
  if (m_maxTRTHighThresholdHits > -1) {
    m_accept.setCutResult("maxTRTHighThresholdHits", iTRTHTHits <= m_maxTRTHighThresholdHits);
  }
  if (m_minTRTHighThresholdHits > -1) {
    m_accept.setCutResult("minTRTHighThresholdHits", iTRTHTHits <= m_minTRTHighThresholdHits);
  }
  if (m_maxTRTHighThresholdOutliers > -1) {
    m_accept.setCutResult("maxTRTHighThresholdOutliers", iTRTHTOutliers <= m_maxTRTHighThresholdOutliers);
  }
  if (m_maxSCTHits > -1) {
    m_accept.setCutResult("maxSCTHits", iSCTHits <= m_maxSCTHits);
  }
  if (m_minSCTHits > -1) {
    m_accept.setCutResult("minSCTHits", iSCTHits >= m_minSCTHits);
  }
  if (m_maxTRTOutliers > -1) {
    m_accept.setCutResult("maxTRTOutliers", iTRTOutliers <= m_maxTRTOutliers);
  }
  if (m_maxBLayerSplitHits > -1) {
    m_accept.setCutResult("maxBLayerSplitHits", iBLayerSplitHits <= m_maxBLayerSplitHits);
  }
  if (m_maxPixelOutliers > -1) {
    m_accept.setCutResult("maxPixelOutliers", iPixOutliers <= m_maxPixelOutliers);
  }

  // Example.
  // if (m_maxEta>-1) m_accept.setCutResult("eta", (p->pt()>1e-7 ? (fabs(p->eta()) < m_maxEta) : false) );

  // Book keep cuts
  for (const auto& cut : m_cuts) {
    unsigned int pos = m_accept.getCutPosition(cut.first);
    if (m_accept.getCutResult(pos)) {
      m_numPassedCuts[pos]++;
    }
  }

  m_numProcessed++;
  if (m_accept) {
    m_numPassed++;
  }

  return m_accept;
}

StatusCode
TrackSelectionTool::finalize() {
  ATH_MSG_INFO("Finalizing " << name() << "...");

  if (m_numProcessed == 0) {
    ATH_MSG_INFO("No tracks processed in selection tool.");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO(m_numPassed << " / " << m_numProcessed << " = "
                           << m_numPassed * 100. / m_numProcessed << "% passed all cuts.");
  for (const auto& cut : m_cuts) {
    ULong64_t numPassed = m_numPassedCuts.at(m_accept.getCutPosition(cut.first));
    ATH_MSG_INFO(numPassed << " = " << numPassed * 100. / m_numProcessed << "% passed "
                           << cut.first << " cut.");
  }

  return StatusCode::SUCCESS;
}
