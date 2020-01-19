/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDPROXYPROVIDER_H
#define CONDPROXYPROVIDER_H

/** @file CondProxyProvider.h
 *  @brief This file contains the class definition for the CondProxyProvider class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaBaseComps/AthService.h"

// Forward declarations
namespace pool {
   class ICollectionCursor;
}
class ISvcLocator;
class PoolCollectionConverter;
class StoreGateSvc;
class IAthenaPoolCnvSvc;

/** @class CondProxyProvider
 *  @brief This class is the AddressProvider for conditions data.
 **/
class CondProxyProvider : public ::AthService, virtual public IAddressProvider {
public: // Constructor and Destructor
   /// Standard Service Constructor
   CondProxyProvider(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~CondProxyProvider();

   /// Required of all Gaudi Services
   virtual StatusCode initialize();
   /// Required of all Gaudi Services
   virtual StatusCode finalize();
   /// Does this object satisfy a given interface?  See Gaudi documentation for details.
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Get all addresses from provider. Called before begin event.
   /// @param storeID [IN] store ID, this function only preloads detector store addresses.
   /// @param tads [OUT] list of the transient addresses which were preloaded.
   virtual StatusCode preLoadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Implementation of the loadAddresses function without any functionality.
   virtual StatusCode loadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Update a transient address.
   /// @param storeID [IN] store ID, this function only preloads detector store addresses.
   /// @param tad [IN] transient address to be updated.
   virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad);

private: // data
   ServiceHandle<IAthenaPoolCnvSvc> m_athenaPoolCnvSvc;

   mutable PoolCollectionConverter* m_poolCollectionConverter;
   mutable pool::ICollectionCursor* m_headerIterator;

private: // properties
   /// InputCollections, vector with names of the input collections.
   StringArrayProperty m_inputCollectionsProp;
   mutable std::vector<std::string>::const_iterator m_inputCollectionsIterator;

private: // internal helper functions
   /// Return pointer to new PoolCollectionConverter
   PoolCollectionConverter* getCollectionCnv();
};

#endif
