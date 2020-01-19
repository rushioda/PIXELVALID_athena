/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonPlots.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
using CLHEP::GeV;

namespace Egamma{


PhotonPlots::PhotonPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
											 m_oKinAllPlots(this, "All/KinPlots/", "Reco " + sParticleType +" Photon"), 
											 m_oShowerShapesAllPlots(this, "All/ShowerShapesPlots/", sParticleType  +" Photon"),
//
											 m_oKinIsoPlots(this, "Iso/KinPlots/", "Reco " + sParticleType +" Photon"),
											 m_oShowerShapesIsoPlots(this, "Iso/ShowerShapesPlots/", sParticleType  +" Photon"),
											 m_oKinIsoUncPlots(this, "Iso/KinPlotsUnc/", "Reco " + sParticleType +" Photon"),
											 m_oKinIsoConvPlots(this, "Iso/KinPlotsConv/", "Reco " + sParticleType +" Photon"),
											 //
											 m_oKinIsoLoosePlots(this, "IsoLoose/KinPlots/", "Reco " + sParticleType +" Photon"),
											 m_oKinIsoLooseUncPlots(this, "IsoLoose/KinPlotsUnc/", "Reco " + sParticleType +" Photon"),
											 m_oKinIsoLooseConvPlots(this, "IsoLoose/KinPlotsConv/", "Reco " + sParticleType +" Photon"),
											 //
											 m_oKinIsoTightPlots(this, "IsoTight/KinPlots/", "Reco " + sParticleType +" Photon"),
											 m_oKinIsoTightUncPlots(this, "IsoTight/KinPlotsUnc/", "Reco " + sParticleType +" Photon"),
											 m_oKinIsoTightConvPlots(this, "IsoTight/KinPlotsConv/", "Reco " + sParticleType +" Photon"),
											 m_sParticleType(sParticleType),
											 m_nParticles(0)
{}	

void PhotonPlots::initializePlots(){
  m_nParticles = Book1D("n", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + "s;Events", 15, 0., 15.);
}

void PhotonPlots::fill(const xAOD::Photon& photon, bool isPrompt){
  m_oKinAllPlots.fill(photon);
  m_oShowerShapesAllPlots.fill(photon);
  
  if(!isPrompt) return;

  m_oKinIsoPlots.fill(photon);
  m_oShowerShapesIsoPlots.fill(photon);
   if (!xAOD::EgammaHelpers::isConvertedPhoton(&photon)) m_oKinIsoUncPlots.fill(photon);
   else m_oKinIsoConvPlots.fill(photon);
  
  bool val_loose=false;    

  photon.passSelection(val_loose, "Loose");
  if(val_loose) {
    m_oKinIsoLoosePlots.fill(photon);
    if (!xAOD::EgammaHelpers::isConvertedPhoton(&photon)) m_oKinIsoLooseUncPlots.fill(photon);
    else m_oKinIsoLooseConvPlots.fill(photon);
  }
  
  bool val_tight=false;    

  photon.passSelection(val_tight, "Tight");
  if(val_tight) {
    m_oKinIsoTightPlots.fill(photon);
    if (!xAOD::EgammaHelpers::isConvertedPhoton(&photon)) m_oKinIsoTightUncPlots.fill(photon);
    else m_oKinIsoTightConvPlots.fill(photon);
  }  
}

}
