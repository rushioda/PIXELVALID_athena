/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_PARAMPLOTS_H
#define TRKVALHISTUTILS_PARAMPLOTS_H

#include "PlotBase.h"
#include "xAODBase/IParticle.h"

namespace Trk{

class ParamPlots:public PlotBase {
  public:      
    ParamPlots(PlotBase *pParent, std::string sDir, std::string sParticleType);
    void fill(const xAOD::IParticle& prt);

    TH1* eta;
    TH1* phi;
    TH1* pt;

    TH1* eta_phi;
    TH1* eta_pt;

  private:
    void initializePlots();
    std::string m_sParticleType;
};

}

#endif // TRKVALHISTUTILS_PARAMPLOTS_H

