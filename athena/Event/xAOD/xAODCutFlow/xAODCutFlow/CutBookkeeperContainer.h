// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCUTFLOW_CUTBOOKKEEPERCONTAINER_H
#define XAODCUTFLOW_CUTBOOKKEEPERCONTAINER_H

// Local include(s):
#include "versions/CutBookkeeperContainer_v1.h"
#include "xAODCutFlow/CutBookkeeper.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CutBookkeeperContainer class
   typedef CutBookkeeperContainer_v1 CutBookkeeperContainer;
}


// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CutBookkeeperContainer, 1234982351, 1 )

#endif // XAODCUTFLOW_CUTBOOKKEEPERCONTAINER_H
