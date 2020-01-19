/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/SegmentTag.h"

namespace MuonCombined {

   SegmentTag::SegmentTag(  const std::vector< MuonCombined::MuonSegmentInfo >& segmentsInfo ) : 
     TagBase(TagBase::Author::MuTagIMO,TagBase::Type::SegmentTagged), 
     m_segmentsInfo(segmentsInfo){}

  SegmentTag::~SegmentTag() {
    //     for( auto x : m_segmentsInfo ) delete &x;
  }

  std::vector<const Muon::MuonSegment*> SegmentTag::associatedSegments() const {
    std::vector<const Muon::MuonSegment*> segments;
    for( const auto& info : m_segmentsInfo ) {
      if( info.segment ) segments.push_back(info.segment);
    }
    return segments;
  }

}
