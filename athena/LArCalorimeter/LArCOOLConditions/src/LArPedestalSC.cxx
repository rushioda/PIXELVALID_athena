/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArPedestalSC.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

LArPedestalSC::LArPedestalSC()   {}

LArPedestalSC::LArPedestalSC(const CondAttrListCollection* attrList) {
 
  StatusCode sc=initializeBase("LArPedestalSC");
  if (sc.isFailure()) return;
 
  readBlob(attrList,*m_log);
}
 
float LArPedestalSC::pedestal(const Identifier&  cellID, int /*gain*/) const {
  const HWIdentifier hwid=m_scCablingTool->createSignalChannelID(cellID);
  return this->pedestal(hwid,0);
}

float LArPedestalSC::pedestalRMS(const Identifier&  cellID, int /*gain*/) const {
  const HWIdentifier hwid=m_scCablingTool->createSignalChannelID(cellID);
  return this->pedestalRMS(hwid,0);
}


float  LArPedestalSC::pedestal(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->pedestalByHash(hash,0);
}
  
float  LArPedestalSC::pedestalRMS(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->pedestalRMSByHash(hash,0);
}
 
