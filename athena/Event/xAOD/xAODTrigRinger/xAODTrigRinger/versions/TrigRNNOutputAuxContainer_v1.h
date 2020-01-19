// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTAUXCONTAINER_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTAUXCONTAINER_V1_H

// System include(s):
extern "C" {
  #include <stdint.h>
}

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {
/// This is a fixed implementation of the trigger TrigRNNOutput
/// auxiliary store. Defining every possible property that TrigRNNOutput
///
/// @author Joao victor da Fonseca Pinto <joao.victor.da.fonseca.pinto@cern.ch>
///
/// $Revsision$
/// $Date: 2014-17-10$
///
   class TrigRNNOutputAuxContainer_v1: public AuxContainerBase{

   public:
      /// Default constructor
      TrigRNNOutputAuxContainer_v1();

   private:
      std::vector< std::vector<float> > decision; // Hold the decision
      std::vector<long>  RoIword;
      std::vector<float> et;

   }; // class RingerRNNOutputAuxContainer_v1
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRNNOutputAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTAUXCONTAINER_V1_H
