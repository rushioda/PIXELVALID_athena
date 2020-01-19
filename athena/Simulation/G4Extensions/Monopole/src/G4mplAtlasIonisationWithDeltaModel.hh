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
// $Id: G4mplAtlasIonisationWithDeltaModel.hh,v 1.7 2009/02/20 16:38:33 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-ref-00 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4mplAtlasIonisationWithDeltaModel
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 06.09.2005
//
// Modifications:
// 12.08.2007 ComputeDEDXAhlen function added (M. Vladymyrov)
//
// Class Description:
//
// Implementation of model of energy loss of the magnetic monopole

// -------------------------------------------------------------------
//

#ifndef MONOPOLE_G4mplAtlasIonisationWithDeltaModel_h
#define MONOPOLE_G4mplAtlasIonisationWithDeltaModel_h 1

#include "G4VEmModel.hh"
#include "G4VEmFluctuationModel.hh"
#include "G4Version.hh"

class G4ParticleChangeForLoss;

class G4mplAtlasIonisationWithDeltaModel : public G4VEmModel, public G4VEmFluctuationModel
{

public:

  G4mplAtlasIonisationWithDeltaModel(G4double mCharge, const G4String& nam = "mplAtlasIonisationWithDelta");

  virtual ~G4mplAtlasIonisationWithDeltaModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual G4double ComputeDEDXPerVolume(const G4Material*,
                                        const G4ParticleDefinition*,
                                        G4double kineticEnergy,
                                        G4double cutEnergy);

  virtual G4double ComputeCrossSectionPerElectron(
                                                  const G4ParticleDefinition*,
                                                  G4double kineticEnergy,
                                                  G4double cutEnergy,
                                                  G4double maxEnergy);

  virtual G4double ComputeCrossSectionPerAtom(
                                              const G4ParticleDefinition*,
                                              G4double kineticEnergy,
                                              G4double Z, G4double A,
                                              G4double cutEnergy,
                                              G4double maxEnergy);

  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
                                 const G4MaterialCutsCouple*,
                                 const G4DynamicParticle*,
                                 G4double tmin,
                                 G4double maxEnergy);


#if G4VERSION_NUMBER > 1009
  virtual G4double SampleFluctuations(const G4MaterialCutsCouple* material,
                                      const G4DynamicParticle* dp,
                                      G4double tmax,
                                      G4double length,
                                      G4double meanLoss);

  virtual G4double Dispersion(const G4Material*,
                              const G4DynamicParticle*,
                              G4double tmax,
                              G4double length);
#else
  virtual G4double SampleFluctuations(const G4Material*,
                                      const G4DynamicParticle*,
                                      G4double& tmax,
                                      G4double& length,
                                      G4double& meanLoss);

  virtual G4double Dispersion(const G4Material*,
                              const G4DynamicParticle*,
                              G4double& tmax,
                              G4double& length);
#endif

protected:

  virtual G4double MaxSecondaryEnergy(const G4ParticleDefinition*,
                                      G4double kinEnergy);

private:

  void SetParticle(const G4ParticleDefinition* p);

  G4double ComputeDEDXAhlen(const G4Material* material, G4double bg2, G4double cut);

  // hide assignment operator
  G4mplAtlasIonisationWithDeltaModel & operator=(const  G4mplAtlasIonisationWithDeltaModel &right);
  G4mplAtlasIonisationWithDeltaModel(const  G4mplAtlasIonisationWithDeltaModel&);

  const G4ParticleDefinition* monopole;
  G4ParticleDefinition* theElectron;
  G4ParticleChangeForLoss*    fParticleChange;

  G4double mass;
  G4double magCharge;
  G4double twoln10;
  G4double betalow;
  G4double betalim;
  G4double beta2lim;
  G4double bg2lim;
  //G4double factlow; <-- Not used.
  G4double chargeSquare;
  G4double dedxlim;
  G4double nmpl;
  G4double pi_hbarc2_over_mc2;
  //G4double approxConst; <-- Not used.
};

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
