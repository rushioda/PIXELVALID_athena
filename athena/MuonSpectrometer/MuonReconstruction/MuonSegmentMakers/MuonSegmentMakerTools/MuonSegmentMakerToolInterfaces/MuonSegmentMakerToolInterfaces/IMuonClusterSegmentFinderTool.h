/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONROADFINDERTOOL_H
#define MUON_IMUONROADFINDERTOOL_H
 
#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include <vector>

static const InterfaceID IID_IMuonClusterSegmentFinderTool("Muon::IMuonClusterSegmentFinderTool", 1, 0);

namespace Trk {
  class Track;
}

namespace Muon {

  class IMuonClusterSegmentFinderTool : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    virtual std::vector<const Muon::MuonSegment*>* find(std::vector< const Muon::MuonClusterOnTrack* >& clusters) = 0;

  };
  
  inline const InterfaceID& IMuonClusterSegmentFinderTool::interfaceID()
  {
    return IID_IMuonClusterSegmentFinderTool;
  }

} // end of name space

#endif 

