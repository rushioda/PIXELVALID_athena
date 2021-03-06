################################################################################
# Package: TrigEgammaHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloUtils
                          Control/DataModel
                          Control/StoreGate
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          LumiBlock/LumiBlockComps
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/egamma/egammaInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/CxxUtils
                          Event/xAOD/xAODEgammaCnv
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaMVACalib
                          Tracking/TrkEvent/TrkCaloExtension
                          Trigger/TrigAlgorithms/TrigCaloRec
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TrigEgammaHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloUtilsLib DataModel StoreGateLib SGtests xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma GaudiKernel LumiBlockCompsLib PATCoreLib ElectronPhotonSelectorToolsLib TrkSurfaces VxVertex TrigCaloEvent TrigInDetEvent TrigParticle TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigTimeAlgsLib CaloEvent CxxUtils ITrackToVertex RecoToolInterfaces egammaEvent egammaMVACalibLib TrkCaloExtension TrigCaloRecLib TrigMissingEtEvent TrigNavigationLib )

# Install files from the package:
atlas_install_headers( TrigEgammaHypo )
atlas_install_python_modules( python/*.py )

