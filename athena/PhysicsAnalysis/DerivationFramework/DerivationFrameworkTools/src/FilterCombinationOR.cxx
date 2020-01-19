/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// FilterCombinationOR.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkTools/FilterCombinationOR.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::FilterCombinationOR::FilterCombinationOR( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("FilterList", m_filtersToCombine);
  }
 
// Destructor
DerivationFramework::FilterCombinationOR::~FilterCombinationOR() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::FilterCombinationOR::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::FilterCombinationOR::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::FilterCombinationOR::eventPassesFilter() const
{
  ++m_ntot;
  bool passesEvent=false;

  ToolHandleArray<DerivationFramework::ISkimmingTool>::const_iterator filterIter = m_filtersToCombine.begin();
  for (; filterIter != m_filtersToCombine.end(); ++filterIter) {
    bool thisFilterPasses = (*filterIter)->eventPassesFilter();
   
    msg(MSG::DEBUG)<<" Filter "<<filterIter->name()<<" passes? "<<thisFilterPasses<<endreq;
    passesEvent = passesEvent || thisFilterPasses ;
  }
  msg(MSG::DEBUG)<<" OR of all filters  passes? "<<passesEvent<<endreq;
  if (passesEvent) m_npass++;
  return passesEvent;
 
} 
