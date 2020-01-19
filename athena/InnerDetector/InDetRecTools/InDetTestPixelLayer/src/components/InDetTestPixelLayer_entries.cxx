#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTestPixelLayer/InDetTestPixelLayerTool.h"


using namespace InDet ;

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTestPixelLayerTool )


/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( InDetTestPixelLayer )
{
  DECLARE_NAMESPACE_TOOL( InDet, InDetTestPixelLayerTool );

}
