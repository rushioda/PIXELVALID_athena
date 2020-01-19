/**
 * @file AthenaPoolTools_entries.cxx
 * @brief Required by Gaudi class factory
 */
  
#include "../EventCount.h"
#include "../RequireUniqueEvent.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(EventCount)
DECLARE_ALGORITHM_FACTORY(RequireUniqueEvent)

DECLARE_FACTORY_ENTRIES(AthenaPoolTools) {
   DECLARE_ALGORITHM(EventCount)
   DECLARE_ALGORITHM(RequireUniqueEvent)
}
