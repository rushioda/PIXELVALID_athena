/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCOND2NTUPLEBASE_H
#define LARCOND2NTUPLEBASE_H

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArRecConditions/ILArBadChanTool.h"
#include "LArCabling/LArCablingService.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "LArElecCalib/ILArFEBTempTool.h"

class HWIdentifier;
class LArOnlineID_Base;
//class LArCablingService;
//class ILArBadChanTool;
class StoreGateSvc;
class LArEM_Base_ID;
class LArHEC_Base_ID;
class LArFCAL_Base_ID;
class CaloCell_ID;
class MsgStream;

class LArCond2NtupleBase : public AthAlgorithm {

 public:
  LArCond2NtupleBase(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCond2NtupleBase();

  //Standard algo methods
  StatusCode initialize();
  StatusCode execute()    {return StatusCode::SUCCESS;}
  //Finalize needs to be implemented by the deriving class


  //StatusCode initializeBase(const std::string& path, const std::string& name);
  bool fillFromIdentifier(const HWIdentifier& id); //returns true if connected

 private:
  bool m_initialized;

 protected:   
  bool  m_addBC, m_addFEBTemp;
  enum {NOT_VALID = -999};

  std::string m_ntpath, m_ntTitle;

  //Ntuple pointer
  NTuple::Tuple* m_nt;

  //Ntuple variables:
  NTuple::Item<long> m_detector, m_region, m_layer, m_eta, m_phi, m_onlChanId, m_oflChanId;
  NTuple::Item<long> m_pos_neg, m_barrel_ec, m_FT, m_slot, m_channel;
  NTuple::Item<long> m_calibLine,m_badChanWord;
  NTuple::Item<long> m_isConnected;
  NTuple::Item<long> m_chanHash, m_febHash, m_oflHash;
  
  NTuple::Item<float> m_FEBTemp1, m_FEBTemp2;


  StoreGateSvc* m_detStore; 
  const LArEM_Base_ID* m_emId;
  const LArHEC_Base_ID* m_hecId;
  const LArFCAL_Base_ID* m_fcalId;
  const LArOnlineID_Base* m_onlineId;
  const CaloCell_ID* m_caloId;
  LArCablingBase* m_larCablingSvc;
  ToolHandle<ILArBadChanTool> m_badChanTool;
  ToolHandle<ILArFEBTempTool> m_FEBTempTool;
  bool m_isSC;
  bool m_isFlat;
  bool m_OffId;
  bool m_addHash;

};
#endif
