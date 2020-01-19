/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKVERTEXASSOCIATIONMAP_H
#define TRACKVERTEXASSOCIATIONMAP_H

#include "xAODTracking/TrackParticleFwd.h"
//#include "xAODTracking/TrackParticleContainerFwd.h"
//#include "xAODTracking/VertexFwd.h"
//#include "xAODTracking/VertexContainerFwd.h"
#include <vector>
#include <map>
//using namespace std;

namespace xAOD
{
  typedef std::vector< const xAOD::TrackParticle* > TrackVertexAssociationList;
  typedef std::map< const xAOD::Vertex* , xAOD::TrackVertexAssociationList > TrackVertexAssociationMap;
}

#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackVertexAssociationList, 1111000001,1 )
CLASS_DEF( xAOD::TrackVertexAssociationMap, 1111000002,1 )
#endif

#endif
