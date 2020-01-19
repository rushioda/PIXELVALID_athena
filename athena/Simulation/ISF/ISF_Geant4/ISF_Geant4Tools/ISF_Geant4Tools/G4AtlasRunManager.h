/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H
#define ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H

// Base class header
#include "G4RunManager.hh"

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena headers
#include "AthenaKernel/MsgStreamMember.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

/// ATLAS custom singleton run manager.
///
/// This is the run manager used for serial (not-MT) jobs.
/// @TODO sync and reduce code duplication with MT run managers.
///

namespace iGeant4
{

  class G4AtlasRunManager : public G4RunManager {

    friend class G4TransportTool; // Does this class really need friends?

  public:

    virtual ~G4AtlasRunManager() {}

    /// Retrieve the singleton instance
    static G4AtlasRunManager* GetG4AtlasRunManager();

    /// Does the work of simulating an ATLAS event
    bool ProcessEvent(G4Event* event);

    /// G4 function called at end of run
    void RunTermination() override final;

  protected:

    /// @name Overridden G4 init methods for customization
    /// @{
    void Initialize() override final;
    void InitializeGeometry() override final;
    void InitializePhysics() override final;
    /// @}

  private:

    /// Pure singleton private constructor
    G4AtlasRunManager();

    void EndEvent();

    /// @name Methods related to flux recording
    /// @{
    /// Initialize flux recording
    void InitializeFluxRecording();
    /// Record fluxes from current event
    void RecordFlux();
    /// Dump flux information to text files
    void WriteFluxInformation();
    /// @}

    /// @name Methods to pass configuration in from G4AtlasAlg
    /// @{
    /// Configure the user action service handle
    void SetUserActionSvc(const std::string& typeAndName) {
      m_userActionSvc.setTypeAndName(typeAndName);
    }

    /// Configure the detector geometry service handle
    void SetDetGeoSvc(const std::string& typeAndName) {
      m_detGeoSvc.setTypeAndName(typeAndName);
    }

    /// Configure the Sensitive Detector Master Tool handle
    void SetSDMasterTool(const std::string& typeAndName) {
      m_senDetTool.setTypeAndName(typeAndName);
    }

    /// Configure the Fast Simulation Master Tool handle
    void SetFastSimMasterTool(const std::string& typeAndName) {
      m_fastSimTool.setTypeAndName(typeAndName);
    }

    /// Configure the Physics List Tool handle
    void SetPhysListTool(const std::string& typeAndName) {
      m_physListTool.setTypeAndName(typeAndName);
    }

    void SetRecordFlux(bool b) { m_recordFlux = b; }
    void SetLogLevel(int) { /* Not implemented */ }
    /// @}

    /// Log a message using the Athena controlled logging system
    MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
    /// Check whether the logging system is active at the provided verbosity level
    bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

    /// Private message stream member
    mutable Athena::MsgStreamMember m_msg;

    bool m_recordFlux;

    ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
    ToolHandle<IFastSimulationMasterTool> m_fastSimTool;
    ToolHandle<IPhysicsListTool> m_physListTool;

    /// Handle to the user action service
    ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc;
    ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc;
  };

}


#endif // ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H
