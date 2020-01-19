/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTCOMBIOVERLAPSUMMARY_H 
#define MUON_MUONSEGMENTCOMBIOVERLAPSUMMARY_H

#include <vector>

namespace Muon{

  class MuonSegment;

  class MuonSegmentCombiOverlapSummary {
  public:
    std::vector< std::pair<const MuonSegment*,const MuonSegment*> > shared; //!< segments shared between combies
    std::vector<const MuonSegment*> uniqueFirst; //!< segments that are only part of the first combi
    std::vector<const MuonSegment*> uniqueSecond;  //!< segments that are only part of the second combi
    
    /** segments that are in both combis, but the one in the first is a subset of the second */
    std::vector< std::pair<const MuonSegment*,const MuonSegment*> > subsetFirst;  

    /** segments that are in both combis, but the second in the first is a subset of the first */
    std::vector< std::pair<const MuonSegment*,const MuonSegment*> > subsetSecond;
  };
}


#endif
