// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMRoIAuxContainer_v1.h 631511 2014-11-27 13:17:07Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMROIAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMROIAUXCONTAINER_V1_H

// STL include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for CPMRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631511 $
  /// $Date: 2014-11-27 14:17:07 +0100 (Thu, 27 Nov 2014) $  
  
  class CPMRoIAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    CPMRoIAuxContainer_v1();

  private:
    std::vector<uint32_t> roiWord;
    
  }; // class CPMRoIAuxContainer_v1 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CPMRoIAuxContainer_v1 , xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_CPMROIAUXCONTAINER_V1_H
