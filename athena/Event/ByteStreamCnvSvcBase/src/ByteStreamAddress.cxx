/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	ByteStreamAddress implementation
//--------------------------------------------------------------------
//
//	Package    : ByteStreamCnvSvcBase
//
//  Description:     BSCnvSvc address implementation
//
//	Author     : Hong Ma
//      created    : MAr 2, 2001
//  History    :
//  Revision:  Sept 19, 2002
//             Store ROB IDs, to be used with ROBDataProviderSvc
//====================================================================

//own
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"

// Externals
extern long ByteStream_StorageType;


/// Standard Constructor
ByteStreamAddress::ByteStreamAddress(const CLID& clid,
		const std::string& fname , const std::string& cname, int p1, int p2)
	: GenericAddress(ByteStream_StorageType, clid, fname, cname, p1, p2) {
}

ByteStreamAddress::ByteStreamAddress(const CLID& clid)
	: GenericAddress(ByteStream_StorageType, clid, "", "") {
}

/** Add ROBID
 */
void ByteStreamAddress::add( uint32_t robid) {
   m_robIDs.push_back(robid);
}

/** access the fragment pointer
 */
const std::vector<uint32_t>&  ByteStreamAddress::getRobIDs() const {
   return(m_robIDs);
}
