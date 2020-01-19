/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file TrkTrackFakeWriter.cxx
*
* @brief Test Algorithm for POOL I/O uses TrkTracks as test data
*
* @author Ed Moyse <Edward.Moyse@cern.ch>
* @author RD Schaffer <R.D.Schaffer@cern.ch>
*
* $Id: TrkTrackFakeWriter.cxx,v 1.10 2008-12-02 11:25:25 pkirill Exp $
*
*/

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "TrkTrackFakeWriter.h"


// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"

// include Tracking EDM
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/DiamondBounds.h"

// include MaterialEffects classes
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

// include InDet EDM
// #include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiCluster.h"
#include "Identifier/IdentifierHash.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include <map>

// Constructor with parameters:
TrkTrackFakeWriter::TrkTrackFakeWriter(const std::string &name, 
ISvcLocator *pSvcLocator) 
:
AthAlgorithm(name,pSvcLocator),
  m_pixMgrLocation("Pixel"),
  m_pixMgr(0),
  m_eventcounter(0),
  m_doInDet(true),
  m_addBrokenTracks(true)
{
  declareProperty("WithInDetEDM",m_doInDet,"flag if to add indet things to fake track");
  declareProperty("AddBrokenTracks",m_addBrokenTracks,"flag if to add broken objects to fake track (i.e. to test convertors)");
}

// Initialize method:
StatusCode TrkTrackFakeWriter::initialize()
{
  // Get the messaging service, print where you are
  ATH_MSG_INFO( "TrkTrackFakeWriter::initialize()"  );

  if (m_doInDet){
    ATH_CHECK( detStore()->retrieve(m_pixMgr, m_pixMgrLocation) );
  }

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode TrkTrackFakeWriter::execute() 
{
  ++m_eventcounter;
  ATH_MSG_DEBUG( "TrkTrackFakeWriter::execute()"  );

  using namespace Trk;
  using namespace InDet;
//create a demo track collection
  TrackCollection* newTracks = new TrackCollection;

  Trk::Track* track = FakeTrackBuilder::buildTrack(m_pixMgr);
  newTracks->push_back(track);
  ATH_MSG_VERBOSE((*track) );
  
  if (m_addBrokenTracks) {
    Trk::Track* track = FakeTrackBuilder::buildBrokenTrack(m_pixMgr);
    newTracks->push_back(track);
  }

  ATH_CHECK( evtStore()->record(newTracks, "Tracks", false) );
  ATH_MSG_DEBUG( "TrkTrackFakeWriter::execute() ended"  );

//now try PRD collections
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode TrkTrackFakeWriter::finalize() 
{
  ATH_MSG_INFO( "TrkTrackFakeWriter::finalize()"  );
  return StatusCode::SUCCESS;
}

