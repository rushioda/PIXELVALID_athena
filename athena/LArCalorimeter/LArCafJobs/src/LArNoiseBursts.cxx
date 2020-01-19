/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// LArNoiseBursts
/// Author: Josu Cantero
/// UAM, November, 2010
/// Modified by Hideki Okawa
/// BNL, June, 2012

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/LArID_Exception.h"


#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArElectrodeID.h"

#include "LArRecEvent/LArEventBitInfo.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArRecEvent/LArCollisionTime.h"
 
#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

// Lar HV
#include "LArElecCalib/ILArHVTool.h"
#include "StoreGate/DataHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVSubgapConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArHV/FCALHVLineConstLink.h"
#include "LArHV/FCALHVLine.h"
#include "LArHV/LArHVManager.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArCondUtils/LArHVToolDB.h"
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "xAODEventInfo/EventInfo.h"

#include "CaloEvent/CaloCellContainer.h"

#include "LArCafJobs/LArNoiseBursts.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

using namespace std;
using xAOD::EventInfo;

int nlarcell=0;
int n_noisy_cell_part[8] = {0,0,0,0,0,0,0,0};
int n_cell_part[8] = {0,0,0,0,0,0,0,0};
std::vector<short> m_ft_noisy, m_slot_noisy, m_channel_noisy;
std::vector<bool> m_isbarrel, m_isendcap, m_isfcal, m_ishec;
std::vector<short> m_layer; 
std::vector<int> m_partition,m_noisycellHVphi,m_noisycellHVeta;
std::vector<float> m_energycell, m_qfactorcell, m_signifcell;
std::vector<float> m_phicell, m_etacell;
std::vector<bool> m_isbadcell;
std::vector<IdentifierHash>  m_IdHash;
std::vector<int> m_cellpartlayerindex;
std::vector<Identifier> m_cellIdentifier;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor


LArNoiseBursts::LArNoiseBursts(const std::string& name,
			 ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_thistSvc(0),
    m_tree(0),
    m_LArCablingService("LArCablingService"),
    m_LArHVCablingTool("LArHVCablingTool"),
    m_calo_noise_tool("CaloNoiseTool/CaloNoiseToolDefault"),
    m_bc_tool("Trig::TrigConfBunchCrossingTool/BunchCrossingTool"),
    m_badchan_tool("LArBadChanTool"),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_LArOnlineIDHelper(0),
    m_LArHVLineIDHelper(0),
    m_LArElectrodeIDHelper(0),
    m_LArEM_IDHelper(0),
    m_LArFCAL_IDHelper(0),
    m_LArHEC_IDHelper(0),
    m_caloIdMgr(0),
    m_calodetdescrmgr(0),
    m_CosmicCaloStream(false),
    m_nb_sat(0),
    m_lowqfactor(0),
    m_medqfactor(0),
    m_hiqfactor(0),
    n_noisycell(0),
    m_nt_larcellsize(0),
    m_nt_cellsize(0),
    m_nt_run(0),
    m_nt_evtId(0),
    m_nt_evtTime(0),
    m_nt_evtTime_ns(0),
    m_nt_lb(0),
    m_nt_bcid(0),
    m_nt_ntracks(0),
    m_nt_isbcidFilled(0),
    m_nt_isbcidInTrain(0),
    m_nt_isBunchesInFront(0),
    m_nt_bunchtype(0),
    m_nt_bunchtime(0),
    m_nt_atlasready(0),
    m_nt_stablebeams(0),
    m_nt_streamTagName(0),
    m_nt_streamTagType(0),
    m_nt_larnoisyro(0),
    m_nt_larnoisyro_opt(0),
    m_nt_larnoisyro_satTwo(0),
    m_nt_larnoisyro_mnbTight(0),
    m_nt_larnoisyro_mnbTight_PSVeto(0),
//    m_nt_veto_mbts(0),
//    //m_nt_veto_indet(0),
//    m_nt_veto_bcm(0),
//    m_nt_veto_lucid(0),
//    m_nt_veto_pixel(0),
//    m_nt_veto_sct(0),
//    m_nt_veto_mbtstdHalo(0),
//    m_nt_veto_mbtstdCol(0),
//    m_nt_veto_lartdHalo(0),
//    m_nt_veto_lartdCol(0),
//    m_nt_veto_csctdHalo(0),
//    m_nt_veto_csctdCol(0),
//    m_nt_veto_bcmtHalo(0),
//    m_nt_veto_bcmtCol(0),
//    m_nt_veto_muontCol(0),
//    m_nt_veto_muontCosmic(0),
    m_nt_larflag_noiseBurstVeto(false),
    m_nt_larflag_dataCorrupted(false),
    m_nt_larflag_dataCorruptedVeto(false),
    m_nt_L1_J75(false),
    m_nt_L1_J10_EMPTY(false),
    m_nt_L1_J30_FIRSTEMPTY(false),
    m_nt_L1_J30_EMPTY(false),
    m_nt_L1_XE40(false),
    m_nt_L1_XE50(false),
    m_nt_L1_XE50_BGRP7(false),
    m_nt_L1_XE70(false),
    m_nt_EF_j165_u0uchad_LArNoiseBurst(false),
    m_nt_EF_j30_u0uchad_empty_LArNoiseBurst(false),
    m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurst(false),
    m_nt_EF_j55_u0uchad_empty_LArNoiseBurst(false),
    m_nt_EF_xe45_LArNoiseBurst(false),
    m_nt_EF_xe55_LArNoiseBurst(false),
    m_nt_EF_xe60_LArNoiseBurst(false),
    m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurstT(false),
    m_nt_EF_j100_u0uchad_LArNoiseBurstT(false),
    m_nt_EF_j165_u0uchad_LArNoiseBurstT(false),
    m_nt_EF_j130_u0uchad_LArNoiseBurstT(false),
    m_nt_EF_j35_u0uchad_empty_LArNoiseBurst(false),
    m_nt_EF_j35_u0uchad_firstempty_LArNoiseBurst(false),
    m_nt_EF_j80_u0uchad_LArNoiseBurstT(false),
    m_nt_ECTimeDiff(0),
    m_nt_ECTimeAvg(0),
    m_nt_nCellA(0),
    m_nt_nCellC(0),
    m_nt_energycell(0),
    m_nt_qfactorcell(0),
    m_nt_phicell(0),
    m_nt_etacell(0),
    m_nt_signifcell(0),
    m_nt_ft_noisy(0),
    m_nt_slot_noisy(0),
    m_nt_channel_noisy(0),
    m_nt_cellpartlayerindex(0),
    m_nt_cellIdentifier(0),
    m_nt_noisycellpart(0),
    m_nt_nominalhv(0),
    m_nt_maximalhv(0),
    m_nt_noisycellHVphi(0),
    m_nt_noisycellHVeta(0),
    m_nt_samples(0),
    m_nt_gain(0),
    m_nt_isbadcell(0),
    m_nt_partition(0),
    m_nt_layer(0),
    m_nt_isbadcell_sat(0),
    m_nt_barrelec_sat(0),
    m_nt_posneg_sat(0),
    m_nt_ft_sat(0),
    m_nt_slot_sat(0),
    m_nt_channel_sat(0),
    m_nt_partition_sat(0),
    m_nt_energy_sat(0),
    m_nt_phicell_sat(0),
    m_nt_etacell_sat(0),
    m_nt_layer_sat(0),
    m_nt_cellIdentifier_sat(0)
 {

   // Trigger
   declareProperty( "TrigDecisionTool", m_trigDec );
   
   /** switches to control the analysis through job options */
   declareProperty("LArCablingService", m_LArCablingService);
   declareProperty("LArHVCablingTool",  m_LArHVCablingTool);
   
   // NEW
   declareProperty( "ICaloNoiseTool", m_calo_noise_tool );
   declareProperty( "BCTool", m_bc_tool );
   //   declareProperty( "SCTClusteringTool",m_sctclustering_tool);
   declareProperty( "ILArBadChanTool", m_badchan_tool );
   
   //event cuts
   declareProperty("SigmaCut", m_sigmacut = 3.0);
   declareProperty("NumberOfBunchesInFront",m_frontbunches = 36);
   
   // Keep cell properties
   declareProperty("KeepOnlyCellID",          m_keepOnlyCellID = false);

   m_calodetdescrmgr = CaloDetDescrManager::instance(); 
 }

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

LArNoiseBursts::~LArNoiseBursts() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate
/// get a handle on the analysis tools
/// book histograms
/*
StatusCode LArNoiseBursts::initializeBeforeEventLoop() {
  MsgStream mLog( messageService(), name() );

  ATH_MSG_DEBUG ( "Initializing LArNoiseBursts (before eventloop)" );
  
  // NEW
  

  // retrieve trigger decision tool
  // needs to be done before the first run/event since a number of
  // BeginRun/BeginEvents are registered by dependent services
  StatusCode sc = StatusCode::SUCCESS;

  return sc;
} */
//////////////////////////////////////////////////////////////////////////////////////
///////////////////          INITIALIZE        ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

StatusCode LArNoiseBursts::initialize() {

  ATH_MSG_DEBUG ( "Initializing LArNoiseBursts" );
 
  // Trigger Decision Tool
  if(!m_trigDec.empty()){
    if(m_trigDec.retrieve().isFailure()){
      ATH_MSG_WARNING ( "Failed to retrieve trigger decision tool " << m_trigDec );
    }else{
      ATH_MSG_INFO ( "Retrieved tool " << m_trigDec );
    }
   }
  
  ATH_CHECK( m_LArCablingService.retrieve() );

  // Retrieve online ID helper
  const DataHandle<LArOnlineID> LArOnlineIDHelper;
  ATH_CHECK( detStore()->retrieve(LArOnlineIDHelper, "LArOnlineID") );
  m_LArOnlineIDHelper = LArOnlineIDHelper;
  ATH_MSG_DEBUG( " Found LArOnline Helper");

  // Retrieve HV line ID helper
  const DataHandle<LArHVLineID> LArHVLineIDHelper;
  ATH_CHECK( detStore()->retrieve(LArHVLineIDHelper, "LArHVLineID") );
  m_LArHVLineIDHelper = LArHVLineIDHelper;
  ATH_MSG_DEBUG( " Found LArOnlineIDHelper Helper");

  // Retrieve HV electrode ID helper
  const DataHandle<LArElectrodeID> LArElectrodeIDHelper;
  ATH_CHECK( detStore()->retrieve(LArElectrodeIDHelper, "LArElectrodeID") );
  m_LArElectrodeIDHelper = LArElectrodeIDHelper;
  ATH_MSG_DEBUG( " Found LArElectrodeIDHelper Helper");

  // Retrieve ID helpers
  ATH_CHECK( detStore()->retrieve(m_caloIdMgr) );
  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();
  
  if ( m_calo_noise_tool.retrieve().isFailure() ) {
    ATH_MSG_WARNING ( "Failed to retrieve tool " << m_calo_noise_tool );
  }else{
    ATH_MSG_INFO ( "Retrieved tool " << m_calo_noise_tool );
  }
 
  if(m_bc_tool.retrieve().isFailure()){
    ATH_MSG_WARNING ( "Failed to retrieve tool " << m_bc_tool );
  }else{
    ATH_MSG_INFO ( "Retrieved tool " << m_bc_tool );
  }

  /** get a handle on the NTuple and histogramming service */
  ATH_CHECK( service("THistSvc", m_thistSvc) );
 
  /** Prepare TTree **/
  m_tree = new TTree( "CollectionTree", "CollectionTree" );
  std::string treeName =  "/TTREE/CollectionTree" ;
  ATH_CHECK( m_thistSvc->regTree(treeName, m_tree) );

  // General properties of events
  m_tree->Branch("Run",&m_nt_run,"Run/I");// Event ID
  m_tree->Branch("EventId",&m_nt_evtId,"EventId/l");// Event ID
  m_tree->Branch("EventTime",&m_nt_evtTime,"EventTime/I");// Event time
  m_tree->Branch("EventTime_ns",&m_nt_evtTime_ns,"EventTime_ns/I");// Event time in nanosecond
  m_tree->Branch("LumiBlock",&m_nt_lb,"LumiBlock/I"); // LB
  m_tree->Branch("BCID",&m_nt_bcid,"BCID/I"); // BCID
  m_tree->Branch("StreamTagName",&m_nt_streamTagName);// stream tag name
  m_tree->Branch("StreamTagType",&m_nt_streamTagType); // stream tag type 
  m_tree->Branch("IsBCIDFilled", &m_nt_isbcidFilled,"IsBCIDFilled/I"); // check if bunch is filled
  m_tree->Branch("IsBCIDInTrain",&m_nt_isbcidInTrain,"ISBCIDInTrain/I"); // check if bunch belong to a train
  m_tree->Branch("BunchesInFront",&m_nt_isBunchesInFront); // check front bunches
  m_tree->Branch("BunchType",&m_nt_bunchtype,"BunchType/I");// Empty = 0, FirstEmpty=1,&middleEmpty=2, Single=100,Front=200,&middle=201,Tail=202 
  m_tree->Branch("TimeAfterBunch",&m_nt_bunchtime,"TimeAfterBunch/F"); //time "distance" between the colliding bunch and the nearest one.
  m_tree->Branch("ATLASIsReady",&m_nt_atlasready,"AtlasIsReady/I"); //check if atlas is ready for physics 
  m_tree->Branch("StableBeams",&m_nt_stablebeams,"StableBeams/I");//check stablebeams

  // Background bits in EventInfo
//  m_tree->Branch("vetoMBTS",&m_nt_veto_mbts,"vetoMBST/S"); //Beam/collision veto based on mbts
//  m_tree->Branch("vetoPixel",&m_nt_veto_pixel,"vetoPixel/S"); //Beam/collision veto based on indet
//  m_tree->Branch("vetoSCT",&m_nt_veto_sct,"vetoSCT/S"); //Beam/collision veto based on indet
//  m_tree->Branch("vetoBcm",&m_nt_veto_bcm,"vetoBcm/S"); //Beam/collision veto based on bcm
//  m_tree->Branch("vetoLucid",&m_nt_veto_lucid,"vetoLucid/S"); //Beam/collision veto based on lucid
//  m_tree->Branch("vetoMBTSDtHalo",&m_nt_veto_mbtstdHalo,"vetoMBTSDtHalo/S");
//  m_tree->Branch("vetoMBTSDtCol",&m_nt_veto_mbtstdCol,"vetoMBTSDtCol/S");
//  m_tree->Branch("vetoLArDtHalo",&m_nt_veto_lartdHalo,"vetoLArDtHalo/S");
//  m_tree->Branch("vetoLArDtCol",&m_nt_veto_lartdCol,"vetoLArDtCol/S");
//  m_tree->Branch("vetoCSCDtHalo",&m_nt_veto_csctdHalo,"vetoCSCDtHalo/S");
//  m_tree->Branch("vetoCSCDtCol",&m_nt_veto_csctdCol,"vetoCSCDtCol/S");
//  m_tree->Branch("vetoBCMDtHalo",&m_nt_veto_bcmtHalo,"vetoBCMDtHalo/S");
//  m_tree->Branch("vetoBCMDtCol",&m_nt_veto_bcmtCol,"vetoBCMDtCol/S");
//  m_tree->Branch("vetoMuonTimmingCol", &m_nt_veto_muontCol,"vetoMuonTimmingCol/S");
//  m_tree->Branch("vetoMuonTimmingCosmic",&m_nt_veto_muontCosmic,"vetoMuonTimmingCosmic/S");

  // LAr event bit info
  m_tree->Branch("larflag_noiseBurstVeto",&m_nt_larflag_noiseBurstVeto,"larflag_noiseBurstVeto/O");
  m_tree->Branch("larflag_dataCorrupted",&m_nt_larflag_dataCorrupted,"larflag_dataCorrupted/O");
  m_tree->Branch("larflag_dataCorruptedVeto",&m_nt_larflag_dataCorruptedVeto,"larflag_dataCorruptedVeto/O");

  // trigger flags
  m_tree->Branch("L1_J75",&m_nt_L1_J75,"L1_J75/O");
  m_tree->Branch("L1_J10_EMPTY",&m_nt_L1_J10_EMPTY,"L1_J10_EMPTY/O");
  m_tree->Branch("L1_J30_FIRSTEMPTY",&m_nt_L1_J30_FIRSTEMPTY,"L1_J30_FIRSTEMPTY/O");
  m_tree->Branch("L1_J30_EMPTY",&m_nt_L1_J30_EMPTY,"L1_J30_EMPTY/O");
  m_tree->Branch("L1_XE40",&m_nt_L1_XE40,"L1_XE40/O");
  m_tree->Branch("L1_XE50",&m_nt_L1_XE50,"L1_XE50/O");
  m_tree->Branch("L1_XE50_BGRP7",&m_nt_L1_XE50_BGRP7,"L1_XE50_BGRP7/O");
  m_tree->Branch("L1_XE70",&m_nt_L1_XE70,"L1_XE70/O");
  
  m_tree->Branch("EF_j165_u0uchad_LArNoiseBurst",&m_nt_EF_j165_u0uchad_LArNoiseBurst,"EF_j165_u0uchad_LArNoiseBurst/O");
  m_tree->Branch("EF_j30_u0uchad_empty_LArNoiseBurst",&m_nt_EF_j30_u0uchad_empty_LArNoiseBurst,"EF_j30_u0uchad_empty_LArNoiseBurst/O");
  m_tree->Branch("EF_j55_u0uchad_firstempty_LArNoiseBurst",&m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurst,"EF_j55_u0uchad_firstempty_LArNoiseBurst/O");
  m_tree->Branch("EF_j55_u0uchad_empty_LArNoiseBurst",&m_nt_EF_j55_u0uchad_empty_LArNoiseBurst,"EF_j55_u0uchad_empty_LArNoiseBurst/O");
  m_tree->Branch("EF_xe45_LArNoiseBurst",&m_nt_EF_xe45_LArNoiseBurst,"EF_xe45_LArNoiseBurst/O");
  m_tree->Branch("EF_xe55_LArNoiseBurst",&m_nt_EF_xe55_LArNoiseBurst,"EF_xe55_LArNoiseBurst/O");
  m_tree->Branch("EF_xe60_LArNoiseBurst",&m_nt_EF_xe60_LArNoiseBurst,"EF_xe60_LArNoiseBurst/O");
  m_tree->Branch("EF_j55_u0uchad_firstempty_LArNoiseBurstT",&m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurstT,"EF_j55_u0uchad_firstempty_LArNoiseBurstT/O");
  m_tree->Branch("EF_j100_u0uchad_LArNoiseBurstT",&m_nt_EF_j100_u0uchad_LArNoiseBurstT,"EF_j100_u0uchad_LArNoiseBurstT/O");
  m_tree->Branch("EF_j165_u0uchad_LArNoiseBurstT",&m_nt_EF_j165_u0uchad_LArNoiseBurstT,"EF_j165_u0uchad_LArNoiseBurstT/O");
  m_tree->Branch("EF_j130_u0uchad_LArNoiseBurstT",&m_nt_EF_j130_u0uchad_LArNoiseBurstT,"EF_j130_u0uchad_LArNoiseBurstT/O");
  m_tree->Branch("EF_j35_u0uchad_empty_LArNoiseBurst",&m_nt_EF_j35_u0uchad_empty_LArNoiseBurst,"EF_j35_u0uchad_empty_LArNoiseBurst/O");
  m_tree->Branch("EF_j35_u0uchad_firstempty_LArNoiseBurst",&m_nt_EF_j35_u0uchad_firstempty_LArNoiseBurst,"EF_j35_u0uchad_firstempty_LArNoiseBurst/O");
  m_tree->Branch("EF_j80_u0uchad_LArNoiseBurstT",&m_nt_EF_j80_u0uchad_LArNoiseBurstT,"EF_j80_u0uchad_LArNoiseBurstT/O");
  
  // 
  m_tree->Branch("LArCellSize", &m_nt_larcellsize,"LArCellSize/I"); // NEW number of online conected LAr cells
  m_tree->Branch("CaloCellSize",&m_nt_cellsize,"CaloCellSize/I");// NEW number of total cells.
  // LAr time difference as computed by LArCollisionTime info
  m_tree->Branch("LArTime_Diff",&m_nt_ECTimeDiff,"LArTime_Diff/F"); // time diff between 2 endcaps
  m_tree->Branch("LArTime_Avg",&m_nt_ECTimeAvg,"LArTime_Avg/F"); // time average of 2 endcaps
  m_tree->Branch("LArTime_nCellA",&m_nt_nCellA,"LArTime_nCellA/I"); // nb of cells used to compute endcap A time
  m_tree->Branch("LArTime_nCellC",&m_nt_nCellC,"LArTime_nCellC/I"); // nb of cells used to compute endcap C time

  // Event properties related to yield of channels in 3 sigma tails 
  m_tree->Branch("PerCentNoisyCellPartition",&m_nt_noisycellpart); // Yield in each partition:0:embc 1:emba 2:emecc 3:emeca 4:fcalc 5:fcala 6:hecc 7:heca

  // LArNoisyRO output
  m_tree->Branch("LArNoisyRO_Std", &m_nt_larnoisyro,"LArNoisyRO_Std/S"); // standard flag (>5 FEB with more than 30 cells with q factor > 4000)
  m_tree->Branch("LArNoisyRO_Std_optimized", &m_nt_larnoisyro_opt,"LArNoisyRO_Std_optimized/S"); // standard flag with a double weight for critical FEB (>5 FEB with more than 30 cells with q factor > 4000)
  m_tree->Branch("LArNoisyRO_SatTight",&m_nt_larnoisyro_satTwo,"LArNoisyRO_SatTight/S"); // tight flag (> 20 cells with E>1000MeV and saturated q factor) 
  m_tree->Branch("LArNoisyRO_mnbTight", &m_nt_larnoisyro_mnbTight,"LArNoisyRO_mnbTight/S"); // MNB tight
  m_tree->Branch("LArNoisyRO_mnbTight_PSVeto", &m_nt_larnoisyro_mnbTight_PSVeto,"LArNoisyRO_mnbTight_PSVeto/S"); // MNB tight


  // Properties of cells with fabs(energy/noise)>3
  m_tree->Branch("NoisyCellPartitionLayerIndex",&m_nt_cellpartlayerindex); /// NEW Identifier of the cell
  m_tree->Branch("NoisyCellOnlineIdentifier",&m_nt_cellIdentifier); // Identifier of the noisy cell
  m_tree->Branch("NoisyCellPartition",&m_nt_partition); // Partition in 1 integer: 0:embc 1:emba 2:emecc 3:emeca 4:fcalc 5:fcala 6:hecc 7:heca
  m_tree->Branch("NoisyCellFT",&m_nt_ft_noisy);                        // FT 
  m_tree->Branch("NoisyCellSlot",&m_nt_slot_noisy);                    // Slot
  m_tree->Branch("NoisyCellChannel",&m_nt_channel_noisy);              // Channel
  m_tree->Branch("NoisyCellADCvalues", &m_nt_samples);                 // ADC values
  m_tree->Branch("NoisyCellGain",&m_nt_gain);                          // Gain
  m_tree->Branch("NoisyCellPhi",&m_nt_phicell);                        // Phi
  m_tree->Branch("NoisyCellEta",&m_nt_etacell);                        // Eta
  m_tree->Branch("NoisyCellLayer", &m_nt_layer);                       // layer
  m_tree->Branch("NoisyCellHVphi", &m_nt_noisycellHVphi);              // Phi of HV
  m_tree->Branch("NoisyCellHVeta", &m_nt_noisycellHVeta);              // Eta of HV
  m_tree->Branch("NoisyCellEnergy",&m_nt_energycell);                  // Energy
  m_tree->Branch("NoisyCellSignificance",&m_nt_signifcell);            // Significance (energy/noise)
  m_tree->Branch("NoisyCellQFactor",&m_nt_qfactorcell);                // Q factor
  m_tree->Branch("NoisyCellIsBad",&m_nt_isbadcell);                    // Bad channel status
  // Event properties related to q factor
  m_tree->Branch("nbLowQFactor", &m_lowqfactor,"m_lowqfactor/i"); // Nb of cells per event with q factor<1000
  m_tree->Branch("nbMedQFactor", &m_medqfactor,"m_medqfactor/i"); // Nb of cells per event with 1000<q factor<10000
  m_tree->Branch("nbHighQFactor", &m_hiqfactor,"m_hiqfactor/i");  // Nb of cells per event with q 10000<factor<65535
  m_tree->Branch("nbSatQFactor",&m_nb_sat,"nbSat/i"); // Nb of cells per event with saturated q factor (65535) 
  // Properties of cells with q factor saturated
  m_tree->Branch("SatCellPartition",&m_nt_partition_sat);
  m_tree->Branch("SatCellBarrelEc",&m_nt_barrelec_sat);
  m_tree->Branch("SatCellPosNeg",&m_nt_posneg_sat);
  m_tree->Branch("SatCellFT",&m_nt_ft_sat);
  m_tree->Branch("SatCellSlot",&m_nt_slot_sat);
  m_tree->Branch("SatCellChannel",&m_nt_channel_sat);
  m_tree->Branch("SatCellEnergy",&m_nt_energy_sat);
  m_tree->Branch("SatCellPhi", &m_nt_phicell_sat);
  m_tree->Branch("SatCellEta",&m_nt_etacell_sat);
  m_tree->Branch("SatCellLayer",&m_nt_layer_sat);
  m_tree->Branch("SatCellIsBad", &m_nt_isbadcell_sat);
  m_tree->Branch("SatCellOnlineIdentifier",&m_nt_cellIdentifier_sat);

  //HV information
  m_tree->Branch("HV_Nominal", &m_nt_nominalhv); //Not yet filled
  m_tree->Branch("HV_Maximal",&m_nt_maximalhv);  // Not yet filled

  ATH_MSG_DEBUG ( "End of Initializing LArNoiseBursts" );
 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode LArNoiseBursts::finalize() {
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
/// Clear - clear CBNT members
StatusCode LArNoiseBursts::clear() {
  /// For Athena-Aware NTuple

  ATH_MSG_DEBUG ( "start clearing variables " );
  
  m_nb_sat     = 0;
  n_noisycell  = 0;
  m_lowqfactor = 0;
  m_medqfactor = 0;
  m_hiqfactor  = 0;

  m_nt_run        = 0;
  m_nt_evtId      = 0;
  m_nt_evtTime    = 0;
  m_nt_evtTime_ns = 0;
  m_nt_lb         = 0;
  m_nt_bcid       = -1;
 
  m_nt_streamTagName.clear();
  m_nt_streamTagType.clear();
  m_nt_isbcidFilled     = -1;
  m_nt_isbcidInTrain    = -1;
  m_nt_isBunchesInFront.clear();
  m_nt_bunchtype        = -1;
  m_nt_bunchtime        = -1.0;
  m_nt_atlasready       = -1;
  m_nt_stablebeams      = -1;
  m_nt_larnoisyro       = -1;
  m_nt_larnoisyro_opt   = -1;
  m_nt_larnoisyro_satTwo= -1;
  m_nt_larnoisyro_mnbTight= -1;
  m_nt_larnoisyro_mnbTight_PSVeto= -1;

  ATH_MSG_DEBUG ( "clearing event info veto variables " );

  //clearing event info veto variables
//  m_nt_veto_mbts        = -1;
//  m_nt_veto_pixel       = -1;
//  m_nt_veto_sct         = -1;
//  m_nt_veto_bcm         = -1;
//  m_nt_veto_lucid       = -1;
//  m_nt_veto_mbtstdHalo  = -1;
//  m_nt_veto_mbtstdCol   = -1;
//  m_nt_veto_lartdHalo   = -1;
//  m_nt_veto_lartdCol    = -1;
//  m_nt_veto_csctdHalo   = -1;
//  m_nt_veto_csctdCol    = -1;
//  m_nt_veto_bcmtHalo    = -1;
//  m_nt_veto_bcmtCol     = -1;
//  m_nt_veto_muontCol    = -1;
//  m_nt_veto_muontCosmic = -1;
//
//  mLog << MSG::DEBUG << "clearing LAr event flags " << endreq;

  ATH_MSG_DEBUG ( "clearing LAr event flags " );

  // lar bit event info
  m_nt_larflag_noiseBurstVeto = false;
  m_nt_larflag_dataCorrupted = false;
  m_nt_larflag_dataCorruptedVeto = false;

  ATH_MSG_DEBUG ( "clearing Pixel variables " );

  // Trigger flags
  m_nt_L1_J75 = true; // turned on for tests
  m_nt_L1_J10_EMPTY = false;
  m_nt_L1_J30_FIRSTEMPTY = false;
  m_nt_L1_J30_EMPTY = false;
  m_nt_L1_XE40 = false;
  m_nt_L1_XE50 = false;
  m_nt_L1_XE50_BGRP7 = false;
  m_nt_L1_XE70 = false;
  
  m_nt_EF_j165_u0uchad_LArNoiseBurst = false;
  m_nt_EF_j30_u0uchad_empty_LArNoiseBurst = false;
  m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurst = false;
  m_nt_EF_j55_u0uchad_empty_LArNoiseBurst = false;
  m_nt_EF_xe45_LArNoiseBurst = false;
  m_nt_EF_xe55_LArNoiseBurst = false;
  m_nt_EF_xe60_LArNoiseBurst = false;
  m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurstT = false;
  m_nt_EF_j100_u0uchad_LArNoiseBurstT = false;
  m_nt_EF_j165_u0uchad_LArNoiseBurstT = false;
  m_nt_EF_j130_u0uchad_LArNoiseBurstT = false;
  m_nt_EF_j35_u0uchad_empty_LArNoiseBurst = false;
  m_nt_EF_j35_u0uchad_firstempty_LArNoiseBurst = false;
  m_nt_EF_j80_u0uchad_LArNoiseBurstT = false;
   
  ATH_MSG_DEBUG ( "clearing trigger flags " );

  ATH_MSG_DEBUG ( "clearing noisy cells variables " );

  //Quantities for noisy cells
  m_nt_energycell.clear();
  m_nt_qfactorcell.clear();
  m_nt_phicell.clear();
  m_nt_etacell.clear();
  m_nt_signifcell.clear();
  m_nt_isbadcell.clear();
  m_nt_partition.clear();
  m_nt_layer.clear();
  m_nt_ft_noisy.clear();
  m_nt_slot_noisy.clear();
  m_nt_channel_noisy.clear();
  m_nt_larcellsize = -1;
  m_nt_cellsize    = -1;
  m_nt_cellpartlayerindex.clear();
  m_nt_cellIdentifier.clear();
  m_nt_noisycellpart.clear();
  m_nt_samples.clear();
  m_nt_gain.clear();

  ///HV branches
  m_nt_nominalhv.clear();
  m_nt_maximalhv.clear();
  m_nt_noisycellHVphi.clear();
  m_nt_noisycellHVeta.clear();

  ATH_MSG_DEBUG ( "clearing saturated cell variables " );

  //clearing quantities for sat cells
  m_nt_barrelec_sat.clear();
  m_nt_posneg_sat.clear();
  m_nt_ft_sat.clear();
  m_nt_slot_sat.clear();
  m_nt_channel_sat.clear();
  m_nt_partition_sat.clear();
  m_nt_energy_sat.clear();
  m_nt_phicell_sat.clear();
  m_nt_etacell_sat.clear();
  m_nt_layer_sat.clear();
  m_nt_cellIdentifier_sat.clear();
  m_nt_isbadcell_sat.clear();

  ATH_MSG_DEBUG ( "clearing LArTimeDiff variables " );
  
  //DiffTime computed with LAR
  m_nt_ECTimeDiff  = 9999;
  m_nt_ECTimeAvg   = 9999;
  m_nt_nCellA      = -1;
  m_nt_nCellC      = -1;

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode LArNoiseBursts::execute() {
  ATH_MSG_DEBUG ( "in execute()" );
  StatusCode sc = clear();
  if(sc.isFailure()) {
    ATH_MSG_WARNING ( "The method clear() failed" );
    return StatusCode::SUCCESS;
  }
  
  if(!m_trigDec.empty()){

  sc = doTrigger();
  if(sc.isFailure()) {
    ATH_MSG_WARNING ( "The method doTrigger() failed" );
    return StatusCode::SUCCESS;
  }
  }

  sc = doEventProperties();
  if(sc.isFailure()) {
    ATH_MSG_WARNING ( "The method doEventProperties() failed" );
    return StatusCode::SUCCESS;
  }

  sc = doLArNoiseBursts();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "The method doLArNoiseBursts() failed" );
    return StatusCode::SUCCESS;
  }

  
  m_tree->Fill();

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////          doTrigger        ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
StatusCode LArNoiseBursts::doTrigger(){
  ATH_MSG_DEBUG ("in doTrigger ");
  
  std::string mychain( "L1_J75" );
  if( ! m_trigDec->getListOfTriggers(mychain).empty() ){
    m_nt_L1_J75 = m_trigDec->isPassed( mychain );
  }
  mychain = "L1_J10_EMPTY";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_L1_J10_EMPTY =  m_trigDec->isPassed( mychain );
  }
  mychain = "L1_J30_FIRSTEMPTY";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_L1_J30_FIRSTEMPTY =  m_trigDec->isPassed( mychain );
  }
  mychain = "L1_J30_EMPTY";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_L1_J30_EMPTY =  m_trigDec->isPassed( mychain );
  }
  mychain = "L1_XE40";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_L1_XE40 =  m_trigDec->isPassed( mychain );
  }
  mychain = "L1_XE50";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_L1_XE50 =  m_trigDec->isPassed( mychain );
  }
  mychain = "L1_XE50_BGRP7";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_L1_XE50_BGRP7 =  m_trigDec->isPassed( mychain );
  }
  mychain = "L1_XE70";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_L1_XE70 =  m_trigDec->isPassed( mychain );
  }

  // EF
  mychain = "EF_j165_u0uchad_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j165_u0uchad_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j30_u0uchad_empty_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j30_u0uchad_empty_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j55_u0uchad_firstempty_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j55_u0uchad_empty_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j55_u0uchad_empty_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_xe45_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_xe45_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_xe55_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_xe55_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_xe60_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_xe60_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j55_u0uchad_firstempty_LArNoiseBurstT";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j55_u0uchad_firstempty_LArNoiseBurstT =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j100_u0uchad_LArNoiseBurstT";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j100_u0uchad_LArNoiseBurstT =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j165_u0uchad_LArNoiseBurstT";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j165_u0uchad_LArNoiseBurstT =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j130_u0uchad_LArNoiseBurstT";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j130_u0uchad_LArNoiseBurstT =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j35_u0uchad_empty_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j35_u0uchad_empty_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j35_u0uchad_firstempty_LArNoiseBurst";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j35_u0uchad_firstempty_LArNoiseBurst =  m_trigDec->isPassed( mychain );
  }
  mychain = "EF_j80_u0uchad_LArNoiseBurstT";
  if( ! m_trigDec->getListOfTriggers(mychain).empty()){
    m_nt_EF_j80_u0uchad_LArNoiseBurstT =  m_trigDec->isPassed( mychain );
  }
 
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////          doEventProperties        //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
StatusCode LArNoiseBursts::doEventProperties(){
  ATH_MSG_DEBUG ("in doEventProperties ");


  //////////////////////////////// EventInfo variables /////////////////////////////////////////////////
  // Retrieve event info
  const EventInfo* eventInfo = nullptr;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );

  int run = eventInfo->runNumber();
  m_nt_run        = eventInfo->runNumber();
  m_nt_evtId      = eventInfo->eventNumber();
  m_nt_evtTime    = eventInfo->timeStamp();
  m_nt_evtTime_ns = eventInfo->timeStampNSOffset();
  m_nt_lb         = eventInfo->lumiBlock();
  m_nt_bcid       = eventInfo->bcid();

  ATH_MSG_DEBUG("Run Number: "<<run<<", event Id "<<m_nt_evtId<<", bcid = "<<m_nt_bcid);
  

  m_nt_isbcidFilled = m_bc_tool->isFilled(m_nt_bcid);
  m_nt_isbcidInTrain = m_bc_tool->isInTrain(m_nt_bcid);
  m_nt_bunchtype = m_bc_tool->bcType(m_nt_bcid);
  ATH_MSG_DEBUG("BCID is Filled: "<<m_nt_isbcidFilled);
  ATH_MSG_DEBUG("BCID is in Train: "<<m_nt_isbcidInTrain);
  ATH_MSG_DEBUG("bunch type "<<m_nt_bunchtype);

  std::vector<bool> isBunchesInFront = m_bc_tool->bunchesInFront(m_nt_bcid,m_frontbunches);
  bool checkfirstbunch = true;
  for(unsigned int i=0;i<isBunchesInFront.size();i++){
     ATH_MSG_DEBUG("bunch "<<i<<" is Filled "<<isBunchesInFront[i]);
     m_nt_isBunchesInFront.push_back(isBunchesInFront[i]);
       if(isBunchesInFront[i]==1){
         if(i!=0){
           if(checkfirstbunch){
             float time = 25.0*i;
             m_nt_bunchtime = time;
             ATH_MSG_DEBUG("next bunch time: "<<time<<" ns ");
             checkfirstbunch = false;
	   }
	 }
     }
  }

  m_CosmicCaloStream = false;
  for (const auto& streamInfo : eventInfo->streamTags()) {
    const std::string& stream_name = streamInfo.name();
    const std::string& stream_type = streamInfo.type();
    m_nt_streamTagName.push_back(stream_name);
    m_nt_streamTagType.push_back(stream_type);
    ATH_MSG_DEBUG("event stream tag name "<<streamInfo.name());
    ATH_MSG_DEBUG("event stream tag type "<<streamInfo.type());
    if(streamInfo.name()=="CosmicCalo" && streamInfo.type()=="physics"){
      m_CosmicCaloStream = true;
    }
  }
  

  ATH_MSG_DEBUG("CosmicCalo stream value: "<<m_CosmicCaloStream);

  // Retrieve output of LArNoisyRO
  bool larnoisyro = eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::BADFEBS);
  bool larnoisyro_opt =eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::BADFEBS_W);
  bool larnoisyro_satTwo = eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ);
  bool larnoisyro_mnbTight = eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT);
  bool larnoisyro_mnbTight_PSVeto = eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT_PSVETO);

  m_nt_larnoisyro        = larnoisyro ? 1 : 0;
  m_nt_larnoisyro_opt    = larnoisyro_opt ? 1 : 0;
  m_nt_larnoisyro_satTwo = larnoisyro_satTwo ? 1 : 0;
  m_nt_larnoisyro_mnbTight = larnoisyro_mnbTight ? 1 : 0;
  m_nt_larnoisyro_mnbTight_PSVeto = larnoisyro_mnbTight_PSVeto ? 1 : 0;
  
 // Retrieve output of EventInfo veto - COMMENTED NOW TO MAKE IT COMPLIANT WITH xAOD::EventInfo
//  mLog << MSG::DEBUG <<"Background: MBTSBeamVeto "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSBeamVeto)<<endreq;
//  mLog << MSG::DEBUG <<"Background: PixSPNonEmpty "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::PixSPNonEmpty)<<endreq;
//  mLog << MSG::DEBUG <<"Background: SCTSPNonEmpty "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTSPNonEmpty)<<endreq;
//  mLog << MSG::DEBUG <<"Background: BCMBeamVeto "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMBeamVeto)<<endreq;
//  mLog << MSG::DEBUG <<"Background: LUCIDBeamVeto "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LUCIDBeamVeto)<<endreq;
//  mLog << MSG::DEBUG <<"Background: MBTSTimeDiffHalo "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffHalo)<<endreq; 
//  mLog << MSG::DEBUG <<"Background: MBTSTimeDiffCol "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffCol)<<endreq;
//  mLog << MSG::DEBUG <<"Background: LArECTimeDiffHalo "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffHalo)<<endreq;
//  mLog << MSG::DEBUG <<"Background: LArECTimeDiffCol "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffCol)<<endreq;  
//  mLog << MSG::DEBUG <<"Background: CSCTimeDiffHalo "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffHalo)<<endreq;
//  mLog << MSG::DEBUG <<"Background: CSCTimeDiffCol "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffCol)<<endreq;
//  mLog << MSG::DEBUG <<"Background: BCMTimeDiffHalo "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffHalo)<<endreq;
//  mLog << MSG::DEBUG <<"Background: BCMTimeDiffCol "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffCol)<<endreq;
//  mLog << MSG::DEBUG <<"Background: MuonTimmingCol "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCol)<<endreq;
//  mLog << MSG::DEBUG <<"Background: MuonTimmingCosmic "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCosmic)<<endreq;
//
//  m_nt_veto_mbts      = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSBeamVeto);
//  m_nt_veto_pixel     = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::PixSPNonEmpty);
//  m_nt_veto_sct       = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTSPNonEmpty);
//  m_nt_veto_bcm       = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMBeamVeto);
//  m_nt_veto_lucid     = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LUCIDBeamVeto);
//
//  //more variables
//  m_nt_veto_mbtstdHalo = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffHalo); 
//  m_nt_veto_mbtstdCol  = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffCol);
//  m_nt_veto_lartdHalo  = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffHalo);
//  m_nt_veto_lartdCol   = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffCol);  
//  m_nt_veto_csctdHalo  = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffHalo);
//  m_nt_veto_csctdCol   = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffCol);
//  m_nt_veto_bcmtHalo   = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffHalo);
//  m_nt_veto_bcmtCol    = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffCol);
//  m_nt_veto_muontCol   = eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCol);
//  m_nt_veto_muontCosmic= eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCosmic);
 
   // LArEventInfo

  ATH_MSG_DEBUG ("NOISEBURSTVETO bit " << eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO) );
  ATH_MSG_DEBUG ("BADFEBS bit " << eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::BADFEBS));
  ATH_MSG_DEBUG ("TIGHTSATURATEDQ bit " << eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ));

  m_nt_larflag_noiseBurstVeto = eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO);
  m_nt_larflag_dataCorrupted = eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::DATACORRUPTED);
  m_nt_larflag_dataCorruptedVeto = eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::DATACORRUPTEDVETO);

  ///////////////////////////////////////end EventInfo variables/////////////////////////////////////////////////////////////////////////

  //const AthenaAttributeList* attrList(0);
  //ATH_CHECK( evtStore()->retrieve(attrList, "/TDAQ/RunCtrl/DataTakingMode") );
  //if (attrList != 0) {
  //  ATH_MSG_DEBUG ("ReadyForPhysics is: " << (*attrList)["ReadyForPhysics"].data<uint32_t>());
  //  //m_valueCache = ((*attrList)["ReadyForPhysics"].data<uint32_t>() != 0);
  //  m_nt_atlasready = (*attrList)["ReadyForPhysics"].data<uint32_t>();
  // }


  // Retrieve LArCollision Timing information
  const LArCollisionTime *  larTime=0;
  if (evtStore()->contains<LArCollisionTime>("LArCollisionTime")) {
     StatusCode sc =  evtStore()->retrieve(larTime,"LArCollisionTime");
     if( sc.isFailure()){
       ATH_MSG_WARNING  ( "Unable to retrieve LArCollisionTime event store" );
       //return StatusCode::SUCCESS; // Check if failure shd be returned. VB
     }else {
       ATH_MSG_DEBUG  ( "LArCollisionTime successfully retrieved from event store" );
     }
  }
  
  if (larTime) {
      m_nt_nCellA = larTime->ncellA();
      m_nt_nCellC = larTime->ncellC();
    if(m_nt_nCellA>0 && m_nt_nCellC>0){
      // Calculate the time diff between ECC and ECA
      m_nt_ECTimeDiff = larTime->timeC() - larTime->timeA();
      m_nt_ECTimeAvg  = (larTime->timeC() + larTime->timeA()) * 0.5;
      ATH_MSG_DEBUG  ( "LAr: Calculated time difference of " << m_nt_ECTimeDiff << " ns" );
    }
  }

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////          doLArNoiseBursts        ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
StatusCode LArNoiseBursts::doLArNoiseBursts(){
  ATH_MSG_DEBUG ( "in doLarCellInfo " );


 // Retrieve LAr calocells container
 // or LArRawChannel container, whatsever available...
  const CaloCellContainer* caloTES=0;
  const LArRawChannelContainer* LArTES=0;
  const LArRawChannelContainer* LArTES_dig=0;
  if(evtStore()->contains<CaloCellContainer>("AllCalo")) {
     ATH_CHECK(evtStore()->retrieve( caloTES, "AllCalo"));
  } else if (evtStore()->contains<LArRawChannelContainer>("LArRawChannels") || evtStore()->contains<LArRawChannelContainer>("LArRawChannels_fB")) {
         if (evtStore()->contains<LArRawChannelContainer>("LArRawChannels")) {
            ATH_CHECK(evtStore()->retrieve( LArTES_dig, "LArRawChannels"));
         }
         if (evtStore()->contains<LArRawChannelContainer>("LArRawChannels_fB")) {
            ATH_CHECK(evtStore()->retrieve( LArTES, "LArRawChannels_fB"));
         }
  } else {
    ATH_MSG_WARNING ( "CaloCell or LArRawChannel Containers not found!" );
    return StatusCode::SUCCESS;
  }

  m_nb_sat = 0;
  n_noisycell =0;
  nlarcell = 0;
  m_ft_noisy.clear();m_slot_noisy.clear();m_channel_noisy.clear();
  m_isbarrel.clear();m_isendcap.clear();m_isfcal.clear();m_ishec.clear();
  m_layer.clear();m_partition.clear();m_energycell.clear();m_qfactorcell.clear(); 
  m_phicell.clear();m_etacell.clear();m_signifcell.clear();m_isbadcell.clear();
  m_IdHash.clear();m_noisycellHVeta.clear();m_noisycellHVphi.clear();
  m_cellpartlayerindex.clear();m_cellIdentifier.clear();

  float eCalo;
  float qfactor;
  CaloGain::CaloGain gain;

  if(caloTES) {
     CaloCellContainer::const_iterator caloItr;
     for(caloItr=caloTES->begin();caloItr!=caloTES->end();caloItr++){
       const CaloDetDescrElement* caloDDE = (*caloItr)->caloDDE();
       if (caloDDE->is_tile())continue;
       HWIdentifier onlID;
       try {onlID = m_LArCablingService->createSignalChannelID((*caloItr)->ID());}
       catch(LArID_Exception& except) {
         ATH_MSG_ERROR  ( "LArID_Exception " << m_LArEM_IDHelper->show_to_string((*caloItr)->ID()) << " " << (std::string) except );
         ATH_MSG_ERROR  ( "LArID_Exception " << m_LArHEC_IDHelper->show_to_string((*caloItr)->ID()) );
         ATH_MSG_ERROR  ( "LArID_Exception " << m_LArFCAL_IDHelper->show_to_string((*caloItr)->ID()) );
         continue;
       }
       bool connected = m_LArCablingService->isOnlineConnected(onlID);
       if(!connected) continue;
       eCalo = (*caloItr)->energy();
       qfactor = (*caloItr)->quality();
       gain = (*caloItr)->gain();
       //if(qfactor > 0. || (*caloItr)->ID() == Identifier((IDENTIFIER_TYPE)0x33c9500000000000) ) ATH_MSG_DEBUG((*caloItr)->ID()<<" : "<<eCalo<<" "<<qfactor<<" "<<gain<<" prov.: "<<(*caloItr)->provenance());
       ATH_CHECK(fillCell(onlID, eCalo, qfactor, gain));
     }//loop over cells
     ATH_MSG_DEBUG("Done cells "<<nlarcell);
  } else {
     std::vector<HWIdentifier> chdone; 
     if (LArTES_dig) {
       LArRawChannelContainer::const_iterator caloItr;
       for(caloItr=LArTES_dig->begin();caloItr!=LArTES_dig->end();caloItr++){
         HWIdentifier onlID=caloItr->identify();
         bool connected = m_LArCablingService->isOnlineConnected(onlID);
         if(!connected) continue;
         eCalo = caloItr->energy();
         qfactor = caloItr->quality();
         gain = caloItr->gain();
         ATH_CHECK(fillCell(onlID, eCalo, qfactor, gain));
         chdone.push_back(onlID);
       }//loop over raw channels
     }  
     ATH_MSG_DEBUG("Done raw chan. from digi "<<nlarcell);
     // dirty hack, if we are already complete:
     if (nlarcell < 182468 && LArTES) { // add those raw channels, which were not from dig.
       LArRawChannelContainer::const_iterator caloItr;
       for(caloItr=LArTES->begin();caloItr!=LArTES->end();caloItr++){
         HWIdentifier onlID=caloItr->identify();
         if(std::find(chdone.begin(), chdone.end(), onlID) != chdone.end()) continue;
         bool connected = m_LArCablingService->isOnlineConnected(onlID);
         if(!connected) continue;
         eCalo = caloItr->energy();
         qfactor = caloItr->quality();
         gain = caloItr->gain();
         ATH_CHECK(fillCell(onlID, eCalo, qfactor, gain));
       }//loop over raw channels
     }  
     ATH_MSG_DEBUG("Done raw chan. "<<nlarcell);
  } // if raw chanels
  
  m_nt_larcellsize = nlarcell;
  if(caloTES) {
    m_nt_cellsize    = caloTES->size();
  } else {
     m_nt_cellsize = LArTES->size();
  }
  ATH_MSG_INFO ("lar cell size = "<<int(nlarcell));
  if(caloTES) ATH_MSG_INFO ("all cell size = "<<int(caloTES->size()));
  else {
     if(LArTES_dig) ATH_MSG_INFO ("all LArRawCh. from digi size = "<<int(LArTES_dig->size()));
     if(LArTES) ATH_MSG_INFO ("all LArRawCh. size = "<<int(LArTES->size()));
  }

  bool checknoise = false;
  //ratio of noisy cells per partition
  for(unsigned int i=0;i<8;i++){
    float noise  =  100.0*(double(n_noisy_cell_part[i])/double(n_cell_part[i]));
    m_nt_noisycellpart.push_back(noise);
    if(noise> 1.0){
      checknoise = true;
      ATH_MSG_DEBUG ("noise burst in this  event ");
    }   
  }

  const LArDigitContainer* LArDigitCont=0;
  if (evtStore()->contains<LArDigitContainer>("FREE")) ATH_CHECK(evtStore()->retrieve( LArDigitCont, "FREE"));
  if(!LArDigitCont) {
     if (evtStore()->contains<LArDigitContainer>("LArDigitContainer_Thinned")) ATH_CHECK(evtStore()->retrieve( LArDigitCont, "LArDigitContainer_Thinned"));
  }
  if (!LArDigitCont) {
    ATH_MSG_WARNING ( "LArDigitContainer Container not found!" );
    return StatusCode::SUCCESS;
  }    

  LArDigitContainer::const_iterator it=LArDigitCont->begin();
  LArDigitContainer::const_iterator it_end=LArDigitCont->end();
  const LArDigit* pLArDigit;
  
  bool store_condition = false;
  // CosmicCalo stream : Store detailed infos of cells only if Y3Sigma>1% or burst found by LArNoisyRO
  if(m_CosmicCaloStream){
    if(checknoise==true || m_nt_larnoisyro==1 || m_nt_larnoisyro_satTwo==1 || m_nt_larnoisyro_mnbTight || m_nt_larnoisyro_mnbTight_PSVeto){
      store_condition = true;
    }
  }
  // Not cosmicCalo stream : Store detailed infos of cells only if burst found by LArNoisyRO
  if(!m_CosmicCaloStream){
    if(m_nt_larnoisyro==1 || m_nt_larnoisyro_satTwo==1 || m_nt_larnoisyro_mnbTight || m_nt_larnoisyro_mnbTight_PSVeto){
      store_condition = true;
    }
  }

  //store the information of the noisy cell
  if(store_condition){
    std::vector<short> samples;
    samples.clear();
    for(unsigned int k=0;k<m_etacell.size();k++){
      m_nt_samples.push_back(samples);
      m_nt_gain.push_back(0);
    }
    for (;it!=it_end;it++) {
         pLArDigit = *it;  
         HWIdentifier id2 = pLArDigit->hardwareID();
         IdentifierHash hashid2 = m_LArOnlineIDHelper->channel_Hash(id2);
          for(unsigned int j=0;j<m_IdHash.size();j++){
            if (hashid2 == m_IdHash[j] ){
              ATH_MSG_DEBUG ( "find a IdentifierHash of the noisy cell in LArDigit container " );
              samples = pLArDigit->samples();
              int gain=-1;
              if (pLArDigit->gain() == CaloGain::LARHIGHGAIN)   gain = 0;
              if (pLArDigit->gain() == CaloGain::LARMEDIUMGAIN) gain = 1;
              if (pLArDigit->gain() == CaloGain::LARLOWGAIN)    gain = 2;
              m_nt_gain.at(j)= gain;
              m_nt_samples.at(j) = samples;
              ATH_MSG_DEBUG ( "I got the samples!" );
              break;
	    }  
	 }
     }
    for(unsigned int i=0;i<m_etacell.size();i++){
      m_nt_energycell.push_back( m_energycell[i]);
      m_nt_qfactorcell.push_back( m_qfactorcell[i]);
      m_nt_signifcell.push_back( m_signifcell[i]);
      m_nt_partition.push_back( m_partition[i]);   
      m_nt_cellIdentifier.push_back(m_cellIdentifier[i].get_identifier32().get_compact());
      if(!m_keepOnlyCellID){
        m_nt_ft_noisy.push_back( m_ft_noisy[i]);
        m_nt_slot_noisy.push_back( m_slot_noisy[i]);
        m_nt_channel_noisy.push_back( m_channel_noisy[i]);
      
        m_nt_layer.push_back( m_layer[i]);
        m_nt_phicell.push_back( m_phicell[i]);
        m_nt_etacell.push_back( m_etacell[i]);
        m_nt_isbadcell.push_back( m_isbadcell[i]);
        m_nt_noisycellHVphi.push_back(m_noisycellHVphi[i]);     
        m_nt_noisycellHVeta.push_back(m_noisycellHVeta[i]);
        m_nt_cellpartlayerindex.push_back(m_cellpartlayerindex[i]);
      }
    }		  
    ATH_MSG_DEBUG ("leaving if checknoise and larnoisyro");

  }//if(checknoisy==true ..)

  return StatusCode::SUCCESS;
  
}

StatusCode LArNoiseBursts::fillCell(HWIdentifier onlID, float eCalo, float qfactor, CaloGain::CaloGain gain){
    const Identifier idd = m_LArCablingService->cnvToIdentifier(onlID);
    nlarcell++;
    IdentifierHash channelHash = m_LArOnlineIDHelper->channel_Hash(onlID);
    CaloDetDescrElement *caloDDE = m_calodetdescrmgr->get_element(idd);
    int layer = caloDDE->getLayer();
    float phi = caloDDE->phi();
    float noise  = m_calo_noise_tool->totalNoiseRMS( caloDDE, gain);
    float significance = eCalo / noise ;
    float eta = caloDDE->eta();
    bool badcell = ! (m_badchan_tool->status(onlID)).good();
    unsigned int partition = 0;
    bool is_lar_em_barrel = caloDDE->is_lar_em_barrel();
    if(is_lar_em_barrel){
       if(eta<0){
          partition = 0;
        }else{
          partition = 1;
        }
    }//barrel
    bool is_lar_em_endcap = caloDDE->is_lar_em_endcap();
    if(is_lar_em_endcap){
        if(eta<0){
          partition = 2;
        }else{
          partition = 3;
	}
    }//endcap
    bool is_lar_fcal   = caloDDE->is_lar_fcal();
    if(is_lar_fcal){
        if(eta<0){
          partition = 4;
        }else{
          partition = 5;
	}
    }//fcal
    bool is_lar_hec    = caloDDE->is_lar_hec();
    if(is_lar_hec){
      if(eta<0){
        partition = 6;
      }else{
        partition = 7;
      }
    }//hec
    for(unsigned int k=0;k<8;k++){
       if(partition==k){
          n_cell_part[k]++;
       }
    }
    if(qfactor <1000)                  {m_lowqfactor++;}
    if(qfactor>=1000 && qfactor<10000) {m_medqfactor++;}
    if(qfactor>=10000 && qfactor<65535){m_hiqfactor++;}
    if(qfactor==65535){
      ATH_MSG_DEBUG ("Satured cell at eta: "<<eta<<", phi: "<<phi<<", energy: "<<eCalo<<", partition: "<<partition);
       m_nb_sat = m_nb_sat +1;
       m_nt_partition_sat.push_back(partition);
       m_nt_energy_sat.push_back(eCalo);
       m_nt_cellIdentifier_sat.push_back((m_LArCablingService->cnvToIdentifier(onlID)).get_identifier32().get_compact());
       if(!m_keepOnlyCellID){
         m_nt_barrelec_sat.push_back(m_LArOnlineIDHelper->barrel_ec(onlID));
         m_nt_posneg_sat.push_back(m_LArOnlineIDHelper->pos_neg(onlID));
         m_nt_ft_sat.push_back(m_LArOnlineIDHelper->feedthrough(onlID));
         m_nt_slot_sat.push_back(m_LArOnlineIDHelper->slot(onlID));
         m_nt_channel_sat.push_back(m_LArOnlineIDHelper->channel(onlID));
         m_nt_phicell_sat.push_back(phi);
         m_nt_etacell_sat.push_back(eta);
         m_nt_layer_sat.push_back(layer);
         m_nt_isbadcell_sat.push_back(badcell);
       }
    }
    // Store all cells in positive and negative 3 sigma tails...
    if(significance > m_sigmacut || qfactor > 4000){
      m_ft_noisy.push_back(m_LArOnlineIDHelper->feedthrough(onlID));
      m_slot_noisy.push_back(m_LArOnlineIDHelper->slot(onlID));
      m_channel_noisy.push_back(m_LArOnlineIDHelper->channel(onlID));

      m_layer.push_back(layer);
      m_energycell.push_back(eCalo);
      m_qfactorcell.push_back(qfactor);
      m_phicell.push_back(phi);
      m_etacell.push_back(eta);
      m_signifcell.push_back(significance);
      m_isbadcell.push_back(badcell);
      m_partition.push_back(partition);
      m_IdHash.push_back(channelHash);
      m_cellIdentifier.push_back(m_LArCablingService->cnvToIdentifier(onlID));
    // ...but count only cells in positive 3 sigma tails!
      if (significance > m_sigmacut){
	n_noisycell++;
        ATH_MSG_DEBUG ("Noisy cell: "<<n_noisycell<<" "<<m_LArCablingService->cnvToIdentifier(onlID)<<" q: "<<qfactor<<" sign.: "<<significance<<" noise: "<<noise);
	for(unsigned int k=0;k<8;k++){
	  if(partition==k){
	    n_noisy_cell_part[k]++;
	  }
	}
      }
      int caloindex = GetPartitionLayerIndex(idd);
      m_cellpartlayerindex.push_back(caloindex);
      //store HV information
      /*std::vector<int> * hvid = new std::vector<int>;
      hvid.clear();
      //hvid = GetHVLines(idd);
      std::vector<std::pair<LArHVCellID, int> > hv_vector;
      hv_vector.clear();
      m_LArHVCablingTool->getLArHVCellID(idd,hv_vector);
      if(hv_vector.size()>0){
        for(unsigned int i=0;i<hv_vector.size();i++){
	  int nominal_HV =  m_LArHVCablingTool->getNominalHV(hv_vector[i].first);//fill the branch but produces lots of warnings. 
	  int maximal_HV =  m_LArHVCablingTool->getMaximalHV(hv_vector[i].first);//fill the branch but produces lots of warnings.
	    m_nt_nominalhv.push_back(nominal_HV);
            m_nt_maximalhv.push_back(maximal_HV);
	}
	}*/
      m_noisycellHVphi.push_back(m_LArElectrodeIDHelper->hv_phi(onlID));
      m_noisycellHVeta.push_back(m_LArElectrodeIDHelper->hv_eta(onlID));
      
    }   
    return StatusCode::SUCCESS;
}    
//////////////////////////////////////////////////////////////////////////////////////
///////////////////          doPhysicsObjects        ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
StatusCode LArNoiseBursts::doPhysicsObjects(){
  ATH_MSG_DEBUG ("in doPhysicsObjects ");

  return StatusCode::SUCCESS;
}


//

int LArNoiseBursts::GetPartitionLayerIndex(const Identifier& id)
{
  // O.Simard -- GetPartitionLayer index [0,32]:
  // Returns a single integer code that corresponds to the
  // mapping of CaloCells in CaloMonitoring histogram frames.

  int caloindex = -1;
  int bc = 0;
  int sampling = 0;

  if(m_LArEM_IDHelper->is_lar_em(id)){ // EMB
    bc = m_LArEM_IDHelper->barrel_ec(id);
    sampling = m_LArEM_IDHelper->sampling(id);
    if(abs(bc)==1){
      if(bc<0) caloindex = sampling+4;
      else caloindex = sampling;
    } else { // EMEC
      if(bc<0) caloindex = sampling+12;
      else caloindex = sampling+8;
    }
  } else if(m_LArHEC_IDHelper->is_lar_hec(id)) { // LAr HEC
    bc = m_LArHEC_IDHelper->pos_neg(id);
    sampling = m_LArHEC_IDHelper->sampling(id);
    if(bc<0) caloindex = sampling+20;
    else caloindex = sampling+16;
  } else if(m_LArFCAL_IDHelper->is_lar_fcal(id)) { // LAr FCAL
    bc = m_LArFCAL_IDHelper->pos_neg(id);
    sampling = (int)m_LArFCAL_IDHelper->module(id); // module instead of sampling
    if(bc<0) caloindex = sampling+26;
    else caloindex = sampling+23;
  }

  return caloindex;
}

std::vector<int>* LArNoiseBursts::GetHVLines(const Identifier& id)
{
  ATH_MSG_DEBUG ( "in GetHVLines function " );

  std::vector<int>  tmplines;
  unsigned int nelec = 0;
  //unsigned int ngap = 0;
  unsigned int nsubgaps = 0;
  unsigned int nlines = 0;
  unsigned int i=0,j=0,igap=0;

  // LAr EMB
  if(m_LArEM_IDHelper->is_lar_em(id) && m_LArEM_IDHelper->sampling(id)>0){
    ATH_MSG_DEBUG ( "LAr EMB");
    if(abs(m_LArEM_IDHelper->barrel_ec(id))==1) {
      const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!embElement)
        return 0;
      const EMBCellConstLink cell = embElement->getEMBCell();
      nelec = cell->getNumElectrodes();
      //ngap = 2*nelec;
      for(i=0;i<nelec;i++) {
	      const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
	      for(igap=0;igap<2;igap++) tmplines.push_back(electrode->hvLineNo(igap));
      }        
    } else { // LAr EMEC
      ATH_MSG_DEBUG ( "LAr EMEC");
      const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!emecElement)
        return 0;
      const EMECCellConstLink cell = emecElement->getEMECCell();
      nelec = cell->getNumElectrodes();
      //ngap = 2*nelec;
      for(i=0;i<nelec;i++) {
	      const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
	      for(igap=0;igap<2;igap++) tmplines.push_back(electrode->hvLineNo(igap));
      }      
    }
  } else if(m_LArHEC_IDHelper->is_lar_hec(id)) { // LAr HEC
    ATH_MSG_DEBUG ( "LAr HEC");
    const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (!hecElement)
      return 0;
    const HECCellConstLink cell = hecElement->getHECCell();
    nsubgaps = cell->getNumSubgaps();
    for(i=0;i<nsubgaps;i++) {
      const HECHVSubgapConstLink subgap = cell->getSubgap(i);
      tmplines.push_back(subgap->hvLineNo());
    }
  } else if(m_LArFCAL_IDHelper->is_lar_fcal(id)) { // LAr FCAL
    ATH_MSG_DEBUG ( "LAr HEC");
    const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (!fcalElement)
      return 0;
    const FCALTile* tile = fcalElement->getFCALTile();
    nlines = tile->getNumHVLines();
    for(i=0;i<nlines;i++) {
      const FCALHVLineConstLink line = tile->getHVLine(i);
      if(line) tmplines.push_back(line->hvLineNo());
    }   
  } else if(m_LArEM_IDHelper->is_lar_em(id) && m_LArEM_IDHelper->sampling(id)==0) { // Presamplers
    ATH_MSG_DEBUG ( "LAr PRESAMPLES");
    
    if(abs(m_LArEM_IDHelper->barrel_ec(id))==1){
      
      const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!embElement) {
        return 0;
      }
      const EMBCellConstLink cell = embElement->getEMBCell();
      const EMBPresamplerHVModuleConstLink hvmodule =  cell->getPresamplerHVModule();
      for(igap=0;igap<2;igap++) tmplines.push_back(hvmodule->hvLineNo(igap));

    } else {
      
      const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!emecElement)
        return 0;
      const EMECCellConstLink cell = emecElement->getEMECCell();
      const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule();
      for(igap=0;igap<2;igap++) tmplines.push_back(hvmodule->hvLineNo(igap));

    }
  } else {
    ATH_MSG_WARNING ( " cell neither in EM nor HEC nor FCAL !!!!!  return empty HV " );
    return (vector<int>*)0;
  }

  // optimization of storage: replace by std:set
  std::vector<int>* hvlines = new std::vector<int>;
  bool found = 0;
  for(i=0;i<tmplines.size();i++){
    found = 0;
    for(j=0;j<hvlines->size();j++){
      if(hvlines->at(j)==tmplines.at(i)){
        found=1;
        break;
      }
    }
    if(!found) hvlines->push_back(tmplines.at(i));
  }

  return hvlines;
}


 
