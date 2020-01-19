/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//====================================================================
//  StoreGateExample_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : StoreGate/example
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "SGRead.h"
#include "SGWrite.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( SGRead )
DECLARE_ALGORITHM_FACTORY( SGWrite )

DECLARE_FACTORY_ENTRIES(SGTutorial) {
    DECLARE_ALGORITHM( SGRead )
    DECLARE_ALGORITHM( SGWrite )
}
