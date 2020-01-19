/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EscapedEnergyRegistry
// 15-Jul-2004 William Seligman

#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "CaloG4Sim/VEscapedEnergyProcessing.h"

#include "G4ios.hh"
#include "globals.hh"

#include <map>

namespace CaloG4 {

  // Standard implementation of a singleton pattern.

  EscapedEnergyRegistry* EscapedEnergyRegistry::GetInstance()
  {
    static EscapedEnergyRegistry instance;
    return &instance;
  }

  EscapedEnergyRegistry::EscapedEnergyRegistry()
  {;}

  EscapedEnergyRegistry::~EscapedEnergyRegistry()
  {
    // Delete all the pointers we've adopted.
    m_processingMap_ptr_t i;
    for ( i = m_processingMap.begin(); i != m_processingMap.end(); i++ )
      {
	delete (*i).second;
      }
  }

  void EscapedEnergyRegistry::AddAndAdoptProcessing( const G4String& name, VEscapedEnergyProcessing* process )
  {
    // Don't bother adding a null pointer.
    if ( process == 0 ) return;

    // Check that we're not adding any duplicates.
    m_processingMap_ptr_t i;
    for ( i = m_processingMap.begin(); i != m_processingMap.end(); i++ )
      {
	if ( name == (*i).first )
	  {
	    G4cout << "CaloG4Sim::EscapedEnergyRegistry::AddAndAdoptProcessing -"
		   << G4endl;
	    G4cout << "   Trying to add a second VEscapedEnergyProcessing with the name '"
		   << name
		   << "'" << G4endl;
	    G4cout << "   Entry is rejected!" << G4endl;
	    return;
	  }
	if ( process == (*i).second )
	  {
	    G4cout << "CaloG4Sim::EscapedEnergyRegistry::AddAndAdoptProcessing -"
		   << G4endl;
	    G4cout << "   The key '"
		   << name
		   << "' has the same VEscapedEnergyProcessing object as the key '"
		   << (*i).first
		   << "'" << G4endl;
	    G4cout << "   Entry is rejected!" << G4endl;
	    return;
	  }
      }

    // There are no duplicates, so add the entry.
    m_processingMap[ name ] = process;
  }

  VEscapedEnergyProcessing* EscapedEnergyRegistry::GetProcessing( const G4String& volumeName ) const
  {
    // Search through the map.  If we find an entry whose text string
    // is a substring of the volume name (e.g., "LAr::" is a substring
    // of "LAr::BarrelCryostat::MotherVolume"), then return that
    // VEscapedEnergyProcessing object.

    // Reminder:
    // m_processingMap = consists of pair< G4String, VEscapedEnergyProcessing* >
    // i = iterator (pointer) to a m_processingMap entry.
    // (*i) = pair< G4String, VEscapedEnergyProcessing* >
    // (*i).first = a G4String
    // (*i).second = a VEscapedEnergyProcessing*

    m_processingMap_const_ptr_t i;
    for ( i = m_processingMap.begin(); i != m_processingMap.end(); i++ )
      {
	if ( volumeName.contains( (*i).first ) )
	  return (*i).second;
      }

    // If we get here, then there was no entry in the map that
    // matched any portion of the volume name.

    return 0;
  }

} // namespace CaloG4
