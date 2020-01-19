/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileIdentifier/TileRodIdHash.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ThreadGaudi.h"
#include <iostream>

// This class converts a TileReadoutModuleID into an integer, 
// 

// default contructor 
TileRodIdHash::TileRodIdHash( ) 
  : m_size(0),
    m_offset(0)
{

}


void TileRodIdHash::initialize( int offset )  {

// 

  m_offset = offset; 

  std::vector<ID>  rmod;
	for(unsigned int i=0x510000;i<=0x51000f;i++){
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
	}
	for(unsigned int i=0x520000;i<=0x52000f;i++){
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
	}
	for(unsigned int i=0x530000;i<=0x53000f;i++){
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
	}
	for(unsigned int i=0x540000;i<=0x54000f;i++){
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
		rmod.push_back(i);
	}
  
  std::vector<ID>::const_iterator 
    it = rmod.begin(); 
  std::vector<ID>::const_iterator 
    it_end = rmod.end() ; 

  int n = 0; 
  for (; it!=it_end;++it) {
    ID id = *it; 
    unsigned int i = id; 
    m_lookup[i] = n ; 
    m_int2id.push_back(id); 
    ++n; 
  } 

  m_size = n; 
}

void TileRodIdHash::initialize( int offset, const std::vector<ID>& rmod )  {

//

  m_offset = offset;

  std::vector<ID>::const_iterator
    it = rmod.begin();
  std::vector<ID>::const_iterator
    it_end = rmod.end() ;

  int n = 0;
  for (; it!=it_end;++it) {
    ID id = *it;
    unsigned int i = id;
    m_lookup[i] = n ;
    m_int2id.push_back(id);
    ++n;
  }

  m_size = n;
}



TileRodIdHash::ID TileRodIdHash::identifier(int index) const {

  return m_int2id[index] ; 

}

int TileRodIdHash::operator() (const ID& id) const {

  unsigned int i = id  ; 
  std::map<unsigned int, int> ::const_iterator it= m_lookup.find(i); 
  if(it!=m_lookup.end()) return (*it).second; 
  std::cout <<" ERROR in TileRodIdHash :  invalid Rod number"
	    <<std::endl;
  return -1; 

}

int TileRodIdHash::max() const {

return m_size; 

}

int TileRodIdHash::offset() const {

return m_offset; 

}

