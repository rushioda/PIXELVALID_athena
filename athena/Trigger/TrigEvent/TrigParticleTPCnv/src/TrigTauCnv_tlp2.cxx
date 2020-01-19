/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigTauCnv_tlp2.h"


//constructor
TrigTauCnv_tlp2::TrigTauCnv_tlp2(){

  // add the "main" base class converter (ie. TrigTauCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_p4PtEtaPhiMCnv );


}

void TrigTauCnv_tlp2::setPStorage( TrigTau_tlp2* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigTau );

   //for all other converters defined in the base class
   m_p4PtEtaPhiMCnv.setPStorage( &storage->m_p4PtEtaPhiM );

}

