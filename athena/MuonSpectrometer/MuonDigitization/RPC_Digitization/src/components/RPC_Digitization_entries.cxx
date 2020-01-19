#include "GaudiKernel/DeclareFactoryEntries.h"
#include "RPC_Digitization/RPC_Digitizer.h"
#include "RPC_Digitization/RpcDigitizationTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

DECLARE_ALGORITHM_FACTORY( RPC_Digitizer )

DECLARE_TOOL_FACTORY( RpcDigitizationTool )

DECLARE_FACTORY_ENTRIES(RPC_Digitization) {
    DECLARE_ALGORITHM( RPC_Digitizer )

    DECLARE_TOOL(  RpcDigitizationTool )

}
