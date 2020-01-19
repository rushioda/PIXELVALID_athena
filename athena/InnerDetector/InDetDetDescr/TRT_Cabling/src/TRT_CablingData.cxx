/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Implementation file for TRT_CablingData class
//     
//

#include "TRT_CablingData.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assert.h>

  //Constructor
TRT_CablingData::TRT_CablingData()
  : m_bofai_sorted (true)
{
}

  //Destructor
TRT_CablingData::~TRT_CablingData()
{
  for (std::map< uint32_t, std::vector<IdentifierHash> *>::iterator i =
         m_collID.begin();
       i != m_collID.end();
       ++i)
    delete i->second;
}

  // Fill m_identfierForAllStraws with zeros (TB case)
void TRT_CablingData::zero_identfierForAllStraws
(int, std::vector<Identifier> )
{  
  return;
}

  // Fill m_identfierHashForAllStraws with zeros (TB case)
void TRT_CablingData::zero_identfierHashForAllStraws
(int, std::vector<IdentifierHash> )
{
  return;
}


TRT_CablingData::idandhash_t&
TRT_CablingData::find_idandhash (int rod, int bufferPosition)
{
  assert (rod >= 0);
  assert (bufferPosition >= 0);
  unsigned int rodhi = ((unsigned)rod & 0xffff0000) >> 16;
  unsigned int rodlo = (rod & 0x0000ffff);
  rodvec_t& rodvec = m_rodoffset_to_id[rodhi];
  if (rodvec.size() <= rodlo)
    rodvec.resize (rodlo + 1);
  offsetvec_t& offsetvec = rodvec[rodlo];
  if (offsetvec.size() <= (unsigned)bufferPosition)
    offsetvec.resize (bufferPosition+1);
  return offsetvec[bufferPosition];
}

  // Set value of Identifier for each straw (TB case)
void TRT_CablingData::set_identfierForAllStraws(int rod, 
  int bufferPosition, Identifier strawID)
{    
  find_idandhash (rod, bufferPosition).first = strawID;
  m_BufferOffsetForAllIdentifiers.push_back(idpair (strawID, bufferPosition));
  m_bofai_sorted = false;

   //   cout << "set m_BufferOffsetForAllIdentifiers[" 
   //	<< std::hex << strawID << std::dec 
   //	<< "] = " << bufferPosition << endl;
}


 // Set value of Identifier for each straw (DC1, DC2)
void TRT_CablingData::set_identfierForAllStraws(Identifier strawID)
{
  m_identfierForAllStraws.push_back(strawID);
}


  // Set value of IdentifierHash for each straw (TB case)
void TRT_CablingData::set_identfierHashForAllStraws(int rod, 
  int bufferPosition, IdentifierHash hashId)
{    
  find_idandhash (rod, bufferPosition).second = hashId;
}

  // Set value of IdentifierHash for each straw (DC1, DC2)
void TRT_CablingData::set_identfierHashForAllStraws(IdentifierHash hashId)
{
  m_identfierHashForAllStraws.push_back(hashId);
}


Identifier TRT_CablingData::get_identfierForAllStraws(int rod, 
  int bufferPosition)
{
  return find_idandhash (rod, bufferPosition).first;
}

IdentifierHash TRT_CablingData::get_identfierHashForAllStraws
  (int rod, int bufferPosition)
{
  return find_idandhash (rod, bufferPosition).second;
}


  // Get value of Identifier for each straw (DC1, DC2)
Identifier TRT_CablingData::get_identfierForAllStraws(int shift)
{
  return m_identfierForAllStraws[shift];
}

  // Get value of IdentifierHash for each straw (DC1, DC2)
IdentifierHash TRT_CablingData::get_identfierHashForAllStraws(int shift)
{
  return m_identfierHashForAllStraws[shift];
}

// Get buffer offset from Identifier
uint32_t TRT_CablingData::get_BufferOffset( Identifier StrawId )
{
  //   cout << "get m_BufferOffsetForAllIdentifiers[" 
  //	<< std::hex << StrawId << std::dec 
  //	<< "] = " << m_BufferOffsetForAllIdentifiers[StrawId] << endl;

  if (!m_bofai_sorted) {
    std::sort (m_BufferOffsetForAllIdentifiers.begin(),
               m_BufferOffsetForAllIdentifiers.end());
    m_bofai_sorted = true;
  }

  std::vector<idpair>::iterator i =
    std::lower_bound (m_BufferOffsetForAllIdentifiers.begin(),
                      m_BufferOffsetForAllIdentifiers.end(),
                      StrawId);
  assert (i != m_BufferOffsetForAllIdentifiers.end() && i->first == StrawId);
  return i->second;
}


    // Add collection IDs for ROB
void TRT_CablingData::add_collID(uint32_t rob_id, std::vector<IdentifierHash> * vectID)
{
  m_collID[rob_id] = vectID;
}
  

  // Get Identifiers of Collections for the given ROBID
const std::vector<IdentifierHash>& TRT_CablingData::get_collID(int shift)
{
  return *m_collID[shift];
}

  // Add SourceID
void TRT_CablingData::add_allRobs(uint32_t sidROB)
{
  m_allRobs.push_back(sidROB);
}

  // Get all RODIDs for TRT
const std::vector<uint32_t>& TRT_CablingData::get_allRods() const
{
  return m_allRobs;
}

