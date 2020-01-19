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
// $Id: G4MuBremsstrahlungModel.cc,v 1.36 2010-10-26 13:52:32 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4MuBremsstrahlungModel
//
// Author:        Vladimir Ivanchenko on base of Laszlo Urban code
//
// Creation date: 24.06.2002
//
// Modifications:
//
// 04-12-02 Change G4DynamicParticle constructor in PostStepDoIt (V.Ivanchenko)
// 23-12-02 Change interface in order to move to cut per region (V.Ivanchenko)
// 24-01-03 Fix for compounds (V.Ivanchenko)
// 27-01-03 Make models region aware (V.Ivanchenko)
// 13-02-03 Add name (V.Ivanchenko)
// 10-02-04 Add m_lowestKinEnergy (V.Ivanchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivanchenko)
// 03-08-05 Angular correlations according to PRM (V.Ivanchenko)
// 13-02-06 add ComputeCrossSectionPerAtom (mma)
// 21-03-06 Fix problem of initialisation in case when cuts are not defined (VI)
// 07-11-07 Improve sampling of final state (A.Bogdanov)
// 28-02-08 Use precomputed Z^1/3 and Log(A) (V.Ivanchenko)
//

//
// Class Description:
//
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4BiasedMuBremsstrahlungModel.hh"
#include "G4Gamma.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "Randomize.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ParticleChangeForLoss.hh"

#include "CLHEP/Units/PhysicalConstants.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;

G4BiasedMuBremsstrahlungModel::G4BiasedMuBremsstrahlungModel(const G4ParticleDefinition* p,
                                                 const G4String& nam)
  : G4VEmModel(nam),
    m_particle(0),
    m_sqrte(sqrt(exp(1.))),
    m_bh(202.4),
    m_bh1(446.),
    m_btf(183.),
    m_btf1(1429.),
    m_fParticleChange(0),
    m_lowestKinEnergy(1.0*CLHEP::GeV),
    m_minThreshold(1.0*CLHEP::keV),
    m_biasFactor(1.)
{
  m_theGamma = G4Gamma::Gamma();
  m_nist = G4NistManager::Instance();

  m_mass = m_rmass = m_cc = m_coeff = 1.0;

  if(p) { SetParticle(p); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4BiasedMuBremsstrahlungModel::~G4BiasedMuBremsstrahlungModel()
{
  size_t n = m_partialSumSigma.size();
  if(n > 0) {
    for(size_t i=0; i<n; i++) {
      delete m_partialSumSigma[i];
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuBremsstrahlungModel::MinEnergyCut(const G4ParticleDefinition*,
                                               const G4MaterialCutsCouple*)
{
  return m_minThreshold;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4BiasedMuBremsstrahlungModel::Initialise(const G4ParticleDefinition* p,
                                         const G4DataVector& cuts)
{
  if(p) { SetParticle(p); }

  // partial cross section is computed for fixed energy
  G4double fixedEnergy = 0.5*HighEnergyLimit();

  const G4ProductionCutsTable* theCoupleTable=
        G4ProductionCutsTable::GetProductionCutsTable();
  if(theCoupleTable) {
    G4int numOfCouples = theCoupleTable->GetTableSize();

    // clear old data    
    G4int nn = m_partialSumSigma.size();
    G4int nc = cuts.size();
    if(nn > 0) {
      for (G4int ii=0; ii<nn; ii++){
	G4DataVector* a = m_partialSumSigma[ii];
	if ( a ) { delete a; }
      } 
      m_partialSumSigma.clear();
    }
    // fill new data
    if (numOfCouples>0) {
      for (G4int i=0; i<numOfCouples; i++) {
        G4double cute = DBL_MAX;

	// protection for usage with extrapolator
        if(i < nc) { cute = cuts[i]; }

        const G4MaterialCutsCouple* couple = 
	  theCoupleTable->GetMaterialCutsCouple(i);
	const G4Material* material = couple->GetMaterial();
	G4DataVector* dv = ComputePartialSumSigma(material,fixedEnergy,cute);
	m_partialSumSigma.push_back(dv);
      }
    }
  }

  // define pointer to G4ParticleChange
  if(!m_fParticleChange) { m_fParticleChange = GetParticleChangeForLoss(); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuBremsstrahlungModel::ComputeDEDXPerVolume(
					      const G4Material* material,
                                              const G4ParticleDefinition*,
                                                    G4double kineticEnergy,
                                                    G4double cutEnergy)
{
  G4double dedx = 0.0;
  if (kineticEnergy <= m_lowestKinEnergy) return dedx;

  G4double tmax = kineticEnergy;
  G4double cut  = std::min(cutEnergy,tmax);
  if(cut < m_minThreshold) cut = m_minThreshold;

  const G4ElementVector* theElementVector = material->GetElementVector();
  const G4double* theAtomicNumDensityVector =
    material->GetAtomicNumDensityVector();

  //  loop for elements in the material
  for (size_t i=0; i<material->GetNumberOfElements(); i++) {

    G4double loss = 
      ComputMuBremLoss((*theElementVector)[i]->GetZ(), kineticEnergy, cut);

    dedx += loss*theAtomicNumDensityVector[i];
  }
  //  G4cout << "BR e= " << kineticEnergy << "  dedx= " << dedx << G4endl;
  if(dedx < 0.) dedx = 0.;
  return dedx;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuBremsstrahlungModel::ComputMuBremLoss(G4double Z,
                                                   G4double tkin, G4double cut)
{
  G4double totalEnergy = m_mass + tkin;
  G4double ak1 = 0.05;
  G4int    k2=5;
  G4double xgi[]={0.03377,0.16940,0.38069,0.61931,0.83060,0.96623};
  G4double wgi[]={0.08566,0.18038,0.23396,0.23396,0.18038,0.08566};
  G4double loss = 0.;

  G4double vcut = cut/totalEnergy;
  G4double vmax = tkin/totalEnergy;

  G4double aaa = 0.;
  G4double bbb = vcut;
  if(vcut>vmax) bbb=vmax ;
  G4int kkk = (G4int)((bbb-aaa)/ak1)+k2 ;
  G4double hhh=(bbb-aaa)/float(kkk) ;

  G4double aa = aaa;
  for(G4int l=0; l<kkk; l++)
  {
    for(G4int i=0; i<6; i++)
    {
      G4double ep = (aa + xgi[i]*hhh)*totalEnergy;
      loss += ep*wgi[i]*ComputeDMicroscopicCrossSection(tkin, Z, ep);
    }
    aa += hhh;
  }

  loss *=hhh*totalEnergy ;

  return loss;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuBremsstrahlungModel::ComputeMicroscopicCrossSection(
                                           G4double tkin,
                                           G4double Z,
                                           G4double cut)
{
  G4double totalEnergy = tkin + m_mass;
  G4double ak1 = 2.3;
  G4int    k2  = 4;
  G4double xgi[]={0.03377,0.16940,0.38069,0.61931,0.83060,0.96623};
  G4double wgi[]={0.08566,0.18038,0.23396,0.23396,0.18038,0.08566};
  G4double cross = 0.;

  if(cut >= tkin) return cross;

  G4double vcut = cut/totalEnergy;
  G4double vmax = tkin/totalEnergy;

  G4double aaa = log(vcut);
  G4double bbb = log(vmax);
  G4int    kkk = (G4int)((bbb-aaa)/ak1)+k2 ;
  G4double hhh = (bbb-aaa)/G4double(kkk);

  G4double aa = aaa;

  for(G4int l=0; l<kkk; l++)
  {
    for(G4int i=0; i<6; i++)
    {
      G4double ep = exp(aa + xgi[i]*hhh)*totalEnergy;
      cross += ep*wgi[i]*ComputeDMicroscopicCrossSection(tkin, Z, ep);
    }
    aa += hhh;
  }

  cross *=hhh;

  //G4cout << "BR e= " << tkin<< "  cross= " << cross/barn << G4endl;

  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuBremsstrahlungModel::ComputeDMicroscopicCrossSection(
                                           G4double tkin,
                                           G4double Z,
                                           G4double gammaEnergy)
//  differential cross section
{
  G4double dxsection = 0.;

  if( gammaEnergy > tkin) return dxsection ;

  G4double E = tkin + m_mass ;
  G4double v = gammaEnergy/E ;
  G4double delta = 0.5*m_mass*m_mass*v/(E-gammaEnergy) ;
  G4double rab0=delta*m_sqrte ;

  G4int iz = G4int(Z);
  if(iz < 1) iz = 1;

  G4double z13 = 1.0/m_nist->GetZ13(iz);
  G4double dn  = 1.54*m_nist->GetA27(iz);

  G4double b,b1,dnstar ;

  if(1 == iz)
  {
    b  = m_bh;
    b1 = m_bh1;
    dnstar = dn;
  }
  else
  {
    b  = m_btf;
    b1 = m_btf1;
    dnstar = dn/std::pow(dn, 1./Z);
  }

  // nucleus contribution logarithm
  G4double rab1=b*z13;
  G4double fn=log(rab1/(dnstar*(CLHEP::electron_mass_c2+rab0*rab1))*
              (m_mass+delta*(dnstar*m_sqrte-2.))) ;
  if(fn <0.) fn = 0. ;
  // electron contribution logarithm
  G4double epmax1=E/(1.+0.5*m_mass*m_rmass/E) ;
  G4double fe=0.;
  if(gammaEnergy<epmax1)
  {
    G4double rab2=b1*z13*z13 ;
    fe=log(rab2*m_mass/((1.+delta*m_rmass/(CLHEP::electron_mass_c2*m_sqrte))*
                              (CLHEP::electron_mass_c2+rab0*rab2))) ;
    if(fe<0.) fe=0. ;
  }

  dxsection = m_coeff*(1.-v*(1. - 0.75*v))*Z*(fn*Z + fe)/gammaEnergy;

  return dxsection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuBremsstrahlungModel::ComputeCrossSectionPerAtom(
                                           const G4ParticleDefinition*,
                                                 G4double kineticEnergy,
						 G4double Z, G4double,
                                                 G4double cutEnergy,
                                                 G4double maxEnergy)
{
  G4double cross = 0.0;
  if (kineticEnergy <= m_lowestKinEnergy) return cross;
  G4double tmax = std::min(maxEnergy, kineticEnergy);
  G4double cut  = std::min(cutEnergy, kineticEnergy);
  if(cut < m_minThreshold) cut = m_minThreshold;
  if (cut >= tmax) return cross;

  cross = ComputeMicroscopicCrossSection (kineticEnergy, Z, cut);
  if(tmax < kineticEnergy) {
    cross -= ComputeMicroscopicCrossSection(kineticEnergy, Z, tmax);
  }
  return m_biasFactor*cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4DataVector* G4BiasedMuBremsstrahlungModel::ComputePartialSumSigma(
                                       const G4Material* material,
				       G4double kineticEnergy,
				       G4double cut)

// Build the table of cross section per element. 
// The table is built for material 
// This table is used to select randomly an element in the material.
{
  G4int nElements = material->GetNumberOfElements();
  const G4ElementVector* theElementVector = material->GetElementVector();
  const G4double* theAtomNumDensityVector = 
    material->GetAtomicNumDensityVector();

  G4DataVector* dv = new G4DataVector();

  G4double cross = 0.0;

  for (G4int i=0; i<nElements; i++ ) {
    cross += theAtomNumDensityVector[i] 
      * ComputeMicroscopicCrossSection(kineticEnergy, 
				       (*theElementVector)[i]->GetZ(), cut);
    dv->push_back(cross);
  }
  return dv;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4BiasedMuBremsstrahlungModel::SampleSecondaries(
                              std::vector<G4DynamicParticle*>* vdp,
			      const G4MaterialCutsCouple* couple,
			      const G4DynamicParticle* dp,
			      G4double minEnergy,
			      G4double maxEnergy)
{
  G4double kineticEnergy = dp->GetKineticEnergy();
  // check against insufficient energy
  G4double tmax = std::min(kineticEnergy, maxEnergy);
  G4double tmin = std::min(kineticEnergy, minEnergy);
  if(tmin < m_minThreshold) tmin = m_minThreshold;
  if(tmin >= tmax) return;

  // ===== sampling of energy transfer ======

  G4ParticleMomentum partDirection = dp->GetMomentumDirection();

  // select randomly one element constituing the material
  const G4Element* anElement = SelectRandomAtom(couple);
  G4double Z = anElement->GetZ();

  G4double totalEnergy   = kineticEnergy + m_mass;
  G4double totalMomentum = sqrt(kineticEnergy*(kineticEnergy + 2.0*m_mass));

  G4double func1 = tmin*
    ComputeDMicroscopicCrossSection(kineticEnergy,Z,tmin);

  G4double lnepksi, epksi;
  G4double func2;

  do {
    lnepksi = log(tmin) + G4UniformRand()*log(kineticEnergy/tmin);
    epksi   = exp(lnepksi);
    func2   = epksi*ComputeDMicroscopicCrossSection(kineticEnergy,Z,epksi);

  } while(func2 < func1*G4UniformRand());

  G4double gEnergy = epksi;

  // ===== sample angle =====

  G4double gam  = totalEnergy/m_mass;
  G4double rmax = gam*std::min(1.0, totalEnergy/gEnergy - 1.0);
  G4double rmax2= rmax*rmax;
  G4double x = G4UniformRand()*rmax2/(1.0 + rmax2);

  G4double theta = sqrt(x/(1.0 - x))/gam;
  G4double sint  = sin(theta);
  G4double phi   = CLHEP::twopi * G4UniformRand() ;
  G4double dirx  = sint*cos(phi), diry = sint*sin(phi), dirz = cos(theta) ;

  G4ThreeVector gDirection(dirx, diry, dirz);
  gDirection.rotateUz(partDirection);

  partDirection *= totalMomentum;
  partDirection -= gEnergy*gDirection;
  partDirection = partDirection.unit();

  // primary change
  kineticEnergy -= gEnergy;
  m_fParticleChange->SetProposedKineticEnergy(kineticEnergy);
  m_fParticleChange->SetProposedMomentumDirection(partDirection);

  // save secondary
  G4DynamicParticle* aGamma = 
    new G4DynamicParticle(m_theGamma,gDirection,gEnergy);
  vdp->push_back(aGamma);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const G4Element* G4BiasedMuBremsstrahlungModel::SelectRandomAtom(
           const G4MaterialCutsCouple* couple) const
{
  // select randomly 1 element within the material

  const G4Material* material = couple->GetMaterial();
  G4int nElements = material->GetNumberOfElements();
  const G4ElementVector* theElementVector = material->GetElementVector();
  if(1 == nElements) { return (*theElementVector)[0]; }
  else if(1 > nElements) { return 0; }

  G4DataVector* dv = m_partialSumSigma[couple->GetIndex()];
  G4double rval = G4UniformRand()*((*dv)[nElements-1]);
  for (G4int i=0; i<nElements; i++) {
    if (rval <= (*dv)[i]) { return (*theElementVector)[i]; }
  }
  return (*theElementVector)[nElements-1];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
