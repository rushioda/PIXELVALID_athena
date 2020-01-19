/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_CSC_DCSCONDITIONSTOOL_H
#define MUONCONDTOOL_CSC_DCSCONDITIONSTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/ICSC_DCSConditionsTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

class Identifier;
class StoreGateSvc;

class CscHelper;
class IIOVSvc;
class StatusCode;

class CSC_DCSConditionsTool: public AthAlgTool, virtual public ICSC_DCSConditionsTool
{

public:   

  CSC_DCSConditionsTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad);
  
  virtual StatusCode initialize();

  virtual std::string DropChamberFolderName() const {return m_dropchamberFolder;}
  virtual std::string HVFolderName() const {return m_hvFolder;}
  virtual std::string ChamberFolder() const {return m_chamberFolder;}
  virtual const std::map<Identifier,int>& CSC_ChamberId(){ return m_CSC_LayerMap;}   

  virtual const std::vector<std::string>& deadStationsStr(){ return m_cachedDeadStationsStr;}
  virtual const std::vector<Identifier>& deadStationsId(){ return m_cachedDeadStationsId;}
  virtual const std::vector<Identifier>& deadWireLayersId(){ return m_cachedDeadWireLayersId;} 

  virtual const std::vector<std::string>& deadStations(){ return m_cachedDeadStationsStr;}
  virtual const std::vector<std::string>& deadWireLayers(){ return m_cachedDeadWireLayers;} 
  
  
 private:


 
   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
   virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS);
   virtual StatusCode loadchamber(IOVSVC_CALLBACK_ARGS);
   
       
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;

  std::map<Identifier, int> m_CSC_LayerMap;
  std::map<Identifier, int> m_CSC_LayerMap_test;
  const CscIdHelper* m_cscIdHelper;
 
  std::vector<int> m_wireLayer;
  std::vector<Identifier> m_cachedDeadStationsId;
  std::vector<Identifier> m_cachedDeadStations;
  std::vector<Identifier> m_cachedDeadStationsId_chamber;
  std::vector<std::string> m_cachedDeadStationsStr;
  std::vector<Identifier> m_cachedDeadWireLayersId;
  std::vector<std::string> m_cachedDeadWireLayers;
  //private comparison function for Identifier sorting
  static bool compareId(Identifier x,Identifier y) { return (x > y); } 
  
  
  std::string      m_DataLocation;
 
  std::string     m_dropchamberFolder;
  std::string     m_hvFolder;
  std::string     m_chamberFolder;

        
  std::stringstream m_CSCChamDrop;
  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;
  
  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;
};
 


#endif
