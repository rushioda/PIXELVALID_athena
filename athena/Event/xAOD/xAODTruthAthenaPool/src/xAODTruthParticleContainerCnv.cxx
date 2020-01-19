/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthParticleContainerCnv.cxx 582241 2014-02-07 17:28:39Z jcatmore $

// System include(s):
#include <exception>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODTruthParticleContainerCnv.h"

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

xAODTruthParticleContainerCnv::xAODTruthParticleContainerCnv( ISvcLocator* svcLoc )
   : xAODTruthParticleContainerCnvBase( svcLoc ) {

}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/
StatusCode xAODTruthParticleContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TruthParticleContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TruthParticleContainer*
xAODTruthParticleContainerCnv::createPersistent( xAOD::TruthParticleContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TruthParticleContainer* result =
      new xAOD::TruthParticleContainer( trans->begin(), trans->end(),
                              SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TruthParticleContainer::iterator itr = result->begin();
   xAOD::TruthParticleContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TruthParticleContainer* xAODTruthParticleContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "58F98A16-E465-4CA5-A099-73033206D8E3" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
        xAOD::TruthParticleContainer* c =
        poolReadObject< xAOD::TruthParticleContainer >();
        setStoreLink( c, m_key );
        return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TruthParticleContainer found" );
   return 0;
}

void xAODTruthParticleContainerCnv::toPersistent( xAOD::TruthParticle* truthParticle ) const {

   // Tell the object to prepare all its smart pointers for persistification:
   truthParticle->toPersistent();

   return;
}
