# $Id: CMakeLists.txt 731930 2016-03-23 16:08:20Z krasznaa $
################################################################################
# Package: VP1CaloSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1CaloSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/Identifier
   graphics/VP1/VP1Base
   PRIVATE
   Calorimeter/CaloConditions
   Calorimeter/CaloDetDescr
   Calorimeter/CaloEvent
   Calorimeter/CaloIdentifier
   Control/StoreGate
   DetectorDescription/GeoModel/GeoModelKernel
   DetectorDescription/GeoModel/GeoModelUtilities
   GaudiKernel
   LArCalorimeter/LArIdentifier
   LArCalorimeter/LArRawEvent
   LArCalorimeter/LArTools
   TileCalorimeter/TileConditions
   TileCalorimeter/TileDetDescr
   TileCalorimeter/TileEvent
   TileCalorimeter/TileIdentifier
   graphics/VP1/VP1HEPVis
   graphics/VP1/VP1Utils )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Qt4 COMPONENTS QtCore QtGui )
find_package( Qwt )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1CaloSystems
   VP1CaloSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1CaloSystems
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${QWT_INCLUDE_DIRS}
   ${CMAKE_CURRENT_BINARY_DIR}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} Identifier VP1Base Qt4::QtCore
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${QWT_LIBRARIES} CaloDetDescrLib
   StoreGateLib LArToolsLib TileConditionsLib CaloConditions CaloEvent
   CaloIdentifier GeoModelKernel GeoModelUtilities GaudiKernel LArIdentifier
   LArRawEvent TileDetDescr TileEvent TileIdentifier VP1HEPVis VP1Utils
   Qt4::QtGui )
