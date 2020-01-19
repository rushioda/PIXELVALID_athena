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

#include "xAODTrigL1Calo/CMXCPTob.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPTobAuxContainer.h"

#include "CmxCpTobByteStreamxAODCnv.h"

namespace LVL1BS {

CmxCpTobByteStreamxAODCnv::CmxCpTobByteStreamxAODCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(), svcloc),
    AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxCpTobByteStreamxAODCnv"),
    m_name("CmxCpTobByteStreamxAODCnv")
{

}

// CLID

const CLID& CmxCpTobByteStreamxAODCnv::classID() {
  return ClassID_traits<xAOD::CMXCPTobContainer>::ID();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CmxCpTobByteStreamxAODCnv::initialize() {
  ATH_MSG_DEBUG(
      "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  //CHECK(m_readTool.retrieve());
  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CmxCpTobByteStreamxAODCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  const std::string nmAux = nm + "Aux.";
  ATH_MSG_DEBUG("Creating xAOD::CMXCPTob interface objects '" << nm << "'");

  xAOD::CMXCPTobContainer* const container =
      new xAOD::CMXCPTobContainer;

  // Create link with AUX container
  DataLink<xAOD::CMXCPTobAuxContainer> link(nmAux);
  ATH_MSG_DEBUG("Creating store with data link to '" << nmAux);

  for(size_t i=0; i < link->size(); ++i){
     container->push_back(new xAOD::CMXCPTob());
  }
  // ========================================================================== 
  container->setStore(link);
  pObj = SG::asStorable(container);
  ATH_MSG_DEBUG("Number of xAOD::CMXCPTob created: " << container->size());

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CmxCpTobByteStreamxAODCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
