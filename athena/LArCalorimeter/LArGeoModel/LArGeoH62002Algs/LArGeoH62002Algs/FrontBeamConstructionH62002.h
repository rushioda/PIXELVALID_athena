/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FrontBeamConstructionH62002
// Return an envelope that contains the LArH62002 Beam Instrumentation.
// Apr-2006 mgf

#ifndef LARGEOH62002ALGS_FRONTBEAMCONSTRUCTIONH62002_H
#define LARGEOH62002ALGS_FRONTBEAMCONSTRUCTIONH62002_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class IRDBAccessSvc;
class IGeoModelSvc;
class LArDetDescrManager;

namespace LArGeo {

  class FrontBeamConstructionH62002 
    {
    public:
    
    FrontBeamConstructionH62002();
    virtual ~FrontBeamConstructionH62002();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    void SetManager(LArDetDescrManager* /*mgr*/){ }

    //void SetAxisVisState(bool state) {_axisVisState=state;}
      
  private:
    GeoPhysVol          *m_H62002FrontBeamPhysical;
  };

} // namespace LArGeo

#endif // LARGEOH62002ALGS_FRONTBEAMCONSTRUCTIONH62002_H
