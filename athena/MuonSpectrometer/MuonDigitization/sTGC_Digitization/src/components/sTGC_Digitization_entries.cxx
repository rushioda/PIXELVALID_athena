#include "GaudiKernel/DeclareFactoryEntries.h"
#include "sTGC_Digitization/sTgcDigitizationTool.h"
#include "sTGC_Digitization/sTGC_Digitizer.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

DECLARE_ALGORITHM_FACTORY( sTGC_Digitizer )
 
DECLARE_TOOL_FACTORY( sTgcDigitizationTool )

DECLARE_FACTORY_ENTRIES(sTGC_Digitization) {
    DECLARE_ALGORITHM( sTGC_Digitizer )
    DECLARE_TOOL( sTgcDigitizationTool )
}
