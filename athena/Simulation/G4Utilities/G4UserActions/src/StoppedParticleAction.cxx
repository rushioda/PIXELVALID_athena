/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/StoppedParticleAction.h"

#include "TrackWriteFastSim/TrackFastSimSD.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"

#include <cmath>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"


namespace G4UA
{

  //---------------------------------------------------------------------------
  bool StoppedParticleAction::isSUSYParticle(const int id) const
  {
    if (id==1000021 || id==1000005 || id==1000006 || id==1000512 || id==1000522 || id==1000991 || id==1000993 ||
        id==1000612 || id==1000622 || id==1000632 || id==1000642 || id==1000652 || id==1005211 ||
        id==1006113 || id==1006211 || id==1006213 || id==1006223 || id==1006311 ||
        id==1006313 || id==1006321 || id==1006323 || id==1006333 ||
        id==1009111 || id==1009113 || id==1009211 || id==1009213 || id==1009311 ||
        id==1009313 || id==1009321 || id==1009323 || id==1009223 || id==1009333 ||
        id==1092112 || id==1091114 || id==1092114 || id==1092212 || id==1092214 || id==1092224 ||
        id==1093114 || id==1093122 || id==1093214 || id==1093224 || id==1093314 || id==1093324 || id==1093334)
      return true;
    return false;
  }

  //---------------------------------------------------------------------------
  StoppedParticleAction::StoppedParticleAction(const int condition)
    : AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc"),
                   "StoppedParticleAction")
    , m_fsSD(0)
    , m_init(false)
    , m_stoppingCondition(condition)
  {}

  //---------------------------------------------------------------------------
  void StoppedParticleAction::UserSteppingAction(const G4Step* aStep)
  {
    // Trigger if the energy is below our threshold or if the R-hadron is decaying
    int id = std::abs(aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding());

    // Special treatment for SUSY particles and R-hadrons
    if (id>=1000000 && id<=1100000 &&
        isSUSYParticle(id)){

      G4Material * mat = aStep->GetTrack()->GetMaterial();
      double minA=1500000.;
      for (unsigned int i=0;i<mat->GetNumberOfElements();++i){
        if (mat->GetElement(i) &&
            minA>mat->GetElement(i)->GetN()){
          minA=mat->GetElement(i)->GetN();
        }
      }

      // Stopping condition based on m_stoppingCondition
      // Normal condition, #1: Stop all particles below threshold
      bool stop = aStep->GetPostStepPoint()->GetVelocity()<0.15*std::pow(minA,-2./3.)*CLHEP::c_light;
      // Condition 2: Stop only negatively charged particles
      if (stop && abs(m_stoppingCondition)==2) stop = stop && aStep->GetTrack()->GetDynamicParticle()->GetCharge()<-0.1;
      // Negative condition: Ignore gas
      // Can't be true if stop is false
      if (stop && m_stoppingCondition<0){
        stop = stop && mat->GetDensity()>1e-2*6.24151e+18;
        // dry air is 1.225 x10-3 g/cm3; set threshold ~10x higher
        // unit conversion in G4: g/cm3 ( g/cm3) = 6.24151e+18
      }

      // Stopping condition
      if (!stop && // Stopping condition or...
           ( !aStep->GetPostStepPoint()->GetProcessDefinedStep() || // null pointer?
             aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType()!=fDecay) ) // Decaying particle (does not fire for hadronic interactions)
        return;

      if (!m_init){
        m_init = true;

        G4SDManager * g4sdm = G4SDManager::GetSDMpointer();
        if (!g4sdm) {
          ATH_MSG_ERROR( "StoppedParticleFastSim could not get sensitive detector catalog." );
        } else {
          G4VSensitiveDetector * g4sd = g4sdm->FindSensitiveDetector("ToolSvc.SensitiveDetectorMasterTool.TrackFastSimSD");
          if (!g4sd) {
            ATH_MSG_ERROR( "StoppedParticleFastSim could not get ToolSvc.SensitiveDetectorMasterTool.TrackFastSimSD sensitive detector." );
          } else {
            m_fsSD = dynamic_cast<TrackFastSimSD*>(g4sd);
            if (!m_fsSD) {
              ATH_MSG_ERROR( "StoppedParticleFastSim could not cast the SD." );
            }
          } // found the SD
        } // got the catalog
      }

      if (m_fsSD) {
        m_fsSD->WriteTrack( aStep->GetTrack() , false , true );
      }
    }

    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    const G4TrackVector *tv = aStep->GetSecondary();
    for (unsigned int i=0;i<tv->size();i++){
      (*tv)[i]->SetTrackStatus(fStopAndKill);
    }
  }

} // namespace G4UA
