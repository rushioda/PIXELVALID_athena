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

#include "MdtVsTgcRawDataMonitoring/MdtVsTgcRawDataValAlg.h"

#include "TrkEventPrimitives/TrkEventPrimitivesDict.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "muonEvent/MuonContainer.h"
//use new MDT segment container 
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <inttypes.h>

#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;


// New Sort Segments in the MDT Endcap into Sides and MDT Stations
void
MdtVsTgcRawDataValAlg::SortMDTSegments(const xAOD::MuonSegmentContainer *m_newsegment,
                                       vector<const Muon::MuonSegment*> (&sortedSegments)[2][4]){
    
  // Loop over all segments in event
  xAOD::MuonSegmentContainer::const_iterator mdtseg_itr = m_newsegment->begin();
  xAOD::MuonSegmentContainer::const_iterator mdtseg_end = m_newsegment->end();
  for(; mdtseg_itr!=mdtseg_end; ++mdtseg_itr){
    if(!(*mdtseg_itr)->muonSegment().isValid())continue;
    // Get segm
    const Muon::MuonSegment* segm = dynamic_cast<const Muon::MuonSegment*>(*(*mdtseg_itr)->muonSegment());
    if (segm == 0) {
      ATH_MSG_ERROR( "no pointer to segm!!!" );
      break;
    }
    
    bool isMdt=false, isEndcap=false; // Flags for whether the Segment has hits from the MDT/Endcap
    int nMdtMeas[4]={0,0,0,0};        // int array to count number of MDT hits in different stations
    
    // Loop through contained ROTs and identify used stations
    for(unsigned int iROT=0; iROT<segm->numberOfContainedROTs(); ++iROT){
      const Trk::RIO_OnTrack* rio = segm->rioOnTrack(iROT);
      Identifier id = rio->identify();
      
      // Identify MDT Endcap Segments
      if(m_mdtIdHelper->is_mdt(id))isMdt=true;
      if(m_mdtIdHelper->isEndcap(id))isEndcap=true;
      
      int stationName = int(m_mdtIdHelper->stationName(id));
      // Large (L) = odd, greater r, Small (S) = even, lower r
      // 13=EIL 49=EIS 14=EEL 15=EES 17=EML 18=EMS 20=EOL 21=EOS
      if((stationName==13)||(stationName==49))nMdtMeas[0]++;// MDT
      if((stationName==14)||(stationName==15))nMdtMeas[1]++;// MDT
      if((stationName==17)||(stationName==18))nMdtMeas[2]++;// MDT
      if((stationName==20)||(stationName==21))nMdtMeas[3]++;// MDT
    }
    
    // If not Endcap and does not contain MDT hits
    if(!isMdt||!isEndcap)continue;
    
    // Get Side
    int segmAC = (segm->globalPosition().eta()<0);// a:0, c:1
    
    // Check MDT Stations included in this Segment
    int nMDTStations=0; int MDTStationj=-1;
    for(int jMDT=0;jMDT<4;jMDT++){
      if(nMdtMeas[jMDT]!=0){
        nMDTStations++;
        MDTStationj=jMDT;
      }
    }
    
    // MC data has an odd channel set always triggering segments, this cuts them
    if((segmAC==0)&&(MDTStationj==0)&&
       (segm->globalPosition().eta()>1.815)&&(segm->globalPosition().eta()<1.82)&&
       (segm->globalPosition().phi()>2.857)&&(segm->globalPosition().phi()<2.862))continue;
    
    // If there is only one station pass the segment data on to the variables
    if(nMDTStations==1){
      //HasStationSegm[segmAC][MDTStationj]=true;
      sortedSegments[segmAC][MDTStationj].push_back(segm);
    }
  }// Loop over segments
}// End of function


// Old Sort Segments in the MDT Endcap into Sides and MDT Stations
void
MdtVsTgcRawDataValAlg::SortMDTSegments(const Trk::SegmentCollection *m_segmcollection,
                                       vector<const Muon::MuonSegment*> (&sortedSegments)[2][4]){
  // Flags for whether event has segments from sides and stations
  //bool HasStationSegm[2][4] = {{false,false,false,false},
  //                             {false,false,false,false}};
  
  // Loop over all segments in event
  for(Trk::SegmentCollection::const_iterator s = m_segmcollection->begin();
      s!=m_segmcollection->end();
      ++s){
    // Get segm
    const Muon::MuonSegment *segm=dynamic_cast<const Muon::MuonSegment*>(*s);
    if (segm == 0) {
      ATH_MSG_ERROR( "no pointer to segm!!!" );
      break;
    }
    
    bool isMdt=false, isEndcap=false; // Flags for whether the Segment has hits from the MDT/Endcap
    int nMdtMeas[4]={0,0,0,0};        // int array to count number of MDT hits in different stations
    
    // Loop through contained ROTs and identify used stations
    for(unsigned int iROT=0; iROT<segm->numberOfContainedROTs(); ++iROT){
      const Trk::RIO_OnTrack* rio = segm->rioOnTrack(iROT);
      Identifier id = rio->identify();
      
      // Identify MDT Endcap Segments
      if(m_mdtIdHelper->is_mdt(id))isMdt=true;
      if(m_mdtIdHelper->isEndcap(id))isEndcap=true;
      
      int stationName = int(m_mdtIdHelper->stationName(id));
      // Large (L) = odd, greater r, Small (S) = even, lower r
      // 13=EIL 49=EIS 14=EEL 15=EES 17=EML 18=EMS 20=EOL 21=EOS
      if((stationName==13)||(stationName==49))nMdtMeas[0]++;// MDT
      if((stationName==14)||(stationName==15))nMdtMeas[1]++;// MDT
      if((stationName==17)||(stationName==18))nMdtMeas[2]++;// MDT
      if((stationName==20)||(stationName==21))nMdtMeas[3]++;// MDT
    }
    
    // If not Endcap and does not contain MDT hits
    if(!isMdt||!isEndcap)continue;
    
    // Get Side
    int segmAC = (segm->globalPosition().eta()<0);// a:0, c:1
    
    // Check MDT Stations included in this Segment
    int nMDTStations=0; int MDTStationj=-1;
    for(int jMDT=0;jMDT<4;jMDT++){
      if(nMdtMeas[jMDT]!=0){
        nMDTStations++;
        MDTStationj=jMDT;
      }
    }
    
    // MC data has an odd channel set always triggering segments, this cuts them
    if((segmAC==0)&&(MDTStationj==0)&&
       (segm->globalPosition().eta()>1.815)&&(segm->globalPosition().eta()<1.82)&&
       (segm->globalPosition().phi()>2.857)&&(segm->globalPosition().phi()<2.862))continue;
    
    // If there is only one station pass the segment data on to the variables
    if(nMDTStations==1){
      //HasStationSegm[segmAC][MDTStationj]=true;
      sortedSegments[segmAC][MDTStationj].push_back(segm);
    }
  }// Loop over segments
}// End of function
