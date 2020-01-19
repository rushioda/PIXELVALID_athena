/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CavernInfraDetectorFactory_h
#define CavernInfraDetectorFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "CavernInfraGeoModel/CavernInfraDetectorManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <string>

class StoreGateSvc;

class CavernInfraDetectorFactory : public GeoVDetectorFactory  
{
 public:
  
  // Constructor:
  CavernInfraDetectorFactory(StoreGateSvc *pDetStore,
			     IRDBAccessSvc *pAccess);
  
  // Destructor:
  ~CavernInfraDetectorFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const CavernInfraDetectorManager * getDetectorManager() const;
  
  // Set version Tag and Node
  void setTagNode(std::string tag, std::string node);

 private:  
  // Illegal operations:
  const CavernInfraDetectorFactory & operator=(const CavernInfraDetectorFactory &right);
  CavernInfraDetectorFactory(const CavernInfraDetectorFactory &right);
  
  // The manager:
  CavernInfraDetectorManager       *detectorManager;
  
  StoreGateSvc             *detectorStore;
  IRDBAccessSvc            *access;
  std::string              m_versionTag;
  std::string              m_versionNode;
};

// Class CavernInfraDetectorFactory 
#endif


