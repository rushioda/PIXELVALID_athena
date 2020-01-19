/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
  test ActiveStoreSvc functionality
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: ActiveStore_test.cxx 698338 2015-10-03 18:39:22Z ssnyder $

//<<<<<< INCLUDES                                                       >>>>>>
#include <string>
#include "TestTools/initGaudi.h"
#include "TestTools/SGassert.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"


using namespace std;
using namespace Athena_test;

class Foo {
public:
  Foo() : m_i(0), m_d(0.0) {}
  Foo(int i) : m_i(i), m_d(0.0) {}
  int i() const { return m_i; }
  ~Foo() {
  }

  // Members public to avoid clang warning about unused private members.
  int m_i;
  double m_d;
};


#include "SGTools/CLASS_DEF.h"
CLASS_DEF(Foo, 8101, 1)

int main() {
  cout << "*** ActiveStoreTest BEGINS ***" << endl;
  ISvcLocator* pSvcLoc;
  initGaudi("ActiveStore_test.txt", pSvcLoc);
  ActiveStoreSvc* pASS(0);
  assert( pSvcLoc->service("ActiveStoreSvc", pASS, true).isSuccess() );
  assert( pASS->activeStore() == pASS->operator->() );
  //as set in ActiveStore_test.txt
  assert( pASS->activeStore()->name() == "E1" ); 
  StoreGateSvc* pE2(0);
  assert( pSvcLoc->service("E2", pE2).isSuccess() );
  pASS->setStore(pE2);
  assert( pASS->activeStore()->name() == "E2" ); 
  assert( pASS->finalize().isSuccess() );
  cout << "*** ActiveStoreTest OK ***\n\n" <<endl;

  return 0;
}

