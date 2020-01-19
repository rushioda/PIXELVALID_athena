/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRecoAlg.cxx

#include "METRecoAlg.h"
#include "METRecoInterface/IMETRecoTool.h"

using std::string;

namespace met {

  //**********************************************************************

  METRecoAlg::METRecoAlg(const std::string& name,
			 ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator ) {
    declareProperty( "RecoTools", m_recotools);
  }

  //**********************************************************************

  METRecoAlg::~METRecoAlg() { }

  //**********************************************************************

  StatusCode METRecoAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");

    // retrieve tools
    for(ToolHandleArray<IMETRecoTool>::const_iterator iTool=m_recotools.begin();
	iTool != m_recotools.end(); ++iTool) {
      ToolHandle<IMETRecoTool> tool = *iTool;
      if( tool.retrieve().isFailure() ) {
	ATH_MSG_ERROR("Failed to retrieve tool: " << tool->name());
	return StatusCode::FAILURE;
      };
      ATH_MSG_INFO("Retrieved tool: " << tool->name() );
    }
  
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METRecoAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METRecoAlg::execute() { 
    ATH_MSG_VERBOSE("Executing " << name() << "...");
    // Loop over tools.

    // Run the top-level MET tools in sequence
    for(ToolHandleArray<IMETRecoTool>::const_iterator iTool=m_recotools.begin();
	iTool != m_recotools.end(); ++iTool) {
      ToolHandle<IMETRecoTool> tool = *iTool;
      ATH_MSG_VERBOSE("Running tool: " << tool->name() );
      if( tool->execute().isFailure() ) {
	ATH_MSG_ERROR("Failed to execute tool: " << tool->name());
	return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

}

