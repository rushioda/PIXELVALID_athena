//
//  Entry file for ATHENA component libraries
//
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LArL1Sim/LArTTL1Maker.h"
#include "LArL1Sim/LArSCL1Maker.h"
#include "LArL1Sim/LArTTL1Calib.h"
#include "LArL1Sim/LArSCSimpleMaker.h"

DECLARE_ALGORITHM_FACTORY( LArTTL1Maker )
DECLARE_ALGORITHM_FACTORY( LArSCL1Maker )
DECLARE_ALGORITHM_FACTORY( LArTTL1Calib )
DECLARE_ALGORITHM_FACTORY( LArSCSimpleMaker )

DECLARE_FACTORY_ENTRIES(LArL1Sim) {

  DECLARE_ALGORITHM(LArTTL1Maker)
  DECLARE_ALGORITHM(LArSCL1Maker)
  DECLARE_ALGORITHM(LArTTL1Calib)
  DECLARE_ALGORITHM(LArSCSimpleMaker)

}
