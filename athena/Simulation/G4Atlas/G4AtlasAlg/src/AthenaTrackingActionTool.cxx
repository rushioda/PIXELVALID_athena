/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "AthenaTrackingActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  AthenaTrackingActionTool::
  AthenaTrackingActionTool(const std::string& type, const std::string& name,
                           const IInterface* parent)
    : ActionToolBase<AthenaTrackingAction>(type, name, parent),
      m_secondarySavingLevel(2)
  {
    declareInterface<IG4TrackingActionTool>(this);
    declareProperty("SecondarySavingLevel", m_secondarySavingLevel,
      "Three valid options: 1 - Primaries; 2 - StoredSecondaries(default); 3 - All");
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode AthenaTrackingActionTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<AthenaTrackingAction>
  AthenaTrackingActionTool::makeAction()
  {
    ATH_MSG_DEBUG("Constructing an AthenaTrackingAction");
    // Create and configure the action plugin.
    return std::make_unique<AthenaTrackingAction>( msg().level(),
                                                   m_secondarySavingLevel );
  }

}
