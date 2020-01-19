/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetContainerCnv.cxx 795694 2017-02-05 23:13:50Z khoo $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJetContainerCnv.h"

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

xAODJetContainerCnv::xAODJetContainerCnv( ISvcLocator* svcLoc )
   : xAODJetContainerCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODJetContainerCnv::createObj( IOpaqueAddress* pAddr,
                                           DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::JetContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::JetContainer*
xAODJetContainerCnv::createPersistent( xAOD::JetContainer* trans ) {

   // Create a view copy of the container:
   xAOD::JetContainer* result =
      new xAOD::JetContainer( trans->begin(), trans->end(),
                              SG::VIEW_ELEMENTS );

   // // Prepare the objects to be written out:
   // xAOD::JetContainer::iterator itr = result->begin();
   // xAOD::JetContainer::iterator end = result->end();
   // for( ; itr != end; ++itr ) {
   //    toPersistent( *itr );
   // }

   // Return the new container:
   return result;
}

xAOD::JetContainer* xAODJetContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "646342DF-D1E1-422A-975D-EA6DBF13CB6D" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::JetContainer* c = poolReadObject< xAOD::JetContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JetContainer found" );
   return 0;
}

// void xAODJetContainerCnv::toPersistent( xAOD::Jet* jet ) const {

//    // Tell the object to prepare all its smart pointers for persistification:
//    jet->toPersistent();

//    return;
// }
