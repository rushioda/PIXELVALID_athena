/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkExUnitTestBase.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

//TrkExUnitTests
#include "TrkExUnitTests/TrkExUnitTestBase.h"

Trk::TrkExUnitTestBase::TrkExUnitTestBase(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_gaussDist(0),
    m_flatDist(0),
    m_landauDist(0),
    m_numTests(100),
    m_scanMode(false)
{
  declareProperty("NumberOfTestsPerEvent",   m_numTests);
  declareProperty("EnableScanMode",          m_scanMode);
}

Trk::TrkExUnitTestBase::~TrkExUnitTestBase()
{
   if( m_gaussDist ) {
      delete m_gaussDist;
   }
   if( m_flatDist ) {
      delete m_flatDist;
   }
   if( m_landauDist ) {
      delete m_landauDist;
   }
}

StatusCode Trk::TrkExUnitTestBase::initialize()
{
    ATH_MSG_INFO( "Creating random number services, call bookTree() and initializeTest()" );

    // intialize the random number generators
    m_gaussDist  = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
    m_flatDist   = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));
    m_landauDist = new Rndm::Numbers(randSvc(), Rndm::Landau(0.,1.)); 

    if  (bookTree().isFailure()){
        ATH_MSG_FATAL( "Could not book the TTree object" );
        return StatusCode::FAILURE;
    }
    
    if (initializeTest().isFailure()){
        ATH_MSG_FATAL( "Could not initialize the test" );
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}
 
StatusCode Trk::TrkExUnitTestBase::execute()
{
  if (m_scanMode) return runScan();
  return runTest();
}

StatusCode Trk::TrkExUnitTestBase::finalize()
{

   ATH_MSG_INFO( "finalize()" );
   return StatusCode::SUCCESS;
}

StatusCode Trk::TrkExUnitTestBase::bookTree() { return StatusCode::SUCCESS; }

StatusCode Trk::TrkExUnitTestBase::initializeTest() { return StatusCode::SUCCESS; }

