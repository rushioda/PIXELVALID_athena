/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetRawDataContainer.h
//   Header file for class InDetRawDataContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container for Raw Data Objects collections for Pixel, SCT and TRT
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef INDETRAWDATA_INDETRAWDATACONTAINER_H
#define INDETRAWDATA_INDETRAWDATACONTAINER_H

// Base classes
#include "EventContainers/IdentifiableContainer.h"
//Needed Classes
#include "InDetRawData/InDetRawDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

template<class CollectionT>
class InDetRawDataContainer 
: public IdentifiableContainer<CollectionT>{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  InDetRawDataContainer(unsigned int max);
  
  // Destructor:
  virtual ~InDetRawDataContainer();

   /** return class ID */
   static const CLID& classID() 
   {
     return ClassID_traits< InDetRawDataContainer <CollectionT> > ::ID();
   }

   /** return class ID */
   virtual const CLID& clID() const
    {
      return classID();
    }


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  InDetRawDataContainer(const InDetRawDataContainer&);
  InDetRawDataContainer &operator=(const InDetRawDataContainer&);

public:
  // The default constructor is required in DEBUG builds (opt is OK),
  // by boost's remove_const.hpp via T_AthenaPoolTPConverter
  InDetRawDataContainer(){};

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};
// member functions that use Collection T
#include"InDetRawData/InDetRawDataContainer.icc"
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#endif // INDETRAWDATA_INDETRAWDATACONTAINER_H
