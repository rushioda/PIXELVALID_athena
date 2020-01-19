#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigMuonHypo/MuonRoiFex.h"
#include "TrigMuonHypo/MufastHypo.h"
#include "TrigMuonHypo/MufastStauHypo.h"
#include "TrigMuonHypo/MufastPEBHypo.h"
#include "TrigMuonHypo/MufastOTRHypo.h"
#include "TrigMuonHypo/MufastCALHypo.h"
#include "TrigMuonHypo/MucombHypo.h"
#include "TrigMuonHypo/MucombStauHypo.h"
#include "TrigMuonHypo/MuisoHypo.h"
#include "TrigMuonHypo/TrigMooreHypo.h"
#include "TrigMuonHypo/TrigMuonEFSegmentFinderHypo.h"
#include "TrigMuonHypo/TrigMuonEFTrackBuilderHypo.h"
#include "TrigMuonHypo/TrigMuonEFTrackBuilderMultiHypo.h"
#include "TrigMuonHypo/TrigMuonEFExtrapolatorHypo.h"
#include "TrigMuonHypo/TrigMuonEFExtrapolatorMultiHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerMultiHypo.h"
#include "TrigMuonHypo/TrigMuonEFSegmentHypo.h"
#include "TrigMuonHypo/TrigMuonEFTrackIsolationHypo.h"
#include "TrigMuonHypo/TrigMuonEFTrackIsolationMultiHypo.h"
#include "TrigMuonHypo/TrigMuonEFCaloIsolationHypo.h"
#include "TrigMuonHypo/TrigL2DimuonMassHypo.h"
#include "TrigMuonHypo/StauHypo.h"
#include "TrigMuonHypo/TileMuHypo.h"
#include "TrigMuonHypo/TrigMuGirlHypo.h"
#include "TrigMuonHypo/TrigMuGirlStauHypo.h"
#include "TrigMuonHypo/TrigMuTagIMOHypo.h"
#include "TrigMuonHypo/TrigL2MuonOverlapRemover.h"
#include "TrigMuonHypo/TrigEFMuonOverlapRemover.h"
#include "TrigMuonHypo/MufastNSWHypo.h"
#include "TrigMuonHypo/TrigMuonEFExtrapolatorNSWHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerDiMuonMassHypo.h"
#include "TrigMuonHypo/TrigMuonEFCombinerDiMuonMassPtImpactsHypo.h"
#include "TrigMuonHypo/TrigMuonCaloTagHypo.h"
#include "TrigMuonHypo/TrigMuonIDTrackMultiHypo.h"

DECLARE_ALGORITHM_FACTORY( MuonRoiFex )
DECLARE_ALGORITHM_FACTORY( MufastHypo )
DECLARE_ALGORITHM_FACTORY( MufastStauHypo )
DECLARE_ALGORITHM_FACTORY( MufastPEBHypo )
DECLARE_ALGORITHM_FACTORY( MufastOTRHypo )
DECLARE_ALGORITHM_FACTORY( MufastCALHypo )
DECLARE_ALGORITHM_FACTORY( MucombHypo )
DECLARE_ALGORITHM_FACTORY( MucombStauHypo )
DECLARE_ALGORITHM_FACTORY( MuisoHypo )
DECLARE_ALGORITHM_FACTORY( TrigMooreHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFSegmentFinderHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFTrackBuilderHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFTrackBuilderMultiHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFExtrapolatorHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFExtrapolatorMultiHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFCombinerHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFCombinerMultiHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFSegmentHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFTrackIsolationHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFTrackIsolationMultiHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFCaloIsolationHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DimuonMassHypo )
DECLARE_ALGORITHM_FACTORY( StauHypo )
DECLARE_ALGORITHM_FACTORY( TileMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuGirlHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuGirlStauHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuTagIMOHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2MuonOverlapRemover )
DECLARE_ALGORITHM_FACTORY( TrigEFMuonOverlapRemover )
DECLARE_ALGORITHM_FACTORY( MufastNSWHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFExtrapolatorNSWHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFCombinerDiMuonMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonEFCombinerDiMuonMassPtImpactsHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonCaloTagHypo )
DECLARE_ALGORITHM_FACTORY( TrigMuonIDTrackMultiHypo )

DECLARE_FACTORY_ENTRIES( TrigMuonHypo ) {

	DECLARE_ALGORITHM( MuonRoiFex )
	DECLARE_ALGORITHM( MufastHypo )
	DECLARE_ALGORITHM( MufastPEBHypo )
        DECLARE_ALGORITHM( MufastOTRHypo )
        DECLARE_ALGORITHM( MufastCALHypo )
	DECLARE_ALGORITHM( MucombHypo )
	DECLARE_ALGORITHM( MuisoHypo )
	DECLARE_ALGORITHM( TrigMooreHypo )
	DECLARE_ALGORITHM( TrigMuonEFSegmentFinderHypo )
	DECLARE_ALGORITHM( TrigMuonEFTrackBuilderHypo )
	DECLARE_ALGORITHM( TrigMuonEFTrackBuilderMultiHypo )
	DECLARE_ALGORITHM( TrigMuonEFExtrapolatorHypo )
	DECLARE_ALGORITHM( TrigMuonEFExtrapolatorMultiHypo )
	DECLARE_ALGORITHM( TrigMuonEFCombinerHypo )
	DECLARE_ALGORITHM( TrigMuonEFCombinerMultiHypo )
	DECLARE_ALGORITHM( TrigMuonEFSegmentHypo )
	DECLARE_ALGORITHM( TrigMuonEFTrackIsolationHypo )
	DECLARE_ALGORITHM( TrigMuonEFTrackIsolationMultiHypo )
	DECLARE_ALGORITHM( TrigMuonEFCaloIsolationHypo )
	DECLARE_ALGORITHM( TrigL2DimuonMassHypo )
        DECLARE_ALGORITHM( StauHypo )
        DECLARE_ALGORITHM( TileMuHypo )
	DECLARE_ALGORITHM( TrigMuGirlHypo )
	DECLARE_ALGORITHM( TrigMuTagIMOHypo )
	DECLARE_ALGORITHM( TrigL2MuonOverlapRemover )
	DECLARE_ALGORITHM( TrigEFMuonOverlapRemover )
	DECLARE_ALGORITHM( MufastNSWHypo )
	DECLARE_ALGORITHM( TrigMuonEFExtrapolatorNSWHypo )
	DECLARE_ALGORITHM( TrigMuonEFCombinerDiMuonMassHypo )
	DECLARE_ALGORITHM( TrigMuonEFCombinerDiMuonMassPtImpactsHypo )
        DECLARE_ALGORITHM( TrigMuonCaloTagHypo )
        DECLARE_ALGORITHM( TrigMuonIDTrackMultiHypo )

}
