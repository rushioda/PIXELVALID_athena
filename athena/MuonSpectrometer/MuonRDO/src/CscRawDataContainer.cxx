/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawDataContainer.h"
#include "EventContainers/SelectAllObject.h"

// Default constructor.
CscRawDataContainer::CscRawDataContainer()
  : IdentifiableContainer<CscRawDataCollection>(0) 
{
    DLV::init(hashFcn().max());	
}

CscRawDataContainer::CscRawDataContainer(unsigned int hashmax)
  : IdentifiableContainer<CscRawDataCollection>(hashmax) 
{
}

// Destructor.
CscRawDataContainer::~CscRawDataContainer() 
{}

/// Convert identifier to idhash
unsigned int 
CscRawDataContainer::idToHash(unsigned int id) const
{
    return (hashFcn()(id));
}

/// Convert identifier to idhash
CscRawDataCollectionIdHash& 
CscRawDataContainer::hashFcn() const
{

    static CscRawDataCollectionIdHash hashFcn;
    return (hashFcn);
}

// Class ID
const CLID& CscRawDataContainer::classID()
{
  return ClassID_traits<CscRawDataContainer>::ID();       
}


// Insert a RawData
void CscRawDataContainer::push_back(CscRawData * /*rawData*/)
{
  // uint16_t collectionId = rawData->identify();
  // 
  // // find CscRawData
  // MyBase::const_iterator it = MyBase::indexFind(collectionId);
  // if(it!=MyBase::end())
  //   {
  //     const CscRawDataCollection * const_coll = *it;
  //     CscRawDataCollection * coll = const_cast<CscRawDataCollection *>(const_coll);
  //     coll->push_back(rawData); 
  //   } 
  // else std::cout << "CscRawDataContainer::ERROR : RawData ID not found, ID = " 
  //         << collectionId << std::endl;
}
