/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTruthToTrack/TruthTrackRecordToTrack.h"

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
//#include "TrkParameters/AtaPlane.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"

//================================================================
Trk::TruthTrackRecordToTrack::TruthTrackRecordToTrack(const std::string& type, const std::string& name,
                                            const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_particleDataTable(0), 
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
{
  declareInterface<ITruthToTrack>(this);

  declareProperty("Extrapolator", m_extrapolator);
  declareProperty("TrackRecordKey",m_reccollkey="CosmicPerigee");
}

//================================================================
StatusCode Trk::TruthTrackRecordToTrack::initialize() {

  // get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  StatusCode sc =  service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not initialize Particle Properties Service");
    return StatusCode::FAILURE;
  }      
  m_particleDataTable = partPropSvc->PDT();

  if ( m_extrapolator.retrieve().isFailure() ) {
    ATH_MSG_FATAL ("Failed to retrieve tool " << m_extrapolator );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_extrapolator);
  }
  
  return StatusCode::SUCCESS;
}

//================================================================
const Trk::TrackParameters* Trk::TruthTrackRecordToTrack::makeProdVertexParameters(const HepMC::GenParticle* part) const {

  if (part == NULL || m_particleDataTable==NULL) return 0;

  Trk::TrackParameters *result = 0;
  Amg::Vector3D prodVertexVector;
  Amg::Vector3D globalPos;
  Amg::Vector3D globalMom;
  int id=0;
  double charge = 0.0;
  const HepPDT::ParticleData* pd = 0;

  /* // -- backup: get the genparticle prod vertex
  if(part->production_vertex()) {
    HepMC::ThreeVector tv = part->production_vertex()->point3d();
    prodVertexVector = CLHEP::Hep3Vector(tv.x(),tv.y(),tv.z());
    globalPos = prodVertexVector;

    HepMC::FourVector fv = part->momentum();
    CLHEP::Hep3Vector hv2(fv.px(),fv.py(),fv.pz());
    globalMom = hv2;

    id = part->pdg_id();
    // the table seems to lack antiparticles, thus the use of abs()
    pd = m_particleDataTable->particle(std::abs(id));

  }*/

  const TrackRecordCollection* recordCollection;
      
  StatusCode sc=evtStore()->retrieve(recordCollection, m_reccollkey);
  if (sc==StatusCode::FAILURE) {
    ATH_MSG_ERROR ("Could not get track record!");
    return 0;
  }
  ATH_MSG_DEBUG("reading from track record, size=" << recordCollection->size());

  if (recordCollection->size() == 0) ATH_MSG_WARNING ("action required but record size is 0");

  for (TrackRecordCollection::const_iterator record = recordCollection->begin();  record != recordCollection->end();++record){
          
    if ( (*record).GetBarCode() == part->barcode() ) {

      id = (*record).GetPDGCode();
      pd = m_particleDataTable->particle(std::abs(id));
      if (!pd) {
        ATH_MSG_WARNING ("found barcode but could not digest pdg_id. " <<
                         part->barcode() << " , " << id);
        continue;
      }

      HepMC::ThreeVector tv = (*record).GetPosition();
      prodVertexVector = Amg::Vector3D(tv.x(),tv.y(),tv.z());
      globalPos = prodVertexVector;

      Amg::Vector3D hv2((*record).GetMomentum().x(), (*record).GetMomentum().y(),
                            (*record).GetMomentum().z());
      globalMom = hv2;

      ATH_MSG_DEBUG("found barcode " << part->barcode() << " with pdg ID " <<
                    id << ", momentum " << hv2 << " production " << globalPos);
      

    } // if barcodes match
  }   // loop over G4 records

  if (pd) {
    charge = (id>0) ? pd->charge() : -pd->charge();

    Amg::Translation3D prodSurfaceCentre( prodVertexVector.x(),
					  prodVertexVector.y(),
					  prodVertexVector.z() );
      
    Amg::Transform3D tmpTransf =  prodSurfaceCentre *  Amg::RotationMatrix3D::Identity();

    Trk::PlaneSurface planeSurface(&tmpTransf, 5., 5. );
    result = new Trk::AtaPlane(globalPos, globalMom, charge, planeSurface);

  } else {
    ATH_MSG_WARNING ("Could not get particle data for particle ID="<<id);
  }
  return result;
}



//================================================================
const Trk::TrackParameters* Trk::TruthTrackRecordToTrack::makeProdVertexParameters(const xAOD::TruthParticle* part) const {

  if (part == NULL || m_particleDataTable==NULL) return 0;

  Trk::TrackParameters *result = 0;
  Amg::Vector3D prodVertexVector;
  Amg::Vector3D globalPos;
  Amg::Vector3D globalMom;
  int id=0;
  double charge = 0.0;
  const HepPDT::ParticleData* pd = 0;

  const TrackRecordCollection* recordCollection;      
  StatusCode sc=evtStore()->retrieve(recordCollection, m_reccollkey);
  if (sc==StatusCode::FAILURE) {
    ATH_MSG_ERROR ("Could not get track record!");
    return 0;
  }
  ATH_MSG_DEBUG("reading from track record, size=" << recordCollection->size());

  if (recordCollection->size() == 0) ATH_MSG_WARNING ("action required but record size is 0");

  for (TrackRecordCollection::const_iterator record = recordCollection->begin();  record != recordCollection->end();++record){
          
    if ( (*record).GetBarCode() == part->barcode() ) {

      id = (*record).GetPDGCode();
      pd = m_particleDataTable->particle(std::abs(id));
      if (!pd) {
        ATH_MSG_WARNING ("found barcode but could not digest pdg_id. " <<
                         part->barcode() << " , " << id);
        continue;
      }

      HepMC::ThreeVector tv = (*record).GetPosition();
      prodVertexVector = Amg::Vector3D(tv.x(),tv.y(),tv.z());
      globalPos = prodVertexVector;

      Amg::Vector3D hv2((*record).GetMomentum().x(), (*record).GetMomentum().y(),
                            (*record).GetMomentum().z());
      globalMom = hv2;

      ATH_MSG_DEBUG("found barcode " << part->barcode() << " with pdg ID " <<
                    id << ", momentum " << hv2 << " production " << globalPos);
      

    } // if barcodes match
  }   // loop over G4 records

  if (pd) {
    charge = (id>0) ? pd->charge() : -pd->charge();

    Amg::Translation3D prodSurfaceCentre( prodVertexVector.x(),
					  prodVertexVector.y(),
					  prodVertexVector.z() );
      
    Amg::Transform3D tmpTransf =  prodSurfaceCentre *  Amg::RotationMatrix3D::Identity();

    Trk::PlaneSurface planeSurface(&tmpTransf, 5., 5. );
    result = new Trk::AtaPlane(globalPos, globalMom, charge, planeSurface);

  } else {
    ATH_MSG_WARNING ("Could not get particle data for particle ID="<<id);
  }
  return result;
}



//================================================================
const Trk::TrackParameters* Trk::TruthTrackRecordToTrack::makePerigeeParameters(const HepMC::GenParticle* part) const {
  const Trk::TrackParameters* generatedTrackPerigee = 0;

  if(part && part->production_vertex() && m_particleDataTable && m_extrapolator) {
    
    MsgStream log(msgSvc(), name());
    
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
const Trk::TrackParameters* Trk::TruthTrackRecordToTrack::makePerigeeParameters(const xAOD::TruthParticle* part) const {
  const Trk::TrackParameters* generatedTrackPerigee = 0;

  if(part && part->hasProdVtx() && m_particleDataTable && m_extrapolator) {
    
    MsgStream log(msgSvc(), name());
    
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
