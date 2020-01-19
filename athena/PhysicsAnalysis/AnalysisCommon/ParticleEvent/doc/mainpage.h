/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage ParticleEvent Package

The package which defines the AOD particle classes.

@authors Ketevi Assamagan, Sebastien Binet, Kyle Cranmer, Tadashi Maeno


@section ParticleEventIntro Introduction

The AOD particle classes are the basic objects to be used in physics
analysis, and are the bulk of the AOD.  Each particle class has some
basic properties defined by the IParticle interface and adds
additional functionality.  

The IParticle interface gives all particles two major pieces of functionality:

  - Four Momentum: provided by the I4Momentum interface and
    implemented in the FourMom package.

  - Navigation; provided by INavigable interface.  Navigation allows
    one to 'navigate' to the constituents of the particle (see the
    INavigable::filltoken() method).The constituents of a particle can either be a
    reconstruction-level object (like a TrackParticle or CaloCluster)
    or a daughter particle.

It is also possible to fill the AOD particle classes from Atlfast;
however, not all of the information is available from the fast
simulation.  It is possible to write your analysis so that it can run
on either full or fast simulation, but you may need to add some logic
so that you do not try and use information that does not exist in the
fast simulation.  To find out if a particle was created from fast or
full simulation use the IParticle::dataType() method.  



@section ParticleEventOverview Class Overview

  The ParticleEvent package contains of following classes:
 
  - ParticleBase: common implementation for all Particles

  - Neutrino: not built from ESD, but created during analysis either
    by hand or by one of the SpecialUtils.


@section ParticleEventBuilders Particle Builders

  Each of the particles is built by an algorithm in the
  PhysicsAnalysis package.  If you want to know what pre-selection is
  applied or what exactly is going into the AOD particles, then follow
  the links below.

  - Neutrino: not built from ESD, but created during analysis either
    by hand or by one of the SpecialUtils.

@section ParticleEventStoreGateKeys StoreGate Keys

  The storegate keys for the particle containers is defined in the
  jobOptions files of the builder algorithms above.  It is tempting to
  add them here, but the keys are really job option properties and
  don't belong in the Doxygen documentation.  There is a wiki page here:
  https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD 


@section ParticleEventSeeAlso See also

You may also be interested in:

   - Electron and Photon

   - Muon

   - TauJet

   - ParticleJet

   - CompositeParticle

   - Analysis::JetTag and Analysis::TagInfo

   - MissingET objects in MissingETEvent

   - Rec::TrackParticle

   - TruthParticle in TruthParticleID/McParticleEvent

@section ParticleEventReq Requirements

@include requirements

*/
