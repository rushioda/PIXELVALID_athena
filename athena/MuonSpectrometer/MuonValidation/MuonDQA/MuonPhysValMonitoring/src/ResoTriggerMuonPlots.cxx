/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ResoTriggerMuonPlots.h"

namespace Muon{

ResoTriggerMuonPlots::ResoTriggerMuonPlots(PlotBase* pParent, std::string sDir, std::string sType):
  PlotBase(pParent, sDir),
  Res_pT(NULL),
  Res_eta(NULL),
  Res_phi(NULL),
  Res_pT_vs_pT(NULL),
  Res_eta_vs_pT(NULL),
  Res_phi_vs_pT(NULL),
  m_sType(sType)
{
  pt_slices = {"0","25","55","100"};
  etaBins= {-3., -2.5, -2.4, -1.918, -1.623, -1.348, -1.2329, -1.1479, -1.05, -0.908, -0.791, -0.652, -0.476, -0.324, -0.132, 0, 0.132, 0.324, 0.476, 0.652, 0.791, 0.908, 1.05, 1.1479, 1.2329, 1.348, 1.623, 1.918, 2.4, 2.5, 3.};

  Res_pT     = Book1D("Res"+m_sType+"_pT","Res"+m_sType+"_pT;(1/pT-1/RECOpT)/(1/RECOpT);Entries",200,-0.25,0.25);
  Res_eta    = Book1D("Res"+m_sType+"_eta","Res"+m_sType+"_eta;eta-RECOeta;Entries",200,-0.02,0.02);
  Res_phi    = Book1D("Res"+m_sType+"_phi","Res"+m_sType+"_phi;phi-RECOphi;Entries",200,-0.005,0.005);

  Res_pT_vs_pT  = Book2D("Res"+m_sType+"_pT_vs_pT","Res"+m_sType+"_pT vs pT;pT [GeV];(1/pT-1/RECOpT)/(1/RECOpT)",
			   150,0., 150.,  100,-0.25,0.25);
  Res_eta_vs_pT  = Book2D("Res"+m_sType+"_eta_vs_pT","Res"+m_sType+"_eta vs pT;pT [GeV];eta-RECOeta",
			   150,0., 150.,  200,-0.02,0.02);
  Res_phi_vs_pT  = Book2D("Res"+m_sType+"_phi_vs_pT","Res"+m_sType+"_phi vs pT;pT [GeV];phi-RECOphi",
                                 150,0., 150.,  200,-0.005,0.005);
  Res_pT_vs_eta.clear();
  Res_pT_vs_phi.clear();
  Res_phi_vs_phi.clear();
  Res_eta_vs_eta.clear();

  for (unsigned int i=0; i<pt_slices.size()-1;i++){
     
     Res_pT_vs_eta.push_back(Book2D("Res"+m_sType+"_pT_vs_eta_from"+pt_slices[i]+"_to"+pt_slices[i+1]+"GeV",
                                    "Res"+m_sType+"_pT vs eta from "+pt_slices[i]+" GeV to "+pt_slices[i+1]+" GeV; eta;(1/pT-1/RECOpT)/(1/RECOpT)",
                                    etaBins.size()-1 ,&etaBins[0], 100,-0.25,0.25));
     Res_pT_vs_phi.push_back(Book2D("Res"+m_sType+"_pT_vs_phi_from"+pt_slices[i]+"_to"+pt_slices[i+1]+"GeV",
                                    "Res"+m_sType+"_pT vs phi from "+pt_slices[i]+" GeV to "+pt_slices[i+1]+" GeV;phi;(1/pT-1/RECOpT)/(1/RECOpT)",
                                    96,-3.14159,3.14159, 100,-0.25,0.25));	
     Res_phi_vs_phi.push_back(Book2D("Res"+m_sType+"_phi_vs_phi_from"+pt_slices[i]+"_to"+pt_slices[i+1]+"GeV",
                                     "Res"+m_sType+"_phi vs phi from "+pt_slices[i]+" GeV to "+pt_slices[i+1]+" GeV;phi;(1/pT-1/RECOpT)/(1/RECOpT)",
                                     96,-3.14159,3.14159, 100,-0.25,0.25));	
     Res_eta_vs_eta.push_back(Book2D("Res"+m_sType+"_eta_vs_eta_from"+pt_slices[i]+"_to"+pt_slices[i+1]+"GeV",
                                     "Res"+m_sType+"_eta vs eta from "+pt_slices[i]+" GeV to "+pt_slices[i+1]+" GeV;eta;eta-RECOeta",
                                     etaBins.size()-1 ,&etaBins[0], 100,-0.25,0.25));	
  }
				

}

void ResoTriggerMuonPlots::fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu) {
  //fill plots Res_pt,eta,phi
  float pt = 0.001*Recomu.pt();
  float eta = Recomu.eta();
  float phi = Recomu.phi();
  //float respt = (Trigmu.pt() - Recomu.pt())/Recomu.pt();
  float respt = (1./Trigmu.pt() - 1./Recomu.pt())/(1./Recomu.pt());

  Res_pT->Fill(respt);
  Res_eta->Fill(Trigmu.eta() - eta);
  Res_phi->Fill(Trigmu.phi() - phi);
     
  Res_pT_vs_pT->Fill(pt,respt);  
  Res_eta_vs_pT->Fill(pt,Trigmu.eta() - eta);  
  Res_phi_vs_pT->Fill(pt,Trigmu.phi() - phi);  

  for (unsigned int i=0; i<pt_slices.size()-1;i++){
    
    float pt_min = atof(pt_slices[i].c_str());
    float pt_max = atof(pt_slices[i+1].c_str());
    if (pt>=pt_min && pt<pt_max) {
       Res_pT_vs_eta[i]->Fill(eta,respt);
       Res_pT_vs_phi[i]->Fill(phi,respt);
       Res_phi_vs_phi[i]->Fill(phi,Trigmu.phi() - phi);
       Res_eta_vs_eta[i]->Fill(eta,Trigmu.eta() - eta);
    }
  }
}


void ResoTriggerMuonPlots::fill(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu) {
  float pt = 0.001*Recomu.pt();
  float eta = Recomu.eta();
  float phi = Recomu.phi();
  float respt =0; 
  if (L2SAmu.pt()>0.) respt = (1./(1000.*L2SAmu.pt()) - 1./Recomu.pt())/(1./Recomu.pt()); 
  if (L2SAmu.pt()<0.) respt = (1./(-1000.*L2SAmu.pt()) - 1./Recomu.pt())/(1./Recomu.pt());

  Res_pT->Fill(respt);
  Res_eta->Fill(L2SAmu.eta() - eta);
  Res_phi->Fill(L2SAmu.phi() - phi);
     
  Res_pT_vs_pT->Fill(pt,respt);  
  Res_eta_vs_pT->Fill(pt,L2SAmu.eta() - eta);  
  Res_phi_vs_pT->Fill(pt,L2SAmu.phi() - phi);  

  for (unsigned int i=0; i<pt_slices.size()-1;i++){
    
    float pt_min = atof(pt_slices[i].c_str());
    float pt_max = atof(pt_slices[i+1].c_str());
    if (pt>=pt_min && pt<pt_max) {
       Res_phi_vs_phi[i]->Fill(phi,L2SAmu.phi() - phi);
       Res_eta_vs_eta[i]->Fill(eta,L2SAmu.eta() - eta);
       Res_pT_vs_eta[i]->Fill(eta,respt);
       Res_pT_vs_phi[i]->Fill(phi,respt);
    }
  }
}

void ResoTriggerMuonPlots::fill(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu) {
  float pt = 0.001*Recomu.pt();
  float eta = Recomu.eta();
  float phi = Recomu.phi();
  float respt =(1./(L2CBmu.pt()) - 1./Recomu.pt())/(1./Recomu.pt()); ; 
  
  Res_pT->Fill(respt);
  Res_eta->Fill(L2CBmu.eta() - eta);
  Res_phi->Fill(L2CBmu.phi() - phi);
     
  Res_pT_vs_pT->Fill(pt,respt);  
  Res_eta_vs_pT->Fill(pt,L2CBmu.eta() - eta);  
  Res_phi_vs_pT->Fill(pt,L2CBmu.phi() - phi);  

  for (unsigned int i=0; i<pt_slices.size()-1;i++){
    
    float pt_min = atof(pt_slices[i].c_str());
    float pt_max = atof(pt_slices[i+1].c_str());
    if (pt>=pt_min && pt<pt_max) {
       Res_phi_vs_phi[i]->Fill(phi,L2CBmu.phi() - phi);
       Res_eta_vs_eta[i]->Fill(eta,L2CBmu.eta() - eta);
       Res_pT_vs_eta[i]->Fill(eta,respt);
       Res_pT_vs_phi[i]->Fill(phi,respt);
    }
  }
}



} // closing namespace Muon
