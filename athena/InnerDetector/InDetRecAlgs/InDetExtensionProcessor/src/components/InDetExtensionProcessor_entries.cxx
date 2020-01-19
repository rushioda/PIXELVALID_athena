#include "InDetExtensionProcessor/InDetExtensionProcessor.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetExtensionProcessor )

DECLARE_FACTORY_ENTRIES(InDetExtensionProcessor) {
    DECLARE_NAMESPACE_ALGORITHM( InDet, InDetExtensionProcessor)
}




