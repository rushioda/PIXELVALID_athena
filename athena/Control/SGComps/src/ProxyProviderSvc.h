// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ProxyProviderSvc.h
    @brief manages the address providers and add proxies on demand to the store
  * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
  * $Id: ProxyProviderSvc.h,v 1.3 2008-03-05 21:57:44 gemmeren Exp $
  */


#ifndef SGCOMPS_PROXYPROVIDERSVC_H
# define SGCOMPS_PROXYPROVIDERSVC_H 1

#include "AthenaKernel/IProxyProviderSvc.h"
#include "AthenaKernel/StoreID.h"
#include "GaudiKernel/Property.h" /*StringArrayProperty*/
#include "GaudiKernel/StatusCode.h"
#include <cassert>
#include <list>
#include <string>
#include <algorithm>

// framework includes
#include "AthenaBaseComps/AthService.h"

namespace SG {
  class DataProxy;
  class TransientAddress;
}

class IAddressProvider;
class IProxyRegistry; //this is DataStore
class IConversionSvc;
class IOpaqueAddress;
class ISvcLocator;
template <class TYPE> class SvcFactory;

///manages the address providers and add proxies on demand to the store
class ProxyProviderSvc : virtual public IProxyProviderSvc,
                         public AthService
{
public:

  // fwd compat w/ gaudi-v21
  using AthMessaging::msg;

  typedef std::list<IAddressProvider*>::iterator  pAPiterator;
  typedef std::list<SG::TransientAddress*> TAdList;
  typedef TAdList::iterator TAdIterator;

  ///IProxyProvider interface
  //@{
  ///add proxies to the store to modify (before Begin Event)
  virtual StatusCode preLoadProxies(IProxyRegistry& storeToModify);

  ///add proxies to the store to modify (during Begin Event)
  virtual StatusCode loadProxies(IProxyRegistry& storeToModify);

  ///get the default proxy. Optionally add proxies to the store to modify
  virtual SG::DataProxy* retrieveProxy(const CLID& id, const std::string& key,
				       IProxyRegistry& storeToModify);

  /// update a transient Address
  virtual StatusCode updateAddress(StoreID::type storeID,
				   SG::TransientAddress* tad);

 ///create a list of transient Addresses:
  StatusCode addAddresses(IProxyRegistry& dataStore,
			  IAddressProvider* iap,
			  TAdList& tad);

  ///create a new Proxy, overriding CLID and/or key
  SG::DataProxy* addAddress(IProxyRegistry& storeToModify, 
			    SG::TransientAddress* tad);
  //@}


  ///IAddressProvider manager functionality
  ///add a provider to the set of known ones. PROVIDER IS OWNED BY THE CLIENT
  virtual void addProvider(IAddressProvider* aProvider);

  /// Service boilerplate
  //@{
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  //@}

protected:    
  /// the Service Factory
  friend class SvcFactory<ProxyProviderSvc>;
  /// Standard Service Constructor
  ProxyProviderSvc(const std::string& name, ISvcLocator* svcLoc);
  virtual ~ProxyProviderSvc();

private:
  /// property: the services declared as providers
  StringArrayProperty m_providerNames;
  /// the handler for m_providerNames
  void providerNamesPropertyHandler( Property& theProp );
  
  /// the providers we know about. WE DON'T OWN THEM
  std::list<IAddressProvider*> m_providers; 
  /// Persistency Service
  IConversionSvc* m_pDataLoader;   
};


inline void ProxyProviderSvc::addProvider(IAddressProvider* aProvider) {
  assert(aProvider);
  if (find(m_providers.begin(), m_providers.end(), aProvider) == m_providers.end()) {
    m_providers.push_back(aProvider);
  }
}

#endif // !SGCOMPS_PROXYPROVIDERSVC_H
