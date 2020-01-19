/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @class IProxyProviderSvc.h 
 * @brief add on demand proxies to the address registry (SG)
 *
 * @author pcalafiura@lbl.gov - ATLAS Collaboration
 ***************************************************************************/

// $Id: IProxyProviderSvc.h,v 1.9 2007-06-23 01:12:06 calaf Exp $

#ifndef ATHENAKERNEL_IPROXYPROVIDERSVC_H
# define ATHENAKERNEL_IPROXYPROVIDERSVC_H

#include <string>
#include <list>
#include "AthenaKernel/StoreID.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"

class IAddressProvider;
class IProxyRegistry; //this is the store

namespace SG {
  class DataProxy;
  class TransientAddress;
}

class IOpaqueAddress;

class IProxyProviderSvc : virtual public IService {
public:
  /// add proxies to the store before Begin Event:
  virtual StatusCode preLoadProxies(IProxyRegistry& dataStore) = 0;

  ///add new proxies to store every Event:
  virtual StatusCode loadProxies(IProxyRegistry& dataStore) = 0;

  ///get the default proxy for a given CLID/Key"
  virtual SG::DataProxy* retrieveProxy(const CLID& id, 
				       const std::string& key,
				       IProxyRegistry& dataStore) = 0;

  /// Update a transient Address:
  virtual StatusCode updateAddress(StoreID::type sID,
				   SG::TransientAddress* pTAd) = 0; 

  ///IAddressProvider manager functionality
  ///add a provider to the set of known ones. PROVIDER IS OWNED BY THE CLIENT
  virtual void addProvider(IAddressProvider* aProvider) = 0;

  static const InterfaceID& interfaceID();
  virtual ~IProxyProviderSvc() {}
};

inline
const InterfaceID&
IProxyProviderSvc::interfaceID() {
  static InterfaceID ID("IProxyProviderSvc", 0 , 0);
  return ID;
}

#endif // ATHENAKERNEL_IPROXYPROVIDERSVC_H
