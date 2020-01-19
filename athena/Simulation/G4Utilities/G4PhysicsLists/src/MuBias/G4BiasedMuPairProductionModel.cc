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
// $Id: G4MuPairProductionModel.cc,v 1.46 2010-10-26 13:52:32 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4MuPairProductionModel
//
// Author:        Vladimir Ivanchenko on base of Laszlo Urban code
//
// Creation date: 24.06.2002
//
// Modifications:
//
// 04-12-02 Change G4DynamicParticle constructor in PostStep (V.Ivanchenko)
// 23-12-02 Change interface in order to move to cut per region (V.Ivanchenko)
// 24-01-03 Fix for compounds (V.Ivanchenko)
// 27-01-03 Make models region aware (V.Ivanchenko)
// 13-02-03 Add model (V.Ivanchenko)
// 06-06-03 Fix in cross section calculation for high energy (V.Ivanchenko)
// 20-10-03 2*xi in ComputeDDMicroscopicCrossSection   (R.Kokoulin)
//          8 integration points in ComputeDMicroscopicCrossSection
// 12-01-04 Take min cut of e- and e+ not its sum (V.Ivanchenko)
// 10-02-04 Update parameterisation using R.Kokoulin model (V.Ivanchenko)
// 28-04-04 For complex materials repeat calculation of max energy for each
//          material (V.Ivanchenko)
// 01-11-04 Fix bug inside ComputeDMicroscopicCrossSection (R.Kokoulin)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
// 03-08-05 Add SetParticle method (V.Ivantchenko)
// 23-10-05 Add protection in sampling of e+e- pair energy needed for 
//          low cuts (V.Ivantchenko)
// 13-02-06 Add ComputeCrossSectionPerAtom (mma)
// 24-04-07 Add protection in SelectRandomAtom method (V.Ivantchenko)
// 12-05-06 Updated sampling (use cut) in SelectRandomAtom (A.Bogdanov) 
// 11-10-07 Add ignoreCut flag (V.Ivanchenko) 

//
// Class Description:
//
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4BiasedMuPairProductionModel.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "Randomize.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ParticleChangeForLoss.hh"
#include "G4ParticleChangeForGamma.hh"

#include "CLHEP/Units/PhysicalConstants.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// static members
//
G4double G4BiasedMuPairProductionModel::s_zdat[]={1., 4., 13., 29., 92.};
G4double G4BiasedMuPairProductionModel::s_adat[]={1.01, 9.01, 26.98, 63.55, 238.03};
G4double G4BiasedMuPairProductionModel::s_tdat[]={1.e3, 1.e4, 1.e5, 1.e6, 1.e7, 1.e8,
                                          1.e9, 1.e10};
G4double G4BiasedMuPairProductionModel::s_xgi[]={ 0.0199, 0.1017, 0.2372, 0.4083,
                                          0.5917, 0.7628, 0.8983, 0.9801 };
G4double G4BiasedMuPairProductionModel::s_wgi[]={ 0.0506, 0.1112, 0.1569, 0.1813,
                                          0.1813, 0.1569, 0.1112, 0.0506 };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;

G4BiasedMuPairProductionModel::G4BiasedMuPairProductionModel(const G4ParticleDefinition* p,
                                                 const G4String& nam)
  : G4VEmModel(nam),
    m_biasFactor(1.),
    m_particle(0),
    m_factorForCross(4.*CLHEP::fine_structure_const*CLHEP::fine_structure_const
                   *CLHEP::classic_electr_radius*CLHEP::classic_electr_radius/(3.*CLHEP::pi)),
    m_sqrte(sqrt(exp(1.))),
    m_currentZ(0),
    m_fParticleChange(0),
    m_minPairEnergy(4.*CLHEP::electron_mass_c2),
    m_lowestKinEnergy(CLHEP::GeV),
    m_nzdat(5),
    m_ntdat(8),
    m_nbiny(1000),
    m_nmaxElements(0),
    m_ymin(-5.),
    m_ymax(0.),
    m_dy((m_ymax-m_ymin)/m_nbiny),
    m_samplingTablesAreFilled(false)
{
  SetLowEnergyLimit(m_minPairEnergy);
  m_nist = G4NistManager::Instance();

  m_theElectron = G4Electron::Electron();
  m_thePositron = G4Positron::Positron();

  m_particleMass = m_lnZ = m_z13 = m_z23 = 0;

  for(size_t i=0; i<1001; ++i) { m_ya[i] = 0.0; }

  if(p) { SetParticle(p); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4BiasedMuPairProductionModel::~G4BiasedMuPairProductionModel()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuPairProductionModel::MinEnergyCut(const G4ParticleDefinition*,
                                               const G4MaterialCutsCouple* )
{
  return m_minPairEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuPairProductionModel::MaxSecondaryEnergy(const G4ParticleDefinition*,
						     G4double kineticEnergy)
{
  G4double maxPairEnergy = kineticEnergy + m_particleMass*(1.0 - 0.75*m_sqrte*m_z13);
  return maxPairEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4BiasedMuPairProductionModel::Initialise(const G4ParticleDefinition* p,
                                         const G4DataVector&)
{ 
  if (!m_samplingTablesAreFilled) {
    if(p) { SetParticle(p); }
    MakeSamplingTables();
  }
  if(!m_fParticleChange) { m_fParticleChange = GetParticleChangeForLoss(); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuPairProductionModel::ComputeDEDXPerVolume(
					      const G4Material* material,
                                              const G4ParticleDefinition*,
                                                    G4double kineticEnergy,
                                                    G4double cutEnergy)
{
  G4double dedx = 0.0;
  if (cutEnergy <= m_minPairEnergy || kineticEnergy <= m_lowestKinEnergy)
    { return dedx; }

  const G4ElementVector* theElementVector = material->GetElementVector();
  const G4double* theAtomicNumDensityVector =
                                   material->GetAtomicNumDensityVector();

  //  loop for elements in the material
  for (size_t i=0; i<material->GetNumberOfElements(); ++i) {
     G4double Z = (*theElementVector)[i]->GetZ();
     SetCurrentElement(Z);
     G4double tmax = MaxSecondaryEnergy(m_particle, kineticEnergy);
     G4double loss = ComputMuPairLoss(Z, kineticEnergy, cutEnergy, tmax);
     dedx += loss*theAtomicNumDensityVector[i];
  }
  if (dedx < 0.) { dedx = 0.; }
  return dedx;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuPairProductionModel::ComputMuPairLoss(G4double Z, 
						   G4double tkin,
						   G4double cutEnergy, 
						   G4double tmax)
{
  SetCurrentElement(Z);
  G4double loss = 0.0;

  G4double cut = std::min(cutEnergy,tmax);
  if(cut <= m_minPairEnergy) { return loss; }

  // calculate the rectricted loss
  // numerical integration in log(PairEnergy)
  G4double ak1=6.9;
  G4double ak2=1.0;
  G4double aaa = log(m_minPairEnergy);
  G4double bbb = log(cut);
  G4int    kkk = (G4int)((bbb-aaa)/ak1+ak2);
  if (kkk > 8) kkk = 8;
  G4double hhh = (bbb-aaa)/(G4double)kkk;
  G4double x = aaa;

  for (G4int l=0 ; l<kkk; l++)
  {

    for (G4int ll=0; ll<8; ll++)
    {
      G4double ep = exp(x+s_xgi[ll]*hhh);
      loss += s_wgi[ll]*ep*ep*ComputeDMicroscopicCrossSection(tkin, Z, ep);
    }
    x += hhh;
  }
  loss *= hhh;
  if (loss < 0.) loss = 0.;
  return loss;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuPairProductionModel::ComputeMicroscopicCrossSection(
                                           G4double tkin,
                                           G4double Z,
                                           G4double cut)
{
  G4double cross = 0.;
  SetCurrentElement(Z);
  G4double tmax = MaxSecondaryEnergy(m_particle, tkin);
  if (tmax <= cut) { return cross; }

  G4double ak1=6.9 ;
  G4double ak2=1.0 ;
  G4double aaa = log(cut);
  G4double bbb = log(tmax);
  G4int kkk = (G4int)((bbb-aaa)/ak1 + ak2);
  if(kkk > 8) { kkk = 8; }
  G4double hhh = (bbb-aaa)/G4double(kkk);
  G4double x = aaa;

  for(G4int l=0; l<kkk; ++l)
  {
    for(G4int i=0; i<8; ++i)
    {
      G4double ep = exp(x + s_xgi[i]*hhh);
      cross += ep*s_wgi[i]*ComputeDMicroscopicCrossSection(tkin, Z, ep);
    }
    x += hhh;
  }

  cross *= hhh;
  if(cross < 0.0) { cross = 0.0; }
  return cross;
}

G4double G4BiasedMuPairProductionModel::ComputeDMicroscopicCrossSection(
                                           G4double tkin,
                                           G4double Z,
                                           G4double pairEnergy)
 // Calculates the  differential (D) microscopic cross section
 // using the cross section formula of R.P. Kokoulin (18/01/98)
 // Code modified by R.P. Kokoulin, V.N. Ivanchenko (27/01/04)
{
  G4double bbbtf= 183. ;
  G4double bbbh = 202.4 ;
  G4double g1tf = 1.95e-5 ;
  G4double g2tf = 5.3e-5 ;
  G4double g1h  = 4.4e-5 ;
  G4double g2h  = 4.8e-5 ;

  G4double totalEnergy  = tkin + m_particleMass;
  G4double residEnergy  = totalEnergy - pairEnergy;
  G4double massratio    = m_particleMass/CLHEP::electron_mass_c2 ;
  G4double massratio2   = massratio*massratio ;
  G4double cross = 0.;

  SetCurrentElement(Z);

  G4double c3 = 0.75*m_sqrte*m_particleMass;
  if (residEnergy <= c3*m_z13) { return cross; }

  G4double c7 = 4.*CLHEP::electron_mass_c2;
  G4double c8 = 6.*m_particleMass*m_particleMass;
  G4double alf = c7/pairEnergy;
  G4double a3 = 1. - alf;
  if (a3 <= 0.) { return cross; }

  // zeta calculation
  G4double bbb,g1,g2;
  if( Z < 1.5 ) { bbb = bbbh ; g1 = g1h ; g2 = g2h ; }
  else          { bbb = bbbtf; g1 = g1tf; g2 = g2tf; }

  G4double zeta = 0;
  G4double zeta1 = 0.073*log(totalEnergy/(m_particleMass+g1*m_z23*totalEnergy))-0.26;
  if ( zeta1 > 0.)
  {
    G4double zeta2 = 0.058*log(totalEnergy/(m_particleMass+g2*m_z13*totalEnergy))-0.14;
    zeta  = zeta1/zeta2 ;
  }

  G4double z2 = Z*(Z+zeta);
  G4double screen0 = 2.*CLHEP::electron_mass_c2*m_sqrte*bbb/(m_z13*pairEnergy);
  G4double a0 = totalEnergy*residEnergy;
  G4double a1 = pairEnergy*pairEnergy/a0;
  G4double bet = 0.5*a1;
  G4double xi0 = 0.25*massratio2*a1;
  G4double del = c8/a0;

  G4double rta3 = sqrt(a3);
  G4double tmnexp = alf/(1. + rta3) + del*rta3;
  if(tmnexp >= 1.0) return cross;

  G4double tmn = log(tmnexp);
  G4double sum = 0.;

  // Gaussian integration in ln(1-ro) ( with 8 points)
  for (G4int i=0; i<8; ++i)
  {
    G4double a4 = exp(tmn*s_xgi[i]);     // a4 = (1.-asymmetry)
    G4double a5 = a4*(2.-a4) ;
    G4double a6 = 1.-a5 ;
    G4double a7 = 1.+a6 ;
    G4double a9 = 3.+a6 ;
    G4double xi = xi0*a5 ;
    G4double xii = 1./xi ;
    G4double xi1 = 1.+xi ;
    G4double screen = screen0*xi1/a5 ;
    G4double yeu = 5.-a6+4.*bet*a7 ;
    G4double yed = 2.*(1.+3.*bet)*log(3.+xii)-a6-a1*(2.-a6) ;
    G4double ye1 = 1.+yeu/yed ;
    G4double ale=log(bbb/m_z13*sqrt(xi1*ye1)/(1.+screen*ye1)) ;
    G4double cre = 0.5*log(1.+2.25*m_z23*xi1*ye1/massratio2) ;
    G4double be;

    if (xi <= 1.e3) be = ((2.+a6)*(1.+bet)+xi*a9)*log(1.+xii)+(a5-bet)/xi1-a9;
    else            be = (3.-a6+a1*a7)/(2.*xi);

    G4double fe = (ale-cre)*be;
    if ( fe < 0.) fe = 0. ;

    G4double ymu = 4.+a6 +3.*bet*a7 ;
    G4double ymd = a7*(1.5+a1)*log(3.+xi)+1.-1.5*a6 ;
    G4double ym1 = 1.+ymu/ymd ;
    G4double alm_crm = log(bbb*massratio/(1.5*m_z23*(1.+screen*ym1)));
    G4double a10,bm;
    if ( xi >= 1.e-3)
    {
      a10 = (1.+a1)*a5 ;
      bm  = (a7*(1.+1.5*bet)-a10*xii)*log(xi1)+xi*(a5-bet)/xi1+a10;
    } else {
      bm = (5.-a6+bet*a9)*(xi/2.);
    }

    G4double fm = alm_crm*bm;
    if ( fm < 0.) fm = 0. ;

    sum += s_wgi[i]*a4*(fe+fm/massratio2);
  }

  cross = -tmn*sum*m_factorForCross*z2*residEnergy/(totalEnergy*pairEnergy);

  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4BiasedMuPairProductionModel::ComputeCrossSectionPerAtom(
                                           const G4ParticleDefinition*,
                                                 G4double kineticEnergy,
						 G4double Z, G4double,
                                                 G4double cutEnergy,
                                                 G4double maxEnergy)
{
  G4double cross = 0.0;
  if (kineticEnergy <= m_lowestKinEnergy) { return cross; }

  SetCurrentElement(Z);

  G4double maxPairEnergy = MaxSecondaryEnergy(m_particle,kineticEnergy);
  G4double tmax = std::min(maxEnergy, maxPairEnergy);
  G4double cut  = std::max(cutEnergy, m_minPairEnergy);
  if (cut >= tmax) return cross;

  cross = ComputeMicroscopicCrossSection (kineticEnergy, Z, cut);
  if(tmax < kineticEnergy) {
    cross -= ComputeMicroscopicCrossSection(kineticEnergy, Z, tmax);
  }
  return m_biasFactor*cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4BiasedMuPairProductionModel::MakeSamplingTables()
{
  for (G4int iz=0; iz<m_nzdat; ++iz)
  {
    G4double Z = s_zdat[iz];
    SetCurrentElement(Z);

    for (G4int it=0; it<m_ntdat; ++it) {

      G4double kineticEnergy = s_tdat[it];
      G4double maxPairEnergy = MaxSecondaryEnergy(m_particle,kineticEnergy);
      // G4cout << "Z= " << m_currentZ << " z13= " << m_z13 
      //<< " mE= " << maxPairEnergy << G4endl;
      G4double CrossSection = 0.0 ;

      if(maxPairEnergy > m_minPairEnergy) {

	G4double y = m_ymin - 0.5*m_dy ;
	G4double yy = m_ymin - m_dy ;
	G4double x = exp(y);
	G4double fac = exp(m_dy);
	G4double dx = exp(yy)*(fac - 1.0);

	G4double c = log(maxPairEnergy/m_minPairEnergy);

	for (G4int i=0 ; i<m_nbiny; ++i) {
	  y += m_dy ;
	  if(c > 0.0) {
	    x *= fac;
	    dx*= fac;
	    G4double ep = m_minPairEnergy*exp(c*x) ;
	    CrossSection += 
	      ep*dx*ComputeDMicroscopicCrossSection(kineticEnergy, Z, ep);
	  }
	  m_ya[i] = y;
	  m_proba[iz][it][i] = CrossSection;
	}
       
      } else {
	for (G4int i=0 ; i<m_nbiny; ++i) {
	  m_proba[iz][it][i] = CrossSection;
	}
      }

      m_ya[m_nbiny]=m_ymax;
      m_proba[iz][it][m_nbiny] = CrossSection;

    }
  }
  m_samplingTablesAreFilled = true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void 
G4BiasedMuPairProductionModel::SampleSecondaries(std::vector<G4DynamicParticle*>* vdp, 
					   const G4MaterialCutsCouple* couple,
					   const G4DynamicParticle* aDynamicParticle,
					   G4double tmin,
					   G4double tmax)
{
  G4double kineticEnergy = aDynamicParticle->GetKineticEnergy();
  G4double totalEnergy   = kineticEnergy + m_particleMass;
  G4double totalMomentum = 
    sqrt(kineticEnergy*(kineticEnergy + 2.0*m_particleMass));

  G4ThreeVector partDirection = aDynamicParticle->GetMomentumDirection();

  G4int it;
  for(it=1; it<m_ntdat; ++it) { if(kineticEnergy <= s_tdat[it]) { break; } }
  if(it == m_ntdat) { --it; }
  G4double dt = log(kineticEnergy/s_tdat[it-1])/log(s_tdat[it]/s_tdat[it-1]);

  // select randomly one element constituing the material
  const G4Element* anElement = 
    SelectRandomAtom(kineticEnergy, dt, it, couple, tmin);
  SetCurrentElement(anElement->GetZ());

  // define interval of enegry transfer
  G4double maxPairEnergy = MaxSecondaryEnergy(m_particle,kineticEnergy);
  G4double maxEnergy     = std::min(tmax, maxPairEnergy);
  G4double minEnergy     = std::max(tmin, m_minPairEnergy);

  if(minEnergy >= maxEnergy) { return; }
  //G4cout << "emin= " << minEnergy << " emax= " << maxEnergy 
  //	 << " minPair= " << m_minPairEnergy << " maxpair= " << maxPairEnergy 
  //       << " ymin= " << m_ymin << " dy= " << m_dy << G4endl;

  // select bins
  G4int iymin = 0;
  G4int iymax = m_nbiny-1;
  if( minEnergy > m_minPairEnergy)
  {
    G4double xc = log(minEnergy/m_minPairEnergy)/log(maxPairEnergy/m_minPairEnergy);
    iymin = (G4int)((log(xc) - m_ymin)/m_dy);
    if(iymin >= m_nbiny) iymin = m_nbiny-1;
    else if(iymin < 0) iymin = 0;
    xc = log(maxEnergy/m_minPairEnergy)/log(maxPairEnergy/m_minPairEnergy);
    iymax = (G4int)((log(xc) - m_ymin)/m_dy) + 1;
    if(iymax >= m_nbiny) iymax = m_nbiny-1;
    else if(iymax < 0) iymax = 0;
  }

  // sample e-e+ energy, pair energy first
  G4int iz, iy;

  for(iz=1; iz<m_nzdat; ++iz) { if(m_currentZ <= s_zdat[iz]) { break; } }
  if(iz == m_nzdat) { --iz; }

  G4double dz = log(m_currentZ/s_zdat[iz-1])/log(s_zdat[iz]/s_zdat[iz-1]);

  G4double pmin = InterpolatedIntegralCrossSection(dt,dz,iz,it,iymin,m_currentZ);
  G4double pmax = InterpolatedIntegralCrossSection(dt,dz,iz,it,iymax,m_currentZ);

  G4double p = pmin+G4UniformRand()*(pmax - pmin);

  // interpolate sampling vector;
  G4double p1 = pmin;
  G4double p2 = pmin;
  for(iy=iymin+1; iy<=iymax; ++iy) {
    p1 = p2;
    p2 = InterpolatedIntegralCrossSection(dt, dz, iz, it, iy, m_currentZ);
    if(p <= p2) { break; }
  }
  // G4cout << "iy= " << iy << " iymin= " << iymin << " iymax= " 
  //        << iymax << " Z= " << m_currentZ << G4endl;
  G4double y = m_ya[iy-1] + m_dy*(p - p1)/(p2 - p1);

  G4double PairEnergy = m_minPairEnergy*exp(exp(y)
                       *log(maxPairEnergy/m_minPairEnergy));
		       
  if(PairEnergy < minEnergy) { PairEnergy = minEnergy; }
  if(PairEnergy > maxEnergy) { PairEnergy = maxEnergy; }

  // sample r=(E+-E-)/PairEnergy  ( uniformly .....)
  G4double rmax =
    (1.-6.*m_particleMass*m_particleMass/(totalEnergy*(totalEnergy-PairEnergy)))
                                       *sqrt(1.-m_minPairEnergy/PairEnergy);
  G4double r = rmax * (-1.+2.*G4UniformRand()) ;

  // compute energies from PairEnergy,r
  G4double ElectronEnergy = (1.-r)*PairEnergy*0.5;
  G4double PositronEnergy = PairEnergy - ElectronEnergy;

  // The angle of the emitted virtual photon is sampled
  // according to the muon bremsstrahlung model
 
  G4double gam  = totalEnergy/m_particleMass;
  G4double gmax = gam*std::min(1.0, totalEnergy/PairEnergy - 1.0);
  G4double gmax2= gmax*gmax;
  G4double x = G4UniformRand()*gmax2/(1.0 + gmax2);

  G4double theta = sqrt(x/(1.0 - x))/gam;
  G4double sint  = sin(theta);
  G4double phi   = CLHEP::twopi * G4UniformRand() ;
  G4double dirx  = sint*cos(phi), diry = sint*sin(phi), dirz = cos(theta) ;

  G4ThreeVector gDirection(dirx, diry, dirz);
  gDirection.rotateUz(partDirection);

  // the angles of e- and e+ assumed to be the same as virtual gamma

  // create G4DynamicParticle object for the particle1
  G4DynamicParticle* aParticle1 = 
    new G4DynamicParticle(m_theElectron, gDirection, 
			  ElectronEnergy - CLHEP::electron_mass_c2);

  // create G4DynamicParticle object for the particle2
  G4DynamicParticle* aParticle2 = 
    new G4DynamicParticle(m_thePositron, gDirection,
			  PositronEnergy - CLHEP::electron_mass_c2);

  // primary change
  kineticEnergy -= (ElectronEnergy + PositronEnergy);
  m_fParticleChange->SetProposedKineticEnergy(kineticEnergy);

  partDirection *= totalMomentum;
  partDirection -= (aParticle1->GetMomentum() + aParticle2->GetMomentum());
  partDirection = partDirection.unit();
  m_fParticleChange->SetProposedMomentumDirection(partDirection);

  // add secondary
  vdp->push_back(aParticle1);
  vdp->push_back(aParticle2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const G4Element* G4BiasedMuPairProductionModel::SelectRandomAtom(
                 G4double kinEnergy, G4double dt, G4int it,
           const G4MaterialCutsCouple* couple, G4double tmin)
{
  // select randomly 1 element within the material

  const G4Material* material = couple->GetMaterial();
  size_t nElements = material->GetNumberOfElements();
  const G4ElementVector* theElementVector = material->GetElementVector();
  if (nElements == 1) { return (*theElementVector)[0]; }

  if(nElements > m_nmaxElements) {
    m_nmaxElements = nElements;
    m_partialSum.resize(m_nmaxElements);
  }

  const G4double* theAtomNumDensityVector=material->GetAtomicNumDensityVector();

  G4double sum = 0.0;
  G4double dl;

  size_t i;
  for (i=0; i<nElements; ++i) {
    G4double Z = ((*theElementVector)[i])->GetZ();
    SetCurrentElement(Z);
    G4double maxPairEnergy = MaxSecondaryEnergy(m_particle,kinEnergy);
    G4double minEnergy     = std::max(tmin, m_minPairEnergy);
    dl = 0.0;
    if(minEnergy < maxPairEnergy) {

      G4int iz;
      if (m_nzdat>=5) std::abort();
      for(iz=1; iz<m_nzdat; ++iz) {if(Z <= s_zdat[iz]) { break; } }
      if(iz == m_nzdat) { --iz; }
      G4double dz = log(Z/s_zdat[iz-1])/log(s_zdat[iz]/s_zdat[iz-1]);

      G4double sigcut;
      if(minEnergy <= m_minPairEnergy)
	sigcut = 0.;
      else
	{
	  G4double xc = log(minEnergy/m_minPairEnergy)/log(maxPairEnergy/m_minPairEnergy);
	  G4int iy = (G4int)((log(xc) - m_ymin)/m_dy);
	  if(iy < 0) { iy = 0; }
	  if(iy >= m_nbiny) { iy = m_nbiny-1; }
	  sigcut = InterpolatedIntegralCrossSection(dt,dz,iz,it,iy, Z);
	}

      G4double sigtot = InterpolatedIntegralCrossSection(dt,dz,iz,it,m_nbiny,Z);
      dl = (sigtot - sigcut)*theAtomNumDensityVector[i];
    }
    // protection
    if(dl < 0.0) { dl = 0.0; }
    sum += dl;
    m_partialSum[i] = sum;
  }

  G4double rval = G4UniformRand()*sum;
  for (i=0; i<nElements; ++i) {
    if(rval<=m_partialSum[i]) { return (*theElementVector)[i]; }
  }

  return (*theElementVector)[nElements - 1];

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


