/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCurvedSegmentCombiner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUONMUONCURVEDSEGMENTCOMBINER_H
#define IMUONMUONCURVEDSEGMENTCOMBINER_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

namespace Muon 
{
  static const InterfaceID IID_IMuonCurvedSegmentCombiner("Muon::IMuonCurvedSegmentCombiner", 1, 0);

  class IMuonCurvedSegmentCombiner : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual MuonSegmentCombinationCollection* combineSegments(  const MuonSegmentCombinationCollection& mdtCombiColl, 
                                                                const MuonSegmentCombinationCollection& csc4DCombiColl, 
                                                                const MuonSegmentCombinationCollection& csc2DCombiColl)=0;
  };

  inline const InterfaceID& Muon::IMuonCurvedSegmentCombiner::interfaceID()
    { 
      return IID_IMuonCurvedSegmentCombiner; 
    }

} // end of namespace

#endif 
