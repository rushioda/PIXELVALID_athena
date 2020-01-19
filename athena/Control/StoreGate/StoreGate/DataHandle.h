/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHandle.h
 *  defines an "iterator" over instances of a given type in StoreGateSvc
 */
#ifndef STOREGATE_DATAHANDLE_H
#define STOREGATE_DATAHANDLE_H

#include "SGTools/DataBucket.h"
#include "SGTools/ProxyMap.h"
#include "AthenaKernel/IResetable.h"
#include "AthenaKernel/IProxyDict.h"
#include "SGTools/DataProxy.h"
#include "SGTools/DataHandleBase.h"
#include <iterator>

template <typename DATA>
class DataHandle;

template <class DATA>
bool operator== (const DataHandle<DATA>& h1,
                 const DataHandle<DATA>& h2);

template <class DATA>
bool operator!= (const DataHandle<DATA>& h1,
                 const DataHandle<DATA>& h2);

/** @class DataHandle
 * @brief an iterator over instances of a given type in StoreGateSvc. It d-casts
 * and caches locally the pointed-at object, to speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * Holds a reference count on the proxy to which it's currently pointing.
 * Note: one may think that we should hold reference counts for the
 * entire range which the DataHandle references (if it is a range).
 * The problem with this is that the range may change underneath us.
 * For example, if someone does a SG record, then a new element may appear
 * within the range.  So we only hold the refcount for the object
 * to which we're currently pointing.
 *
 * @param DATA the data object type
 * @author ATLAS Collaboration
 * $Id: DataHandle.h,v 1.40 2008-09-26 23:37:52 calaf Exp $
 **/
template <typename DATA> 
class DataHandle :
  public DataHandleBase,
  public std::iterator<std::forward_iterator_tag, DATA> 
{
public:
  typedef std::iterator<std::forward_iterator_tag, DATA> base_t;

  //FIXME this should come from iterator inher!
  typedef typename base_t::iterator_category iterator_category;
  typedef typename base_t::value_type value_type;
  typedef typename base_t::difference_type difference_type;
  typedef typename base_t::pointer pointer_type; 
  typedef const DATA* const_pointer_type; 
  typedef typename base_t::reference reference_type;
  typedef const DATA& const_reference_type; 

  typedef DataHandleBase::ID_type ID_type;

  /// \name structors and assignment
  //@{
  DataHandle();
  DataHandle(const DataHandle& h);
  DataHandle& operator= (const DataHandle& h);
  DataHandle& operator= (const DATA& d) 
  { 
    typename DataHandle<DATA>::pointer_type ptr = this->ptr();
    if (ptr) {
      *ptr = d; 
    } else {
      std::cerr << "invalid proxy\n";
    }
    return *this; 
  }

  virtual ~DataHandle();  ///< unbind from the proxy before we go
  //@}

  /// \name validity checks
  //@{
  bool isValid() const; ///<RETRIEVES the DO to check it is valid and unlocked
  bool isValid();       ///<RETRIEVES the DO to check it is valid

  // FIXME op! is to keep backward compatibility with Gaudi
  // FIXME similar to checking the SmartDataPtr
  // FIXME dangerous stuff: remove!
  ///DEPRECATED for statements like:  if (!DataHandle<XXX>) {...} 
  bool operator !() const { return !isValid(); }

  //FIXME VERY dangerous stuff: remove!
  ///DEPRECATED for statements like:  if (DataHandle<XXX>) {...} 
  operator int() const  { return isValid(); }
  //@}

  /// \name iterator interface
  //@{
  const DataHandle& operator++ () const;        ///<prefix
  DataHandle operator++ (int) const;           ///<postfix
    
  const_pointer_type operator->() const   { return cptr(); }
  pointer_type operator->()               { return ptr();  }

  const_reference_type operator*() const    { return *cptr(); }   
  reference_type operator*()                { return *ptr();  }
  //@}

  /// \name access to the underlying ptr
  //@{
  operator pointer_type()             { return ptr(); }  ///< often ambiguos
  operator const_pointer_type() const { return cptr(); } ///< often ambiguos

  const_pointer_type cptr() const;   ///< safer explicit ptr accessor 
  pointer_type ptr();                ///< safer explicit ptr accessor 

  // Should be like this.
  //virtual void reset (bool /*hard*/) override { m_ptr = 0; }        ///< reset pointer
  // Temp workaround for TrigDecisionTool back-compat.
  virtual void reset (bool /*hard*/) override { m_ptr = 0; reset(); }
  virtual void reset () override { m_ptr = 0; }
  //@}

  /// \name other constructors and methods for SG internal use
  //@{
  DataHandle(SG::DataProxy* proxy); ///< 
  DataHandle(const SG::ConstProxyIterator& itr1,
             const SG::ConstProxyIterator& itr2);

  //@}

  /// the CLID of the object we are bound to
  virtual CLID clid() const override { return ClassID_traits<DATA>::ID(); }

  friend
  bool operator==<>(const DataHandle<DATA>& h1,
                    const DataHandle<DATA>& h2); 
  friend
  bool operator!=<>(const DataHandle<DATA>& h1,
                    const DataHandle<DATA>& h2); 
private:

  mutable pointer_type m_ptr;
  pointer_type dataPointer() const;


};

/// \name Comparison ops (compare proxies)
//@{
template <class DATA>
bool operator==(const DataHandle<DATA>& h1,
                const DataHandle<DATA>& h2)
{
  return (h1.m_proxy == h2.m_proxy); 
}
template <class DATA>
bool operator!=(const DataHandle<DATA>& h1,
                const DataHandle<DATA>& h2)
{
  return (h1.m_proxy != h2.m_proxy); 
}
//@}

#ifndef STOREGATE_DATAHANDLE_ICC
#include "StoreGate/DataHandle.icc"
#endif

/* FIXME LEGACY - No dependency on ActiveStoreSvc here, but a number of Muon AtlasEvent packages are 
   getting the include through this one!!!! */

#include "StoreGate/ActiveStoreSvc.h"

#endif // STOREGATE_DATAHANDLE_H











