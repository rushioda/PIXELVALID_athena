/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TILEMUFEATURECONTAINER
#define TRIG_TILEMUFEATURECONTAINER

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "TrigMuonEvent/TileMuFeature.h"
#include "SGTools/BaseInfo.h"

class TileMuFeatureContainer : public DataVector<TileMuFeature> { }; 

CLASS_DEF( TileMuFeatureContainer , 1297000983 , 1 )

SG_BASE(TileMuFeatureContainer, DataVector<TileMuFeature>);

#endif 
