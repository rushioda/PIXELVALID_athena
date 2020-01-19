/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonIsolationPlots.h"

namespace Muon{

MuonIsolationPlots::MuonIsolationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
//iso variables
m_oPtCone20(this,"", "ptcone20"),
m_oPtCone30(this,"", "ptcone30"),
m_oPtCone40(this,"", "ptcone40"),
									    
m_oEtCone20(this,"", "etcone20"),
m_oEtCone30(this,"", "etcone30"),
m_oEtCone40(this,"", "etcone40"),

m_oTopoEtCone20(this,"", "topoetcone20"),
m_oTopoEtCone30(this,"", "topoetcone30"),
m_oTopoEtCone40(this,"", "topoetcone40"),

m_oNEFlowIso20(this,"", "neflowisol20"),
m_oNEFlowIso30(this,"", "neflowisol30"),
m_oNEFlowIso40(this,"", "neflowisol40"),

m_oPtVarCone20(this,"", "ptvarcone20"),
m_oPtVarCone30(this,"", "ptvarcone30"),
m_oPtVarCone40(this,"", "ptvarcone40")

#ifndef XAOD_ANALYSIS
  //iso corrections
, m_oEtCone_coreCone(this,"","etcone_coreCone")
, m_oTopoEtCone_coreCone(this,"","topoetcone_coreCone")
, m_oNEFlowIso_coreCone(this,"","neflowisol_coreCone")
, m_oEtCone_coreMuon(this,"","etcone_coreMuon")
#endif // not XAOD_ANALYSIS
{}	

void MuonIsolationPlots::fill(const xAOD::Muon& muon)
{
  m_oPtCone20.fill(muon, xAOD::Iso::ptcone20);
  m_oPtCone30.fill(muon, xAOD::Iso::ptcone30);
  m_oPtCone40.fill(muon, xAOD::Iso::ptcone40);

  m_oEtCone20.fill(muon, xAOD::Iso::etcone20);
  m_oEtCone30.fill(muon, xAOD::Iso::etcone30);
  m_oEtCone40.fill(muon, xAOD::Iso::etcone40);

  m_oTopoEtCone20.fill(muon, xAOD::Iso::topoetcone20);
  m_oTopoEtCone30.fill(muon, xAOD::Iso::topoetcone30);
  m_oTopoEtCone40.fill(muon, xAOD::Iso::topoetcone40);
#ifndef XAOD_ANALYSIS
  m_oNEFlowIso20.fill(muon, xAOD::Iso::neflowisol20);
  m_oNEFlowIso30.fill(muon, xAOD::Iso::neflowisol30);
  m_oNEFlowIso40.fill(muon, xAOD::Iso::neflowisol40);
  
  m_oPtVarCone20.fill(muon, xAOD::Iso::ptvarcone20);
  m_oPtVarCone30.fill(muon, xAOD::Iso::ptvarcone30);
  m_oPtVarCone40.fill(muon, xAOD::Iso::ptvarcone40);

  m_oEtCone_coreCone.fill(muon, xAOD::Iso::etcone20, xAOD::Iso::etcone30, xAOD::Iso::etcone40, xAOD::Iso::etcone, xAOD::Iso::coreCone, xAOD::Iso::coreEnergy);
  m_oTopoEtCone_coreCone.fill(muon, xAOD::Iso::topoetcone20, xAOD::Iso::topoetcone30, xAOD::Iso::topoetcone40, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::coreEnergy);
  m_oNEFlowIso_coreCone.fill(muon, xAOD::Iso::neflowisol20, xAOD::Iso::neflowisol30, xAOD::Iso::neflowisol40, xAOD::Iso::neflowisol, xAOD::Iso::coreCone, xAOD::Iso::coreEnergy);
  m_oEtCone_coreMuon.fill(muon, xAOD::Iso::etcone20, xAOD::Iso::etcone30, xAOD::Iso::etcone40, xAOD::Iso::etcone, xAOD::Iso::coreMuon, xAOD::Iso::coreEnergy);
#endif // not XAOD_ANALYSIS

}

  
IsoPlots::IsoPlots(PlotBase* pParent, std::string sDir, std::string sConeSize):PlotBase(pParent, sDir),
m_sConeSize(sConeSize), cone(NULL), conerel(NULL)
{}

void IsoPlots::initializePlots()
{
  cone    = Book1D(m_sConeSize, m_sConeSize + ";" + m_sConeSize + ";Entries", 60, -5., 25.);
  conerel = Book1D(m_sConeSize + "rel", m_sConeSize + "rel;" + m_sConeSize + "rel;Entries", 40, 0., 2.);
}
  
void IsoPlots::fill(const xAOD::Muon& muon, const xAOD::Iso::IsolationType &isoType)
{
  float fIso = 0;
  try{
    muon.isolation(fIso, isoType);
  }
  catch(SG::ExcBadAuxVar&){
    return;
  }
  if (fIso) fill(fIso, muon.pt());
}
void IsoPlots::fill(float fIso, float fPt)
{
  cone->Fill(fIso*0.001);
  conerel->Fill(fIso/fPt);
}


#ifndef XAOD_ANALYSIS
IsoCorrPlots::IsoCorrPlots(PlotBase* pParent, std::string sDir, std::string sCorrType):PlotBase(pParent, sDir),
m_sCorrType(sCorrType), isocorr(NULL), isocorr_relPt(NULL), isocorr_relIsocone20(NULL), isocorr_relIsocone30(NULL), isocorr_relIsocone40(NULL)
{}

void IsoCorrPlots::initializePlots()
{
  isocorr = Book1D(m_sCorrType, m_sCorrType + ";" + m_sCorrType + ";Entries", 60, -5., 25.);
  isocorr_relPt = Book1D(m_sCorrType + "_relPt", m_sCorrType + "_relPt;" + m_sCorrType+ "_relPt;Entries", 40, 0., 2.);
  isocorr_relIsocone20 = Book1D(m_sCorrType + "_relIsocone20", m_sCorrType + "_relIsocone20;" + m_sCorrType+ "_relIsocone20;Entries", 150, -25., 25.);
  isocorr_relIsocone30 = Book1D(m_sCorrType + "_relIsocone30", m_sCorrType + "_relIsocone30;" + m_sCorrType+ "_relIsocone30;Entries", 150, -25., 25.);
  isocorr_relIsocone40 = Book1D(m_sCorrType + "_relIsocone40", m_sCorrType + "_relIsocone40;" + m_sCorrType+ "_relIsocone40;Entries", 150, -25., 25.);
}


void IsoCorrPlots::fill(const xAOD::Muon& muon,
			const xAOD::Iso::IsolationType &isoType_cone20,
			const xAOD::Iso::IsolationType &isoType_cone30,
			const xAOD::Iso::IsolationType &isoType_cone40,
			const xAOD::Iso::IsolationFlavour& flavour,
			const xAOD::Iso::IsolationCaloCorrection &isoCorrType,
			const xAOD::Iso::IsolationCorrectionParameter& isoCorrParam)
{
    float fIso20 = 0;
    float fIso30 = 0;
    float fIso40 = 0;
    float fIsoCorr = 0;

    try{
      muon.isolation(fIso20, isoType_cone20);
      muon.isolation(fIso30, isoType_cone30);
      muon.isolation(fIso40, isoType_cone40);
      muon.isolationCaloCorrection(fIsoCorr, flavour, isoCorrType, isoCorrParam);      
    }
    catch(SG::ExcBadAuxVar&){
      return;
    }
    if (fIso20 && fIso30 && fIso40)
      fill(fIso20, fIso30, fIso40, muon.pt(), fIsoCorr);
}

void IsoCorrPlots::fill(float fIso20, float fIso30, float fIso40, float fPt, float fIsoCorr)
{
  isocorr->Fill(fIsoCorr*0.001);
  isocorr_relPt->Fill(fIsoCorr/fPt);
  isocorr_relIsocone20->Fill(fIsoCorr/fIso20);
  isocorr_relIsocone30->Fill(fIsoCorr/fIso30);
  isocorr_relIsocone40->Fill(fIsoCorr/fIso40);
}
#endif // not XAOD_ANALYSIS

}//namespace Muon
