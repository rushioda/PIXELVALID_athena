/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	TrigStreamAddress - based on ByteStreamAddress 
//====================================================================

//own 
#include "TrigSerializeCnvSvc/TrigStreamAddress.h"

// Framework include files

long TrigStream_StorageType(0x51);


/// Standard Constructor
TrigStreamAddress::TrigStreamAddress(const CLID& clid, 
const std::string& fname , const std::string& cname, int p1, int p2)
 : GenericAddress(TrigStream_StorageType, clid,fname,cname, p1, p2 ) 
{
  m_key = fname;
}

TrigStreamAddress::TrigStreamAddress(const CLID& clid) 
  : GenericAddress(TrigStream_StorageType, clid,"","")
{

}


void TrigStreamAddress::add(const std::vector<uint32_t> &a){
  m_rep.insert(m_rep.end(), a.begin(), a.end());
  //  std::cout << "TrigStreamAddress::add with size "<< a.size() << std::endl;
}

const std::vector<uint32_t>& TrigStreamAddress::get() const {
  return m_rep;
}




