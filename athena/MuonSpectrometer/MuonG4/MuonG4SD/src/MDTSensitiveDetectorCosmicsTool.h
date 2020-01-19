/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSensitiveDetectorCosmicsTool_H
#define MDTSensitiveDetectorCosmicsTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class MDTSensitiveDetectorCosmicsTool : public SensitiveDetectorBase {

public:
    MDTSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~MDTSensitiveDetectorCosmicsTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
