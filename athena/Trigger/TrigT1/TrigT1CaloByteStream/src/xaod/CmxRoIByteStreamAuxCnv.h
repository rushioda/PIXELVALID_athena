/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMXROIBYTESTREAMAUXCNV_H
#define TRIGT1CALOBYTESTREAM_CMXROIBYTESTREAMAUXCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;

// Externals
extern long ByteStream_StorageType;


namespace LVL1BS {
class JepRoiByteStreamV2Tool;

/** ByteStream converter for Pre-processor Module DAQ data / TriggerTowers.
 *
 *  @author alexander.mazurov@cern.ch
 */

class CmxRoIByteStreamAuxCnv: public Converter, public ::AthMessaging {

  friend class CnvFactory<CmxRoIByteStreamAuxCnv>;

protected:

  CmxRoIByteStreamAuxCnv(ISvcLocator* svcloc);

public:

  virtual ~CmxRoIByteStreamAuxCnv(){};

  virtual StatusCode initialize();
  /// Create TriggerTowers from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Create ByteStream from TriggerTowers
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static  long storageType(){ return ByteStream_StorageType; }

  static const CLID& classID();
private:
  /// Converter name
  std::string m_name;

  /// Do the main job - retrieve objects from robs
  ToolHandle<JepRoiByteStreamV2Tool> m_readTool;
};



} // end namespace
#endif
