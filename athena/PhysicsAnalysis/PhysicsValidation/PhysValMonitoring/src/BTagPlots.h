/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALMONITORING_BTagPLOTS_H
#define PHYSVALMONITORING_BTagPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBTagging/BTagging.h"

namespace PhysVal{
  
class BTagPlots:public PlotBase {
    public:
    BTagPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::BTagging* btag);
      void fill(unsigned int nbtag);
      
      // Reco only information
      std::string m_sParticleType;
      TH1* n;

    private:
      virtual void initializePlots();      
};

}

#endif
