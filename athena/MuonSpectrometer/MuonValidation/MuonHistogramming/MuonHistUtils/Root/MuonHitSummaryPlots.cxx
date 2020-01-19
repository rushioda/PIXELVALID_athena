/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonHitSummaryPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{
  
MuonHitSummaryPlots::MuonHitSummaryPlots(PlotBase* pParent, std::string sDir):
PlotBase(pParent, sDir),

m_oMSHitPlots(this, ""),

innerSmallHits(this, "innersmallhits", "inner small hits", 0, 20),
innerLargeHits(this, "innerlargehits", "inner large hits", 0, 30),
middleSmallHits(this, "middlesmallhits", "middle small hits", 0, 20),
middleLargeHits(this, "middlelargehits", "middle large hits", 0, 20),
outerSmallHits(this, "outersmallhits", "outer small hits", 0, 15),
outerLargeHits(this, "outerlargehits", "outer large hits", 0, 20),
extendedSmallHits(this, "extendedsmallhits", "extended small hits", 0, 15),
extendedLargeHits(this, "extendedlargehits", "extended large hits", 0, 15),

phiLayer1Hits(this, "phiLayer1hits", "phi Layer 1 hits", 0, 10),
phiLayer2Hits(this, "phiLayer2hits", "phi Layer 2 hits", 0, 10),
phiLayer3Hits(this, "phiLayer3hits", "phi Layer 3 hits", 0, 10),
phiLayer4Hits(this, "phiLayer4hits", "phi Layer 4 hits", 0, 10),

etaLayer1Hits(this, "etaLayer1hits", "eta Layer 1 hits", 0, 10),
etaLayer2Hits(this, "etaLayer2hits", "eta Layer 2 hits", 0, 15), //more hits for this one
etaLayer3Hits(this, "etaLayer3hits", "eta Layer 3 hits", 0, 10),
etaLayer4Hits(this, "etaLayer4hits", "eta Layer 4 hits", 0, 10),

innerSmallHoles(this, "innersmallholes", "inner small holes", 0, 5),
innerLargeHoles(this, "innerlargeholes", "inner large holes", 0, 5),
middleSmallHoles(this, "middlesmallholes", "middle small holes", 0, 5),
middleLargeHoles(this, "middlelargeholes", "middle large holes", 0, 5),
outerSmallHoles(this, "outersmallholes", "outer small holes", 0, 5),
outerLargeHoles(this, "outerlargeholes", "outer large holes", 0, 5),
extendedSmallHoles(this, "extendedsmallholes", "extended small holes", 0, 5),
extendedLargeHoles(this, "extendedlargeholes", "extended large holes", 0, 5),

phiHoles(this, "phiHoles", "phi holes", 0, 5),
etaHoles(this, "etaHoles", "eta holes", 0, 5),
etaphiHoles(this, "etaphiHoles", "eta+phi holes", 0, 5)

// phiLayer1Holes(this, "phiLayer1holes", "phi Layer 1 holes", 0, 5),
// phiLayer2Holes(this, "phiLayer2holes", "phi Layer 2 holes", 0, 5),
// phiLayer3Holes(this, "phiLayer3holes", "phi Layer 3 holes", 0, 5),
// phiLayer4Holes(this, "phiLayer4holes", "phi Layer 4 holes", 0, 5),

// etaLayer1Holes(this, "etaLayer1holes", "eta Layer 1 holes", 0, 5),
// etaLayer2Holes(this, "etaLayer2holes", "eta Layer 2 holes", 0, 5),
// etaLayer3Holes(this, "etaLayer3holes", "eta Layer 3 holes", 0, 5),
// etaLayer4Holes(this, "etaLayer4holes", "eta Layer 4 holes", 0, 5)

{  }	

 
void MuonHitSummaryPlots::fill(const xAOD::Muon& muon){

  const xAOD::TrackParticle *tp = (muon.muonType()==xAOD::Muon::SiliconAssociatedForwardMuon)? muon.trackParticle(xAOD::Muon::CombinedTrackParticle) : muon.trackParticle(xAOD::Muon::Primary);
  if (tp) m_oMSHitPlots.fill(*tp);
  
  fillPlot(innerSmallHits, xAOD::innerSmallHits, muon);
  fillPlot(innerLargeHits, xAOD::innerLargeHits, muon);
  fillPlot(middleSmallHits, xAOD::middleSmallHits, muon);
  fillPlot(middleLargeHits, xAOD::middleLargeHits, muon);
  fillPlot(outerSmallHits, xAOD::outerSmallHits, muon);
  fillPlot(outerLargeHits, xAOD::outerLargeHits, muon);
  fillPlot(extendedSmallHits, xAOD::extendedSmallHits, muon);
  fillPlot(extendedLargeHits, xAOD::extendedLargeHits, muon);
  fillPlot(phiLayer1Hits, xAOD::phiLayer1Hits, muon);
  fillPlot(phiLayer2Hits, xAOD::phiLayer2Hits, muon);
  fillPlot(phiLayer3Hits, xAOD::phiLayer3Hits, muon);
  fillPlot(phiLayer4Hits, xAOD::phiLayer4Hits, muon);
  fillPlot(etaLayer1Hits, xAOD::etaLayer1Hits, muon);
  fillPlot(etaLayer2Hits, xAOD::etaLayer2Hits, muon);
  fillPlot(etaLayer3Hits, xAOD::etaLayer3Hits, muon);
  fillPlot(etaLayer4Hits, xAOD::etaLayer4Hits, muon);

  fillPlot(innerSmallHoles, xAOD::innerSmallHoles, muon);
  fillPlot(innerLargeHoles, xAOD::innerLargeHoles, muon);
  fillPlot(middleSmallHoles, xAOD::middleSmallHoles, muon);
  fillPlot(middleLargeHoles, xAOD::middleLargeHoles, muon);
  fillPlot(outerSmallHoles, xAOD::outerSmallHoles, muon);
  fillPlot(outerLargeHoles, xAOD::outerLargeHoles, muon);
  fillPlot(extendedSmallHoles, xAOD::extendedSmallHoles, muon);
  fillPlot(extendedLargeHoles, xAOD::extendedLargeHoles, muon);

  int netaholes=0, nphiholes=0;
  uint8_t hitval = 0;
  if (muon.summaryValue(hitval,xAOD::etaLayer1Holes)) netaholes+=(int)hitval;
  if (muon.summaryValue(hitval,xAOD::etaLayer2Holes)) netaholes+=(int)hitval;
  if (muon.summaryValue(hitval,xAOD::etaLayer3Holes)) netaholes+=(int)hitval;
  if (muon.summaryValue(hitval,xAOD::etaLayer4Holes)) netaholes+=(int)hitval;

  if (muon.summaryValue(hitval,xAOD::phiLayer1Holes)) nphiholes+=(int)hitval;
  if (muon.summaryValue(hitval,xAOD::phiLayer2Holes)) nphiholes+=(int)hitval;
  if (muon.summaryValue(hitval,xAOD::phiLayer3Holes)) nphiholes+=(int)hitval;
  if (muon.summaryValue(hitval,xAOD::phiLayer4Holes)) nphiholes+=(int)hitval;

  etaHoles.fill(netaholes, muon.eta(), muon.phi());
  phiHoles.fill(nphiholes, muon.eta(), muon.phi());
  etaphiHoles.fill(netaholes+nphiholes, muon.eta(), muon.phi());    

  // fillPlot(phiLayer1Holes, xAOD::phiLayer1Holes, muon);
  // fillPlot(phiLayer2Holes, xAOD::phiLayer2Holes, muon);
  // fillPlot(phiLayer3Holes, xAOD::phiLayer3Holes, muon);
  // fillPlot(phiLayer4Holes, xAOD::phiLayer4Holes, muon);
  // fillPlot(etaLayer1Holes, xAOD::etaLayer1Holes, muon);
  // fillPlot(etaLayer2Holes, xAOD::etaLayer2Holes, muon);
  // //fillPlot(etaLayer3Holes, xAOD::etaLayer3Holes, muon);
  // fillPlot(etaLayer4Holes, xAOD::etaLayer4Holes, muon);

}

void MuonHitSummaryPlots::fill(const xAOD::TruthParticle& truthprt){

  m_oMSHitPlots.fill(truthprt);
  
  fillPlot(innerSmallHits, "innerSmallHits", truthprt);
  fillPlot(innerLargeHits, "innerLargeHits", truthprt);
  fillPlot(middleSmallHits, "middleSmallHits", truthprt);
  fillPlot(middleLargeHits, "middleLargeHits", truthprt);
  fillPlot(outerSmallHits, "outerSmallHits", truthprt);
  fillPlot(outerLargeHits, "outerLargeHits", truthprt);
  fillPlot(extendedSmallHits, "extendedSmallHits", truthprt);
  fillPlot(extendedLargeHits, "extendedLargeHits", truthprt);
  fillPlot(phiLayer1Hits, "phiLayer1Hits", truthprt);
  fillPlot(phiLayer2Hits, "phiLayer2Hits", truthprt);
  fillPlot(phiLayer3Hits, "phiLayer3Hits", truthprt);
  fillPlot(phiLayer4Hits, "phiLayer4Hits", truthprt);
  fillPlot(etaLayer1Hits, "etaLayer1Hits", truthprt);
  fillPlot(etaLayer2Hits, "etaLayer2Hits", truthprt);
  fillPlot(etaLayer3Hits, "etaLayer3Hits", truthprt);
  fillPlot(etaLayer4Hits, "etaLayer4Hits", truthprt);
}

void MuonHitSummaryPlots::fillPlot(Trk::HitTypePlots& hitPlots, xAOD::MuonSummaryType info, const xAOD::Muon& muon){
  uint8_t hitval = 0;
  if (!muon.summaryValue(hitval,info)) return;
  hitPlots.fill(hitval, muon.eta(), muon.phi());
}

void MuonHitSummaryPlots::fillPlot(Trk::HitTypePlots& hitPlots, const std::string& sInfo, const xAOD::TruthParticle& truthprt){
  if (!truthprt.isAvailable<uint8_t>(sInfo)) return;
  hitPlots.fill(truthprt.auxdata<uint8_t>(sInfo), truthprt.eta(), truthprt.phi());
}


}
