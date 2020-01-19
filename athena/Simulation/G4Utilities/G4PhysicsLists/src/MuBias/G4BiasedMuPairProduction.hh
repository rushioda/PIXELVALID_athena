//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4MuPairProduction.hh,v 1.31 2009-02-20 14:48:16 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4MuPairProduction
//
// Author:        Laszlo Urban
//
// Creation date: 30.05.1998
//
// Modifications:

// 10-02-00 modifications , new e.m. structure, L.Urban
// 10-08-01 new methods Store/Retrieve PhysicsTable (mma)
// 29-10-01 all static functions no more inlined (mma)
// 10-05-02 V.Ivanchenko update to new design
// 26-12-02 secondary production moved to derived classes (VI)
// 27-01-03 Make models region aware (V.Ivanchenko)
// 05-02-03 Fix compilation warnings (V.Ivanchenko)
// 13-02-03 SubCutoff regime is assigned to a region (V.Ivanchenko)
// 08-08-03 STD substitute standard  (V.Ivanchenko)
// 12-11-03 G4EnergyLossSTD -> G4EnergyLossProcess (V.Ivanchenko)
// 21-01-04 Migrade to G4ParticleChangeForLoss (V.Ivanchenko)
// 28-04-04 Fix minor bug in energy balance (V.Ivanchenko)
// 17-08-04 Rename the process "Mu" -> "mu" (V.Ivanchenko)
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivantchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
//
// Class Description:
//
// This class manages the PairProduction process for muons.
// it inherites from G4VContinuousDiscreteProcess via G4VEnergyLossProcess.
//

// -------------------------------------------------------------------
//

#ifndef G4BiasedMuPairProduction_h
#define G4BiasedMuPairProduction_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "globals.hh"
#include "G4VEnergyLossProcess.hh"
#include "G4VEmModel.hh"

#include <iostream>

class G4ParticleChangeForLoss;

class G4BiasedMuPairProduction : public G4VEnergyLossProcess
{
public:

  G4BiasedMuPairProduction(const G4String& processName = "muPairProd");

  virtual ~G4BiasedMuPairProduction();

  virtual G4bool IsApplicable(const G4ParticleDefinition& p);

  virtual G4double MinPrimaryEnergy(const G4ParticleDefinition* p,
				    const G4Material*, G4double cut);

  // Print out of the class parameters
  virtual void PrintInfo();
  
  void SetBiasFactor(G4double x) 
  {
    std::cout<<" this is G4BiasedMuPairProduction::SetBiasFactor being called"<<
		" with factor "<<x<<std::endl;
  	m_biasFactor=x;
  }

protected:

  virtual void InitialiseEnergyLossProcess(const G4ParticleDefinition*,
					   const G4ParticleDefinition*);

private:

  G4BiasedMuPairProduction & operator=(const G4BiasedMuPairProduction &right);
  G4BiasedMuPairProduction(const G4BiasedMuPairProduction&);

  const G4ParticleDefinition* m_theParticle;
  const G4ParticleDefinition* m_theBaseParticle;

  G4ParticleChangeForLoss*    m_fParticleChange;

  G4double                    m_lowestKinEnergy;
  G4bool                      m_isInitialised;
  
  G4double m_biasFactor;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif
