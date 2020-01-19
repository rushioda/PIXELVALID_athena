/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_READRPCDIGIT
#define MUONBYTESTREAMCNVTEST_READRPCDIGIT

#include <string.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"

class RpcIdHelper;

class ReadRpcDigit : public AthAlgorithm
{
 public:
  // Agorithm constructor
  ReadRpcDigit (const std::string &name, ISvcLocator *pSvcLocator);
  ~ReadRpcDigit();

  // Gaudi hooks
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ServiceHandle<ActiveStoreSvc> m_activeStore;
  StatusCode accessNtuple();
  bool m_rpcNtuple;
  std::string m_NtupleLocID;
  NTuple::Tuple* m_ntuplePtr;
  const RpcIdHelper* m_rpcIdHelper;

  // Ntuple Variables
  // Digits block
  NTuple::Item<long> m_nColl;    // number of collection in the container
  NTuple::Item<long> m_nDig;     // number of digit in the collection 
  NTuple::Array<float> m_time;
  NTuple::Array<float> m_station;    // Return the station
  NTuple::Array<float> m_eta;        // Return the station eta
  NTuple::Array<float> m_phi;        // Return the station phi
  NTuple::Array<float> m_doubletR;   // Return doubletR
  NTuple::Array<float> m_doubletZ;   // Return doubletZ
  NTuple::Array<float> m_doubletPhi; // Return doubletPhi
  NTuple::Array<float> m_gasGap;     // Return gasGap
  NTuple::Array<float> m_measuresPhi;     // Return measuresPhi
  NTuple::Array<float> m_strip;       // Return the strip # 
};
#endif     // MUONBYTESTREAMCNVTEST_READRPCDIGIT

