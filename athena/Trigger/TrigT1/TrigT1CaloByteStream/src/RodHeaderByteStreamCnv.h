/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IOpaqueAddress;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;

// Externals
extern long ByteStream_StorageType;

namespace LVL1BS {

class RodHeaderByteStreamTool;

/** ByteStream converter for L1Calo ROD header info
 *
 *  @author Peter Faulkner
 */

class RodHeaderByteStreamCnv: public Converter {

  friend class CnvFactory<RodHeaderByteStreamCnv>;

protected:

  RodHeaderByteStreamCnv(ISvcLocator* svcloc);

public:

  ~RodHeaderByteStreamCnv();

  virtual StatusCode initialize();
  /// Create RodHeaders from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

  //  Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static  long storageType(){ return ByteStream_StorageType; }
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::RodHeaderByteStreamTool> m_tool;



  /// Message log
  mutable MsgStream m_log;
  bool m_debug;

};

} // end namespace

#endif
