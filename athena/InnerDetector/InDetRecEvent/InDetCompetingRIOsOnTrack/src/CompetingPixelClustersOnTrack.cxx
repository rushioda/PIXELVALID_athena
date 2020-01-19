/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingPixelClustersOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"
#include <math.h>

// default constructor
InDet::CompetingPixelClustersOnTrack::CompetingPixelClustersOnTrack():
        Trk::CompetingRIOsOnTrack(),
        m_globalPosition(0),
        m_containedChildRots(0)
        {}

// copy constructor
InDet::CompetingPixelClustersOnTrack::CompetingPixelClustersOnTrack(const InDet::CompetingPixelClustersOnTrack& compROT) :
        Trk::CompetingRIOsOnTrack(compROT),
        m_globalPosition(compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0),
        m_containedChildRots(0) {
    m_containedChildRots = new std::vector< const InDet::PixelClusterOnTrack* >;
    std::vector< const InDet::PixelClusterOnTrack* >::const_iterator rotIter = compROT.m_containedChildRots->begin();
    for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter) {
        m_containedChildRots->push_back((*rotIter)->clone());
    }
}

// explicit constructor
InDet::CompetingPixelClustersOnTrack::CompetingPixelClustersOnTrack(
    std::vector<const InDet::PixelClusterOnTrack*>* childrots,
    std::vector<AssignmentProb>* assgnProb
    //const Trk::LocalParameters* effecLocalPars,
    //const Trk::ErrorMatrix* effecLocalErrMat
    //int ROTsHaveComSrfc // meaning of the values are described in the definition of ROTsHaveCommonSurface()
    )
    :
    Trk::CompetingRIOsOnTrack( assgnProb),
    //m_localParameters(effecLocalPars);
    //m_localErrorMatrix(effecLocalErrMat);
    m_globalPosition(0),
    m_containedChildRots(childrots)
{
  // initialize local position and error matrix
  setLocalParametersAndErrorMatrix();
}

InDet::CompetingPixelClustersOnTrack& InDet::CompetingPixelClustersOnTrack::operator=(const InDet::CompetingPixelClustersOnTrack& compROT) {
    if (this!=&compROT) {
        // assignment operator of base class
        Trk::CompetingRIOsOnTrack::operator=(compROT);
        // clear rots
        clearChildRotVector();
        delete m_containedChildRots;
        delete m_globalPosition;
        m_containedChildRots = new std::vector<const InDet::PixelClusterOnTrack*>;
        m_globalPosition     = compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0;
        //m_ROTsHaveCommonSurface     = compROT.m_ROTsHaveCommonSurface;
        std::vector<const InDet::PixelClusterOnTrack*>::const_iterator rotIter = compROT.m_containedChildRots->begin();
        for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter)
            m_containedChildRots->push_back((*rotIter)->clone());
    }
    return (*this);
}

InDet::CompetingPixelClustersOnTrack& InDet::CompetingPixelClustersOnTrack::operator=(InDet::CompetingPixelClustersOnTrack&& compROT) {
    if (this!=&compROT) {
        // move operator of base class
        Trk::CompetingRIOsOnTrack::operator=(std::move(compROT));
        // clear rots
        clearChildRotVector();
        delete m_containedChildRots;
        delete m_globalPosition;
        m_containedChildRots = compROT.m_containedChildRots;
        compROT.m_containedChildRots = nullptr;

        m_globalPosition = compROT.m_globalPosition;
        compROT.m_globalPosition = nullptr;
    }
    return (*this);
}

InDet::CompetingPixelClustersOnTrack::~CompetingPixelClustersOnTrack() {
    delete m_globalPosition;
    clearChildRotVector();
    delete m_containedChildRots;
}

void InDet::CompetingPixelClustersOnTrack::clearChildRotVector() {
    std::vector<const InDet::PixelClusterOnTrack*>::const_iterator rotIter = m_containedChildRots->begin();
    for (; rotIter!=m_containedChildRots->end(); ++rotIter)
        delete (*rotIter);
}

MsgStream& InDet::CompetingPixelClustersOnTrack::dump( MsgStream& out ) const {
  using std::ios;
  out << "Trk::CompetingPixelClustersOnTrack with [" << numberOfContainedROTs() 
        << "] competing Pixel RIO_OnTrack objects" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<std::endl;
  else out << *m_globalPosition<<endreq;
  return out;
}

std::ostream& InDet::CompetingPixelClustersOnTrack::dump( std::ostream& out ) const {
  using std::ios;
  out << "Trk::CompetingPixelClustersOnTrack with [" << numberOfContainedROTs() 
        << "] competing Pixel RIO_OnTrack objects" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<std::endl;
  else out << *m_globalPosition<<std::endl;
  return out;
}

// Have all the contained ROTs a common associated surface?
bool InDet::CompetingPixelClustersOnTrack::ROTsHaveCommonSurface(const bool) const {
    return true;
}

