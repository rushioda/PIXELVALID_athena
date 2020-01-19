/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_DUMMYLUMIRANGETOOL_H
# define ATHENAPOOLMULTITEST_DUMMYLUMIRANGETOOL_H

/**
 * @file DummyLumirangeTool.h
 *
 * @brief make dummy grl and put xml in metadata store
 *
 * @author Jack Cranshaw <Jack.Cranshaw@cern.ch>
 *
 * $Id: DummyLumirangeTool.h,v 1.1 2009-04-28 19:04:06 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <map>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IAddressCreator;
class StoreGateSvc;
class CollectionMetadata;

/**
 * @class DummyLumirangeTool
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 */

class DummyLumirangeTool : public AthAlgorithm
{
public:

  /// Standard DummyLumirangeTool Constructor
  DummyLumirangeTool(const std::string& name,
                            ISvcLocator* pSvcLocator);

  /// Initialize DummyLumirangeTool
  virtual StatusCode initialize();
  virtual StatusCode execute();

  /// Terminate DummyLumirangeTool
  virtual StatusCode finalize();

protected:

  /// Standard Destructor
  virtual ~DummyLumirangeTool();

  /// name of store:
  StringProperty   m_storeName;
  IntegerProperty  m_lumigran, m_firstrun;
  IntegerProperty  m_rangesize, m_blocksperrun, m_lumitot;

};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLMULTITEST_DUMMYLUMIRANGETOOL_H
