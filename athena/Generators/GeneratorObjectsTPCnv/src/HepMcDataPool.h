///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMcDataPool.h
// Header file for a set of utilities for DataPool w/ HepMC classes
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef GENERATOROBJECTSATHENAPOOL_HEPMCDATAPOOL_H
#define GENERATOROBJECTSATHENAPOOL_HEPMCDATAPOOL_H

// HepMC / CLHEP includes
#define private public
#define protected public
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#undef private
#undef protected

#include "AthAllocators/DataPool.h"

// specialization of the destruction functions for our various DataPools
// these specializations are needed because we have to work-around the
// 'shared' ownership of particles and vertices by both GenEvent and the
// various DataPool<Xyz>.
namespace SG {

  template<>
  inline void
  ArenaAllocatorBase::destroy_fcn<HepMC::GenParticle>(ArenaAllocatorBase::pointer p)
  {
    HepMC::GenParticle* part = reinterpret_cast<HepMC::GenParticle*>(p);
    part->m_production_vertex = 0;
    part->m_end_vertex = 0;
    part->~GenParticle();
  }

  template<>
  inline void
  ArenaAllocatorBase::destroy_fcn<HepMC::GenVertex>(ArenaAllocatorBase::pointer p)
  {
    HepMC::GenVertex* vtx = reinterpret_cast<HepMC::GenVertex*>(p);
    vtx->m_event = 0;
    vtx->m_particles_in.clear();
    vtx->m_particles_out.clear();
    vtx->~GenVertex();
  }

  template<>
  inline void
  ArenaAllocatorBase::destroy_fcn<HepMC::GenEvent>(ArenaAllocatorBase::pointer p)
  {
    HepMC::GenEvent* evt = reinterpret_cast<HepMC::GenEvent*>(p);
    evt->m_particle_barcodes.clear();
    evt->m_vertex_barcodes.clear();
    delete evt->m_pdf_info; evt->m_pdf_info = 0;
    evt->~GenEvent();
  }
} // end namespace SG

namespace HepMC {
  struct DataPool {

    typedef ::DataPool<HepMC::GenEvent> GenEvtPool_t;
    /// an arena of @c HepMC::GenEvent for efficient object instantiation
    GenEvtPool_t evt;

    typedef ::DataPool<HepMC::GenVertex> GenVtxPool_t;
    /// an arena of @c HepMC::GenVertex for efficient object instantiation
    GenVtxPool_t vtx;

    typedef ::DataPool<HepMC::GenParticle> GenPartPool_t;
    /// an arena of @c HepMC::GenParticle for efficient object instantiation
    GenPartPool_t part;
  };

} // end namespace HepMC

#endif // GENERATOROBJECTSATHENAPOOL_HEPMCDATAPOOL_H
