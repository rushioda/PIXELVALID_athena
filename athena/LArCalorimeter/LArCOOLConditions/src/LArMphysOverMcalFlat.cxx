/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMphysOverMcalFlat.h"


LArMphysOverMcalFlat::LArMphysOverMcalFlat() {}

LArMphysOverMcalFlat::~LArMphysOverMcalFlat() {}


LArMphysOverMcalFlat::LArMphysOverMcalFlat(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArMphysOverMcalFlat");
  if (sc.isFailure()) return;
  
  this->readBlob(attrList,"MphysOverMcal",*m_log);

  return;
}

const float& LArMphysOverMcalFlat::MphysOverMcal(const HWIdentifier& chid, int gain) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),gain);
}

  // retrieving MphysOverMcal using offline ID  
const float& LArMphysOverMcalFlat::MphysOverMcal(const Identifier& id, int gain) const  {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(id);
  return MphysOverMcal(OnId,gain);
}
