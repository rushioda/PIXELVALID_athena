/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlignableTrackingVolume.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/AlignableTrackingVolume.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkVolumes/VolumeBounds.h"

Trk::AlignableTrackingVolume::AlignableTrackingVolume() :
  Trk::TrackingVolume(),
  m_alignedTV(0),
  m_sampleID(0),
  m_alignment(0),
  m_binnedMaterial(nullptr)
{}

Trk::AlignableTrackingVolume::AlignableTrackingVolume(Amg::Transform3D* htrans,
						      Amg::Transform3D* align,
						      VolumeBounds*   volbounds,
						      const Trk::BinnedMaterial* matprop,
						      int sampleID,
						      const std::string& volumeName):
  Trk::TrackingVolume(htrans, volbounds,*matprop,0,0,volumeName),
  m_alignedTV(0),
  m_sampleID(sampleID),
  m_alignment(align),
  m_binnedMaterial(matprop) 
{
  if (m_alignment) m_alignedTV = this->cloneTV(*m_alignment); 
} 
 
Trk::AlignableTrackingVolume::~AlignableTrackingVolume()
{
  // the volume does not own entry/exit surfaces 
  delete m_alignedTV;
  delete m_alignment;
  delete m_binnedMaterial;
}

const Trk::TrackingVolume* Trk::AlignableTrackingVolume::alignedTrackingVolume() const
{
  if ( m_alignedTV ) return m_alignedTV; 
  else return this;  
}
