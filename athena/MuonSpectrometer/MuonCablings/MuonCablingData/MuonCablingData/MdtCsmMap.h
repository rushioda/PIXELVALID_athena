/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTCSMMAP_H
#define MUONMDT_CABLING_MDTCSMMAP_H

#include "MuonCablingData/MdtMapBase.h"

class MdtAmtMap;

class MdtCsmMap : public MdtMapBase<MdtAmtMap> {

 public:

  MdtCsmMap(uint8_t csmId);
  ~MdtCsmMap();

  /** Set the map information */
  bool setAmtMap(uint8_t tdcId , MdtAmtMap* amtMap);

  /** Access the map information */
  MdtAmtMap* getTdcMap(uint8_t tdcId);

 private:

};


#endif   // MUONMDT_CABLING_MDTCSMMAP_H
