/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasControl_SimControl_H
#define G4AtlasControl_SimControl_H

#include <string>

/// @class SimControl
/// @brief C++ class used for passing configuration
/// from the python layer to C++ layer.
///
/// This code was originally written to allow configuration of G4 from
/// the python layer prior to the introduction of configurable
/// properties in Athena classes. Once the FADS migration
/// (ATLASSIM-2256) is complete all of this code should be obsolete.
class SimControl
{
public:
  /// Empty Constructor
  SimControl();
  /// Empty Destructor
  virtual ~SimControl();
  /// Still used from PyG4Atlas.G4AtlasEngine to initialize Geant4.
  void initializeG4(bool isMT=false) const;
};

#endif
