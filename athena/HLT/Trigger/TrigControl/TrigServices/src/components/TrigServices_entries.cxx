#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigServices/TrigMessageSvc.h"
#include "../TrigMonTHistSvc.h"
#include "TrigServices/HltEventLoopMgr.h"
#include "TrigServices/HltROBDataProviderSvc.h"
#include "TrigServices/TrigIS.h"
#include "TrigServices/TrigISHelper.h"
#include "../TrigPreFlightCheck.h"
#include "../TrigCOOLUpdateHelper.h"

DECLARE_SERVICE_FACTORY( TrigMessageSvc )
DECLARE_SERVICE_FACTORY( TrigMonTHistSvc )
DECLARE_SERVICE_FACTORY( TrigIS )
DECLARE_SERVICE_FACTORY( HltEventLoopMgr )
DECLARE_SERVICE_FACTORY( HltROBDataProviderSvc )
DECLARE_TOOL_FACTORY( TrigISHelper )
DECLARE_TOOL_FACTORY( TrigPreFlightCheck )
DECLARE_TOOL_FACTORY( TrigCOOLUpdateHelper )  


DECLARE_FACTORY_ENTRIES( TrigServices ) {
  DECLARE_SERVICE( TrigMessageSvc );
  DECLARE_SERVICE( TrigMonTHistSvc );
  DECLARE_SERVICE( TrigIS );  
  DECLARE_SERVICE( HltEventLoopMgr );
  DECLARE_SERVICE( HltROBDataProviderSvc );
  DECLARE_TOOL( TrigISHelper );
  DECLARE_TOOL( TrigPreFlightCheck );
  DECLARE_TOOL( TrigCOOLUpdateHelper );
}
