/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PdgToParticleHypothesis.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_PDGTOPARTICLEHYPOTHESIS_H
#define TRKEVENTPRIMITIVES_PDGTOPARTICLEHYPOTHESIS_H

// Fatras
#include "TrkEventPrimitives/ParticleHypothesis.h"
// STD
#include <cmath>  

namespace Trk {

    /** @class PdgToParticleHypothesis 

small converter from the (abs) PDG code 
to the particle hypothsis used in Tracking

@author Andreas.Salzburger@cern.ch

**/

class PdgToParticleHypothesis {

  public:
    /** Constructor */
    PdgToParticleHypothesis(){}

    /** ~Destructor */
    ~PdgToParticleHypothesis(){}

    /** Converter method : PDG -> Particle Hyptothesis */
    Trk::ParticleHypothesis  convert(int pdg, bool& stable, bool& exiting, double charge=1.) const;

    /** Converter method : PDG -> Particle Hyptothesis , w/o stable exiting*/
    Trk::ParticleHypothesis  convert(int pdg, double charge=1.) const;


    /** Converter method : Particle Hyptothesis -> PDG*/
    int convert(Trk::ParticleHypothesis particleHypo, double charge, bool dist=true) const;


};

inline Trk::ParticleHypothesis PdgToParticleHypothesis::convert(int pdg, double charge) const {
    bool stable, exiting; 
    return convert(pdg,stable,exiting,charge); 
}


inline Trk::ParticleHypothesis PdgToParticleHypothesis::convert(int pdg, bool& stable, bool& exiting, double charge) const {

    int pdgCode = abs(pdg);

    stable       = false;
    exiting      = false;

    Trk::ParticleHypothesis particleType;

  // try to follow number of appearance 
    switch (pdgCode )
    {
    // leptons
        case 11: // e+/e-
        particleType = Trk::electron;
        stable       = true;
        exiting      = false;
        break;
        case 13: // mu+/mu-
        particleType = Trk::muon;
        stable       = false;
        exiting      = false;
        break;
        case 12: // e neutrino
        case 14: // mu neutrino
        case 16: // tau neutrino
        particleType = Trk::nonInteracting;
        stable       = true;
        exiting      = true;
        break;
        case 22: // gamma
        particleType = Trk::photon;
        stable       = true;
        exiting      = false;
        break; 
        case 211: // pi+/pi-
        particleType = Trk::pion;
        stable       = false;
        exiting      = false;
        break;
        case 111: // pi0
        particleType = Trk::pi0;              
        stable       = false;
        exiting      = false;
        break;
        case 2212: // proton
        particleType = Trk::proton;
        stable       = true;
        exiting      = false;
        break;
        case 2112: // neutron               
        particleType = Trk::neutron;
        stable       = true;
        exiting      = true;
        break;
        case 321: // K
        particleType = Trk::kaon;
        stable       = false;
        exiting      = false;
        break;
        case 130: // K_long
        particleType = Trk::k0;
        stable       = false;
        exiting      = false;
        break;
        case 310: // K_short
        particleType = Trk::k0;
        stable       = false;
        exiting      = false;
        break;
        default: // treat mesons as pions
        particleType = charge != 0. ? Trk::pion : Trk::pi0 ;                               
        stable       = false;
        exiting      = false;
    }

  // and all baryons as proton hypo
    if (pdgCode > 999 && pdgCode!=2112 )
    {
        particleType = charge != 0. ? Trk::proton : Trk::neutron ;
        stable       = false;
        exiting      = false;
    }

  // ignore SUSY particles for now
    if (pdgCode > 1000000)
    {
        particleType = Trk::nonInteracting;
        stable       = true;
        exiting      = true;
    }

    return particleType;
}


inline int PdgToParticleHypothesis::convert(Trk::ParticleHypothesis particleHypo, double charge, bool dist) const 
{

    int pdg = 0;

    switch (particleHypo) {
     // the electron case
        case Trk::electron   :  {  pdg = 11; pdg *= charge > 0. ? -1 : 1;   } return pdg;  
     // the muon case
        case Trk::muon       :  {  pdg = 13; pdg *= charge > 0. ? -1 : 1;   } return pdg;  
     // the kaon case
        case Trk::kaon       :  {  pdg = 321; pdg *= charge > 0. ? 1 : -1; 
            if (charge*charge < 0.0001)
                pdg = dist ? 130 : 310;                } return pdg;
     // the proton case
            case Trk::proton     :  {  pdg = 2212; pdg *= charge > 0. ? 1 : -1; 
                if (charge*charge < 0.0001)
                    pdg = dist ? 2112 : -2112;             } return pdg;
     // the photon case
                case Trk::photon     :  { pdg = 22;                                   } return pdg;
     // the neutron case
                case Trk::neutron     :  { pdg = 2112;                                } return pdg;
     // the neutral pion case
                case Trk::pi0         :  { pdg = 111;                      } return pdg;
     // the neutral kaon case
                case Trk::k0          :  { pdg = dist ? 130 : 310;                      } return pdg;
     // the pion case - is the default
                default              :  {  pdg = 211; pdg *= charge > 0. ? 1 : -1; 
                    if (charge*charge < 0.0001)
                        pdg = 111;                             };  
                }

                return pdg;
            }


        }
#endif
