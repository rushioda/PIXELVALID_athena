/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerCnvAlg.cxx 576327 2013-12-19 16:08:56Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

// Local include(s):
#include "TriggerTowerCnvAlg.h"

namespace xAODMaker {

   TriggerTowerCnvAlg::TriggerTowerCnvAlg( const std::string& name,
                                           ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TriggerTowerCnvTool/TriggerTowerCnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "TriggerTowers" );
      declareProperty( "xAODKey", m_xaodKey = "TriggerTowers" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TriggerTowerCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerTowerCnvAlg::execute() {
      
      // Retrieve the ESD container:
      const TriggerTowerCollection* esd = 0;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::TriggerTowerAuxContainer* aux = new xAOD::TriggerTowerAuxContainer();
      xAOD::TriggerTowerContainer* xaod = new xAOD::TriggerTowerContainer();
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
