/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Author:   M.King(Kobe)
// Feb.  2011
//
// DESCRIPTION:
// Subject: TGC Efficiency     -->TGC Efficiency plots including EIFI by comparing with MDT Segments
///////////////////////////////////////////////////////////////////////////////////////////// 

#ifndef MdtVsTgcRawDataValAlg_SegmTrack
#define MdtVsTgcRawDataValAlg_SegmTrack

#include "MuonSegment/MuonSegment.h"

// Class to hold pointers to Segments which have been matched into a track, these Segments are not owned by this class
// Holds an array of 4 MuonSegment pointers, one in each MDT station
// Pointer is equal to zero if there is no matching segment in that station
class SegmTrack{
 public:
  // Constructor, sets all pointers to zero and side to -1
  SegmTrack(){
    dSegments[0] = 0;
    dSegments[1] = 0;
    dSegments[2] = 0;
    dSegments[3] = 0;
    dAC=-1;
  }
  
  // Constructor, sets all pointers individually and gets side from them
  SegmTrack(const Muon::MuonSegment *pSegm0, const Muon::MuonSegment *pSegm1, const Muon::MuonSegment *pSegm2, const Muon::MuonSegment *pSegm3){
    dSegments[0] = pSegm0;
    dSegments[1] = pSegm1;
    dSegments[2] = pSegm2;
    dSegments[3] = pSegm3;
    SetSide();
  }
  
  // Constructor, sets all pointers from array and gets side from them
  SegmTrack(const Muon::MuonSegment *p[4]){
    dSegments[0] = p[0];
    dSegments[1] = p[1];
    dSegments[2] = p[2];
    dSegments[3] = p[3];
    SetSide();
  }
  
  // Constructor, effectively copies the pointers from a different SegmTrack
  // Pointers in new SegmTrack still point to the same actual Segments as the original
  SegmTrack(const SegmTrack &p){
    dSegments[0] = p.at(0);
    dSegments[1] = p.at(1);
    dSegments[2] = p.at(2);
    dSegments[3] = p.at(3);
    dAC = p.Side();
  }
  
  SegmTrack & operator=( const SegmTrack& p ) {
    dSegments[0] = p.at(0);
    dSegments[1] = p.at(1);
    dSegments[2] = p.at(2);
    dSegments[3] = p.at(3);
    dAC = p.Side();
    return *this;
  }

  // Indexer, gets element from pointer array
  const Muon::MuonSegment* operator[](int j) const{
    return at(j);
  }
  // At function, gets element from pointer array
  const Muon::MuonSegment* at(int j) const{
    if(j>3||j<0)return 0;
    return dSegments[j];
  }
  // Gets side, variable determined using the SetSide function
  int Side() const{
    return dAC;
  }
  
 private:
  // SetSide function, sets dAC variable using dSegments array
  void SetSide();
  // Variables containing the side index and the Segments in this track
  int dAC;
  const Muon::MuonSegment *dSegments[4];
};

// is equal operator, returns true if the pointers point to the same segments
inline bool operator==(const SegmTrack a, const SegmTrack b){
  for(int j=0;j<4;j++){
    if(a.at(j)!=b.at(j))return false;
  }
  return true;
}
// SetSide function
inline void SegmTrack::SetSide(){
  int AC=-1;
  // Loop over segments/stations, ignore empty pointers
  for(int jMDT=0;jMDT<4;jMDT++){
    if(dSegments[jMDT]==0)continue;
    // Get Side from segment
    int tempAC = (dSegments[jMDT]->globalPosition().eta()<0);
    if(AC==-1){// If first segment
      AC = tempAC;
    }
    else if(AC!=tempAC){
      // If two segments give different side values in the same track
      std::cout<<"ERROR: SegmTrack: Segments are on different sides"<<std::endl;
      AC=-1;
      break;
    }
  }
  dAC=AC;
}

#endif
