/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <stdint.h>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "SGTools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTrigL1Calo/CPMTobRoI.h"
#include "xAODTrigL1Calo/CPMTobRoIContainer.h"
#include "xAODTrigL1Calo/CPMTobRoIAuxContainer.h"

#include "CpmTobRoiByteStreamxAODCnv.h"

namespace LVL1BS {

CpmTobRoiByteStreamxAODCnv::CpmTobRoiByteStreamxAODCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(), svcloc),
    AthMessaging(svcloc != 0 ? msgSvc() : 0, "CpmTobRoiByteStreamxAODCnv"),
    m_name("CpmTobRoiByteStreamxAODCnv")
{

}

// CLID

const CLID& CpmTobRoiByteStreamxAODCnv::classID() {
  return ClassID_traits<xAOD::CPMTobRoIContainer>::ID();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CpmTobRoiByteStreamxAODCnv::initialize() {
  ATH_MSG_DEBUG(
      "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  //CHECK(m_readTool.retrieve());
  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CpmTobRoiByteStreamxAODCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  const std::string nmAux = nm + "Aux.";
  ATH_MSG_DEBUG("Creating xAOD::CPMTobRoI interface objects '" << nm << "'");

  xAOD::CPMTobRoIContainer* const container =
      new xAOD::CPMTobRoIContainer;

  // Create link with AUX container
  DataLink<xAOD::CPMTobRoIAuxContainer> link(nmAux);
  ATH_MSG_DEBUG("Creating store with data link to '" << nmAux);

  for(size_t i=0; i < link->size(); ++i){
     container->push_back(new xAOD::CPMTobRoI());
  }
  // ========================================================================== 
  container->setStore(link);
  pObj = SG::asStorable(container);
  ATH_MSG_DEBUG("Number of CPMTobRoI created: " << container->size());

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CpmTobRoiByteStreamxAODCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
