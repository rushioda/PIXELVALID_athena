/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowAuxContainer.cxx 793737 2017-01-24 20:11:10Z ssnyder $

// System include(s):
#include <iostream>

// EDM include(s):
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"

// Local include(s):
#include "xAODCore/ShallowAuxContainer.h"

namespace xAOD {

   /// @param standalone <code>true</code> if the store will be used for a
   ///                   standalone object, <code>false</code> for a container
   ///
   ShallowAuxContainer::ShallowAuxContainer( bool standalone )
      : m_selection(), 
        m_store( new SG::AuxStoreInternal( standalone ) ),
        m_storeIO( 0 ), m_ownsStore( true ), m_locked( false ),
        m_parentLink(), m_parentIO( 0 ), m_shallowIO( true ), m_tick( 1 ),
        m_name( "UNKNOWN" ) {

      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
   }

   ShallowAuxContainer::ShallowAuxContainer( const ShallowAuxContainer& parent )
      : SG::IAuxStore(), SG::IAuxStoreIO(), SG::IAuxStoreHolder(),
        m_selection( parent.m_selection ), 
        m_store( parent.m_store ), m_storeIO( parent.m_storeIO ),
        m_ownsStore( false ), m_locked( parent.m_locked ),
        m_parentLink( parent.m_parentLink ),
        m_parentIO( parent.m_parentIO ), m_shallowIO( parent.m_shallowIO ),
        m_tick( 1 ), m_name( parent.m_name ) {

   }

   /// @param parent The parent object to make a shallow copy of
   /// @param standalone <code>true</code> if the store will be used for a
   ///                   standalone object, <code>false</code> for a container
   ///
   ShallowAuxContainer::
   ShallowAuxContainer( const DataLink< SG::IConstAuxStore >& parent,
                        bool standalone )
      : m_selection(), 
        m_store( new SG::AuxStoreInternal( standalone ) ),
        m_storeIO( 0 ), m_ownsStore( true ), m_locked( false ),
        m_parentLink( parent ), m_parentIO( 0 ), m_shallowIO( true ),
        m_tick( 1 ), m_name( "UNKNOWN" ) {

      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      const SG::IAuxStoreIO* temp =
         dynamic_cast< const SG::IAuxStoreIO* >( m_parentLink.cptr() );
      m_parentIO = const_cast< SG::IAuxStoreIO* >( temp );
   }

   ShallowAuxContainer::~ShallowAuxContainer() {

      if( m_ownsStore && m_store ) {
         delete m_store;
      }
   }

   ShallowAuxContainer&
   ShallowAuxContainer::operator= ( const ShallowAuxContainer& rhs ) {

      guard_t guard (m_mutex);
      // Check if anything needs to be done:
      if( this == &rhs ) {
         return *this;
      }

      // Clean up if necessary:
      if( m_ownsStore && m_store ) {
         delete m_store;
         m_store = 0;
      }

      m_selection  = rhs.m_selection;
      m_store      = rhs.m_store;
      m_storeIO    = rhs.m_storeIO;
      m_ownsStore  = false;
      m_locked     = rhs.m_locked;
      m_parentLink = rhs.m_parentLink;
      m_parentIO   = rhs.m_parentIO;
      m_shallowIO  = rhs.m_shallowIO;
      m_name       = rhs.m_name;
      ++m_tick;

      // Return this object:
      return *this;
   }

   const DataLink< SG::IConstAuxStore >& ShallowAuxContainer::parent() const {

      return m_parentLink;
   }

   void ShallowAuxContainer::
   setParent( const DataLink< SG::IConstAuxStore >& link ) {

      guard_t guard (m_mutex);
      m_parentLink = link;
      const SG::IAuxStoreIO* temp =
         dynamic_cast< const SG::IAuxStoreIO* >( m_parentLink.cptr() );
      m_parentIO = const_cast< SG::IAuxStoreIO* >( temp );
      ++m_tick;
      return;
   }

   bool ShallowAuxContainer::shallowIO() const {

      return m_shallowIO;
   }

   void ShallowAuxContainer::setShallowIO( bool value ) {

      guard_t guard (m_mutex);
      m_shallowIO = value;
      return;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //           Implementation of the SG::IAuxStoreHolder interface
   //

   SG::IAuxStore* ShallowAuxContainer::getStore() const {

      return m_store;
   }

   void ShallowAuxContainer::setStore( SG::IAuxStore* store ) {

      // Check if anything needs to be done:
      guard_t guard (m_mutex);
      if( store == m_store ) return;

      if( m_ownsStore && m_store ) {
         delete m_store;
         m_store = 0;
      }

      // Do the assignment:
      m_store = store;
      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      m_ownsStore = true;
      ++m_tick;

      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the SG::IConstAuxStore interface
   //

   const void* ShallowAuxContainer::getData( auxid_t auxid ) const {

      guard_t guard( m_mutex );

      // Check if we have this variable ourselves. I use getData(...)
      // instead of getAuxIDs(), as a dynamic store may not have connected
      // to all the variables yet that it has available...
      size_t nids = m_store->getAuxIDs().size();
      const void* result = m_store->getData( auxid );
      if( result ) {
         if( nids != m_store->getAuxIDs().size() ) {
            ++m_tick;
         }
         return result;
      }

      // If not, then leave it up to the parent object do deal with this:
      if( m_parentLink.isValid() ) {
         nids = m_parentLink->getAuxIDs().size();
         result = m_parentLink->getData( auxid );
         if( result && ( nids != m_parentLink->getAuxIDs().size() ) ) {
            ++m_tick;
         }
         return result;
      }

      // Apparently the variable wasn't found:
      return 0;
   }

   const ShallowAuxContainer::auxid_set_t&
   ShallowAuxContainer::getAuxIDs() const {

      guard_t guard (m_mutex);
      if( m_tsAuxids.get() == 0 ) {
         m_tsAuxids.reset( new TSAuxidSet );
      }

      if( m_tsAuxids->m_tick != m_tick ) {
         if( m_parentLink.isValid() ) {
            m_tsAuxids->m_set = m_parentLink->getAuxIDs();
         } else {
            m_tsAuxids->m_set.clear();
         }
         const SG::auxid_set_t& store_auxids = m_store->getAuxIDs();
         m_tsAuxids->m_set.insert( store_auxids.begin(), store_auxids.end() );
         m_tsAuxids->m_tick = m_tick;
      }
      return m_tsAuxids->m_set;
   }

   /// Return the data vector for one aux data decoration item.
   void* ShallowAuxContainer::getDecoration( auxid_t auxid,
                                             size_t size,
                                             size_t capacity ) {

      guard_t guard( m_mutex );

      // If the parent has such a variable, then we need to check one more
      // thing. If it's a decoration on the parent, then we should be allowed
      // to override it in this (possibly locked) shallow copy. But let's leave
      // the logic of this up to the parent. We ask the parent to return this
      // variable as a decoration. If this is not possible (because it's not a
      // decoration, but a regular variable), then let the parent throw the
      // exception. Otherwise we continue, and let the internal store return
      // a pointer to this derivation. (Which may still throw an exception.)
      //
      // It's quite ugly, as we have to do a const_cast to do this. But the
      // SG::IConstAuxStore interface doesn't provide any other way of figuring
      // out whether a given variable is a regular variable or a decoration.
      //
      if( m_locked && m_parentLink.isValid() &&
          ( m_parentLink->getAuxIDs().count( auxid ) > 0 ) ) {
         SG::IConstAuxStore* parent =
            const_cast< SG::IConstAuxStore* >( m_parentLink.cptr() );
         parent->getDecoration( auxid, size, capacity );
      }

      // If we got this far without any exception, then let the internal store
      // try to provide the decoration for us:
      const size_t nids = m_store->getAuxIDs().size();
      void* result = m_store->getDecoration( auxid, size, capacity );
      if( result && ( nids != m_store->getAuxIDs().size() ) ) {
         ++m_tick;
      }
      return result;
   }


   /// Lock the container.
   void ShallowAuxContainer::lock()
   { 
     guard_t guard (m_mutex);
     m_locked = true;
     m_store->lock();
   }


   /// Clear all decorations.
   void ShallowAuxContainer::clearDecorations()
   { 
     guard_t guard (m_mutex);
     m_store->clearDecorations();
     ++m_tick;
   }

   size_t ShallowAuxContainer::size() const {

      guard_t guard( m_mutex );
      size_t sz = m_store->size();
      if( sz > 0 ) {
         return sz;
      }
      if( m_parentLink.isValid() ) {
         return m_parentLink->size();
      }
      return 0;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //                Implementation of the SG::IAuxStore interface
   //

   void* ShallowAuxContainer::getData( auxid_t auxid, size_t size,
                                       size_t capacity ) {

      guard_t guard (m_mutex);
      // Remember that we now manage this variable:
      ++m_tick;

      // Create the variable in the dynamic store:
      void* ptr = m_store->getData( auxid, size, capacity );

      // If the parent doesn't have this variable, then we're done already:
      if( ! m_parentLink.isValid() ) {
         return ptr;
      }
      const void* pptr = m_parentLink->getData( auxid );
      if( ! pptr ) {
         return ptr;
      }

      // If the variable does exist in the parent, then copy it over to this
      // store before returning.

      // Get the registry:
      SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

      // Get the type of this variable:
      const std::type_info* type = reg.getType( auxid );
      if( ! type ) {
         std::cerr << "xAOD::ShallowAuxContainer::getData ERROR Couldn't find "
                   << "the type of auxiliary ID " << auxid << std::endl;
         std::cerr << "xAOD::ShallowAuxContainer::getData ERROR Can't copy the "
                   << "contents of the parent's decoration" << std::endl;
         return ptr;
      }

      // First let's get the vector factory of this variable:
      const SG::IAuxTypeVectorFactory* factory =
         SG::AuxTypeRegistry::instance().getFactory( *type );
      if( ! factory ) {
         std::cerr << "xAOD::ShallowAuxContainer::getData ERROR Couldn't find "
                   << "factory for type " << type->name() << std::endl;
         std::cerr << "xAOD::ShallowAuxContainer::getData ERROR Can't copy the "
                   << "contents of the parent's decoration" << std::endl;
         return ptr;
      }

      // Copy each element of the parent's decoration:
      for( size_t i = 0; i < size; ++i ) {
         factory->copy( ptr, i, pptr, i );
      }

      // Now we're done:
      return ptr;
   }

   const ShallowAuxContainer::auxid_set_t&
   ShallowAuxContainer::getWritableAuxIDs() const {

      // Only the variables in the dynamic store are writable. Maybe not
      // even all of those...
      return m_store->getWritableAuxIDs();
   }

   bool ShallowAuxContainer::resize( size_t /*size*/ ) {

      // Nope, not allowed...
      throw std::runtime_error( "Trying to call resize on a shallow copy "
                                "container" );
   }

   void ShallowAuxContainer::reserve( size_t /*size*/ ) {

      // Nope, not allowed...
      throw std::runtime_error( "Trying to call reserve on a shallow copy "
                                "container" );
   }

   void ShallowAuxContainer::shift( size_t /*pos*/, ptrdiff_t /*offs*/ ) {

      // Nope, not allowed...
      throw std::runtime_error( "Trying to call shift on a shallow copy "
                                "container" );
   }

   bool ShallowAuxContainer::insertMove( size_t /*pos*/,
                                         IAuxStore& /*other*/,
                                         const SG::auxid_set_t& /*ignore*/) {

      // Nope, not allowed...
      throw std::runtime_error( "Trying to call insertMove on a shallow copy "
                                "container" );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the SG::IAuxStoreIO interface
   //

   const void* ShallowAuxContainer::getIOData( auxid_t auxid ) const {

      guard_t guard (m_mutex);
      // Do we have it?
      const SG::auxid_set_t& store_auxids = m_store->getAuxIDs();
      if( m_storeIO && ( store_auxids.find( auxid ) !=
                         store_auxids.end() ) ) {
         return m_storeIO->getIOData( auxid );
      }

      // Do we have a parent that has it?
      if( m_parentLink.isValid() ) {
         if( ! m_parentIO ) {
            const SG::IAuxStoreIO* temp =
               dynamic_cast< const SG::IAuxStoreIO* >( m_parentLink.cptr() );
            m_parentIO = const_cast< SG::IAuxStoreIO* >( temp );
         }
         if( m_parentIO ) {
            return m_parentIO->getIOData( auxid );
         }
      }

      // If not, then where did this variable come from?!?
      std::cerr << "ERROR xAOD::ShallowAuxContainer::getIOData Unknown "
                << "variable ("
                << SG::AuxTypeRegistry::instance().getName( auxid )
                << ") requested" << std::endl;
      return 0;
   }

   const std::type_info* ShallowAuxContainer::getIOType( auxid_t auxid ) const {

      guard_t guard (m_mutex);
      // Do we have it?
      const SG::auxid_set_t& store_auxids = m_store->getAuxIDs();
      if( m_storeIO && ( store_auxids.find( auxid ) !=
                         store_auxids.end() ) ) {
         return m_storeIO->getIOType( auxid );
      }

      // Do we have a parent that has it?
      if( m_parentLink.isValid() ) {
         if( ! m_parentIO ) {
            const SG::IAuxStoreIO* temp =
               dynamic_cast< const SG::IAuxStoreIO* >( m_parentLink.cptr() );
            m_parentIO = const_cast< SG::IAuxStoreIO* >( temp );
         }
         if( m_parentIO ) {
            return m_parentIO->getIOType( auxid );
         }
      }

      // If not, then where did this variable come from?!?
      std::cerr << "ERROR xAOD::ShallowAuxContainer::getIOType Unknown "
                << "variable ("
                << SG::AuxTypeRegistry::instance().getName( auxid )
                << ") requested" << std::endl;
      return 0;
   }

   const ShallowAuxContainer::auxid_set_t&
   ShallowAuxContainer::getDynamicAuxIDs() const {

      if( m_shallowIO ) {
         if( m_storeIO ) {
            return m_store->getAuxIDs();
         } else {
            static const auxid_set_t dummy {};
            return dummy;
         }
      } else {
         return getAuxIDs();
      }
   }

   void ShallowAuxContainer::
   selectAux( const std::set< std::string >& attributes ) {

      guard_t guard (m_mutex);
      m_selection.selectAux( attributes );
      return;
   }

   const ShallowAuxContainer::auxid_set_t&
   ShallowAuxContainer::getSelectedAuxIDs() const {

      if( m_shallowIO ) {
         if( m_storeIO ) {
            return m_selection.getSelectedAuxIDs( m_store->getAuxIDs() );
         } else {
            static const auxid_set_t dummy {};
            return dummy;
         }
      } else {
         return m_selection.getSelectedAuxIDs( getAuxIDs() );
      }
   }


   //
   /////////////////////////////////////////////////////////////////////////////

   const char* ShallowAuxContainer::name() const {

      return m_name.c_str();
   }

   void ShallowAuxContainer::setName( const char* name ) {

      m_name = name;
      return;
   }

} // namespace xAOD
