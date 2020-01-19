/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CBNT_TBTDCRAW_H
#define CBNT_TBTDCRAW_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CBNT_TBRecBase.h"
#include <fstream>
#include <string>
#include <vector>

class StoreGateSvc;

/** @class CBNTAA_TBTDCRaw CBNTAA_TBTDCRaw.h "TBRec/CBNTAA_TBTDCRaw.h"
ntpl-dumper for TBEvent/TBTDCRaw.h.
@author Iftach Sadeh
*/

class CBNTAA_TBTDCRaw : public CBNT_TBRecBase
{
 public:
  CBNTAA_TBTDCRaw(const std::string & name, ISvcLocator * pSvcLocator);

  ~CBNTAA_TBTDCRaw();

  //standart algorithm methods
  virtual StatusCode CBNT_initialize();
  virtual StatusCode CBNT_execute();
  virtual StatusCode CBNT_finalize();

 private:

  bool m_neverReturnFailure;   // controls when StatusCode::FAILURE can be issued in execute()

  std::vector<unsigned int> *m_tdc;
  std::vector<bool> *m_underThreshold;
  std::string m_containerKey1;

  //Private function to build names for ntuple entries. Concatinates strings
  //but omits characters that are problematic for root like -+:*/
  std::string add_name(const char* base, const std::string extension);

  StatusCode CBNT_clear();
};

#endif
