#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigT1TGC/LVL1TGCTrigger.h"
#include "TrigT1TGC/MakeCoincidenceOut.h"

typedef LVL1TGCTrigger::LVL1TGCTrigger Lvl1LVL1TGCTrigger;
DECLARE_ALGORITHM_FACTORY( Lvl1LVL1TGCTrigger )
typedef LVL1TGCTrigger::MakeCoincidenceOut Lvl1MakeCoincidenceOut;
DECLARE_ALGORITHM_FACTORY( Lvl1MakeCoincidenceOut )

DECLARE_FACTORY_ENTRIES(TrigT1TGC) {
  DECLARE_ALGORITHM ( Lvl1LVL1TGCTrigger )
  DECLARE_ALGORITHM ( Lvl1MakeCoincidenceOut )
}
