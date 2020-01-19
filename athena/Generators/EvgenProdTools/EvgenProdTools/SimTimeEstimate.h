/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef EVGENPRODTOOLS_SIMTIMEESTIMATE_H
#define EVGENPRODTOOLS_SIMTIMEESTIMATE_H

#include "GeneratorModules/GenBase.h"
#include <vector>

/// Algorithm to estimate the amount of CPU time that simulation will take
///
/// This algorithm runs on generated events and attempts to estimate
/// the amount of CPU time that the simulation will require to run.
///
class SimTimeEstimate : public GenBase {
public:

  SimTimeEstimate(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode execute();
  StatusCode finalize();

 private:
  double      m_etaMax;   //!< Max eta that will be used for simulation
  int         m_total_Events; //!< Number of events that I've seen
  double      m_total_Energy; //!< Amount of energy that I've seen
  double      eventEnergy; //!< Amount of energy that I've seen in one event
  std::vector<double>   particleEtas; //!< list holds eta of each particle
  std::vector<double>   particleEnergies; //!< list holds energy of each particle
  std::vector<int> particleIDs; //!< list holds all the particle IDs for one event
  int         particleNumber; //!< Number of particles in one event
  std::vector<int> pidsToSkip; // !< PIDs of particles which should not be taken into account 
  
};


#endif

#endif
