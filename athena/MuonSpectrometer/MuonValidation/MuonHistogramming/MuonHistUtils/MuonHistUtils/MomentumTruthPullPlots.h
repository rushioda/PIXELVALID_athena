/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MOMENTUMTRUTHPULLPLOTS_H
#define MUONHISTUTILS_MOMENTUMTRUTHPULLPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

namespace Muon{

class MomentumTruthPullPlots:public PlotBase {
 public:
  MomentumTruthPullPlots(PlotBase* pParent, std::string sDir, std::string sType);
  ~MomentumTruthPullPlots();

  std::string m_sType;
  
  TH1* dp_CB_truthIP;
  TH1* dp_ME_truthIP;
  TH1* dp_ME_truthIP_MS_truthMS;
  TH1* dp_eloss;

  TH2* dp_CB_truthIP_vs_pt;
  TH2* dp_ME_truthIP_vs_pt;
  TH2* dp_ME_truthIP_MS_truthMS_vs_pt;
  TH2* dp_eloss_vs_pt;

  TH2* dp_CB_truthIP_vs_eta;
  TH2* dp_ME_truthIP_vs_eta;
  TH2* dp_ME_truthIP_MS_truthMS_vs_eta;
  TH2* dp_eloss_vs_eta;

  
  void fill(const xAOD::Muon& muon, const xAOD::TrackParticle* muontpIP, const xAOD::TruthParticle& truthprt);
  void initializePlots();
};

 
}//namespace Muon

#endif
