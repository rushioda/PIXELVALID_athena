/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOLOCALHADCALIB_CALOREADLCOUTOFCLUSTERFILE
#define CALOLOCALHADCALIB_CALOREADLCOUTOFCLUSTERFILE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include <string>


class CaloReadLCOutOfClusterFile : public AthAlgorithm
{
 public:
  CaloReadLCOutOfClusterFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloReadLCOutOfClusterFile();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();
  StatusCode initDataFromFile(std::string theLCOutOfClusterFileName);

 private:
  std::string m_key;
  std::string m_LCOutOfClusterFileName;

  CaloLocalHadCoeff * m_data;
};

#endif
