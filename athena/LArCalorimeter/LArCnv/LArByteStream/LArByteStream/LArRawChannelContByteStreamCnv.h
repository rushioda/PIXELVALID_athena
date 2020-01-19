//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRAWCHANNELCONTRAWEVENTCNV_H
#define LARBYTESTREAM_LARRAWCHANNELCONTRAWEVENTCNV_H

#include <stdint.h>
#include <map>
#include "GaudiKernel/Converter.h"
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
//#include "LArByteStream/Hid2RESrcID.h"
#

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class MsgStream; 
class LArRawChannelContainer; 
class LArRawDataContByteStreamTool;
class IROBDataProviderSvc;  
class ByteStreamCnvSvc;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

class LArRawChannelContByteStreamCnv: public Converter {
  friend class CnvFactory<LArRawChannelContByteStreamCnv>;

 public:
  LArRawChannelContByteStreamCnv(ISvcLocator* svcloc);
  virtual ~LArRawChannelContByteStreamCnv();


  typedef LArRawDataContByteStreamTool  BYTESTREAMTOOL ; 

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType; }
  static long storageType()     { return ByteStream_StorageType; }
  static const CLID& classID();

private: 
  MsgStream* m_log;
  BYTESTREAMTOOL* m_tool; 
  ByteStreamCnvSvc* m_ByteStreamEventAccess;
  IROBDataProviderSvc *m_rdpSvc;
  StoreGateSvc* m_storeGate; 
  size_t m_contSize;
};
#endif



