/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 test the store ID setting
 -------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: StoreID_test.cxx 753911 2016-06-09 13:40:32Z calaf $

#include <iostream>

#undef NDEBUG

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"


#ifndef NOGAUDI
#include "TestTools/initGaudi.h"
using namespace Athena_test;
using namespace std;

int main() {
  cout << "*** StoreID_test BEGINS ***" <<endl;
  ISvcLocator* pSvcLoc;
  if (!initGaudi("StoreGateTestCommon.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  StoreGateSvc* pStore(0);
  static const bool CREATE(true);
  assert((pSvcLoc->service("StoreGateSvc", pStore, CREATE)).isSuccess());
  assert(pStore);
  
  assert(pStore->storeID() == StoreID::EVENT_STORE);
  pStore->setStoreID(StoreID::SPARE_STORE);
  assert(pStore->storeID() == StoreID::SPARE_STORE);

  assert((pSvcLoc->service("DetectorStore", pStore)).isSuccess());
  assert(pStore);
  assert(pStore->storeID() == StoreID::DETECTOR_STORE);
  
  assert((pSvcLoc->service("ConditionsStore", pStore, CREATE)).isSuccess());
  assert(pStore);
  assert(pStore->storeID() == StoreID::CONDITION_STORE);
  
  cout << "*** StoreID_test OK ***" <<endl;
  return 0;
}
#endif /*NOGAUDI*/













