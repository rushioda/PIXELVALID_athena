//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTIMETUNINGNTUPLE
#define LARTIMETUNINGNTUPLE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LArRawConditions/LArGlobalTimeOffset.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArCellTimeOffset.h"
#include "TBEvent/TBPhase.h"

#include "LArIdentifier/LArOnlineID.h"

#include <string>
#include <map>

class LArTimeTuningNtuple : public AthAlgorithm
{
 public:
  LArTimeTuningNtuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArTimeTuningNtuple();

  //standard algorithm methods
  StatusCode initialize(); 
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_TBPhaseKey;
  std::string m_GlobalTimeKey;
  std::string m_FebTimeKey;
  std::string m_CellTimeOffsetKey;
  
  NTuple::Item<float> m_phaseNt;

  NTuple::Item<float> m_globalTimeNt;

  NTuple::Item<unsigned long> m_nFebNt;

  NTuple::Array<float> m_febTimeNt;
  NTuple::Array<long>  m_febSlotNt;
  NTuple::Array<long>  m_febFTNt;
  NTuple::Array<long>  m_febIDNt;

  NTuple::Item<unsigned long> m_nCellNt;

  NTuple::Array<float> m_cellTimeNt;
  NTuple::Array<long>  m_cellSlotNt;
  NTuple::Array<long>  m_cellFTNt;
  NTuple::Array<long>  m_cellIDNt;
  NTuple::Array<long>  m_cellRegionNt;
  NTuple::Array<long>  m_cellLayerNt; 
  NTuple::Array<long>  m_cellEtaNt; 
  NTuple::Array<long>  m_cellPhiNt; 
  NTuple::Array<long>  m_cellGainNt;  
  NTuple::Array<long>  m_cellChannelNt;
  NTuple::Array<long>  m_cellCalibLineNt;

  //NTuplePtr nt;
  NTuple::Tuple* m_ntuplePtr;

  const LArOnlineID* m_larOnlineHelper;
};

#endif
