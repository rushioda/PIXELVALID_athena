/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALMONITORING_KINEMATICSPLOTS_H
#define PHYSVALMONITORING_KINEMATICSPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticle.h"

namespace PhysVal{
  
class KinematicsPlots:public PlotBase {
    public:
      KinematicsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::IParticle* part);
      void fill();

      void initializeEvent();
      
      // Reco only information
      std::string m_sParticleType;

      TH1* n;

      TH1* et;
      TH1* eta;
      TH1* phi;

      TH1* lead_et;
      TH1* lead_eta;
      TH1* lead_phi;

    private:

      const xAOD::IParticle* lead = nullptr;
      unsigned int npart = 0;

      virtual void initializePlots();

      
};

}

#endif
