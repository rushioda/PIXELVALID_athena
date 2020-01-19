/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronAuxContainerCnv_v1.cxx 621103 2014-10-10 12:30:01Z christos $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODElectronAuxContainerCnv_v1.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/versions/ElectronContainer_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODElectronAuxContainerCnv_v1::xAODElectronAuxContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::ElectronAuxContainer,
                           xAOD::ElectronAuxContainer_v1 >() {

}

void xAODElectronAuxContainerCnv_v1::
persToTrans( const xAOD::ElectronAuxContainer_v1* oldObj,
             xAOD::ElectronAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::ElectronAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

   // Set up interface containers on top of them:

   //The old  uses v_
   xAOD::ElectronContainer_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) { 
     oldInt.push_back( new xAOD::Electron_v1() ); 
   }
   oldInt.setStore( oldObj );
   
   xAOD::ElectronContainer newInt;
   for( size_t i = 0; i < newObj->size(); ++i ) { 
     newInt.push_back( new xAOD::Electron() ); 
   }
   newInt.setStore( newObj );

   for( size_t i = 0; i < oldInt.size(); ++i ) {

     float e237 = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::e237);
     float e277 = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::e277);
     float Reta= ( e277 != 0 ? e237/e277 : 0);
     newInt[ i ]->setShowerShapeValue(Reta, xAOD::EgammaParameters::Reta);
     //
     float e233 = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::e233);
     float Rphi= ( e237 != 0 ? e233/e237 : 0);
     newInt[ i ]->setShowerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
     //
     float emax = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::emaxs1);
     float emax2 = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::e2tsts1);
     float Eratio = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;
     newInt[ i ]->setShowerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
     //
     float emin = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::emins1);
     float DeltaE = emax2-emin;
     newInt[ i ]->setShowerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE);
     //
     const xAOD::CaloCluster* cluster  = oldInt[ i ]->caloCluster();
     const float eta2   = fabsf(cluster->etaBE(2));
     // transverse energy in calorimeter (using eta position in second sampling)
     const double energy =  cluster->e();
     double et = 0.;
     if (eta2<999.) {
       const double cosheta = cosh(eta2);
       et = (cosheta != 0.) ? energy /cosheta : 0.;
     }
     float ethad = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::ethad);
     float Rhad =  fabs(et) > 0. ? ethad/et : 0.;
     newInt[ i ]->setShowerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);
     //
     float ethad1 = oldInt[ i ]->showerShapeValue(xAOD::EgammaParameters::ethad1);
     float Rhad1 = fabs(et) > 0. ? ethad1/et : 0.;
     newInt[ i ]->setShowerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1); 
   }

   // Print what happened:
   ATH_MSG( "Converting xAOD::ElectronAuxContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODElectronAuxContainerCnv_v1::
transToPers( const xAOD::ElectronAuxContainer*,
             xAOD::ElectronAuxContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODElectronAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODElectronAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}
