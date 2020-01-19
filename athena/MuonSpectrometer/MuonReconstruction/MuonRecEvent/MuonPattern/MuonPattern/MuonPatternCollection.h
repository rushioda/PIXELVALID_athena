/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPATTERNCOLLECTION_H
#define MUON_MUONPATTERNCOLLECTION_H

#include "DataModel/DataVector.h"
#include "MuonPattern/MuonPattern.h"
#include "MuonPattern/MuonPrdPattern.h"
#include "CLIDSvc/CLASS_DEF.h"

/**
This typedef represents a collection of MuonPatternCombination objects. 
It is a DataVector. It can be saved
to storegate and persistified using POOL.
*/
typedef DataVector<Muon::MuonPattern> MuonPatternCollection;
CLASS_DEF(MuonPatternCollection , 1141514 , 1 )

typedef DataVector<Muon::MuonPrdPattern> MuonPrdPatternCollection;
CLASS_DEF(MuonPrdPatternCollection , 1141515 , 1 )

#endif
