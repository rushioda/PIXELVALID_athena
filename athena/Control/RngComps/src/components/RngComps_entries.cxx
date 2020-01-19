#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../AtDSFMTGenSvc.h"
#include "../AtRndmGenSvc.h"
#include "../AtRanluxGenSvc.h"

DECLARE_SERVICE_FACTORY( AtDSFMTGenSvc )
DECLARE_SERVICE_FACTORY( AtRanluxGenSvc )
DECLARE_SERVICE_FACTORY( AtRndmGenSvc )

DECLARE_FACTORY_ENTRIES(RngComps) {
    DECLARE_SERVICE( AtDSFMTGenSvc )
    DECLARE_SERVICE( AtRanluxGenSvc )
    DECLARE_SERVICE( AtRndmGenSvc )
}


