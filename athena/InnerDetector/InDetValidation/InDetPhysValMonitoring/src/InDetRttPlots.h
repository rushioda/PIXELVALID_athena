/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETRTTPLOTS
#define INDETPHYSVALMONITORING_INDETRTTPLOTS
/**
 * @file InDetRttPlots.h
 * @author shaun roe
 * @date 29/3/2014
 **/


// std includes
#include <string>
// local includes
#include "InDetPlotBase.h"
#include "InDetPerfPlot_Pt.h"
#include "InDetBasicPlot.h"

#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "InDetPerfPlot_res.h"
#include "InDetPerfPlot_fakes.h"
#include "InDetPerfPlot_Eff.h"
#include "InDetPerfPlot_hitResidual.h"
#include "InDetPerfPlot_hitEff.h"
#include "InDetPerfPlot_spectrum.h"
#include "InDetPerfPlot_duplicate.h"

#include "InDetDummyPlots.h"
#include "InDet_BadMatchRate.h"

#include "TrkValHistUtils/IDHitPlots.h"
#include "InDetPerfPlot_HitDetailed.h"
#include "InDetPerfPlot_Vertex.h"
#include "InDetPerfPlot_VertexContainer.h"

#include "InDetPerfPlot_TrkInJet.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "InDetPerfPlot_resITk.h"

///class holding all plots for Inner Detector RTT Validation and implementing fill methods
class InDetRttPlots: public InDetPlotBase {
public:
  InDetRttPlots(InDetPlotBase* pParent, const std::string& dirName);
  void
  SetFillExtraTIDEPlots(bool fillthem) {
    m_moreJetPlots = fillthem;
  }

  ///fill for things needing truth and track only
  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle);
  ///fill for things needing track only
  void fill(const xAOD::TrackParticle& particle);
  ///fill for things needing truth only
  void fill(const xAOD::TruthParticle& particle);
  ///Fill for efficiency plots
  void fillEfficiency(const xAOD::TruthParticle& truth, const bool isGood, unsigned int nMuEvents);

  void fillSpectrum(const xAOD::TrackParticle& trackParticle);
  void fillSpectrum(const xAOD::TruthParticle& particle);
  void fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::TruthVertex& truthVrt);
  void fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::Vertex& vertex);
  void fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::Vertex& vertex, bool fill);
  void fillSpectrumLinked(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle, float weight);
  void fillLinkedandUnlinked(const xAOD::TrackParticle& particle, float Prim_w, float Sec_w, float Unlinked_w,unsigned int nMuEvents);
  void fillSpectrumUnlinked2(const xAOD::TrackParticle& particle);
  void fillSingleMatch(const xAOD::TrackParticle& trackParticle);
  void fillTwoMatchDuplicate(Float_t prob1, Float_t prob2, const xAOD::TrackParticle& trackParticle,
                             const xAOD::TrackParticle& particle, const xAOD::TruthParticle& tp);
  ///fill for things needing all truth - not just the ones from the reco tracks
  
  //Track Author Plots  - M. Baugh
  void algoEfficiency(double radius, int SiSPweight, int TRTSeededweight, int TRTStandaloneweight, int other_weight);
  void track_author(std::bitset<52> authorset);
  
  void lepton_fill(const xAOD::TruthParticle& truth, float weight);
  void prim_photon_fill(const xAOD::TruthParticle& truth);
  void brem_photon_fill(const xAOD::TruthParticle& truth);
  void track_vs_truth(const xAOD::TrackParticle& track, const xAOD::TruthParticle& truth, float tmp);
  void minDR(float min_dR, float prod_rad, float bestmatch, double BIDPt);
  void BT_fill(const xAOD::TruthParticle& truth, float weight);

  // fill the fake and bad match rate plots
  void fillBMR(const xAOD::TrackParticle& track, float weight);
  void fillRF(const xAOD::TrackParticle& track, float weight);

  ///fill reco-vertex related plots
  void fill(const xAOD::VertexContainer& vertexContainer);
  ///fill reco-vertex related plots that need EventInfo
  void fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei);

  // New set has replaced fillJetPlot
  bool filltrkInJetPlot(const xAOD::TrackParticle& particle, const xAOD::Jet& jet);
  void fillSimpleJetPlots(const xAOD::TrackParticle& particle, float prob);
  void fillJetHitsPlots(const xAOD::TrackParticle& particle, float prob, int barcode);
  void fillJetResPlots(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truth, const xAOD::Jet& jet);
  void fillJetEffPlots(const xAOD::TruthParticle& truth, const xAOD::Jet& jet);

  void jet_fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet, float weight);
  void jetBMR(const xAOD::TrackParticle& track, const xAOD::Jet& jet, float weight);

  void fillJetPlotCounter(const xAOD::Jet& jet);
  void fillJetTrkTruth(const xAOD::TruthParticle& truth, const xAOD::Jet& jet);
  void fillJetTrkTruthCounter(const xAOD::Jet& jet);

  virtual ~InDetRttPlots() {/**nop**/
  };
  ///fill for Counters
  void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter);
  ///fill for fakes
  void fillFakeRate(const xAOD::TrackParticle& particle, const bool match,
                    const InDetPerfPlot_fakes::Category c = InDetPerfPlot_fakes::ALL);
  //void fillIncTrkRate(const unsigned int nMuEvents, std::vector<int> incTrkNum, std::vector<int> incTrkDenom);
  void fillFakeRateLinked(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle);
  void fillFakeRateUnlinked(const xAOD::TrackParticle& particle);
private:
  InDetPerfPlot_Pt m_ptPlot;
  InDetBasicPlot m_basicPlot;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots;
  Trk::TruthInfoPlots m_TrackTruthInfoPlots;
  InDetPerfPlot_nTracks m_nTracks;
  InDetPerfPlot_res m_resPlots;
  InDetPerfPlot_hitResidual m_hitResidualPlot;
  InDetPerfPlot_hitEff m_hitEffPlot;
  InDetPerfPlot_fakes m_fakePlots; // fakes vs eta etc, as per original RTT code

  // ITk resolutions
  InDetPerfPlot_resITk* m_ITkResolutionPlotPrim;
  InDetPerfPlot_resITk* m_ITkResolutionPlotSecd;

  InDetPerfPlot_HitDetailed m_hitsMatchedTracksPlots;
  InDetPerfPlot_HitDetailed m_hitsDetailedPlots;
  InDetPerfPlot_Eff m_effPlots;
  InDetDummyPlots m_dumPlots;
  InDet_BadMatchRate m_BadMatchRate;

  InDetPerfPlot_VertexContainer m_verticesPlots;
  InDetPerfPlot_Vertex m_vertexPlots;
  InDetPerfPlot_Vertex m_hardScatterVertexPlots;

  InDetPerfPlot_duplicate m_duplicatePlots;

  bool m_moreJetPlots;
  bool m_ITkResPlots;
  InDetPerfPlot_TrkInJet m_trkInJetPlot;
  InDetPerfPlot_TrkInJet m_trkInJetPlot_highPt;
  InDetPerfPlot_Pt m_trkInJetPtPlot;
  InDetBasicPlot m_trkInJetBasicPlot;
  // Trk::ParamPlots m_trkInJetPtEtaPlots;
  // Trk::ImpactPlots m_trkInJetIPPlots;
  Trk::RecoInfoPlots m_trkInJetTrackRecoInfoPlots;
  InDetPerfPlot_HitDetailed m_trkInJetHitsDetailedPlots;
  InDetPerfPlot_fakes m_trkInJetFakePlots; // fakes vs eta etc, as per original RTT code
  InDetPerfPlot_res m_trkInJetResPlots;
  InDetPerfPlot_res* m_trkInJetResPlotsDr0010;
  InDetPerfPlot_res* m_trkInJetResPlotsDr1020;
  InDetPerfPlot_res* m_trkInJetResPlotsDr2030;
  InDetPerfPlot_Eff m_trkInJetEffPlots;
  InDetPerfPlot_res m_trkInJetHighPtResPlots;
  InDetPerfPlot_HitDetailed m_trkInJetHitsFakeTracksPlots;
  InDetPerfPlot_HitDetailed m_trkInJetHitsMatchedTracksPlots;
  Trk::TruthInfoPlots m_trkInJetTrackTruthInfoPlots;
  InDetPerfPlot_spectrum m_specPlots;

  std::string m_trackParticleTruthProbKey;
  float m_truthProbLowThreshold;
};




#endif
