/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSerializeCnvSvc/TrigSerializeCnvSvc.h"
//#include <GaudiKernel/ICnvManager.h>
#include "GaudiKernel/MsgStream.h"


StatusCode TrigSerializeCnvSvc::initialize(){
  MsgStream log(msgSvc(), "TrigSerializeCnvSvc");
  log << MSG::INFO << "initialize()" << endmsg;
  /*
  ICnvManager::CnvIterator i, stop;
  for ( i = cnvManager()->cnvBegin(), stop = cnvManager()->cnvEnd(); i != stop; i++ ) {
    if ( repSvcType() == (*i)->repSvcType() ) {
      //StatusCode iret = addConverter( *(*i) );
      CLID obj = (i)->objType();
      
      StatusCode iret = initializeConverter(storageType(), obj, *(*i) );

    }
  }
  */
  return StatusCode::SUCCESS;
}


long int
TrigSerializeCnvSvc::storageType() {
  static long type(0x51);
  return type;
}

long int TrigSerializeCnvSvc::repSvcType() const {
  return storageType();
}
