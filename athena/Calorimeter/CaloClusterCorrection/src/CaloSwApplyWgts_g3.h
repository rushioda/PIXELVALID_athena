/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWAPPLYWGTS_G3_H
#define CALOCLUSTERCORRECTION_CALOSWAPPLYWGTS_G3_H
/********************************************************************

NAME:     CaloSwApplyWgts_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  J. McDonald
CREATED:  Feb. 2, 2003

PURPOSE:  correction for the weights which are applied to 
          the data.  In order to minimize the effects 
          Tuned on DC1 data.

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

********************************************************************/


#include "CaloClusterCorrection/CaloClusterCorrection.h"

class CaloSwApplyWgts_g3 : public CaloClusterCorrection
{

 public:

  // constructor 
  CaloSwApplyWgts_g3(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
  // destructor 
  virtual ~CaloSwApplyWgts_g3() override;
  
  // virtual method in CaloClusterCorrection
  virtual void makeCorrection(const EventContext& ctx,
                              xAOD::CaloCluster* cluster) const override;

 private:

  CaloSwApplyWgts_g3() = delete;

  static const int s_numEtaBins=100;
  float GetWgt(int table, float eta) const;
  static const float m_table_eta[s_numEtaBins+1];
  static const float m_table_p[4][s_numEtaBins];
};

#endif

