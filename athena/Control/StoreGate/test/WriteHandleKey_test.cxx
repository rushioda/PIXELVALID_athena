/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/test/WriteHandleKey_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for WriteHandleKey.
 */


#undef NDEBUG
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/exceptions.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)


void test1()
{
  std::cout << "test1\n";

  SG::WriteHandleKey<MyObj> k1 ("aaa");
  assert (k1.clid() == 293847295);
  assert (k1.key() == "aaa");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k1.storeHandle().name() == "StoreGateSvc");
  assert (!k1.storeHandle().isSet());
  assert (k1.initialize().isSuccess());
  assert (k1.storeHandle().isSet());

  k1 = "aab";
  assert (k1.clid() == 293847295);
  assert (k1.key() == "aab");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k1.storeHandle().name() == "StoreGateSvc");
  assert (k1.storeHandle().isSet());

  k1 = "FeeSvc/aac";
  assert (k1.clid() == 293847295);
  assert (k1.key() == "aac");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k1.storeHandle().name() == "FeeSvc");
  assert (!k1.storeHandle().isSet());

  SG::WriteHandleKey<MyObj> k2 ("bbb", "FooSvc");
  assert (k2.clid() == 293847295);
  assert (k2.key() == "bbb");
  assert (k2.mode() == Gaudi::DataHandle::Writer);
  assert (k2.storeHandle().name() == "FooSvc");
  assert (!k2.storeHandle().isSet());
}


int main()
{
  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need MessageSvc

  test1();
}
