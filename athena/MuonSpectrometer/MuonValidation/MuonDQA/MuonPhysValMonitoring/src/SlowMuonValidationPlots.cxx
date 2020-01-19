/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SlowMuonValidationPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"

#include <iostream>

typedef ElementLink< xAOD::MuonContainer > MuonLink;

SlowMuonValidationPlots::SlowMuonValidationPlots(PlotBase* pParent, std::string sDir, bool isData):
  PlotBase(pParent, sDir),  m_isData(isData)
{    
  //histogram classes for all muons
  m_oSlowMuonRecoPlots = new Muon::SlowMuonParamPlots(this, "reco/AllMuons/parameters/");
  if (!m_isData) m_oSlowMuonMatchedPlots = new Muon::SlowMuonParamPlots(this, "matched/AllMuons/parameters/");
}

SlowMuonValidationPlots::~SlowMuonValidationPlots()
{  
  delete m_oSlowMuonRecoPlots;
  m_oSlowMuonRecoPlots = 0;

  if (!m_isData) {
    delete m_oSlowMuonMatchedPlots;
    m_oSlowMuonMatchedPlots = 0;
  }  
}

void SlowMuonValidationPlots::fill(const xAOD::SlowMuon& smu, const xAOD::Muon& mu)
{
  fillRecoMuonPlots(smu,mu);
}
void SlowMuonValidationPlots::fill( const xAOD::TruthParticle* truthMu, const xAOD::SlowMuon* smu, const xAOD::Muon* mu )
{
  if (smu) fillRecoMuonPlots(*smu,*mu);
  if ( (smu) && (truthMu) ) {
    m_oSlowMuonMatchedPlots->fill(*smu);
  }
}

void SlowMuonValidationPlots::fillRecoMuonPlots(const xAOD::SlowMuon& smu, const xAOD::Muon& /*mu*/)
{
  //fill hists for all muons
  m_oSlowMuonRecoPlots->fill(smu);
}

