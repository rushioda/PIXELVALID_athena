/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOCALLOCALHADCALIB_CALOREADLCWEIGHTSFILE
#define CALOCALLOCALHADCALIB_CALOREADLCWEIGHTSFILE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include <string>


class CaloReadLCWeightsFile : public AthAlgorithm
{
 public:
  CaloReadLCWeightsFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloReadLCWeightsFile();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();
  StatusCode initDataFromFile(std::string theLCWeightFileName);

 private:
  std::string m_key;
  std::string m_LCWeightFileName;

  CaloLocalHadCoeff * m_data;
};

#endif
