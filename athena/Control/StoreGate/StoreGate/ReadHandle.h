// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ReadHandle.h 726621 2016-02-27 20:03:45Z ssnyder $
/**
 * @file StoreGate/ReadHandle.h
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov>
 * @date Updated: Feb, 2016
 * @brief Handle class for reading from StoreGate.
 */

#ifndef STOREGATE_SG_READHANDLE_H
#define STOREGATE_SG_READHANDLE_H 1


#include "StoreGate/VarHandleBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "SGTools/ClassID_traits.h"
#include "GaudiKernel/EventContext.h"
#include <string>



namespace SG {


/**
 * @class SG::ReadHandle<T>
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * @c SG::ReadHandle<T> can access const and non-const proxies in StoreGate but
 * cannot modify them (ie: it is actually a const T*).
 * A valid proxy must already exist in StoreGate.
 *
 * Usage example:
 * @code
 *   class MyAlg : public AthAlgorithm
 *   {
 *     SG::ReadHandle<int> m_int;
 *   };
 *
 *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
 *      declareProperty("IntHandle",
 *                      m_int = SG::ReadHandle<int>("MyIntSgKey"),
 *                      "a handle to an int in StoreGate");
 *   }
 *
 *   StatusCode MyAlg::execute()
 *   {
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
class ReadHandle
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
  ReadHandle();


  /**
   * @brief Constructor with full arguments.
   * @param sgkey StoreGate key of the referenced object.
   * @param storename Name of the referenced event store.
   */
  explicit ReadHandle(const std::string& sgkey, 
                      const std::string& storename = "StoreGateSvc");


  /**
   * @brief Constructor from a ReadHandleKey.
   * @param key The key object holding the clid/key/store.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  explicit ReadHandle (const ReadHandleKey<T>& key);


  /**
   * @brief Constructor from a ReadHandleKey and an explicit event context.
   * @param key The key object holding the clid/key.
   * @param ctx The event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  explicit ReadHandle (const ReadHandleKey<T>& key, const EventContext& ctx);


  /**
   * @brief Copy constructor.
   */
  ReadHandle( const ReadHandle& rhs );


  /**
   * @brief Move constructor.
   */
  ReadHandle( ReadHandle&& rhs );


  /**
   * @brief Assignment operator.
   */
  ReadHandle& operator=( const ReadHandle& rhs ); 


  /**
   * @brief Move operator.
   */
  ReadHandle& operator=( ReadHandle&& rhs ); 


  //************************************************************************
  // Dereference.
  //

  
  /**
   * @brief Derefence the pointer.
   * Throws ExcNullReadHandle on failure.
   */
  const_pointer_type  operator->();


  /**
   * @brief Derefence the pointer.
   * Throws ExcNullReadHandle on failure.
   */
  const_reference_type operator*();


  /**
   * @brief Dereference the pointer.
   * Returns nullptr on failure.
   */
  const_pointer_type cptr();


  /**
   * @brief Dereference the pointer.
   * Returns nullptr on failure.
   */
  const_pointer_type ptr();


  /**
   * @brief Return the cached pointer directly; no lookup.
   */
  const_pointer_type cachedPtr() const;


  /**
   * @brief Can the handle be successfully dereferenced?
   */
  virtual bool isValid() override final;

  
private:
  /**
   * @brief Helper: dereference the pointer.
   * Throws ExcNullReadHandle on failure.
   */
  const_pointer_type checkedCPtr();
}; 


/**
 * @brief Return a @c ReadHandle referencing @c key.
 * @param key The key object holding the clid/key/store.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 */
template <class T>
ReadHandle<T> makeHandle (const ReadHandleKey<T>& key);


/**
 * @brief Return a @c ReadHandle referencing @c key for an explicit context.
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
ReadHandle<T> makeHandle (const ReadHandleKey<T>& key,
                          const EventContext& ctx);


} /* namespace SG */



#include "StoreGate/ReadHandle.icc"


#ifndef NO_LEGACY_HANDLES
namespace SG {
  template <class T>
  using RVar = ReadHandle<T>;
} 
#endif


#endif //> !STOREGATE_SG_READHANDLE_H
