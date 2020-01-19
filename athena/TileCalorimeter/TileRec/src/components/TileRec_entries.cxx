#include "TileRec/TileAANtuple.h"
#include "TileRec/TileRawChannelToNtuple.h"
#include "TileRec/TileCellIDCToNtuple.h"
#include "TileRec/TileCellToNtuple.h"
#include "TileRec/TileHitToNtuple.h"
#include "TileRec/TileTBHitToNtuple.h"
#include "TileRec/TileHitVecToNtuple.h"
#include "TileRec/TileDigitsToNtuple.h"
#include "TileRec/TileTTL1ToNtuple.h"
#include "TileRec/TileEventFilter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TileRec/TileMuonReceiverDecisionToNtuple.h"

DECLARE_ALGORITHM_FACTORY( TileAANtuple )
DECLARE_ALGORITHM_FACTORY( TileRawChannelToNtuple )
DECLARE_ALGORITHM_FACTORY( TileCellIDCToNtuple )
DECLARE_ALGORITHM_FACTORY( TileCellToNtuple )
DECLARE_ALGORITHM_FACTORY( TileHitToNtuple )
DECLARE_ALGORITHM_FACTORY( TileTBHitToNtuple )
DECLARE_ALGORITHM_FACTORY( TileHitVecToNtuple )
DECLARE_ALGORITHM_FACTORY( TileDigitsToNtuple )
DECLARE_ALGORITHM_FACTORY( TileTTL1ToNtuple )
DECLARE_ALGORITHM_FACTORY( TileEventFilter )
DECLARE_ALGORITHM_FACTORY( TileMuonReceiverDecisionToNtuple )

DECLARE_FACTORY_ENTRIES(TileRec) {
  DECLARE_ALGORITHM( TileAANtuple )
  DECLARE_ALGORITHM( TileRawChannelToNtuple )
  DECLARE_ALGORITHM( TileCellIDCToNtuple )
  DECLARE_ALGORITHM( TileCellToNtuple )
  DECLARE_ALGORITHM( TileHitToNtuple )
  DECLARE_ALGORITHM( TileTBHitToNtuple )
  DECLARE_ALGORITHM( TileHitVecToNtuple )
  DECLARE_ALGORITHM( TileDigitsToNtuple )
  DECLARE_ALGORITHM( TileTTL1ToNtuple )
  DECLARE_ALGORITHM( TileEventFilter )
  DECLARE_ALGORITHM( TileMuonReceiverDecisionToNtuple )
}
