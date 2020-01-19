#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "MM_Digitization/MmDigitizationTool.h"
#include "MM_Digitization/MM_Digitizer.h"
#include "MM_Digitization/MM_Response_DigitTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

DECLARE_ALGORITHM_FACTORY( MM_Digitizer )

DECLARE_TOOL_FACTORY( MM_Response_DigitTool )
DECLARE_TOOL_FACTORY( MmDigitizationTool )

DECLARE_FACTORY_ENTRIES(MM_Digitization) {
    DECLARE_ALGORITHM( MM_Digitizer )
    DECLARE_TOOL( MM_Response_DigitTool )
    DECLARE_TOOL( MmDigitizationTool )
}
