/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArHVPathologyDbAlg_H
#define LArHVPathologyDbAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class EventInfo;
class IIOVRegistrationSvc;
class ILArHVPathologyDbTool;
class LArCablingService ;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArOnlineID;
class CaloIdManager;
class CaloDetDescrManager;
class Identifier;

class LArHVPathologyDbAlg : public AthAlgorithm 
{
 public:
  LArHVPathologyDbAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArHVPathologyDbAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
  StatusCode createCondObjects();
  StatusCode printCondObjects();
  StatusCode registerCondObjects();
  std::vector<unsigned int> getElectInd(const Identifier& id, unsigned int module, unsigned int line);
  int getHVline(const Identifier& id, short unsigned int ElectInd);
 
  BooleanProperty           m_writeCondObjs;
  StringProperty            m_inpFile;
  StringProperty            m_outFile;
 
  StringProperty            m_folder;
  StringProperty            m_outpTag;

  const EventInfo*          m_evt;
 
  ServiceHandle<IIOVRegistrationSvc>   m_regSvc;
  ToolHandle<ILArHVPathologyDbTool>    m_pathologyTool;

  int m_mode;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  ToolHandle<LArCablingService> m_cablingService;
  const LArEM_ID*       m_larem_id;
  const LArHEC_ID*       m_larhec_id;
  const LArFCAL_ID*       m_larfcal_id;
  const LArOnlineID*      m_laronline_id;

};

#endif
