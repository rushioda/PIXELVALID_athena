/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "MuonMeasurementsCnv.h"

#include <iostream>
#include <stdexcept>

MuonMeasurementsCnv::MuonMeasurementsCnv(ISvcLocator* svcloc):
    MuonMeasurementsCnvBase(svcloc)
{
}


StatusCode MuonMeasurementsCnv::initialize()
{
   if( !MuonMeasurementsCnvBase::initialize().isSuccess() )
      return StatusCode::FAILURE;
   return StatusCode::SUCCESS;
}


// this method just reads the persistent object - no TP conversion here
void 
MuonMeasurementsCnv::readObjectFromPool( const std::string& token )
{
  static pool::Guid p1_guid( "C4979DA5-4193-410B-9476-A51708C01CF7" );
  static pool::Guid p2_guid( "87FC613F-390A-4AB0-9BBF-28CE788867D5" );
  static pool::Guid p3_guid( "5C8C4552-9F05-11E3-8164-6C3BE51AB9F1" );

   // set the POOL token which will be used for reading from POOL
   setToken( token );

   // select the object type based on its GUID 
   if( compareClassGuid( p2_guid ) )     {
      // read MuonMeasurements_PERS object from POOL using given TLP converter
      poolReadObject< MuonMeasurements_PERS >( m_TPConverter );
    }else if( compareClassGuid( p2_guid ) )     {
      // read MuonMeasurements_PERS object from POOL using given TLP converter
      poolReadObject< MuonMeasurements_PERS >( m_TPConverter_p2 );
    }else   if( compareClassGuid( p1_guid ) )    {
      poolReadObject< TPCnv::MuonMeasurements_tlp1 >( m_TPConverter_p1 );   }
   else
      throw std::runtime_error( "Unsupported version of MuonMeasurements_PERS (unknown GUID)" );
}

