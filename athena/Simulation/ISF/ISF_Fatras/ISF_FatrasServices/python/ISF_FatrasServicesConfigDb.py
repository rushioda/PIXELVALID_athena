# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF
Elmar Ritsch, 10/11/2014
"""

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
import AthenaCommon.SystemOfUnits as Units


addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getInDetTrackingGeometryBuilder",            "ISF_InDetTrackingGeometryBuilder")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasCaloTrackingGeometryBuilder",       "ISF_FatrasCaloTrackingGeometryBuilder")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasMuonTrackingGeometryBuilder",       "ISF_FatrasMuonTrackingGeometryBuilder")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasGeometryBuilder",                   "ISF_FatrasGeometryBuilder")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasNavigator",                         "ISF_FatrasNavigator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasChargedPropagator",                 "ISF_FatrasChargedPropagator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasSTEP_Propagator",                   "ISF_FatrasSTEP_Propagator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPropagator",                        "ISF_FatrasPropagator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasStaticPropagator",                  "ISF_FatrasStaticPropagator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasStaticNavigationEngine",            "ISF_FatrasStaticNavigationEngine")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasG4HadIntProcessor",                 "ISF_FatrasG4HadIntProcessor")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasParametricHadIntProcessor",         "ISF_FatrasParametricHadIntProcessor")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasBetheHeitlerEnergyLossUpdator",     "ISF_FatrasBetheHeitlerEnergyLossUpdator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasEnergyLossUpdator",                 "ISF_FatrasEnergyLossUpdator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasEnergyLossSamplerBetheHeitler",     "ISF_FatrasEnergyLossSamplerBetheHeitler")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasEnergyLossSamplerBetheBloch",       "ISF_FatrasEnergyLossSamplerBetheBloch")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasMultipleScatteringUpdator",         "ISF_FatrasMultipleScatteringUpdator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasMultipleScatteringSamplerHighland", "ISF_FatrasMultipleScatteringSamplerHighland")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasMultipleScatteringSamplerGaussianMixture", "ISF_FatrasMultipleScatteringSamplerGaussianMixture")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasMultipleScatteringSamplerGeneralMixture", "ISF_FatrasMultipleScatteringSamplerGeneralMixture")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPhysicsValidationTool",             "ISF_FatrasPhysicsValidationTool")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasProcessSamplingTool",               "ISF_FatrasProcessSamplingTool")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasMaterialUpdator" ,                  "ISF_FatrasMaterialUpdator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasExtrapolator",                      "ISF_FatrasExtrapolator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasMaterialEffectsEngine",             "ISF_FatrasMaterialEffectsEngine")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasStaticExtrapolator",                "ISF_FatrasStaticExEngine")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasExEngine",                          "ISF_FatrasExEngine")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasConversionCreator",                 "ISF_FatrasConversionCreator")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasHitCreatorPixel",                   "ISF_FatrasHitCreatorPixel")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasHitCreatorSCT",                     "ISF_FatrasHitCreatorSCT")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasHitCreatorTRT",                     "ISF_FatrasHitCreatorTRT")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasSimHitCreatorMS",                   "ISF_FatrasSimHitCreatorMS")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasSimHitCreatorID",                   "ISF_FatrasSimHitCreatorID")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPdgG4Particle",                     "ISF_FatrasPdgG4Particle")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasParticleDecayHelper",               "ISF_FatrasParticleDecayHelper")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasKinematicFilter",                   "ISF_FatrasKinematicFilter")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasSimTool",                           "ISF_FatrasSimTool")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasSimEngine",                         "ISF_FatrasSimEngine")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPileupSimTool",                     "ISF_FatrasPileupSimTool")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getG4RunManagerHelper",                      "ISF_G4RunManagerHelper")

addService("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasTrackingGeometrySvc",            "ISF_FatrasTrackingGeometrySvc")
addService("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasSimServiceID",                   "ISF_FatrasSimSvc")
addService("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasNewExtrapolationSimServiceID",   "ISF_FatrasNewExtrapolationSimSvc")
addService("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPileupSimServiceID",             "ISF_FatrasPileupSimSvc")

addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPileupHitCreatorPixel",             "ISF_FatrasPileupHitCreatorPixel")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPileupHitCreatorSCT",               "ISF_FatrasPileupHitCreatorSCT")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPileupHitCreatorTRT",               "ISF_FatrasPileupHitCreatorTRT")
addTool("ISF_FatrasServices.ISF_FatrasServicesConfig.getFatrasPileupSimHitCreatorID",             "ISF_FatrasPileupSimHitCreatorID")
