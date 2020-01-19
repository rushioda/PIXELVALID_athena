#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TruthParticleBuilder.h"
#include "../McAodFilter.h"
#include "../McAodValidationAlg.h"
#include "../GenEventTupleWriter.h"
#include "../GenEventAsciiWriter.h"
#include "../GenEventAsciiReader.h"
#include "../McAodTupleWriter.h"

DECLARE_ALGORITHM_FACTORY( TruthParticleBuilder )
DECLARE_ALGORITHM_FACTORY( McAodFilter )
DECLARE_ALGORITHM_FACTORY( McAodValidationAlg )
DECLARE_ALGORITHM_FACTORY( GenEventTupleWriter )
DECLARE_ALGORITHM_FACTORY( GenEventAsciiWriter )
DECLARE_ALGORITHM_FACTORY( GenEventAsciiReader )
DECLARE_ALGORITHM_FACTORY( McAodTupleWriter )

DECLARE_FACTORY_ENTRIES(McParticleAlgs) {
    
  DECLARE_ALGORITHM( TruthParticleBuilder )
  DECLARE_ALGORITHM( McAodFilter )
  DECLARE_ALGORITHM( McAodValidationAlg )
  DECLARE_ALGORITHM( GenEventTupleWriter )
  DECLARE_ALGORITHM( GenEventAsciiWriter )
  DECLARE_ALGORITHM( GenEventAsciiReader )
  DECLARE_ALGORITHM( McAodTupleWriter )
}
