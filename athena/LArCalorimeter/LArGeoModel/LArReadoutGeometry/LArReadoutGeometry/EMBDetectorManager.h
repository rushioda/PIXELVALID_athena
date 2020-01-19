/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBDetectorManager_h
#define EMBDetectorManager_h 1
#include "CLIDSvc/CLASS_DEF.h"
#include <vector>
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "LArReadoutGeometry/EMBAccordionDetails.h"
class EMBDetDescr;
class EMBDetectorRegion;
class EMBBasicReadoutNumbers;
class EMBHVManager;
class EMBPresamplerHVManager;
/**
 *     @brief A manager class providing access to readout geometry information
 *     for the electromagnetic barrel calorimeter.
 */

/**
 *	The EMBDetectorManager provides access to EMB Regions,
 *	to Descriptors for EMB regions, to the physical volumes
 *	(tree tops) within the EMB, and to a number of important
 *	engineering numbers within the EMB.
 */

class EMBDetectorManager : public GeoVDetectorManager  
{
  
 public:
  typedef std::vector<const EMBDetectorRegion  *>::const_iterator DetectorRegionConstIterator;
  typedef EMBDetectorRegion const * EMBDetRegionArray[6][4][2]; // x 2 endcaps

  public:

  /**
   * @brief Constructor
   */
  EMBDetectorManager();
  
  /**
   * @brief Destructor
   */
  ~EMBDetectorManager();
  
  /**
   * @brief	Iterate over detector regions
   */
  EMBDetectorManager::DetectorRegionConstIterator beginDetectorRegion () const;
  
  
  /**
   * @brief	Iterate over detector regions
   */
  EMBDetectorManager::DetectorRegionConstIterator endDetectorRegion () const;
  
  /**
   * @brief	Random Access to detector regions
   */
  const EMBDetectorRegion * getDetectorRegion (unsigned int endcap, unsigned int sampling, unsigned int region) const;
  
  /**
   * @brief	Gets the number of detectors in the set of detector regions.
   */
  unsigned int getNumDetectorRegions () const;
  
  /**
   * @brief	Gets the ith tree top.
   */
  virtual PVConstLink getTreeTop (unsigned int i) const;
  
  /**
   * @brief	Gets the total number of tree tops.
   */
  virtual unsigned int getNumTreeTops () const;
  
  /**
   * @brief	Add a new Detector Region
   */
  void addDetectorRegion (const EMBDetectorRegion *region);
  
  /**
   * @brief	Add a Tree Top
   */
  virtual void addTreeTop (PVLink treeTop);
  
  /**
   * @brief	Collection of useful numerical data for the description of the barrel cells.
   */
  const EMBBasicReadoutNumbers * getBasicReadoutNumbers () const;
  
  
  /**
   * @brief       Get accordion details class
   */
  const EMBAccordionDetails *getAccordionDetails () const;

  /**
   * @brief       Get the HV Manager
   */
  const EMBHVManager *getHVManager () const;
  
  /**
   * @brief       Get the HV Manager (Presampler)
   */
  const EMBPresamplerHVManager *getPresamplerHVManager () const;
  
  
 private:
  
  EMBDetectorManager(const EMBDetectorManager &right);
  EMBDetectorManager & operator=(const EMBDetectorManager &right);
  /**
   * @brief	This is the set of tree tops managed by this detector  node.
   */
  std::vector<PVLink> m_treeTop;
  /**
   * @brief	A vector of Region pointers which exists in order to
   * provide iterative  access to any EMBDetectorRegion held
   *	by this manager.
   */ 
  std::vector<const EMBDetectorRegion *> m_DetRegionsIterative;
  /**
   * @brief	A vector of Region pointers which exists in order to
   *	provide iterative  access to any EMBDetectorRegion held
   *	by this manager.
   */
  EMBDetRegionArray m_DetRegionsRandom;
  const EMBBasicReadoutNumbers *m_basicReadoutNumbers;
  mutable EMBAccordionDetails *m_accordionDetails;
  mutable const EMBHVManager           *m_hvManager;
  mutable const EMBPresamplerHVManager *m_presamplerHVManager;
};


inline const EMBAccordionDetails * EMBDetectorManager::getAccordionDetails () const
{

  if (!m_accordionDetails) m_accordionDetails = new EMBAccordionDetails();
  return m_accordionDetails;
}


inline const EMBBasicReadoutNumbers * EMBDetectorManager::getBasicReadoutNumbers () const
{
  return m_basicReadoutNumbers;
}


CLASS_DEF(EMBDetectorManager, 123812929, 1)



#endif
