/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/RecoMuonTrackPlotOrganizer.h"

namespace Muon{
  
RecoMuonTrackPlotOrganizer::RecoMuonTrackPlotOrganizer(PlotBase* pParent, std::string sDir, std::vector<int> *selPlots):
  PlotBase(pParent, sDir)
  // Reco plots
  , m_oTrkParamPlots(NULL)
  , m_oTrkRecoInfoPlots(NULL)
  , m_oImpactPlots(NULL)
  , m_oMSHitPlots(NULL)
  , m_oIDHitPlots(NULL)
{
  
  if (!selPlots) {
    m_selPlots.clear();
    for (unsigned int i=0; i<MAX_RECOTRKPLOTCLASS; i++) m_selPlots.push_back(i);
  }
  else m_selPlots = *selPlots;
    
  for (auto p: m_selPlots) {
    switch (p) {
    case TRK_PARAM:
      m_oTrkParamPlots = new Trk::ParamPlots(this, "/kinematics/", "Reco Muon");
      m_allPlots.push_back(m_oTrkParamPlots);
      break;
    case TRK_RECOINFO:
      m_oTrkRecoInfoPlots = new Trk::RecoInfoPlots(this, "/parameters/", "Reco Muon");
      m_allPlots.push_back(m_oTrkParamPlots);
      break;      
    case TRK_IMPACT:
      m_oImpactPlots = new Trk::ImpactPlots(this, "/parameters/");
      m_allPlots.push_back(m_oImpactPlots);
      break;
    case TRK_MSHITS:
      m_oMSHitPlots = new Trk::MSHitPlots(this,"/hits/");
      m_allPlots.push_back(m_oMSHitPlots);
      break;    
    case TRK_IDHITS:
      m_oIDHitPlots = new Muon::IDHitSummaryPlots(this,"/hits/");
      m_allPlots.push_back(m_oIDHitPlots);
      break;
    }
  }
}
  
RecoMuonTrackPlotOrganizer::~RecoMuonTrackPlotOrganizer()
{
  for (auto plots: m_allPlots)  delete plots;
  m_allPlots.clear();
}
  
void RecoMuonTrackPlotOrganizer::fill(const xAOD::TrackParticle& tp) {
  if (m_oTrkParamPlots) m_oTrkParamPlots->fill(tp);
  if (m_oTrkRecoInfoPlots) m_oTrkRecoInfoPlots->fill(tp);
  if (m_oImpactPlots) m_oImpactPlots->fill(tp);  
  if (m_oMSHitPlots) m_oMSHitPlots->fill(tp);
  if (m_oIDHitPlots) m_oIDHitPlots->fill(tp);
}

}
