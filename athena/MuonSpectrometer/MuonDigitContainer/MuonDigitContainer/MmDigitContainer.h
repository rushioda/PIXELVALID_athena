/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MmDigitContainer.h

#ifndef MUONDIGITCONTAINER_MMDIGITCONAINTER_H
#define MUONDIGITCONTAINER_MMDIGITCONAINTER_H

// Nektarios Chr. Benekos
// March 2013
// 
// Muon digit container.
//
// This container provides access to muon digits and provides
// a mechanism for recording them.
//
// Converters will enable reading from raw data, zebra and
// conventional StoreGate technologies (Objectivty, root).
// They will also enable writing to all but zebra.
//
// Usage:
//
//   // Create module ID's.
//   MmModuleId bil11(BIL,1,1);
//   MmModuleId bml11(BML,1,1);
//   MmModuleId bol11(BOL,1,1);
//   // Insert some digits.
//   digs.push_back(MmDigit(MmChannelId(bil11, 0, 0,  8), 101));
//   digs.push_back(MmDigit(MmChannelId(bil11, 0, 1, 16), 101));
//   digs.push_back(MmDigit(MmChannelId(bml11, 1, 0, 12),  72));
//   // Check subcontainers.
//   assert( digs.digits(bil11).size() == 2 );
//   assert( digs.digits(bml11).size() == 1 );
//   assert( digs.digits(bol11).size() == 0 );

/** 
    Use IdentifiableContainer with MmDigitCollection
*/ 

#include <vector>
#include <string>
#include "MuonDigitContainer/MmDigitCollection.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 


class MmDigitContainer
:public IdentifiableContainer<MmDigitCollection> 
{
 public:  
   MmDigitContainer(); 
   
  MmDigitContainer(unsigned int hash_max) ; 
  ~MmDigitContainer() ; 

  typedef MmDigitCollection::size_type size_type ; 
  typedef IdentifiableContainer<MmDigitCollection> MyBase; 

  size_type digit_size() const ; 

  // IdentifiableContainer is still a DataObject
  // Put CLID here. 
  static const CLID& classID();

  /** return class ID */
  virtual const CLID& clID() const
  {
    return classID(); 
  }

 private:


};

CLASS_DEF(MmDigitContainer,1274132786,0) 

// Output stream.
     std::ostream& operator<<(std::ostream& lhs, const MmDigitContainer& rhs);

#endif


