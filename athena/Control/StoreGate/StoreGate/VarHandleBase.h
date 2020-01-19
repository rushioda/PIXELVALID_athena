// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VarHandleBase.h 757253 2016-06-23 13:01:18Z ssnyder $
/**
 * @file StoreGate/VarHandleBase.h
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov>
 * @date Updated: Feb, 2016
 * @brief Base class for VarHandle classes.
 */

#ifndef STOREGATE_SG_VARHANDLEBASE_H
#define STOREGATE_SG_VARHANDLEBASE_H 1

// STL includes
#include <string>

#include "StoreGate/VarHandleKey.h"

// fwk includes
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IResetable.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"

// SGTools includes
#include "SGTools/DataProxy.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/BuiltinsClids.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/StlMapClids.h"

namespace Athena_test {
  void varHandleTest(void);
  void resetableTest(void);
  void refCountTest(void);
}
namespace SG {
#ifdef NDEBUG
  const bool defaultQuiet = true;
#else
  const bool defaultQuiet = false;
#endif


  /**
   * @class SG::VarHandleBase
   * @brief Base class for VarHandle types.
   *
   * This is the base class for smart pointer types used to access objects
   * in an @c IProxyDict (such as a StoreGateSvc).  The handle types
   * @c ReadHandle, @c WriteHandle, and @c UpdateHandle derive from this;
   * see those classes for usage information.
   *
   * This class derives from @c VarHandleKey, which holds the CLID of the class
   * we're referencing (which gets passed to the constructor of this class
   * from the templated derived classes), the StoreGate key of the object
   * we're referencing, and a handle to the event store.  In this class, we
   * keep a pointer to the actual event store being used
   * (may be thread-dependent), a pointer to the @c DataProxy for the
   * referenced object, and a cached pointer to the object itself.
   *
   * A handle object may be used as a algorithm/tool property directly.
   * Because the handle caches state, however, this means that the component
   * using it cannot be reentrant.  In such a case, the handle will be reset
   * when the current algorithm completes.
   *
   * The preferred way of using handles is to use a HandleKey object
   * (one of ReadHandleKey<T>, WriteHandleKey<T>, UpdateHandleKey<T>)
   * as the property, and to create a handle instance on the stack from
   * the key object (and the event context, if available).
   */
  class VarHandleBase : public VarHandleKey, public IResetable
  {
    // For testing.
    friend void Athena_test::varHandleTest(void);
    friend void Athena_test::resetableTest(void);
    friend void Athena_test::refCountTest(void);
    friend std::ostream& operator<<( std::ostream&, const VarHandleBase&);
 
  public: 
    /**
     * @brief Constructor with default key.
     * @param clid CLID of the referenced class.
     * @param mode Mode of this handle (read/write/update).
     */
    explicit VarHandleBase(CLID clid, Gaudi::DataHandle::Mode mode);


    /**
     * @brief Constructor with full arguments.
     * @param clid CLID of the referenced class.
     * @param sgkey StoreGate key of the referenced object.
     * @param mode Mode of this handle (read/write/update).
     * @param storename Name of the referenced event store.
     */
    explicit VarHandleBase(CLID clid,
                           const std::string& sgkey,
                           Gaudi::DataHandle::Mode mode,
                           const std::string& storename = "StoreGateSvc");


    /**
     * @brief Constructor from a VarHandleKey.
     * @param key The key object holding the clid/key/store.
     *
     * This will raise an exception if the StoreGate key is blank,
     * or if the event store cannot be found.
     */
    explicit VarHandleBase (const VarHandleKey& key);


    /**
     * @brief Constructor from a VarHandleKey and an explicit event context.
     * @param key The key object holding the clid/key.
     * @param ctx The current event context.
     *
     * This will raise an exception if the StoreGate key is blank,
     * or if the event store cannot be found.
     *
     * If the default event store has been requested, then the thread-specific
     * store from the event context will be used.
     */
    explicit VarHandleBase (const VarHandleKey& key, const EventContext& ctx);


    /**
     * @brief Copy constructor.
     */
    VarHandleBase( const VarHandleBase& rhs );


    /**
     * @brief Move constructor.
     */
    VarHandleBase( VarHandleBase&& rhs );


    /**
     * @brief Assignment operator.
     */
    VarHandleBase& operator=( const VarHandleBase& rhs ); 


    /**
     * @brief Move operator.
     */
    VarHandleBase& operator=( VarHandleBase&& rhs ); 


    /**
     * @brief Destructor.
     */
    virtual ~VarHandleBase() override;


    //*************************************************************************
    // Accessors
    //
    // Inherited from VarHandleKey:
    //  CLID clid() const;
    //  ServiceHandle<IProxyDict> storeHandle() const;


    /**
     * @brief Return the StoreGate ID for the referenced object.
     *
     * This is defined in @c VarHandleKey.  We need to redefine it here because
     * it's also in @c IResetable.  (Otherwise there would be an ambiguity.)
     */
    virtual const std::string& key() const override final;


    /**
     * @brief Return the StoreGate ID for the referenced object.
     *
     * A synonym for key().
     */
    const std::string& name() const ;


    /**
     * @brief Return the name of the store holding the object we are proxying.
     */
    std::string store() const;


    //*************************************************************************
    // Validity checking.
    //


    /**
     * @brief Can the handle be successfully dereferenced?
     *
     * Non-const method; the handle may cache information as a result of this.
     */
    virtual bool isValid() = 0;


    /**
     * @brief Is the referenced object present in SG?
     *
     * Const method; the handle does not change as a result of this.
     */
    bool isPresent() const;


    /**
     * @brief Has a proxy been retrieved from SG?
     *
     * (Weaker test than @c isValid, but does not touch the disk.)
     */
    bool isInitialized() const;


    /**
     * @brief Has a proxy been retrieved from SG?
     *
     * Same as @c isInitialized; this is an interface required by @c IResetable.
     */
    virtual bool isSet() const override final;


    /**
     * @brief True if this handle has a proxy, and the proxy is const.
     *
     * Refers to the state of the proxy, not of the handle.
     */
    bool isConst() const;


    /**
     * @brief Retrieve and cache all information managed by a handle.
     *
     * This will retrieve and cache the associated @c DataProxy.
     *
     * Note for the case of a WriteHandle that has not yet been written to,
     * the proxy may not exist.  We return Success in that case; however,
     * @c isInitialized will still return false.
     */
    StatusCode initialize();


    /**
     * @brief Retrieve and cache all information managed by a handle.
     *
     * Synonym for initialize().
     */
    StatusCode setState();


    //*************************************************************************
    // State setting.
    //

  
    /**
     * @brief Explicitly set the event store.
     * @param store The new event store.
     *
     * This implicitly does a reset().
     */
    StatusCode setProxyDict (IProxyDict* store);


    // FIXME: Remove this once IResetable is cleaned up.
    using IResetable::reset;


    /**
     * @brief Reset this handle.
     * @param hard If true, anything depending on the event store is cleared.
     *
     * If the handle stays associated with a given event store, then hard=false.
     * In that case, we clear the cached pointer; the proxy is also dropped
     * if it is reset only.  If hard=true, then we always drop the proxy and
     * in addition clear the cached pointer to the event store.
     */
    virtual void reset (bool hard) override;


    /**
     * @brief Reset this handle at the end of processing.
     * @brief hard If true, anything depending on the event store is cleared.
     *
     * Same as reset(true);
     */
    virtual void finalReset() override final;


    /**
     * @brief Set the 'const' bit for the bound proxy in the store.
     */
    StatusCode setConst();

  protected: 
    //*************************************************************************
    // Protected methods.
    //


    /**
     * @brief Set the state of the handle to a given proxy.
     * @param proxy The proxy to set.
     *
     * The proxy must be valid; otherwise FAILURE will be returned.
     */
    StatusCode setState(SG::DataProxy* proxy);


    /**
     * @brief Set the state of a handle from a store and a key name.
     * @param store The event store to access.
     * @param name The StoreGate key to search for.
     *
     * Fails if no such object is recorded.
     */
    StatusCode setState(IProxyDict* store, const std::string& name);


    /**
     * @brief Helper to record an object in the event store.
     * @param The wrapped data object (DataBucket) to record.
     * @param dataPtr Pointer to the transient object itself.
     * @param allowMods If false, record the object as const.
     * @param returnExisting Allow an existing object.
     *
     * If there is already an existing object with our key, then return
     * failure, unless @c returnExisting is true, in which case
     * return success.  In either case, @c dobj is destroyed.
     */
    StatusCode record_impl (std::unique_ptr<DataObject> dobj,
                            void* dataPtr,
                            bool allowMods,
                            bool returnExisting);


    /**
     * @brief Retrieve an object from StoreGate.
     * @param quiet If true, suppress failure messages.
     */
    void* typeless_dataPointer_impl(bool quiet);


    /**
     * @brief Retrieve an object from StoreGate.
     * @param quiet If true, suppress failure messages.
     *
     * Inline method: first check cached pointer, then call the _impl method.
     */
    void* typeless_dataPointer(bool quiet=defaultQuiet);


    /**
     * @brief Retrieve an object from StoreGate as a const pointer.
     *
     * Same as typeless_dataPointer with the return value converted to const.
     */
    const void* typeless_cptr();
    const void* typeless_cptr(bool quiet);


    /**
     * @brief Retrieve an object from StoreGate as non-const pointer.
     *
     * Calls typeless_dataPointer, then raises an exception if the
     * proxy is marked as const.
     */
    void* typeless_ptr(bool quiet=defaultQuiet);


  protected: 
    //*************************************************************************
    // Protected data.
    //

    /// The object to which we are bound.
    void* m_ptr;

    /// Proxy holding the object to which we are bound.
    SG::DataProxy* m_proxy;
  
    /// Pointer to the store that owns the object.
    IProxyDict* m_store;


  private:
    /**
     * @brief Initialize the store pointer from the store handle.
     *        Also checks that the key is valid.
     */
    StatusCode storeFromHandle();


    /**
     * @brief Clear the m_proxy field and release the old proxy.
     */
    void resetProxy();


    /**
     * @brief Set a new proxy.  Release any old one first.
     * @param proxy The new proxy.
     */
    void setProxy (SG::DataProxy* proxy);
  }; 


  //*************************************************************************
  // Free functions.
  //


  /**
   * @brief Output stream.
   * @param out Stream to which to write.
   * @parma o Object to write.
   */
  std::ostream& operator<<( std::ostream& out, const VarHandleBase& o );


  /**
   * @brief Equality comparison.
   */
  bool operator==(const VarHandleBase& l, const VarHandleBase& r);


  /**
   * @brief Inequality comparison.
   */
  bool operator!=(const VarHandleBase& l, const VarHandleBase& r);


} /* namespace SG */

// For the ConditionHandles
#include "SGTools/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondContBase , 34480459 , 1 )

#include "StoreGate/VarHandleBase.icc"


#endif //> !STOREGATE_SG_VARHANDLEBASE_H
