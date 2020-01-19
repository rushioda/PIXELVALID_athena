/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VxTrackAtVertexTrkTrackAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/VxTrackAtVertexTrkTrackAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a VxTrackAtVertex to a TrkTrack.
 */

#include "VxTrackAtVertexTrkTrackAssociationTool.h"

#include <VxVertex/VxTrackAtVertex.h>
#include <TrkTrack/Track.h>
#include <TrkTrack/LinkToTrack.h>
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VxTrackAtVertexTrkTrackAssociationTool::VxTrackAtVertexTrkTrackAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::Track *VxTrackAtVertexTrkTrackAssociationTool::get (const Trk::VxTrackAtVertex& track)
{

  const Trk::LinkToTrack *link = dynamic_cast<const Trk::LinkToTrack*>(track.trackOrParticleLink());
  if(0 == link){  // it's not a link to TrkTrack
    REPORT_MESSAGE (MSG::WARNING) << "Trying to associate a Trk::Track to a vertex"
				  << " while the vertex was not fitted using Trk::Track";
    return 0;
  }

  const Trk::Track *trktrack = **link;
  if(0 == trktrack){
    REPORT_MESSAGE (MSG::WARNING) << "link from VxCandidate to TrkTrack is broken";
    return 0;
  }

  return trktrack;
}

} // namespace D3PD
