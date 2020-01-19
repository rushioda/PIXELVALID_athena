/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/** @file Analysis_Efficiency.h */

#ifndef TrigInDetAnalysisExample_Analysis_Efficiency_H
#define TrigInDetAnalysisExample_Analysis_Efficiency_H


#include <iostream>

#include "TH1.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"


class Analysis_Efficiency : public TrackAnalysis { 
  
 public:
  
  Analysis_Efficiency(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut);
  
  virtual void initialise();
  
  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
		       const std::vector<TIDA::Track*>& testTracks,
		       TrackAssociator* associator);
  
  virtual void finalise();
  
 private:

  double phi(double p);

 private:
  
  // Integral efficiency histograms
  TH1* h_integralD;
  TH1* h_integralN;
  TH1* h_integral;
    
  // Differential efficiency histograms
  TH1* h_etaD;
  TH1* h_phiD;
  TH1* h_z0D;
  TH1* h_d0D;
  TH1* h_pTD;
  TH1* h_etaN;
  TH1* h_phiN;
  TH1* h_z0N;
  TH1* h_d0N;
  TH1* h_pTN;
  TH1* h_eta;
  TH1* h_phi;
  TH1* h_z0;
  TH1* h_d0;
  TH1* h_pT;

  // Efficiency plateau cuts
  double m_pTCut;
  double m_etaCut;
  double m_d0Cut;
  double m_z0Cut;

};


#endif // TrigInDetAnalysisExample_Analysis_Efficiency_H
