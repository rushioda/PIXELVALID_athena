// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: UpdateHandle.h 733875 2016-04-04 23:33:03Z leggett $
/**
 * @file StoreGate/UpdateHandle.h
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov>
 * @date Updated: Feb, 2016
 * @brief Handle class for modifying an existing object in StoreGate.
 */

#ifndef STOREGATE_SG_UPDATEHANDLE_H
#define STOREGATE_SG_UPDATEHANDLE_H 1


#include "StoreGate/VarHandleBase.h"
#include "StoreGate/UpdateHandleKey.h"
#include "StoreGate/StoreGateSvc.h" /* needed by clients */
#include "GaudiKernel/EventContext.h"
#include <string>



namespace SG {


  /**
   * @class SG::UpdateHandle<T>
   * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
   * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
   * speed-up subsequent accesses.
   * It can be reset by the store for asynchronous updates (IOVSvc)
   *
   * @c SG::UpdateHandle<T> can only access non-const proxies in StoreGate.
   * A valid proxy must already exist in StoreGate.
   *
   * Usage example:
   * @code
   *   class MyAlg : public AthAlgorithm
   *   {
   *     SG::UpdateHandle<int> m_int;
   *   };
   *
   *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
   *      declareProperty("IntHandle",
   *                      m_int = SG::UpdateHandle<int>("MyIntSgKey"),
   *                      "a handle to an int in StoreGate");
   *   }
   *
   *   StatusCode MyAlg::execute()
   *   {
   *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
   *                  << *m_int);
   *     *m_int = 10;
   *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
   *                  << *m_int);
   *     return StatusCode::SUCCESS;
   *   }
   * @endcode
   *
   * For more information have a look under the package
   *     Control/AthenaExamples/AthExHelloWorld
   *
   */
  template <class T>
  class UpdateHandle
    : public SG::VarHandleBase
  { 
  public: 
    typedef T*               pointer_type; // FIXME: better handling of
    typedef const T*   const_pointer_type; //        qualified T type ?
    typedef T&             reference_type;
    typedef const T& const_reference_type;


    //************************************************************************
    // Constructors, etc.
    //


    /**
     * @brief Default constructor.
     *
     * The handle will not be usable until a non-blank key is assigned.
     */
    UpdateHandle();


    /**
     * @brief Constructor with full arguments.
     * @param sgkey StoreGate key of the referenced object.
     * @param storename Name of the referenced event store.
     */
    UpdateHandle(const std::string& sgkey,
                 const std::string& storename = "StoreGateSvc");


    /**
     * @brief Constructor from an UpdateHandleKey.
     * @param key The key object holding the clid/key/store.
     *
     * This will raise an exception if the StoreGate key is blank,
     * or if the event store cannot be found.
     */
    explicit UpdateHandle (const UpdateHandleKey<T>& key);


    /**
     * @brief Constructor from an UpdateHandleKey and an explicit event context.
     * @param key The key object holding the clid/key.
     * @param ctx The event context.
     *
     * This will raise an exception if the StoreGate key is blank,
     * or if the event store cannot be found.
     *
     * If the default event store has been requested, then the thread-specific
     * store from the event context will be used.
     */
    explicit UpdateHandle (const UpdateHandleKey<T>& key,const EventContext& ctx);


    /**
     * @brief Copy constructor.
     */
    UpdateHandle( const UpdateHandle& rhs );


    /**
     * @brief Move constructor.
     */
    UpdateHandle( UpdateHandle&& rhs );


    /**
     * @brief Assignment operator.
     */
    UpdateHandle& operator=( const UpdateHandle& rhs ); 


    /**
     * @brief Move operator.
     */
    UpdateHandle& operator=( UpdateHandle&& rhs ); 


    //************************************************************************
    // Dereference.
    //


    /**
     * @brief Derefence the pointer.
     * Throws ExcNullReadHandle on failure.
     *
     * This will inform Hive that the object has been modified.
     */
    pointer_type operator->();


    /**
     * @brief Derefence the pointer.
     * Throws ExcNullReadHandle on failure.
     *
     * This will inform Hive that the object has been modified.
     */
    reference_type operator*();


    /**
     * @brief Derefence the pointer.
     * Returns nullptr on failure.
     *
     * This will _not_ inform Hive that the object has been modified.
     */
    const_pointer_type cptr();


    /**
     * @brief Derefence the pointer.
     * Returns nullptr on failure.
     *
     * This will inform Hive that the object has been modified.
     */
    pointer_type ptr();

  
    /**
     * @brief Return the cached pointer directly; no lookup.
     */
    pointer_type cachedPtr() const;


    /**
     * @brief Can the handle be successfully dereferenced?
     */
    virtual bool isValid() override final;


    // FIXME: Remove this once IResetable is cleaned up.
    using IResetable::reset;


    /**
     * @brief Reset this handle.
     * @param hard If true, anything depending on the event store is cleared.
     *
     * Clear the updated flag, then call reset() from the base class.
     */
    virtual void reset (bool hard) override;

  
  private:
    /**
     * @brief Helper: dereference the pointer.
     * Throws ExcNullUpdateHandle on failure.
     */
    pointer_type checkedPtr();

    /// Flag to prevent multiple calls to IProxyDict::udpatedObject.
    bool m_updated;
  }; 


  /**
   * @brief Return an @c UpdateHandle referencing @c key.
   * @param key The key object holding the clid/key/store.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  template <class T>
  UpdateHandle<T> makeHandle (const UpdateHandleKey<T>& key);


  /**
   * @brief Return an @c UpdateHandle referencing @c key for an explicit context.
   * @param key The key object holding the clid/key/store.
   * @param ctx The event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  template <class T>
  UpdateHandle<T> makeHandle (const UpdateHandleKey<T>& key,
                              const EventContext& ctx);


} /* namespace SG */


#include "StoreGate/UpdateHandle.icc"


#ifndef NO_LEGACY_HANDLES
namespace SG {
  template <class T>
  using RWVar = UpdateHandle<T>;
} 
#endif

#endif //> !STOREGATE_SG_UPDATEHANDLE_H
