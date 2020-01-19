/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonHitDiffSummaryPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{

MuonHitDiffSummaryPlots::MuonHitDiffSummaryPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir), 
innerSmallHits(this, "diff_innersmallhits", "diff(reco - truth) inner small hits", -10, 10),
innerLargeHits(this, "diff_innerlargehits", "diff(reco - truth) inner large hits", -10, 10),
middleSmallHits(this, "diff_middlesmallhits", "diff(reco - truth) middle small hits", -10, 10),
middleLargeHits(this, "diff_middlelargehits", "diff(reco - truth) middle large hits", -10, 10),
outerSmallHits(this, "diff_outersmallhits", "diff(reco - truth) outer small hits", -10, 10),
outerLargeHits(this, "diff_outerlargehits", "diff(reco - truth) outer large hits", -10, 10),
extendedSmallHits(this, "diff_extendedsmallhits", "diff(reco - truth) extended small hits", -10, 10),
extendedLargeHits(this, "diff_extendedlargehits", "diff(reco - truth) extended large hits", -10, 10),

phiLayer1Hits(this, "diff_phiLayer1hits", "diff(reco - truth) phi Layer 1 hits", -10, 10),
phiLayer2Hits(this, "diff_phiLayer2hits", "diff(reco - truth) phi Layer 2 hits", -10, 10),
phiLayer3Hits(this, "diff_phiLayer3hits", "diff(reco - truth) phi Layer 3 hits", -10, 10),
phiLayer4Hits(this, "diff_phiLayer4hits", "diff(reco - truth) phi Layer 4 hits", -10, 10),

etaLayer1Hits(this, "diff_etaLayer1hits", "diff(reco - truth) eta Layer 1 hits", -10, 10),
etaLayer2Hits(this, "diff_etaLayer2hits", "diff(reco - truth) eta Layer 2 hits", -10, 10),
etaLayer3Hits(this, "diff_etaLayer3hits", "diff(reco - truth) eta Layer 3 hits", -10, 10),
etaLayer4Hits(this, "diff_etaLayer4hits", "diff(reco - truth) eta Layer 4 hits", -10, 10)
{  
}	

void MuonHitDiffSummaryPlots::fill(const xAOD::Muon& muon, const xAOD::TruthParticle& truthprt) {
  fillPlot(innerSmallHits, xAOD::innerSmallHits, muon, "innerSmallHits", truthprt);
  fillPlot(innerLargeHits, xAOD::innerLargeHits, muon, "innerLargeHits", truthprt);
  fillPlot(middleSmallHits, xAOD::middleSmallHits, muon, "middleSmallHits", truthprt);
  fillPlot(middleLargeHits, xAOD::middleLargeHits, muon, "middleLargeHits", truthprt);
  fillPlot(outerSmallHits, xAOD::outerSmallHits, muon, "outerSmallHits", truthprt);
  fillPlot(outerLargeHits, xAOD::outerLargeHits, muon, "outerLargeHits", truthprt);
  fillPlot(extendedSmallHits, xAOD::extendedSmallHits, muon, "extendedSmallHits", truthprt);
  fillPlot(extendedLargeHits, xAOD::extendedLargeHits, muon, "extendedLargeHits", truthprt);
  fillPlot(phiLayer1Hits, xAOD::phiLayer1Hits, muon, "phiLayer1Hits", truthprt);
  fillPlot(phiLayer2Hits, xAOD::phiLayer2Hits, muon, "phiLayer2Hits", truthprt);
  fillPlot(phiLayer3Hits, xAOD::phiLayer3Hits, muon, "phiLayer3Hits", truthprt);
  fillPlot(phiLayer4Hits, xAOD::phiLayer4Hits, muon, "phiLayer4Hits", truthprt);
  fillPlot(etaLayer1Hits, xAOD::etaLayer1Hits, muon, "etaLayer1Hits", truthprt);
  fillPlot(etaLayer2Hits, xAOD::etaLayer2Hits, muon, "etaLayer2Hits", truthprt);
  fillPlot(etaLayer3Hits, xAOD::etaLayer3Hits, muon, "etaLayer3Hits", truthprt);
  fillPlot(etaLayer4Hits, xAOD::etaLayer4Hits, muon, "etaLayer4Hits", truthprt);
}

void MuonHitDiffSummaryPlots::fillPlot(Trk::HitTypePlots& hitPlots, xAOD::MuonSummaryType info, const xAOD::Muon& muon, std::string sInfo, const xAOD::TruthParticle& truthprt){
  uint8_t hitval = 0;
  if (!muon.summaryValue(hitval,info)) return;
  if (!truthprt.isAvailable<uint8_t>(sInfo)) return;
  hitPlots.fill(hitval - truthprt.auxdata<uint8_t>(sInfo), muon.eta(), muon.phi());
}

}
