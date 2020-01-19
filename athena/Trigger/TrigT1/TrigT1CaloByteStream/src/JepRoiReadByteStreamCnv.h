/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPROIREADBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_JEPROIREADBYTESTREAMCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;

// Externals
extern long ByteStream_StorageType;

namespace LVL1BS {

class JepRoiByteStreamTool;

/** ByteStream converter for JEP component containers.
 *
 *  @author Peter Faulkner
 */

template <typename Container>
class JepRoiReadByteStreamCnv: public Converter {

  friend class CnvFactory<JepRoiReadByteStreamCnv<Container> >;

protected:

  JepRoiReadByteStreamCnv(ISvcLocator* svcloc);

public:

  ~JepRoiReadByteStreamCnv();

  virtual StatusCode initialize();
  /// Create Container from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

  //  Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static  long storageType(){ return ByteStream_StorageType; }
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::JepRoiByteStreamTool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  /// Message log
  mutable MsgStream m_log;
  bool m_debug;

};

} // end namespace

#include "JepRoiReadByteStreamCnv.icc"

#endif
