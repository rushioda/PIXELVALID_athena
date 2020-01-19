/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHALG_H
#define TRACKPARTICLETRUTHALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"

class IMCTruthClassifier;

class TrackParticleTruthAlg: public AthAlgorithm {
public:
  TrackParticleTruthAlg(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

  
  SG::ReadHandle<xAOD::TrackParticleContainer> m_particles;//std::string m_trackParticleName;  /// TrackParticle input name
  SG::ReadHandle<xAODTruthParticleLinkVector>  m_truthParticleLinkVec;//std::string m_truthLinkVecName;   /// link vector to map HepMC onto xAOD truth
  SG::ReadHandle<TrackTruthCollection> m_truthTracks;//std::string m_truthName;          /// Track(Particle)TruthCollection input name
  
  
  ToolHandle<IMCTruthClassifier> m_truthClassifier;
};

#endif/*TRACKTRUTHSELECTOR_H*/
