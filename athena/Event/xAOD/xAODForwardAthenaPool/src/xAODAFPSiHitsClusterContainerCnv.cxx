/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPSiHitsClusterContainerCnv.cxx
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-18
 * 
 * @brief  Implementation of class xAODAFPSiHitsClusterContainerCnv
 * 
 */


// System include(s):
#include <exception>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODAFPSiHitsClusterContainerCnv.h"


namespace {

  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    
    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
   
    // Give it to the container:
    cont->setStore( link );
    
    return;
  }
    
} // private namespace
    
 
xAODAFPSiHitsClusterContainerCnv::xAODAFPSiHitsClusterContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPSiHitsClusterContainerCnvBase( svcLoc ) {
 
}
 
/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODAFPSiHitsClusterContainerCnv::createObj( IOpaqueAddress* pAddr,
							DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::AFPSiHitsClusterContainer: " << m_key );
    
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
}
 
xAOD::AFPSiHitsClusterContainer* xAODAFPSiHitsClusterContainerCnv::createPersistent( xAOD::AFPSiHitsClusterContainer* trans )
{
 
  // Create a view copy of the container:
  xAOD::AFPSiHitsClusterContainer* result =
    new xAOD::AFPSiHitsClusterContainer( trans->begin(), trans->end(),
					 SG::VIEW_ELEMENTS );
 
  // Prepare the objects to be written out:
  xAOD::AFPSiHitsClusterContainer::iterator itr = result->begin();
  xAOD::AFPSiHitsClusterContainer::iterator end = result->end();
  for( ; itr != end; ++itr ) {
    toPersistent( *itr );
  }
 
  // Return the new container:
  return result;
}
 
xAOD::AFPSiHitsClusterContainer* xAODAFPSiHitsClusterContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "CCBFAFB3-9A9E-486B-AD37-43C0FFB68998" );
 
  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::AFPSiHitsClusterContainer* c =
      poolReadObject< xAOD::AFPSiHitsClusterContainer >();
    setStoreLink( c, m_key );
    return c;
  }
 
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPSiHitsClusterContainer found" );
  return 0;
}
 
void xAODAFPSiHitsClusterContainerCnv::toPersistent( xAOD::AFPSiHitsCluster* cluster ) const {
 
  // Tell the object to prepare all its smart pointers for persistification:
  cluster->toPersistent();
  
  return;
}

