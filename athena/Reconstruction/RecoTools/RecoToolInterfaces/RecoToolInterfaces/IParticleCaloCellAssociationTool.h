/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleCaloCellAssociationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECALOCELLASSOCIATIONTOOL_H
#define ITRKTRACKPARTICLECALOCELLASSOCIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include <vector>
#include "ParticleCaloExtension/ParticleCellAssociation.h"

class CaloCellContainer;


namespace Rec 
{
  

  static const InterfaceID IID_IParticleCaloCellAssociationTool("Rec::IParticleCaloCellAssociationTool", 1, 0);

  /** @brief Interface for extending Particles with calo cells */
  class IParticleCaloCellAssociationTool : virtual public IAlgTool {
  public:

    /** Method to get the ParticleCellAssociation of a given TrackParticle
        @param trackParticle  input track particle
        @param extension      reference to a pointer to a ParticleCellAssociation, will be updated if call is successfull
                              NEVER delete the pointer, you will cause a crash! 
        @param dr             cone size used for the association
                              If caching is enabled, the cells associated to the association contain at least all cells
                              in dr but could contain more. Users ALWAYS have to recalculate the associated cells in their cone.
        @param container      cell container to be used if provided 
        @param useCaching     configure whether the tool caches the result on the track particle
                              The default behavior is 'true' to ensure optimal performance
                              If caching is enabled, the code will perform a consistency check on the container pointer
                              If the function is called twice on the same particle with different containers, the call will fail.
                              The same is true if the function is called once without container and once with on the same particle.
        @return true if the call was successful
    */
    virtual bool particleCellAssociation( const xAOD::IParticle& particle,  const ParticleCellAssociation*& association, float dr, 
                                          const CaloCellContainer* container = 0, bool useCaching = true ) const = 0;

    static const InterfaceID& interfaceID( ) ;
  };

  inline const InterfaceID& IParticleCaloCellAssociationTool::interfaceID() {
    return IID_IParticleCaloCellAssociationTool; 
  }

} // end of namespace

#endif 
