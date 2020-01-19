/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  BCM_RawContByteStreamCnv.cxx
//   Implementation file for class BCM_RawContByteStreamCnv
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// classID() - returning BCM_RDO_Container ID
// createRep() - convert BCM_RawData in the container into ByteStream
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos for pixels
///////////////////////////////////////////////////////////////////

#include "BCM_RawDataByteStreamCnv/BCM_RawContByteStreamCnv.h"

#include "BCM_RawDataByteStreamCnv/BCM_RawContByteStreamTool.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h"

////////////////////////
// constructor
////////////////////////
BCM_RawContByteStreamCnv::BCM_RawContByteStreamCnv(ISvcLocator* svcloc):Converter(ByteStream_StorageType, classID(),svcloc) {
}

////////////////////////
// destructor
////////////////////////
BCM_RawContByteStreamCnv::~BCM_RawContByteStreamCnv() {
}

////////////////////////
// initialize()
////////////////////////
StatusCode BCM_RawContByteStreamCnv::initialize() {

  StatusCode sc = Converter::initialize(); 
  if(StatusCode::SUCCESS!=sc) { 
    return sc; 
  } 

  // Check ByteStreamCnvSvc
  if (StatusCode::SUCCESS != service("ByteStreamCnvSvc", m_ByteStreamEventAccess) || !m_ByteStreamEventAccess) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Can't get ByteStreamEventAccess interface" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLevel(MSG::INFO)) msg(MSG::INFO) << "ByteStreamCnvSvc retireved" << endreq;

  // retrieve Tool
  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Can't get ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }
  std::string toolType;
  toolType = "BCM_RawContByteStreamTool"; 
  if(StatusCode::SUCCESS != toolSvc->retrieveTool(toolType,m_BCMRawContBSTool)) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Can't get BCM_RawContByteStreamTool Tool" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode scChrono =serviceLocator()->service("ChronoStatSvc",m_ChronoStat );
  if (!scChrono.isSuccess()) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Cannot retrieve ChronoStatSvc" << endreq;
  }

  return StatusCode::SUCCESS; 
}

////////////////////////
// classID() - returning BCM_RDO_Container ID
////////////////////////
const CLID& BCM_RawContByteStreamCnv::classID() {
  return ClassID_traits<BCM_RDO_Container>::ID();
}

////////////////////////
// createRep() - convert BCM_RawData in the container into ByteStream
////////////////////////
StatusCode BCM_RawContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {

  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();
  BCM_RDO_Container* cont = 0;
  StoreGateSvc::fromStorable(pObj,cont);
  if(!cont) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Can not cast to BCM_RDO_Container" << endreq;
    return StatusCode::FAILURE;
  }

  std::string nm = pObj->registry()->name();
  ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,"");
  pAddr = addr;

  StatusCode sc = m_BCMRawContBSTool->convert(cont, re);
  if(sc.isFailure()){
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << " Could not convert rdo with m_BCMRawContBSTool " << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
