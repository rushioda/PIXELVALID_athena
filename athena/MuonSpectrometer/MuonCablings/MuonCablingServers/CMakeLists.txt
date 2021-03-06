################################################################################
# Package: MuonCablingServers
################################################################################

# Declare the package name:
atlas_subdir( MuonCablingServers )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonCablings/TGCcablingInterface
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Event/EventInfoMgt )

# Component(s) in the package:
atlas_add_library( MuonCablingServersLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingServers
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel RPCcablingInterfaceLib TGCcablingInterfaceLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES EventInfo )

atlas_add_component( MuonCablingServers
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel RPCcablingInterfaceLib TGCcablingInterfaceLib StoreGateLib SGtests EventInfo MuonCablingServersLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

