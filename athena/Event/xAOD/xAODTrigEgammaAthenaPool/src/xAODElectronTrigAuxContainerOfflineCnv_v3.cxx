/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronTrigAuxContainerOfflineCnv_v3.cxx 705790 2015-11-04 13:09:23Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODElectronTrigAuxContainerOfflineCnv_v3.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODElectronTrigAuxContainerOfflineCnv_v3::
xAODElectronTrigAuxContainerOfflineCnv_v3()
   : T_AthenaPoolTPCnvBase< xAOD::ElectronTrigAuxContainer,
                            xAOD::ElectronAuxContainer_v3 >() {

}

void xAODElectronTrigAuxContainerOfflineCnv_v3::
persToTrans( const xAOD::ElectronAuxContainer_v3* oldObj,
             xAOD::ElectronTrigAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::ElectronAuxContainer_v3 to "
            "xAOD::ElectronTrigAuxContainer..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

   // Print what happened:
   ATH_MSG( "Converting xAOD::ElectronAuxContainer_v3 to "
            "xAOD::ElectronTrigAuxContainer [OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODElectronTrigAuxContainerOfflineCnv_v3::
transToPers( const xAOD::ElectronTrigAuxContainer*,
             xAOD::ElectronAuxContainer_v3*,
             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODElectronTrigAuxContainerOfflineCnv_v3::"
       << "transToPers" << endmsg;
   throw std::runtime_error( "Somebody called "
                             "xAODElectronTrigAuxContainerOfflineCnv_v3::"
                             "transToPers" );

   return;
}
