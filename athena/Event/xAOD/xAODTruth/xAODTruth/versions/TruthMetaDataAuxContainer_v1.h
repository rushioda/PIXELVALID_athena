// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRUTH_VERSIONS_TRUTHMETADATAAUXCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHMETADATAAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODTruth/TruthParticleContainer.h"

namespace xAOD {

  /// Auxiliary store for the truth meta data
  ///
  /// @author Tobias Bisanz <tobias.bisanz@cern.ch>
  ///
  /// $Revision$
  /// $Date$
  ///
  class TruthMetaDataAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    TruthMetaDataAuxContainer_v1();

  private:
    //Two vectors (of vectors) to store association between weight name and weight
    //index. No std::map is used for increased read-back speed in ROOT
    std::vector < std::vector < std::string > > weightNames;
    std::vector < uint32_t > mcChannelNumber;

  }; // class TruthMetaDataAuxContainer_v1

} // namespace xAOD


// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthMetaDataAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRUTH_VERSIONS_TRUTHMETADATAAUXCONTAINER_V1_H
