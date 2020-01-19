// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SGTools/TestStore.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2015
 * @brief Dummy event store, for regression tests.
 */


#ifndef SGTOOLS_TESTSTORE_H
#define SGTOOLS_TESTSTORE_H


#include "AthenaKernel/IProxyDict.h"
#include "SGTools/StringPool.h"
#include "SGTools/DataProxy.h"
#include "SGTools/CurrentEventStore.h"
#include <unordered_map>


namespace SGTest {


typedef SG::sgkey_t sgkey_t;


struct TestStoreRemap
{
  TestStoreRemap (SG::sgkey_t the_key=0, size_t the_index = 0)
    : key (the_key), index (the_index) {}
  bool operator== (const TestStoreRemap& other) const
  { return other.key == key && other.index == index; }
  SG::sgkey_t key;
  size_t index;
};


struct TestStoreRemapHash
{
  size_t operator() (const TestStoreRemap& m) const
  { return m.key + m.index; }
};


class TestStore
  : virtual public IProxyDict
{
public:
  // These are unimplemented and will abort.
  virtual unsigned long addRef() override;
  virtual unsigned long release() override;
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override;
  virtual std::vector<const SG::DataProxy*> proxies() const override;
  virtual const std::string* keyToString (sgkey_t /*key*/) const override;
  virtual void registerKey (sgkey_t /*key*/,
                            const std::string& /*str*/,
                            CLID /*clid*/) override;
  virtual SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                       const std::string& key,
                                       bool allowMods,
                                       bool returnExisting) override;
  virtual StatusCode updatedObject (CLID id, const std::string& key) override;

  
  // These have dummy implementations.
  virtual const std::string& name() const override;
  virtual SG::DataProxy* proxy(const void* const pTransient) const override;
  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const override;
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const override;
  virtual sgkey_t stringToKey (const std::string& str, CLID clid) override;
  virtual const std::string* keyToString (sgkey_t key, CLID& clid) const override;
  virtual bool tryELRemap (sgkey_t sgkey_in, size_t index_in,
                           sgkey_t& sgkey_out, size_t& index_out) override;
  virtual StatusCode addToStore (CLID /*id*/, SG::DataProxy* proxy) override;
  virtual void boundHandle (IResetable* handle) override;
  virtual void unboundHandle (IResetable* handle) override;

  SG::DataProxy* record1 (const void* p, DataObject* obj,
                          CLID clid, const std::string& key);


  template <class T>
  void record (const T* p, const std::string& key)
  {
    DataObject* obj = SG::asStorable<T>(const_cast<T*>(p));
    CLID clid = ClassID_traits<T>::ID();
    record1 (p, obj, clid, key);
  }


  void remap (sgkey_t sgkey_in, sgkey_t sgkey_out,
              size_t index_in = 0, size_t index_out = 0);


  template <class T>
  void remap (const std::string& key_in, const std::string& key_out,
              size_t index_in = 0, size_t index_out = 0)
  {
    CLID clid = ClassID_traits<T>::ID();
    sgkey_t sgkey_in = stringToKey (key_in, clid);
    sgkey_t sgkey_out = stringToKey (key_out, clid);
    remap (sgkey_in, sgkey_out, index_in, index_out);
  }

  typedef std::unordered_map<const void*, SG::DataProxy*> tmap_t;
  tmap_t m_tmap;

  typedef std::unordered_map<sgkey_t, SG::DataProxy*> kmap_t;
  kmap_t m_kmap;

  typedef std::unordered_map<TestStoreRemap, TestStoreRemap, TestStoreRemapHash> remap_t;
  remap_t m_remap;

  SG::StringPool m_stringPool;

  std::vector<IResetable*> m_boundHandles;

  std::vector<std::string> m_updated;
  bool m_failUpdatedObject = false;

  // Log failed calls to proxy(CLID, std::string).
  mutable std::vector<std::pair<CLID, std::string> > m_missedProxies;
};


extern TestStore store;
void initTestStore();


} // namespace SGTest


#endif // not SGTOOLS_TESTSTORE_H
