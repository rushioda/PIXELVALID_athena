################################################################################
# Package: ISF_FatrasEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          Simulation/ISF/ISF_Fatras/ISF_FatrasEvent
                          Simulation/ISF/ISF_Fatras/ISF_FatrasEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( ISF_FatrasEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ISF_FatrasEvent/PlanarClusterContainer.h
                           TYPES_WITH_NAMESPACE iFatras::PlanarClusterContainer
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel ISF_FatrasEvent ISF_FatrasEventTPCnv )

# Install files from the package:
atlas_install_headers( ISF_FatrasEventAthenaPool )

