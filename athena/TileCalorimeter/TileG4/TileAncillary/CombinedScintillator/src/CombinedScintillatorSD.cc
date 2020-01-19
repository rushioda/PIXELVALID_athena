/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillatorSD.
// Sensitive detector for the Scintillator between LAr/Tile
//
//************************************************************

#include "CombinedScintillatorSD.hh"

#include "CaloIdentifier/TileTBID.h"
#include "CxxUtils/make_unique.h"// For make unique
#include "StoreGate/StoreGateSvc.h"
#include "TileSimEvent/TileHitVector.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "G4HCofThisEvent.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"

CombinedScintillatorSD::CombinedScintillatorSD(const std::string& name, const std::string& hitCollectionName)
    : G4VSensitiveDetector(name),
    m_hit(),
    m_HitColl(hitCollectionName)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StoreGateSvc* detStore(nullptr);
  if (svcLocator->service("DetectorStore", detStore).isFailure()) {
    G4ExceptionDescription description;
    description << "Constructor: DetectorStoreSvc not found!";
    G4Exception("CombinedScintillatorSD", "NoDetStore", FatalException, description);
    abort();
  } else if (verboseLevel > 5) {
    G4cout << "DetectorStoreSvc initialized" << G4endl;
  }

  if (detStore->retrieve(m_tileTBID).isFailure()) {
    G4ExceptionDescription description;
    description << "Constructor: No TileTBID helper!";
    G4Exception("CombinedScintillatorSD", "NoTileTBIDHelper", FatalException, description);
    abort();
  } else if (verboseLevel > 5) {
    G4cout << "TileTBID helper retrieved" << G4endl;
  }
  int type = TileTBID::ADC_TYPE, module = TileTBID::CRACK_WALL;
  for (int channel = 0; channel < N_CELLS; ++channel) {
    m_id[channel] = m_tileTBID->channel_id(type, module, channel);
  }
}

CombinedScintillatorSD::~CombinedScintillatorSD() {
}

void CombinedScintillatorSD::StartOfAthenaEvent() {
  if (verboseLevel > 5) {
    G4cout << "Initializing SD" << G4endl;
  }

  memset(m_nhits, 0, sizeof(m_nhits));
}

void CombinedScintillatorSD::Initialize(G4HCofThisEvent* /* HCE */) {
  if (verboseLevel > 5) {
    G4cout << "CombinedScintillatorSD::Initialize()" << G4endl;
  }

  if (!m_HitColl.isValid()) {
    m_HitColl = CxxUtils::make_unique<TileHitVector>(m_HitColl.name());
  }
}

G4bool CombinedScintillatorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /* ROhist */) {
  if (verboseLevel > 10) {
    G4cout << "CombinedScintillatorSD::ProcessHits" << G4endl;
  }

  const G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  const G4VPhysicalVolume* physVol = theTouchable->GetVolume();
  const G4LogicalVolume* logiVol = physVol->GetLogicalVolume();
  const G4String nameLogiVol = logiVol->GetName();
  const G4int nScinti = physVol->GetCopyNo();

  const G4double edep = aStep->GetTotalEnergyDeposit();
  G4double stepl = 0.;
  G4ThreeVector pStep = aStep->GetDeltaPosition();

  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {// FIXME not-equal check on double

    stepl = aStep->GetStepLength();
  }

  if ((edep == 0.) && (stepl == 0.)) {//FIXME equality check on double

    return false;
  }

  if (nameLogiVol.find("CScintillatorLayer") != G4String::npos) {
    int ind = nScinti; // Only one scintillator at the test beam
    // No copy nScinti == 0 !!

    if ( m_nhits[ind] > 0 ) {
      if (verboseLevel > 10) {
        G4cout << "Additional hit in CombinedScintillator " << nScinti
               << " ene=" << edep << G4endl;
      }
      m_hit[ind]->add(edep, 0.0, 0.0);
    } else {
      // First hit in a cell
      if (verboseLevel > 10) {
        G4cout << "First hit in CombinedScintillator " << nScinti
               << " ene=" << edep << G4endl;
      }
      m_hit[ind] = new TileSimHit(m_id[ind], edep, 0.0, 0.0);
    }
    ++m_nhits[ind];
  }
  return true;
}

void CombinedScintillatorSD::EndOfAthenaEvent() {
  for (int ind = 0; ind < N_CELLS; ++ind) {
    int nhit = m_nhits[ind];
    if (nhit > 0) {
      if (verboseLevel > 5) {
        G4cout << "Cell id=" << m_tileTBID->to_string(m_id[ind])
               << " nhit=" << nhit
               << " ene=" << m_hit[ind]->energy() << G4endl;
      }
      m_HitColl->Insert(TileHit(m_hit[ind]));
      delete m_hit[ind];
    } else if (verboseLevel > 10) {
      G4cout << "Cell id=" << m_tileTBID->to_string(m_id[ind]) << " nhit=0" << G4endl;
    }
  }

  if (verboseLevel > 5) {
    G4cout << "Total number of hits is " << m_HitColl->size() << G4endl;
  }
  return ;
}
