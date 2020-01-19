/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawUtils/LArRawOrdering.h" 
#include "LArRawUtils/LArRoI_Map.h" 


  LArRawOrdering::LArRawOrdering(LArRoI_Map* m): m_roiMap(m) 
  { }


  LArRawOrdering::LArRawOrdering( ): m_roiMap(NULL) 
  { }
  void LArRawOrdering::setMap( LArRoI_Map* map )
  { m_roiMap = map; }

  bool  
  LArRawOrdering::operator () 
	(const LArRawChannel* ch1, const LArRawChannel* ch2)
  {

  LArRoI_Map::TT_ID rId1 = m_roiMap->TrigTowerID(ch1->channelID() ); 
  LArRoI_Map::TT_ID rId2 = m_roiMap->TrigTowerID(ch2->channelID() ); 

  // order in eta first 
  if(rId1 == rId2) {
   unsigned int id1 = ch1->channelID().get_identifier32().get_compact(); 
   unsigned int id2 = ch2->channelID().get_identifier32().get_compact(); 

   return id1<id2 ; // compare the channel ID.
  }
  return rId1< rId2; 

  }
