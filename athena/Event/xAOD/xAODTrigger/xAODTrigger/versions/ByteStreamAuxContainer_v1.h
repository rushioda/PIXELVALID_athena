// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ByteStreamAuxContainer_v1.h 797235 2017-02-14 20:30:30Z ssnyder $
#ifndef XAODTRIGGER_VERSIONS_BYTESTREAMAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_BYTESTREAMAUXCONTAINER_V1_H

// STL include(s):
#include <vector>
#include <map>
#include <string>

// EDM include(s):
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainers/tools/threading.h"
#ifndef XAOD_STANDALONE
#   include "AthenaKernel/ILockable.h"
#endif

// Forward declaration(s):
namespace SG {
   class IAuxTypeVector;
}

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

   /// Base class for dynamic auxiliary stores saved into ByteStream
   ///
   /// The point of this class is to behave similarly to xAOD::AuxContainerBase.
   /// The trigger developers can develop their specific auxiliary store
   /// classes on top of this class. The base class can manage the storage of
   /// simple variable types, while the classes inheriting from it need to
   /// manage any specific variables that they want to store.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 797235 $
   /// $Date: 2017-02-14 21:30:30 +0100 (Tue, 14 Feb 2017) $
   ///
   class ByteStreamAuxContainer_v1
     : public SG::IAuxStore
#ifndef XAOD_STANDALONE
     , public ILockable
#endif
   {

   public:
      /// The aux ID type definition from IConstAuxStore
      typedef SG::auxid_t auxid_t;
      /// The aux ID set type definition from IConstAuxStore
      typedef SG::auxid_set_t auxid_set_t;

      /// Default constructor
      ByteStreamAuxContainer_v1();
      /// Copy constructor
      ByteStreamAuxContainer_v1( const ByteStreamAuxContainer_v1& parent );
      /// Destructor
      ~ByteStreamAuxContainer_v1();

      /// Assignment operator
      ByteStreamAuxContainer_v1&
      operator=( const ByteStreamAuxContainer_v1& rhs );

      /// @name Functions implementing the SG::IConstAuxStore interface
      /// @{

      /// Get a pointer to a given array
      virtual const void* getData( auxid_t auxid ) const;

      /// Get the types(names) of variables handled by this container
      virtual const auxid_set_t& getAuxIDs() const;

      /// Get a pointer to a given array, as a decoration.
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

      /// Function resetting the internal (cached) state of the object
      void reset();

      /// @name Functions managing the instance name of the container
      /// @{

      /// Get the name of the container instance
      const char* name() const;
      /// Set the name of the container instance
      void setName( const char* name );

      /// @}

   protected:
      /// Register one of the user defined persistent variables internally
      template< typename T >
      void regAuxVar( const std::string& name,
                      std::vector< T >& vec );

   private:
      /// Internal method: return size without taking out the lock.
      size_t size_noLock() const;

     /// Function retrieving a simple dynamic variable.
      /// If capacity > 0, a new verable will be created if necessary.
      template< typename T >
      void* getData1( auxid_t auxid,
                      std::map< std::string, std::vector< T > >& pers,
                      size_t size, size_t capacity, bool quiet,
                      bool forDecor) const;

      void* getData1( auxid_t auxid,
                      size_t size, size_t capacity, bool quiet,
                      bool forDecor ) const;

      /// @name Variables persisified into the ByteStream
      /// @{

      /// Variable holding integer auxiliary variables
      mutable std::map< std::string, std::vector< int > > m_int;
      /// Variable holding float auxiliary variables
      mutable std::map< std::string, std::vector< float > > m_float;
      /// Variable holding vector of integer auxiliary variables
      mutable std::map< std::string, std::vector< std::vector< int > > > m_vecInt;
      /// Variable holding vector of float auxiliary variables
      mutable std::map< std::string, std::vector< std::vector< float > > > m_vecFloat;

      /// @}

      /// @name Transient variables used to implement the IAuxStore interface
      /// @{

      /// Internal list of auxiliary variables
      mutable auxid_set_t m_auxids;
      /// Internal list of static managed variables
      std::vector< SG::IAuxTypeVector* > m_staticVecs;
      /// Internal list of dynamic managed variables
      mutable std::vector< SG::IAuxTypeVector* > m_dynamicVecs;
      /// Has the container been locked?
      bool m_locked;
      /// Mark variables as decorations.
      std::vector<bool> m_isDecoration;

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
      };
      mutable AthContainers_detail::thread_specific_ptr<TSAuxidSet> m_tsAuxids;

      /// @}

      /// Name of the container in memory. Set externally.
      std::string m_name;

   }; // class ByteStreamAuxContainer_v1

} // namespace xAOD

// Declare the inheritace of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ByteStreamAuxContainer_v1, SG::IAuxStore );

/// @namespace SG
/// Namespace used by StoreGate for some of its internal structures/classes

// Include the template implementation:
#include "ByteStreamAuxContainer_v1.icc"

#endif // XAODTRIGGER_VERSIONS_BYTESTREAMAUXCONTAINER_V1_H
