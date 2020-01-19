/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINED_SORTINDETCANDIDATES
#define MUONCOMBINED_SORTINDETCANDIDATES

#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/StacoTag.h"
#include "MuonCombinedEvent/CombinedFitTag.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "MuonCombinedEvent/CaloTag.h"
#include "MuonCombinedEvent/MuGirlTag.h"

namespace MuonCombined {


  class SortInDetCandidates {
  public:
    bool operator()( const InDetCandidate* c1, const InDetCandidate* c2 ) const {
      return this->operator()(*c1,*c2);
    }
    bool operator()( const InDetCandidate& c1, const InDetCandidate& c2 ) const {
      
      if( c1.combinedDataTags().empty() ){
        if( c2.combinedDataTags().empty() ) return false;      
        return true;
      }
      if( c2.combinedDataTags().empty() ) return false;
     
      const TagBase& t1 = *c1.combinedDataTags().front();
      const TagBase& t2 = *c2.combinedDataTags().front();
      // compare based on author/type
      bool s1 = t1 < t2;
      bool s2 = t2 < t1;
      if( s1 && !s2 ) return true;
      if( s2 && !s1 ) return false;

      // if we get here we have the same author and type
            // this is not too elegant, maybe rethink implementation
      if( t1.type() == xAOD::Muon::Combined ){
	
	// work out type of tag
	const CombinedFitTag* cbFitTag1 = dynamic_cast<const CombinedFitTag*>(&t1);
	if( cbFitTag1 ) {
	  const CombinedFitTag* cbFitTag2 = dynamic_cast<const CombinedFitTag*>(&t2);
	  if( !cbFitTag2 ) return false;
	  return *cbFitTag1 < *cbFitTag2;
	}
	const StacoTag* stacoTag1 = dynamic_cast<const StacoTag*>(&t1);
	if( stacoTag1 ) {
	  const StacoTag* stacoTag2 = dynamic_cast<const StacoTag*>(&t2);
	  if( !stacoTag2 ) return false;
	  return *stacoTag1 < *stacoTag2;
	}
	const MuGirlTag* muGirlTag1 = dynamic_cast<const MuGirlTag*>(&t1);
	if( muGirlTag1 ) {
	  const MuGirlTag* muGirlTag2 = dynamic_cast<const MuGirlTag*>(&t2);
	  if( !muGirlTag2 ) return false;
	  return *muGirlTag1 < *muGirlTag2;	    
	}
	return false;
      }else if( t1.type() == xAOD::Muon::SegmentTagged ){
	const SegmentTag* segmentTag1 = dynamic_cast<const SegmentTag*>(&t1);
	if( segmentTag1 ) {
	  const SegmentTag* segmentTag2 = dynamic_cast<const SegmentTag*>(&t2);
	  if( !segmentTag2 ) return false;
	  return *segmentTag1 < *segmentTag2;	    
	}
	const MuGirlTag* muGirlTag1 = dynamic_cast<const MuGirlTag*>(&t1);
	if( muGirlTag1 ) {
	  const MuGirlTag* muGirlTag2 = dynamic_cast<const MuGirlTag*>(&t2);
	  if( !muGirlTag2 ) return false;
	  return *muGirlTag1 < *muGirlTag2;	    
	}
	return false;
      }else if( t1.type() == xAOD::Muon::CaloTagged ){
	const CaloTag* caloTag1 = dynamic_cast<const CaloTag*>(&t1);
	if( caloTag1 ) {
	  const CaloTag* caloTag2 = dynamic_cast<const CaloTag*>(&t2);
	  if( !caloTag2 ) return false;
	  return *caloTag1 < *caloTag2;	    
	}
	return false;
      }
      return false;
    }
  };

}

#endif
