// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGSAUXCONTAINER_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGSAUXCONTAINER_V1_H

// System include(s):
extern "C" {
  #include <stdint.h>
}

// STL include(s):
#include <vector>
// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {
/// This is a fixed implementation of the trigger TrigRingerRings
/// auxiliary store. Defining every possible property that TrigRingerRings
///
/// @author Joao victor da Fonseca Pinto <joao.victor.da.fonseca.pinto@cern.ch>
///
/// $Revsision$
/// $Date: 2014-17-10$
///
   class TrigRingerRingsAuxContainer_v1: public AuxContainerBase{

   public:
      /// Default constructor
      TrigRingerRingsAuxContainer_v1();

   private:
      std::vector< std::vector< float > > rings; // Holds the generated rings.
      std::vector< long > RoIword;
      
   };
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRingerRingsAuxContainer_v1, xAOD::AuxContainerBase );


#endif // XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGSRAUXCONTAINER_V1_H
