/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "AFP_EventTPCnv/AFP_TDDigiCollection_p1.h"
#undef private

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_EventTPCnv/AFP_TDDigiCnv_p1.h"


typedef T_AtlasHitsVectorCnv< AFP_TDDigiCollection, AFP_TDDigiCollection_p1, AFP_TDDigiCnv_p1 >  AFP_TDDigiCollectionCnv_p1;

