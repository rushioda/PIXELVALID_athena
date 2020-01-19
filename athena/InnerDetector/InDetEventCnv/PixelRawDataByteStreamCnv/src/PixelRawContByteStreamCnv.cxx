/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  PixelRawContByteStreamCnv.cxx
//   Implementation file for class PixelRawContByteStreamCnv
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// classID() - returning PixelRDO_Container ID
// createObj() - creates PixelRDO_Container
// createRep() - convert Pixel_RDO in the container into ByteStream
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "PixelRawContByteStreamCnv.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelRawContByteStreamTool.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "InDetRawData/PixelRDORawData.h"
#include "GaudiKernel/IChronoStatSvc.h"

//#define PIXEL_DEBUG


////////////////////////
// constructor
////////////////////////
PixelRawContByteStreamCnv::PixelRawContByteStreamCnv(ISvcLocator* svcloc) : 
  Converter(ByteStream_StorageType, classID(),svcloc),
  m_PixelRawContBSTool(NULL),
  m_ByteStreamEventAccess(NULL),
  m_PixelRDO_Container(NULL),
  m_StoreGate(NULL),
  m_ChronoStat(NULL),
  m_log(msgSvc(), "PixelRawContByteStreamCnv")
{}

////////////////////////
// destructor
////////////////////////
PixelRawContByteStreamCnv::~PixelRawContByteStreamCnv() {
}

////////////////////////
// initialize()
////////////////////////
StatusCode PixelRawContByteStreamCnv::initialize() {

  StatusCode sc = Converter::initialize(); 
  if(StatusCode::SUCCESS!=sc) { 
    return sc; 
  } 

  // Check ByteStreamCnvSvc
  if (StatusCode::SUCCESS != service("ByteStreamCnvSvc", m_ByteStreamEventAccess) || !m_ByteStreamEventAccess) {
    m_log << MSG::ERROR << "Can't get ByteStreamEventAccess interface" << endreq;
    return StatusCode::FAILURE;
  }
  m_log << MSG::INFO << "ByteStreamCnvSvc retrieved" << endreq;

  // retrieve Tool
  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)) {
    m_log << MSG::ERROR << "Can't get ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }

  std::string toolType;

  toolType = "PixelRawContByteStreamTool"; 
  if(StatusCode::SUCCESS != toolSvc->retrieveTool(toolType,m_PixelRawContBSTool)) {
    m_log << MSG::ERROR << "Can't get PixelRawContByteStreamTool Tool" << endreq;
    return StatusCode::FAILURE;
  }

  // Get an Identifier helper object
  sc = service("DetectorStore",m_StoreGate);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "Detector service not found" << endreq;
    return StatusCode::FAILURE;
  } 

  StatusCode scChrono =serviceLocator()->service("ChronoStatSvc",m_ChronoStat );
  if (!scChrono.isSuccess()) {
    m_log << MSG::ERROR << "Cannot retrieve ChronoStatSvc" << endreq;
  }

  return StatusCode::SUCCESS; 
}

////////////////////////
// classID() - returning PixelRDO_Container ID
////////////////////////
const CLID& PixelRawContByteStreamCnv::classID() {

  return ClassID_traits<PixelRDO_Container>::ID() ;

}


////////////////////////
// createRep() - convert Pixel_RDO in the container into ByteStream
////////////////////////
StatusCode PixelRawContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {

  //  StatusCode sc = StatusCode::SUCCESS; 
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent(); 
  PixelRDO_Container* cont=0; 
  StoreGateSvc::fromStorable(pObj,cont); 

  if(!cont) {
    m_log << MSG::ERROR << "Can not cast to PixelRDO_Container" << endreq; 
    return StatusCode::FAILURE;    
  } 

  std::string nm = pObj->registry()->name(); 
  ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,""); 
  pAddr = addr; 
  //  return m_PixelRawContBSTool->convert(cont,re,m_log); 

  StatusCode sc = m_PixelRawContBSTool->convert(cont, re) ;
  if(sc.isFailure()){
    m_log << MSG::ERROR<< " Could not convert rdo with m_PixelRawContBSTool " << endreq ;
    return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS ;
}
