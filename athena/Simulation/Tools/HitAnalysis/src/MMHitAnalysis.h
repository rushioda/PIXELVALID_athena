/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_HIT_ANALYSIS_H
#define MM_HIT_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"



class TH1;
class TH2;
class TTree;

class MMHitAnalysis : public AthAlgorithm {

 public:

  MMHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~MMHitAnalysis(){}

  virtual StatusCode initialize();
  virtual StatusCode execute();

 private:

  /** Some variables**/
  TH2 *m_h_S1_xy_A;
  TH2 *m_h_S1_rz_A;
  TH1 *m_h_S1_r_A;
  TH2 *m_h_S1_xy_C;
  TH2 *m_h_S1_rz_C;
  TH1 *m_h_S1_r_C;
  TH2 *m_h_S2_xy_A;
  TH2 *m_h_S2_rz_A;
  TH1 *m_h_S2_r_A;
  TH2 *m_h_S2_xy_C;
  TH2 *m_h_S2_rz_C;
  TH1 *m_h_S2_r_C;
  TH2 *m_h_S_xy_A;
  TH2 *m_h_S_xy_C;
  TH2 *m_h_S_rz_A;
  TH2 *m_h_S_rz_C;

  TH2 *m_h_xy_A;
  TH2 *m_h_xy_C;
  TH2 *m_h_rz_A;
  TH2 *m_h_rz_C;

  TH2 *m_h_L1_xy_A;
  TH2 *m_h_L1_rz_A;
  TH1 *m_h_L1_r_A;
  TH2 *m_h_L1_xy_C;
  TH2 *m_h_L1_rz_C;
  TH1 *m_h_L1_r_C;
  TH2 *m_h_L2_xy_A;
  TH2 *m_h_L2_rz_A;
  TH1 *m_h_L2_r_A;
  TH2 *m_h_L2_xy_C;
  TH2 *m_h_L2_rz_C;
  TH1 *m_h_L2_r_C;

  TH2 *m_h_L_xy_A;
  TH2 *m_h_L_xy_C;
  TH2 *m_h_L_rz_A;
  TH2 *m_h_L_rz_C;

  std::vector<float>* m_hits_x;

  TTree * m_tree;
  std::string m_ntupleFileName;

  std::string m_path;
  ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // MM_HIT_ANALYSIS_H
