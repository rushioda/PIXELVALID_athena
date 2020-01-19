/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GoodRunsLists includes
#include "GRLSelectorAlg.h"

#include "xAODEventInfo/EventInfo.h"

GRLSelectorAlg::GRLSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator )
										    , m_grlTool("GoodRunsListSelectionTool")
{

  declareProperty( "Tool", m_grlTool , "The GoodRunsListSelectionTool" );

}


GRLSelectorAlg::~GRLSelectorAlg() {}


StatusCode GRLSelectorAlg::initialize() {
  //ATH_MSG_INFO ("Initializing " << name() << "...");
  m_total=0;
  m_passed=0;
  CHECK( m_grlTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode GRLSelectorAlg::finalize() {
  //  ATH_MSG_INFO ("Finalizing " << name() << "...");

  ATH_MSG_INFO("Events passing GRL " << m_grlTool.name() << " : " << m_passed << " / " << m_total);

  return StatusCode::SUCCESS;
}

StatusCode GRLSelectorAlg::execute() {  
  setFilterPassed(false);

  m_total++;
  const xAOD::EventInfo* evtInfo = 0;
  CHECK( evtStore()->retrieve( evtInfo ) );
  if(!m_grlTool->passRunLB(*evtInfo)) return StatusCode::SUCCESS;

  m_passed++;
  setFilterPassed(true);

  return StatusCode::SUCCESS;
}


