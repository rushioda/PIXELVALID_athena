/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/test/VarHandleProperty_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for VarHandlerPoperty specializations.
 */


#undef NDEBUG
#include "StoreGate/VarHandleProperty.h"
#include "StoreGate/exceptions.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <cassert>
#include <iostream>
#include <sstream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)


class PropTest
  : public IProperty
{
public:
  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual unsigned long refCount() const override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override
  { std::abort(); }

  virtual StatusCode setProperty( const Property& p ) override
  { return mgr.setProperty(p); }
  virtual StatusCode setProperty( const std::string& s ) override
  { return mgr.setProperty(s); }
  virtual StatusCode setProperty( const std::string& n, const std::string& v ) override
  { return mgr.setProperty(n, v); }
  virtual StatusCode getProperty( Property* p ) const override
  { return mgr.getProperty (p); }
  virtual const Property& getProperty( const std::string& name) const override
  { return mgr.getProperty (name); }
  virtual StatusCode getProperty( const std::string& n, std::string& v ) const override
  { return mgr.getProperty (n, v); }
  virtual const std::vector<Property*>& getProperties( ) const override
  { return mgr.getProperties(); }
  virtual bool hasProperty(const std::string& name) const override
  { return mgr.hasProperty(name); }

  PropertyMgr mgr;
};


void test1()
{
  std::cout << "test1\n";

  PropTest ptest;

  SG::ReadHandle<MyObj> k1;
  ptest.mgr.declareProperty ("k1", k1);
  SG::WriteHandle<MyObj> k2;
  ptest.mgr.declareProperty ("k2", k2);
  SG::UpdateHandle<MyObj> k3;
  ptest.mgr.declareProperty ("k3", k3);
  //SG::VarHandleBase k4 (1234, "", Gaudi::DataHandle::Reader);
  //ptest.mgr.declareProperty ("k4", k4);

  ServiceHandle<IJobOptionsSvc> jo ("JobOptionsSvc", "test");
  assert (jo.retrieve().isSuccess());
  assert (jo->setMyProperties ("test1", &ptest).isSuccess());

  assert (k1.clid() == 293847295);
  assert (k1.key() == "aaa");
  assert (k1.storeHandle().name() == "FooSvc");

  assert (k2.clid() == 293847295);
  assert (k2.key() == "bbb");
  assert (k2.storeHandle().name() == "StoreGateSvc");

  assert (k3.clid() == 293847295);
  assert (k3.key() == "ccc");
  assert (k3.storeHandle().name() == "BarSvc");

  //assert (k4.clid() == 1234);
  //assert (k4.key() == "ddd");
  //assert (k4.storeHandle().name() == "BazSvc");
}


int main()
{
  ISvcLocator* pDum;
  Athena_test::initGaudi("VarHandleProperty_test.txt", pDum);

  test1();
}
