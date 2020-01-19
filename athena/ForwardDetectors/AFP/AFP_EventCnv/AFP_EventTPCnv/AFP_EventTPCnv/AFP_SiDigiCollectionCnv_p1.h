/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "AFP_EventTPCnv/AFP_SiDigiCollection_p1.h"
#undef private

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "AFP_EventTPCnv/AFP_SiDigiCnv_p1.h"


typedef T_AtlasHitsVectorCnv< AFP_SiDigiCollection, AFP_SiDigiCollection_p1, AFP_SiDigiCnv_p1 >  AFP_SiDigiCollectionCnv_p1;

