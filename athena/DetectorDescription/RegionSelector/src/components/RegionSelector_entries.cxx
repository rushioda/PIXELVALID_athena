#include "GaudiKernel/DeclareFactoryEntries.h"
// #include "RegionSelector/RegSelSvc.h"
#include "../RegSelSvc.h"
// #include "../NewRegSelSvc.h"

DECLARE_SERVICE_FACTORY( RegSelSvc )
// DECLARE_SERVICE_FACTORY( NewRegSelSvc )

DECLARE_FACTORY_ENTRIES(RegionSelector) {
    DECLARE_SERVICE( RegSelSvc )
      //    DECLARE_SERVICE( NewRegSelSvc )
}
