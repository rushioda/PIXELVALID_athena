// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowAuxContainer.h 793737 2017-01-24 20:11:10Z ssnyder $
#ifndef XAODCORE_SHALLOWAUXCONTAINER_H
#define XAODCORE_SHALLOWAUXCONTAINER_H

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/tools/threading.h"
#ifndef XAOD_STANDALONE
#   include "AthenaKernel/ILockable.h"
#endif // not XAOD_STANDALONE

// Local include(s):
#include "xAODCore/AuxSelection.h"

namespace xAOD {

   /// Class creating a shallow copy of an existing auxiliary container
   ///
   /// During analysis, in order to create objects that override just some
   /// of the properties of an original, it's wasteful to make full copies
   /// of the objects. Not to mention that the full copies have generally
   /// issues with dynamic variables.
   ///
   /// The behaviour of this class is that it takes all the properties that
   /// it doesn't know about from the parent auxiliary store, but the ones
   /// that it has an override for, it provides itself.
   ///
   /// The object is persistifiable. The user can choose whether to only
   /// write out the variables that were modified wrt. the original container,
   /// or possibly all the variables. (In case the original container is
   /// not getting written out.)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 793737 $
   /// $Date: 2017-01-24 21:11:10 +0100 (Tue, 24 Jan 2017) $
   ///
   class ShallowAuxContainer : public SG::IAuxStore,
                               public SG::IAuxStoreIO,
                               public SG::IAuxStoreHolder
#ifndef XAOD_STANDALONE
                          , public ILockable
#endif // not XAOD_STANDALONE
   {

   public:
      /// The aux ID type definition
      typedef SG::auxid_t auxid_t;
      /// The aux ID set type definition
      typedef SG::auxid_set_t auxid_set_t;

      /// Default constructor
      ShallowAuxContainer( bool standalone = false );
      /// Copy constructor
      ShallowAuxContainer( const ShallowAuxContainer& parent );
      /// Constructor with a parent object
      ShallowAuxContainer( const DataLink< SG::IConstAuxStore >& parent,
                           bool standalone = false );
      /// Destructor
      ~ShallowAuxContainer();

      /// Assignment operator
      ShallowAuxContainer& operator= ( const ShallowAuxContainer& rhs );

      /// Get the parent store
      const DataLink< SG::IConstAuxStore >& parent() const;
      /// Set the parent store
      void setParent( const DataLink< SG::IConstAuxStore >& link );

      /// Check whether only the overriden parameters will be written out or not
      bool shallowIO() const;
      /// Set whether only the overriden parameters should be written out
      void setShallowIO( bool value );

      /// @name Functions implementing the SG::IAuxStoreHolder interface
      /// @{

      /// Get the currently used internal store object
      virtual SG::IAuxStore* getStore() const;
      /// Set a different internal store object
      virtual void setStore( SG::IAuxStore* store );
      /// Return the type of the store object
      virtual AuxStoreType getStoreType() const { return AST_ContainerStore; }

      /// @}

      /// @name Functions implementing the SG::IConstAuxStore interface
      /// @{

      /// Get a pointer to a given array
      virtual const void* getData( auxid_t auxid ) const;

      /// Get the types(names) of variables handled by this container
      virtual const auxid_set_t& getAuxIDs() const;

      /// Return the data vector for one aux data decoration item.
      virtual void* getDecoration (auxid_t auxid, size_t size, size_t capacity);
      /// Lock the container.
      virtual void lock();
      /// Clear all decorations.
      virtual void clearDecorations();
      /// Get the size of the container.
      virtual size_t size() const;
      /// @}

      /// @name Functions implementing the SG::IAuxStore interface
      /// @{

      /// Get a pointer to a given array, creating the array if necessary
      virtual void* getData( auxid_t auxid, size_t size,
                             size_t capacity );

      /// Return a set of writable data identifiers
      virtual const auxid_set_t& getWritableAuxIDs() const;

      /// Resize the arrays to a given size
      virtual bool resize( size_t size );
      /// Reserve a given size for the arrays
      virtual void reserve( size_t size );
      /// Shift the contents of the stored arrays
      virtual void shift( size_t pos, ptrdiff_t offs );
      /// Insert contents of another store via move.
      virtual bool insertMove (size_t pos,
                               IAuxStore& other,
                               const SG::auxid_set_t& ignore);
 
      /// @}

      /// @name Functions implementing the SG::IAuxStoreIO interface
      /// @{

      /// Get a pointer to the data being stored for one aux data item
      virtual const void* getIOData( auxid_t auxid ) const;

      /// Return the type of the data to be stored for one aux data item
      virtual const std::type_info* getIOType( auxid_t auxid ) const;

      /// Get the types(names) of variables created dynamically
      virtual const auxid_set_t& getDynamicAuxIDs() const;

      /// Select dynamic Aux attributes by name (for writing)
      virtual void selectAux( const std::set< std::string >& attributes );

      /// Get the IDs of the selected dynamic Aux variables (for writing)
      virtual const auxid_set_t& getSelectedAuxIDs() const;

      /// @}

      /// @name Functions managing the instance name of the container
      /// @{

      /// Get the name of the container instance
      const char* name() const;
      /// Set the name of the container instance
      void setName( const char* name );

      /// @}

   private:
      /// Dynamic attributes selection implementation
      AuxSelection m_selection;

      /// Internal dynamic auxiliary store object
      SG::IAuxStore* m_store;
      /// The IO interface to the internal auxiliary store
      SG::IAuxStoreIO* m_storeIO;
      /// Flag deciding if the object owns the dynamic store or not
      bool m_ownsStore;
      /// Flag keeping track of whether this object is locked or not
      bool m_locked;

      /// Link to the parent object
      DataLink< SG::IConstAuxStore > m_parentLink;
      /// Optional pointer to the IO interface of the parent object
      mutable SG::IAuxStoreIO* m_parentIO;
      /// Flag for whether to do "shallow IO" or not
      bool m_shallowIO;

      /// Count changes to @c m_auxids.
      mutable size_t m_tick;

      /// Mutex for multithread synchronization.
      typedef AthContainers_detail::mutex mutex_t;
      typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
      mutable mutex_t m_mutex;

      /// Thread-local versions of the auxid set.
      struct TSAuxidSet
      {
        size_t m_tick;
        auxid_set_t m_set;
        TSAuxidSet (size_t tick, const auxid_set_t& set)
          : m_tick (tick), m_set (set) {}
        TSAuxidSet ()
          : m_tick (0) {}
      };
      mutable AthContainers_detail::thread_specific_ptr<TSAuxidSet> m_tsAuxids;

      /// Name of the container in memory. Set externally.
      std::string m_name;

   }; // class ShallowAuxContainer

} // namespace xAOD

// Declare a class ID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ShallowAuxContainer, 1111823638, 1 )

// Describe the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASES3( xAOD::ShallowAuxContainer, SG::IAuxStore, SG::IAuxStoreIO,
           SG::IAuxStoreHolder );

#endif // XAODCORE_SHALLOWAUXCONTAINER_H
