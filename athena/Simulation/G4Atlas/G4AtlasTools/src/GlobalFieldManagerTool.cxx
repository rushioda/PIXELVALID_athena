/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Main header
#include "G4AtlasTools/GlobalFieldManagerTool.h"

// Local package include
#include "TightMuonSteppingFieldManager.h"

// Geant4 includes
#include "G4TransportationManager.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4PropagatorInField.hh"
#include "G4MagIntegratorStepper.hh"


//-----------------------------------------------------------------------------
// Tool constructor
//-----------------------------------------------------------------------------
GlobalFieldManagerTool::GlobalFieldManagerTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)
  : G4FieldManagerToolBase(type, name, parent)
{
  declareProperty("UseTightMuonStepping", m_useTightMuonStepping=false,
                  "Use tight muon stepping parameters by default");
}

//-----------------------------------------------------------------------------
// Initialize a field manager
//-----------------------------------------------------------------------------
StatusCode GlobalFieldManagerTool::initializeField()
{
  ATH_MSG_DEBUG("initializeField");

  if (m_fieldOn) {

    // Retrieve the G4MagneticField
    G4MagneticField* field = m_fieldSvc->getField();

    // Retrieve the global field manager
    auto transpManager = G4TransportationManager::GetTransportationManager();
    G4FieldManager* fieldMgr(nullptr);
    if (m_useTightMuonStepping){
      // In the case of tight stepping we need to make our own global field manager
      // If field manager already exists for current thread, error.
      // There is no foreseen use-case for this situation.
      if(m_fieldMgrHolder.get()) {
        ATH_MSG_ERROR("GlobalFieldManagerTool::initializeField() - " <<
                      "Field manager already exists!");
        return StatusCode::FAILURE;
      }
      // Create a new field manager
      fieldMgr = new TightMuonSteppingFieldManager();

      // Save it in the TL holder
      m_fieldMgrHolder.set(fieldMgr);

      // Assign it to the global field manager
      transpManager->SetFieldManager(fieldMgr);
    } else {
      // Otherwise get the default from the transportation manager
      fieldMgr = transpManager->GetFieldManager();
    }

    // Configure the field manager
    fieldMgr->SetDetectorField(field);
    ATH_CHECK( setFieldParameters(fieldMgr) );

    // Construct the stepper
    auto stepper = getStepper(m_integratorStepper, field);

    // Create and configure the ChordFinder
    fieldMgr->CreateChordFinder(field);
    fieldMgr->GetChordFinder()->GetIntegrationDriver()->RenewStepperAndAdjust(stepper);

    // Configure the propagator
    G4PropagatorInField* propagator = transpManager->GetPropagatorInField();
    if (m_maxStep>0) propagator->SetLargestAcceptableStep(m_maxStep);

  }

  return StatusCode::SUCCESS;
}
