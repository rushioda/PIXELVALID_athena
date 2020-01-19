#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_ElectronPidTools/BaseTRTPIDCalculator.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidTool.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidToolRun2.h"
#include "TRT_ElectronPidTools/TRT_LocalOccupancy.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_ElectronPidTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_ElectronPidToolRun2 )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_LocalOccupancy )

DECLARE_FACTORY_ENTRIES( TRT_ElectronPidTools )
{
	DECLARE_NAMESPACE_TOOL( InDet, BaseTRTPIDCalculator)
	DECLARE_NAMESPACE_TOOL( InDet, TRT_ElectronPidTool )
	DECLARE_NAMESPACE_TOOL( InDet, TRT_ElectronPidToolRun2 )
	DECLARE_NAMESPACE_TOOL( InDet, TRT_LocalOccupancy )
}

