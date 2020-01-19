/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define TRUTHTOTRACK_IMP
#include "TrkTruthToTrack/TruthToTrack.h"

#include <cmath>
#include <memory>

#include "GaudiKernel/IPartPropSvc.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"

//#include "CLHEP/Geometry/Transform3D.h"

//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/GlobalMomentum.h"

#include "HepPDT/ParticleDataTable.hh"
//#include "TrkParameters/Perigee.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/AtaPlane.h"
#include "TrkExInterfaces/IExtrapolator.h"

//================================================================
Trk::TruthToTrack::TruthToTrack(const std::string& type, const std::string& name, const IInterface* parent)
  : ::AthAlgTool(type,name,parent)
  , m_particleDataTable(0)
  , m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
{
  declareInterface<ITruthToTrack>(this);
  declareProperty("Extrapolator", m_extrapolator);
}

//================================================================
StatusCode Trk::TruthToTrack::initialize() {
  // get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  StatusCode sc =  service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not initialize Particle Properties Service");
    return StatusCode::FAILURE;
  }      
  m_particleDataTable = partPropSvc->PDT();

  if ( m_extrapolator.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_extrapolator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_extrapolator);
  }
  
  return StatusCode::SUCCESS;
}



//================================================================
const Trk::TrackParameters* Trk::TruthToTrack::makeProdVertexParameters(const HepMC::GenParticle* part) const {
  Trk::TrackParameters *result = 0;

  if(part && part->production_vertex() && m_particleDataTable) {
    HepMC::ThreeVector tv = part->production_vertex()->point3d();
    Amg::Vector3D hv(tv.x(),tv.y(),tv.z());
    Amg::Vector3D globalPos = hv;
    
    HepMC::FourVector fv = part->momentum();
    Amg::Vector3D hv2(fv.px(),fv.py(),fv.pz());
    Amg::Vector3D globalMom = hv2;
      
    int id = part->pdg_id();
    // the table seems to lack antiparticles, thus the use of abs()
    const HepPDT::ParticleData* pd = m_particleDataTable->particle(std::abs(id));

    if(pd) {
      // pd could point to an antiparticle. recover the sign:
      double charge = (id>0) ? pd->charge() : -pd->charge();
      //      Trk::PlaneSurface surface(new HepGeom::Translate3D(part->production_vertex()->point3d()));
      Amg::Translation3D tmpTransl(hv);
      Amg::Transform3D tmpTransf = tmpTransl * Amg::RotationMatrix3D::Identity(); 
      Trk::PlaneSurface surface(new Amg::Transform3D(tmpTransf));
      result = new Trk::AtaPlane(globalPos, globalMom, charge, surface);
    }
    else {
      ATH_MSG_WARNING("Could not get particle data for particle ID="<<id);
    }
  }

  return result;
}


//================================================================
const Trk::TrackParameters* Trk::TruthToTrack::makeProdVertexParameters(const xAOD::TruthParticle* part) const {
  Trk::TrackParameters *result = 0;

  if(part && part->hasProdVtx() && m_particleDataTable) {
    Amg::Vector3D hv(part->prodVtx()->x(),part->prodVtx()->y(),part->prodVtx()->z());
    Amg::Vector3D globalPos = hv;
    
    Amg::Vector3D hv2(part->p4().Px(),part->p4().Py(),part->p4().Pz());
    Amg::Vector3D globalMom = hv2;
      
    int id = part->pdgId();
    // the table seems to lack antiparticles, thus the use of abs()
    const HepPDT::ParticleData* pd = m_particleDataTable->particle(std::abs(id));

    if(pd) {
      // pd could point to an antiparticle. recover the sign:
      double charge = (id>0) ? pd->charge() : -pd->charge();
      //      Trk::PlaneSurface surface(new HepGeom::Translate3D(part->production_vertex()->point3d()));
      Amg::Translation3D tmpTransl(hv);
      Amg::Transform3D tmpTransf = tmpTransl * Amg::RotationMatrix3D::Identity(); 
      Trk::PlaneSurface surface(new Amg::Transform3D(tmpTransf));
      result = new Trk::AtaPlane(globalPos, globalMom, charge, surface);
    }
    else {
      ATH_MSG_WARNING("Could not get particle data for particle ID="<<id);
    }
  }

  return result;
}



//================================================================
const Trk::TrackParameters* Trk::TruthToTrack::makePerigeeParameters(const HepMC::GenParticle* part) const {
  const Trk::TrackParameters* generatedTrackPerigee = 0;

  if(part && part->production_vertex() && m_particleDataTable && m_extrapolator) {

    std::auto_ptr<const Trk::TrackParameters> productionVertexTrackParams( makeProdVertexParameters(part) );
    if(productionVertexTrackParams.get()) {
      
      // Extrapolate the TrackParameters object to the perigee. Direct extrapolation,
      // no material effects.
      generatedTrackPerigee = m_extrapolator->extrapolateDirectly( *productionVertexTrackParams,
								   Trk::PerigeeSurface(),
								   Trk::anyDirection,
								   false,
								   Trk::nonInteracting );
    }
  }

  return generatedTrackPerigee;
}



//================================================================
const Trk::TrackParameters* Trk::TruthToTrack::makePerigeeParameters(const xAOD::TruthParticle* part) const {
  const Trk::TrackParameters* generatedTrackPerigee = 0;

  if(part && part->hasProdVtx() && m_particleDataTable && m_extrapolator) {

    std::auto_ptr<const Trk::TrackParameters> productionVertexTrackParams( makeProdVertexParameters(part) );
    if(productionVertexTrackParams.get()) {
      
      // Extrapolate the TrackParameters object to the perigee. Direct extrapolation,
      // no material effects.
      generatedTrackPerigee = m_extrapolator->extrapolateDirectly( *productionVertexTrackParams,
								   Trk::PerigeeSurface(),
								   Trk::anyDirection,
								   false,
								   Trk::nonInteracting );
    }
  }

  return generatedTrackPerigee;
}
