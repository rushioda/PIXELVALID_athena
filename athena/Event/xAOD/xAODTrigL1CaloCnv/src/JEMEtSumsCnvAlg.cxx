/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsCnvAlg.cxx 576052 2013-12-18 09:13:50Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/JEMEtSumsCollection.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsAuxContainer.h"

// Local include(s):
#include "JEMEtSumsCnvAlg.h"

namespace xAODMaker {

   JEMEtSumsCnvAlg::JEMEtSumsCnvAlg( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::JEMEtSumsCnvTool/JEMEtSumsCnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "JEMEtSums" );
      declareProperty( "xAODKey", m_xaodKey = "JEMEtSums" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode JEMEtSumsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode JEMEtSumsCnvAlg::execute() {
      
      // Retrieve the ESD container:
      const JEMEtSumsCollection* esd = 0;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::JEMEtSumsAuxContainer* aux = new xAOD::JEMEtSumsAuxContainer();
      xAOD::JEMEtSumsContainer* xaod = new xAOD::JEMEtSumsContainer();
      xaod->setStore( aux );
      
      // Fill the xAOD container:
      CHECK( m_cnvTool->convert( esd, xaod ) );     
      
      // Record the xAOD containers:
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;     
   }

} // namespace xAODMaker
