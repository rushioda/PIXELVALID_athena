/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  BCM_RawContByteStreamCnv.h
//   Header file for class BCM_RawContByteStreamCnv
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// classID() - return BCM_RDO_Container ID
// createRep() - convert BCM_RawData in the container into ByteStream
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos for pixels
///////////////////////////////////////////////////////////////////

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RAWCONTBYTESTREAMCNV_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RAWCONTBYTESTREAMCNV_H

#include "GaudiKernel/Converter.h"

#include "ByteStreamData/RawEvent.h" 

//including the Message Stream Member
#include "AthenaKernel/MsgStreamMember.h"

class BCM_RawContByteStreamTool;
class IByteStreamEventAccess;
class StoreGateSvc;
class IChronoStatSvc;

// Externals 
extern long ByteStream_StorageType;

class BCM_RawContByteStreamCnv: public Converter {

  ~BCM_RawContByteStreamCnv();

 public:

  BCM_RawContByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize();

  // create the RawEvent fragments for BCM
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  // Storage type and class ID
  virtual long repSvcType() const { 
    return ByteStream_StorageType;
  }
  static long storageType() {
    return ByteStream_StorageType;
  }
  static const CLID& classID();

  //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

  //Declaring the Method providing Verbosity Level
  bool msgLevel (MSG::Level lvl){ return m_msg.get().level() <= lvl; }

private: 

  BCM_RawContByteStreamTool* m_BCMRawContBSTool;

  IByteStreamEventAccess* m_ByteStreamEventAccess; 

  StoreGateSvc* m_StoreGate;

  IChronoStatSvc* m_ChronoStat;

  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg;

};
#endif
