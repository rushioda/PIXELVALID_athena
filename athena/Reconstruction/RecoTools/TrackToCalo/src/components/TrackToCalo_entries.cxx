//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../ParticleCaloExtensionTool.h"
#include "../ParticleCaloCellAssociationTool.h"
#include "../ParticleCaloClusterAssociationTool.h"
#include "../TrackParticleCaloExtensionAlg.h"
#include "../MuonCaloEnergyTool.h"

using namespace Trk;
using namespace Rec;
DECLARE_TOOL_FACTORY( ParticleCaloExtensionTool )
DECLARE_TOOL_FACTORY( ParticleCaloCellAssociationTool )
DECLARE_TOOL_FACTORY( ParticleCaloClusterAssociationTool )
DECLARE_TOOL_FACTORY( MuonCaloEnergyTool )
DECLARE_ALGORITHM_FACTORY( TrackParticleCaloExtensionAlg )

DECLARE_FACTORY_ENTRIES	( TrackToCalo ) {
    DECLARE_TOOL( ParticleCaloExtensionTool );
    DECLARE_TOOL( ParticleCaloCellAssociationTool );
    DECLARE_TOOL( ParticleCaloClusterAssociationTool );
    DECLARE_TOOL( MuonCaloEnergyTool );
    DECLARE_ALGORITHM( TrackParticleCaloExtensionAlg );
}


