/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Identifier utility
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 28 April 2004
 * Last Update  : 07 May 2004
 ***************************************************************************/

#ifndef MUONCALIBIDENTIFIER_IDENTIFIERHASHTABLE_H
# define MUONCALIBIDENTIFIER_IDENTIFIERHASHTABLE_H
// std
#include <iostream>
#include <sstream>
#include <limits.h>
// other packages
// this package
#include "MuonCalibIdentifier/IdentifierToHash.h"

/** @class IdentifierHashTable<T>
 * it uses a multi-dimensional array and identifier fields for fast access of
 * Identifier-to-hash conversions.
 * For hast-to-Identifier conversion it uses a one-dimensional array of Identifiers.
 *
 * The template parameter is a helper class which is used to extract the fields
 * from the identifier. This helper class must derive from class IdHashTableHelper,
 * which is again a templated class, and also serves as a traits class
 * (i.e. supplies some type definitions + number of fields).
 * 
 */
template <class T>
class IdentifierHashTable {
public:
   //
   // nested types
   //
   /** define the type IdType */
   typedef typename T::IdentifierType IdType;
   /** define the type HashType */
   typedef typename T::HashType HashType;
   /** Helper function to get a properly initialised hash */
   static HashType defaultHashValue() {
      return IdentifierToHash<T>::defaultHashValue();
   }
   //
   // Member functions
   //
   /** Default constructor makes empty hash table with default idFields object */
   IdentifierHashTable();
   /** Make empty hash table initialised with idFields object */
   explicit IdentifierHashTable( const T& idFields );
   /** Clear the hashtable */
   void clear();
   /** Number of hashes in the table */
   unsigned int size() const;
   /** Check that the table is internally consistent. An empty table will return false. */
   bool checkValidity() const;
   /** Add an identifier to the table. Return the assigned hash. If adding the entry
       fails, a default HashType is returned. */
   HashType addEntry( const IdType& id );
   /** Get hash from 0 to size()-1. Returns invalid hash if id is not in table. */
   HashType getHash( const IdType& id ) const;
   /** Get identifier from hash. Returns invalid identifier if hash is out of range. */
   IdType getIdentifier( const HashType& hash ) const;
   /** Dump complete table to output stream */
   void dump( std::ostream& os = std::cout ) const;
   /** Dump complete table into a string */
   std::string dumpToString() const;
private:
   IdentifierToHash<T> m_idToHash;
   std::vector<IdType> m_hashToId;
};

template <class T>
inline IdentifierHashTable<T>::IdentifierHashTable()
{}

template <class T>
inline IdentifierHashTable<T>::IdentifierHashTable( const T& idFields )
   : m_idToHash(idFields)
{}

template <class T>
typename IdentifierHashTable<T>::HashType
IdentifierHashTable<T>::addEntry( const typename IdentifierHashTable<T>::IdType& id ) {
#ifdef IDENTIFIERHASHTABLE_DEBUG
   std::cout << "IdentifierHashTable<T>::addEntry(0x" << std::hex << id << std::dec << ")";
#endif
   if ( !T::isValid( id ) ) {
#ifdef IDENTIFIERHASHTABLE_DEBUG
      std::cout << ": id is invalid. Nothing added." << std::endl;    
#endif
      return defaultHashValue();
   }
#ifdef IDENTIFIERHASHTABLE_DEBUG
   std::cout << "::IdToHashTable";
#endif
   HashType theHash( m_hashToId.size() );
   if ( !m_idToHash.addEntry( id, theHash ) ) {
#ifdef IDENTIFIERHASHTABLE_DEBUG
      std::cout << " WARNING: could not add to table" << std::endl;
#endif
      return defaultHashValue();
   } else {
      m_hashToId.push_back( id );
#ifdef IDENTIFIERHASHTABLE_DEBUG
      std::cout << " ADDED at hash=" << theHash << std::endl;
#endif
   }
   
   return theHash;
}

template <class T>
inline typename IdentifierHashTable<T>::HashType
IdentifierHashTable<T>::getHash( const typename IdentifierHashTable<T>::IdType& id ) const {
   return m_idToHash.getHash( id );
}

template <class T>
inline typename IdentifierHashTable<T>::IdType
IdentifierHashTable<T>::getIdentifier( const typename IdentifierHashTable<T>::HashType& hash ) const {
   unsigned int idx = hash;
   if ( idx >= m_hashToId.size() ) return IdType();
   return m_hashToId[ idx ];
}

template <class T>
inline void IdentifierHashTable<T>::clear() {
   m_idToHash.clear();
   m_hashToId.clear();
}

template <class T>
inline unsigned int IdentifierHashTable<T>::size() const {
   return m_hashToId.size();
}

template <class T>
bool IdentifierHashTable<T>::checkValidity() const {
   // check that both tables are same size
   unsigned int idToHashSize = m_idToHash.size();
   if ( idToHashSize != m_hashToId.size() ) {
      std::cout << "IdentifierHashTable<T>::checkValidity() ERROR: idToHash size ("
		<< idToHashSize << ") not equal to hashToId size ("
		<< m_hashToId.size() << ")" << std::endl;
      return false;
   }
   // check that table is non-empty
   if ( !m_hashToId.size() ) {
      std::cout << "IdentifierHashTable<T>::checkValidity() WARNING: Table is empty." << std::endl;
      return false;
   }
   // check that tables are each other's inverse
   unsigned int nErrors = 0;
   for ( unsigned int i = 0; i < m_hashToId.size(); ++i ) {
      HashType tryHash(i);
      IdType id = getIdentifier( tryHash );
      HashType gotHash = getHash( id );
      if ( gotHash != tryHash ) {
	 std::cout << "IdentifierHashTable<T>::checkValidity() ERROR: getIdentifier("
		   << tryHash << ")=0x" << std::hex << id << std::dec
		   << " whereas getHash(" << std::hex << id << std::dec << ")=" << gotHash
		   << std::endl;
	 ++nErrors;
      } else {
#ifdef IDENTIFIERHASHTABLE_DEBUG
	 std::cout << "hash=" << i
		   << " <--> id=0x" << std::hex << id << std::dec << ": OK" << std::endl;
#endif
      }
   }
#ifdef IDENTIFIERHASHTABLE_DEBUG
   if ( nErrors ) {
      std::cout << "IdentifierHashTable<T>::checkValidity() ERROR: table contains "
		<< nErrors << " errors." << std::endl;
   } else {
      std::cout << "IdentifierHashTable<T>::checkValidity(): table OK" << std::endl;
   }
#endif
   
   if ( nErrors ) return false;
   
   // if we get here, everything is OK!
   return true;
}

template <class T>
void IdentifierHashTable<T>::dump( std::ostream& os ) const {
   int n = size();
   for ( int i = 0; i < n; ++i ) {
      HashType iHash(i);
      IdType id = getIdentifier( iHash );
      os << "ID=0x" << std::hex << id << std::dec << " fields";
      os << m_idToHash.dumpOneEntryToString( id ) << "\n";
   }
   int nTotal = m_idToHash.totalSize();
   os << "IdentifierHashTable has " << n << " valid entries and "
      << nTotal - n << " wasted entries" << std::endl;
}

template <class T>
inline std::string IdentifierHashTable<T>::dumpToString() const {
   std::ostringstream oss;
   dump( oss );
   return oss.str();
}

#endif // MUONCALIBIDENTIFIER_IDENTIFIERHASHTABLE_H
