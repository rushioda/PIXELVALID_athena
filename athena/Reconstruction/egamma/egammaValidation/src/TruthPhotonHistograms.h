/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_TRUTHPHOTONHISTOGRAMS_H
#define EGAMMAVALIDATION_TRUTHPHOTONHISTOGRAMS_H

#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"

#include "ParticleHistograms.h"

#include "TH1.h"

namespace egammaMonitoring {

  class TruthPhotonHistograms : public ParticleHistograms {
  public:

    using ParticleHistograms::ParticleHistograms;
    using ParticleHistograms::initializePlots;

    StatusCode initializePlots();

    using ParticleHistograms::fill;

    void fill(const xAOD::IParticle&);

  private:

    const xAOD::TruthParticle *m_tmp;

    float m_cR_bins[15] = {0, 50, 89, 123, 170, 210, 250, 299, 335, 371, 443, 514, 554, 800, 1085};
    float m_x, m_y, m_trueR;

  };

}

#endif
