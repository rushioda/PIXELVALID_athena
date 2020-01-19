/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigHLTResultByteStreamCnv_h
#define TrigHLTResultByteStreamCnv_h

#include <stdint.h>
#include <map>
/**********************************************************************************
 * @Project: HLT Steering
 * @Package: HLTResultByteStream
 * @Class  : HLTResultByteStreamCnv
 *
 * @brief  Gaudi bytestream Converter for the HLTResult class
 *
 *
 * File and Version Information:
 * $Id: HLTResultByteStreamCnv.h,v 1.4 2009-02-26 16:29:25 tbold Exp $
 **********************************************************************************/

#include <string>

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include <string>

// forward declarations
class DataObject;
class StatusCode;
class IAddressCreator;
class IROBDataProviderSvc;
class IByteStreamEventAccess;

class MsgStream;


// Externals
extern unsigned char ByteStream_StorageType;

namespace HLT {

  class HLTResultByteStreamTool;


  /**
     @class HLTResultByteStreamCnv
     @brief Gaudi bytestream Converter for the HLTResult class

     This class essentially provides two methods:
     1. createObj(IOpaqueAddress*,  DataObject*&)
     2. createRep(DataObject*, IOpaqueAddress*& )

     The first one creates a DataObject from persistent information, captured in
     IOpaqueAddress, and the second is supposed to write the DataObject to
     a persistent representation. Here, DataObject will be casted to HLTResult

     see here for a general ByteStream converter description:
     http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Event/ByteStreamCnvSvc/doc/ByteStream.txt

     beginning of this file:<br>
     ByteStream conversion Service (ByteStreamCnvSvc) provides a means to
     convert the raw data in the ByteStream format to and from the transient
     representation, Raw Data Objects (RDO).   In Gaudi's terminology, ByteStream
     can be considered as a persistency for reading and storing raw data. In
     practice, the source of the ByteStream can be a file, or the online data
     source (or its emulation) through network, for example.   Together with the
     IdentifiableContainer, used to contain the Raw Data Objects, ByteStreamCnvSvc
     provides a mechanism for creating the RDOs on demand.

  */
  class HLTResultByteStreamCnv : public Converter {

  protected:
    HLTResultByteStreamCnv(ISvcLocator* svcloc); //!< std Gaudi converter constructor

  public:

    virtual StatusCode initialize(); //!< Gaudi initialize
    virtual StatusCode finalize();   //!< Gaudi finalize

    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); //!< create RDO (HLTResult) from bytestream
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr); //!< create bytestream from RDOs (HLTResult)

    // Storage type and class ID
    static unsigned char storageType()     { return ByteStream_StorageType; } //!< used for the converter
    static const CLID& classID(); //!< CLID

    long repSvcType() const { return i_repSvcType(); } //!< return repSvcType

    friend class CnvFactory<HLTResultByteStreamCnv>; //!< declare factory as friend

  private:

    MsgStream* m_log; //!< std message stream

    ToolHandle< HLTResultByteStreamTool > m_tool ; //!< tool for conversion work

    ServiceHandle< IByteStreamEventAccess > m_ByteStreamEventAccess; //!< Gaudi helper

    ServiceHandle< IROBDataProviderSvc > m_robDataProvider; //!< Gaudi ROB helper

  };
}

#endif
