################################################################################
# Package: LArConditionsTest
################################################################################

# Declare the package name:
atlas_subdir( LArConditionsTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/MinimalRunTime
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArTools
                          TestPolicy
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArCOOLConditions )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( LArConditionsTest
                     src/LArConditionsTest_entries.cxx
                     src/LArConditionsTest_load.cxx
                     src/TestLArConditionsTools.cxx
                     src/FixLArElecCalib.cxx
                     src/LArCondDataTest.cxx
                     src/LArConditionsTestAlg.cxx
                     src/LArFecLvTempDcsTest.cxx
                     src/LArCablingTest.cxx
                     src/LArIdMapConvert.cxx
                     src/LArShapeToSCShape.cxx
                     src/LArOFCtoOFC.cxx
                     src/LArFebRodMapConvert.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel LArCablingLib LArIdentifier LArRawConditions LArRawUtilsLib LArToolsLib CaloDetDescrLib CaloEvent CaloIdentifier AthenaKernel AthenaPoolUtilities xAODEventInfo LArCOOLConditions )

# Install files from the package:
atlas_install_headers( LArConditionsTest )
atlas_install_joboptions( share/*.py )



function (larconditions_run_test testName)
  cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/larconditions_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/larconditions_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/larconditions_${testName}.sh
                  ENVIRONMENT PATH=${CMAKE_CURRENT_SOURCE_DIR}/test:$ENV{PATH}
                  PROPERTIES TIMEOUT 300
                   )
  if( ARG_DEPENDS )
    set_tests_properties( LArConditionsTest_${testName}_ctest
                          PROPERTIES DEPENDS LArConditionsTest_${ARG_DEPENDS}_ctest )
  endif()
endfunction (larconditions_run_test)


larconditions_run_test (LArConditionsTest)
larconditions_run_test (LArConditionsTestWriteNoReg)
larconditions_run_test (LArConditionsTestReadNoReg DEPENDS LArConditionsTestWriteNoReg)

# Remaining tests have been disabled since forever.
# LArConditionsTestReadAndReg
# LArConditionsTestReadTwoStep
# LArConditionsTestWriteNoCorr
# LArConditionsTestReadNoCorr
# LArConditionsTestWriteCorr
# LArConditionsTestReadCorr
# LArConditionsTestWriteOneStep
# LArConditionsTestReadOneStep
