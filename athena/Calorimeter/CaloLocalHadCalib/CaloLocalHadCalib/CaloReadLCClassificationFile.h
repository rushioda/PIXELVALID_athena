/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOLOCALHADCALIB_CALOREADLCCLASSIFICATIONFILE
#define CALOLOCALHADCALIB_CALOREADLCCLASSIFICATIONFILE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include <string>


class CaloReadLCClassificationFile : public AthAlgorithm
{
 public:
  CaloReadLCClassificationFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloReadLCClassificationFile();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();
  StatusCode initDataFromFile(std::string theLCClassificationFileName);

 private:
  std::string m_key;
  std::string m_LCClassificationFileName;

  CaloLocalHadCoeff * m_data;
};

#endif
