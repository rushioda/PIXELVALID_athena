/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "METPlots.h"

using CLHEP::GeV;

namespace PhysVal{

  METPlots::METPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
m_sParticleType(sParticleType)
{}	
  
void METPlots::initializePlots(){
  met_ex  = Book1D("Ex", "MET " + m_sParticleType + " Ex; E_{x}^{miss} ;Events", 100, 0., 400);
  met_ey  = Book1D("Ey", "MET " + m_sParticleType + " Ey; E_{y}^{miss} ;Events", 100, 0., 400);
  met_et  = Book1D("Et", "MET " + m_sParticleType + " Et; E_{t}^{miss} ;Events", 100, 0., 400);
  met_sumet  = Book1D("SumEt", "MET " + m_sParticleType + " Ex; #Sum E_{t} ;Events", 100, 0., 2000);
}

void METPlots::fill(const xAOD::MissingET* met){

  met_ex->Fill(met->mpx()/GeV);
  met_ey->Fill(met->mpy()/GeV);
  met_et->Fill(met->met()/GeV);
  met_sumet->Fill(met->sumet()/GeV);

}
}
