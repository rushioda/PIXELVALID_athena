// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRingsCnvAlg.h  $
#ifndef XAODTRIGRINGERCNV_TRIGRINGERRINGSCNVALG_H
#define XAODTRIGRINGERCNV_TRIGRINGERRINGSCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigRingerCnv/ITrigRingerRingsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::RingerRings from an AOD
    *
    *         This algorithm can be used to create an xAOD RingerRings
    *         out of the object found in an AOD file.
    *
    * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
    *
    * $Revision:  $
    * $Date:  $
    */
   class TrigRingerRingsCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigRingerRingsCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the input container
      std::string m_aodKey;
      /// StoreGate key for the output container
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< ITrigRingerRingsCnvTool > m_cnvTool;

   }; // class RingerRingsCnvAlg

} // namespace xAODMaker

#endif // 
