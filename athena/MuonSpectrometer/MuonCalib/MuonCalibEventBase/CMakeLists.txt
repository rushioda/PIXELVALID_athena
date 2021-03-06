################################################################################
# Package: MuonCalibEventBase
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibEventBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonRDO
                          PRIVATE
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCalibEventBase
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibEventBase
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives MuonCalibIdentifier MuonRDO )

