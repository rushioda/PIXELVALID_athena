/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArNoiseSC.h"


//const float LArNoiseSC::errorcode=ILArNoise::ERRORCODE;

LArNoiseSC::LArNoiseSC() {}

LArNoiseSC::~LArNoiseSC() {}


LArNoiseSC::LArNoiseSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArNoiseSC");
  if (sc.isFailure()) return;
 
  readBlob(attrList,"Noise",*m_log);

  return;
}


const float& LArNoiseSC::noise(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

// retrieving LArNoise using offline ID  
const float& LArNoiseSC::noise(const Identifier& cellID, int /*gain*/) const  {
  const HWIdentifier hwid=m_scCablingTool->createSignalChannelID(cellID);
  return this->noise(hwid,0);

}
