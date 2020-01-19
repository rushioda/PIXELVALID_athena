/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHADRONS_G4PROCESSHELPER_HH
#define RHADRONS_G4PROCESSHELPER_HH 1

#include"globals.hh"
#include"G4ParticleDefinition.hh"
#include"G4DynamicParticle.hh"
#include"G4Element.hh"
#include"G4Track.hh"
#include<vector>
#include<map>

//Typedefs just made to make life easier :-)
typedef std::vector<G4int> ReactionProduct;
typedef std::vector<ReactionProduct > ReactionProductList;
typedef std::map<G4int , ReactionProductList> ReactionMap;

class G4ParticleTable;

class G4ProcessHelper {

public:
  static G4ProcessHelper* Instance();

  G4bool ApplicabilityTester(const G4ParticleDefinition& aPart);

  G4double GetInclusiveCrossSection(const G4DynamicParticle *aParticle,
                                    const G4Element *anElement);

  //Make sure the element is known (for n/p-decision)
  ReactionProduct GetFinalState(const G4Track& aTrack,G4ParticleDefinition*& aTarget);

protected:
  G4ProcessHelper();
  G4ProcessHelper(const G4ProcessHelper&);
  G4ProcessHelper& operator= (const G4ProcessHelper&);

private:

  G4double checkfraction;
  G4int n_22;
  G4int n_23;

  G4ParticleDefinition* theTarget;
  G4ParticleDefinition* theProton;
  G4ParticleDefinition* theNeutron;
  G4ParticleDefinition* theRmesoncloud;
  G4ParticleDefinition* theRbaryoncloud;

  ReactionMap* theReactionMap;

  static G4ProcessHelper* pinstance;

  G4double Regge(const double boost);
  G4double Pom(const double boost);

  G4double PhaseSpace(const ReactionProduct& aReaction,const G4DynamicParticle* aDynamicParticle);

  G4double ReactionProductMass(const ReactionProduct& aReaction,const G4DynamicParticle* aDynamicParticle);

  G4bool ReactionIsPossible(const ReactionProduct& aReaction,const G4DynamicParticle* aDynamicParticle);
  G4bool ReactionGivesBaryon(const ReactionProduct& aReaction);
  void ReadAndParse(const G4String& str,
                    std::vector<G4String>& tokens,
                    const G4String& delimiters = " ");

  //Map of applicable particles
  std::map<const G4ParticleDefinition*,G4bool> known_particles;

  //Map for physics parameters, name to value
  std::map<G4String,G4double> parameters;

  //The parameters themselves
  bool resonant;
  bool reggemodel;
  double ek_0;
  double gamma;
  double amplitude;
  double xsecmultiplier;
  double suppressionfactor;
  double hadronlifetime;
  double mixing;
  int doDecays;

  //Proton-scattering processes
  ReactionMap pReactionMap;

  //Neutron-scattering processes
  ReactionMap nReactionMap;

  G4ParticleTable* particleTable;

};
#endif // RHADRONS_G4PROCESSHELPER_HH
