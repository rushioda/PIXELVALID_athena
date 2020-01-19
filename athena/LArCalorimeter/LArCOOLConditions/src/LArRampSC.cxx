/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArRampSC.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"

LArRampSC::LArRampSC() {}

LArRampSC::~LArRampSC()  {}


LArRampSC::LArRampSC(const CondAttrListCollection* attrList) {

  StatusCode sc=initializeBase("LArRampSC");
  if (sc.isFailure()) return;
 
  if (!attrList) return;

  readBlob(attrList,*m_log);

}

ILArRamp::RampRef_t LArRampSC::ADC2DAC(const HWIdentifier& hwid, int /*gain*/) const { 
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->ADC2DACByHash(hash, 0);
}

ILArRamp::RampRef_t LArRampSC::ADC2DAC(const Identifier&  cellID, int /*gain*/) const{
  const HWIdentifier hwid=m_scCablingTool->createSignalChannelID(cellID);
  return LArRampSC::ADC2DAC(hwid, 0);
}

