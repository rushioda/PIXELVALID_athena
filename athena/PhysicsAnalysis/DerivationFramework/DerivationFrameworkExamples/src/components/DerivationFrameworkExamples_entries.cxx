#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkExamples/SkimmingToolExample.h"
#include "DerivationFrameworkExamples/ThinningToolExample.h"
#include "DerivationFrameworkExamples/AugmentationToolExample.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolExample )
DECLARE_TOOL_FACTORY( ThinningToolExample )
DECLARE_TOOL_FACTORY( AugmentationToolExample )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkExamples ) {
   DECLARE_TOOL( SkimmingToolExample )
   DECLARE_TOOL( ThinningToolExample )
   DECLARE_TOOL( AugmentationToolExample )
}

