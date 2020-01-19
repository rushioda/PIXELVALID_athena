/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleAmbiguityProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkTrack/TrackInfo.h"
#include <map>
#include <ext/functional>
#include <iterator>
#ifdef SIMPLEAMBIGPROCNTUPLECODE
#include "xAODEventInfo/EventInfo.h"
#include "HepPDT/ParticleDataTable.hh"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GaudiKernel/ITHistSvc.h" 
#include "TTree.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#endif
#ifdef SIMPLEAMBIGPROCDEBUGCODE
#include "TrkTruthData/TrackTruthCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "HepMC/GenVertex.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "GeneratorObjects/McEventCollection.h"
#endif

//==================================================================================================
Trk::SimpleAmbiguityProcessorTool::SimpleAmbiguityProcessorTool(const std::string& t, 
								const std::string& n,
								const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_particleHypothesis{undefined},
  m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
  m_fitterTool ("Trk::KalmanFitter/InDetTrackFitter"), 
  m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"),
  m_finalTracks(0),
  m_Nevents(0),
  m_Ncandidates(5), m_NcandScoreZero(5), m_NcandDouble(5),
  m_NscoreOk(5),m_NscoreZeroBremRefit(5),m_NscoreZeroBremRefitFailed(5),
  m_NscoreZeroBremRefitScoreZero(5),m_NscoreZero(5),
  m_Naccepted(5),m_NsubTrack(5),m_NnoSubTrack(5),m_NacceptedBrem(5),
  m_NbremFits(5),m_Nfits(5),m_NrecoveryBremFits(5),m_NgoodFits(5),m_NfailedFits(5)
#ifdef SIMPLEAMBIGPROCNTUPLECODE
  ,m_particlePropSvc("PartPropSvc",n),
  m_particleDataTable(0),
  m_validationTreeName("AmbiguityProcessorTool"+n),
  m_validationTreeDescription("Validation of the AmbiguityProcessor "+n),
  m_validationTreeFolder("/val/AmbiProcessor"+n),
  m_validationTree(0),
  m_event(0),
  m_trackSeedMap(0),
  m_trackSeedMapLocation("TrackSeedMap"),
  m_trackTrackMap(0)
#endif
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  ,m_truthToTrack(0)//the comma in front of m_truthToTrack is necessary  
#endif
{
  // statitics stuff
  m_etabounds.push_back(0.8);
  m_etabounds.push_back(1.6);
  m_etabounds.push_back(2.5);
  m_etabounds.push_back(2.5);
  m_etabounds.push_back(10.0);


  declareInterface<ITrackAmbiguityProcessorTool>(this);
  declareProperty("DropDouble"           , m_dropDouble         = true);
  declareProperty("ForceRefit"           , m_forceRefit         = true);
  declareProperty("RefitPrds"            , m_refitPrds          = false);
  declareProperty("MatEffects"           , m_matEffects         = 3); // pion
  declareProperty("ScoringTool"          , m_scoringTool);
  declareProperty("SelectionTool"        , m_selectionTool);
  declareProperty("Fitter"               , m_fitterTool );
  declareProperty("SuppressHoleSearch"   , m_suppressHoleSearch = false);
  declareProperty("SuppressTrackFit"     , m_suppressTrackFit   = false);
  declareProperty("tryBremFit"           , m_tryBremFit         = false);
  declareProperty("caloSeededBrem"       , m_caloSeededBrem     = false);
  declareProperty("pTminBrem"            , m_pTminBrem          = 1000.);
  declareProperty("etaBounds"            , m_etabounds,"eta intervals for internal monitoring");

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  declareProperty("IsBackTracking"       , m_isBackTracking     = false);
  declareProperty("TruthLocationTRT"       , m_truth_locationTRT     );  
  declareProperty("ResolvedTrackConnection", m_resolvedTrackConnection = "SiSPS_ResolvedTrackConnection");
  declareProperty("TruthCollection"        , m_truthCollection = "SiSPSeededTracksTruthCollection");
  m_truth_locationTRT      = "PRD_MultiTruthTRT"              ;
  //to get brem truth
  m_generatedEventCollectionName = "TruthEvent"                ;
#endif

#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
  declareProperty("TruthLocationSCT"       ,m_truth_locationSCT     );
  declareProperty("TruthLocationPixel"     ,m_truth_locationPixel   );
  m_truth_locationPixel    = "PRD_MultiTruthPixel"            ;
  m_truth_locationSCT      = "PRD_MultiTruthSCT"              ;
#endif


}
//==================================================================================================

Trk::SimpleAmbiguityProcessorTool::~SimpleAmbiguityProcessorTool()
{
}
//==================================================================================================

StatusCode Trk::SimpleAmbiguityProcessorTool::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "AlgTool::initialise failed" << endmsg;
      return StatusCode::FAILURE;
    }
  
  sc = m_scoringTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_scoringTool << endmsg;
      return StatusCode::FAILURE;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_scoringTool << endmsg;
  
  sc = m_selectionTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_selectionTool << endmsg;
      return StatusCode::FAILURE;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_selectionTool << endmsg;
  
  sc = m_fitterTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_fitterTool << endmsg;
      return sc;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_fitterTool << endmsg;
  
  // suppress refit overwrites force refit
  if (m_forceRefit && m_suppressTrackFit ) 
    {
      msg(MSG::WARNING) << "Inconsistent parameter settings, forced refit is true, but fitting suppressed, resetingt force refit !" << endmsg;
      m_forceRefit = false;
    }

  // Print out memo that tracks have to be fitted
  if (!m_forceRefit) 
    {
        msg(MSG::INFO) << "The forced refit of Tracks is switched off." << endmsg;
        msg(MSG::INFO) << "Ensure, that the tracks are fitted after the ambiguity processing!" << endmsg;
    }

  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];

  // brem fitting enabled ?
  if (m_tryBremFit)
    msg(MSG::INFO) << "Try brem fit and recovery for electron like tracks." << endmsg;

  // statistics
  for (int i=0; i<5; i++) {
    m_Ncandidates[i]      = 0;
    m_NcandScoreZero[i]   = 0;
    m_NcandDouble[i]      = 0;
    m_NscoreOk[i]         = 0;
    m_NscoreZeroBremRefit[i]          = 0;
    m_NscoreZeroBremRefitFailed[i]    = 0;
    m_NscoreZeroBremRefitScoreZero[i] = 0;
    m_NscoreZero[i]       = 0;
    m_Naccepted[i]        = 0;
    m_NacceptedBrem[i]    = 0;
    m_NsubTrack[i]        = 0;
    m_NnoSubTrack[i]      = 0;
    m_NbremFits[i]        = 0;
    m_Nfits[i]            = 0;
    m_NrecoveryBremFits[i]= 0;
    m_NgoodFits[i]        = 0;
    m_NfailedFits[i]      = 0;
  }

#ifdef SIMPLEAMBIGPROCNTUPLECODE

  
  // retrieve the ParticleProperties handle
  if( m_particlePropSvc.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "[ ---- ] Can not retrieve " << m_particlePropSvc << " ! Abort. " );
      return StatusCode::FAILURE;
    }
  
  // and the particle data table 
  m_particleDataTable = m_particlePropSvc->PDT();
  if (m_particleDataTable==0)
    {
      ATH_MSG_FATAL( " [ ---- ] Could not get ParticleDataTable! Cannot associate pdg code with charge! Abort. " );
      return StatusCode::FAILURE;
    }
  
  ITHistSvc* tHistSvc = 0;
  // now register the Tree
  if (service("THistSvc",tHistSvc).isFailure()) 
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !" ); 
  ATH_MSG_VERBOSE(  "Booking the validation TTree ... " );
  // create the new Tree
  m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());
  // create the branches
  
  if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
    ATH_MSG_ERROR("Could not register the  validation Tree -> Switching  ValidationMode Off !" );
    delete m_validationTree; m_validationTree = 0;
  } else {
    ATH_MSG_INFO( "TTree for Validation booked." );
    m_validationTree->Branch( "Event",                  &m_event      , "event/I");
    m_validationTree->Branch( "Track",                  &m_track      , "track/I");    
    m_validationTree->Branch( "Pt",                     &m_pt         , "pt/F");
    m_validationTree->Branch( "Eta",                    &m_eta        , "eta/F");
    m_validationTree->Branch( "Phi",                    &m_phi        , "phi/F");
    m_validationTree->Branch( "Score",                  &m_score      , "score/I");
    m_validationTree->Branch( "Accepted",               &m_accepted   , "accepted/I");
    m_validationTree->Branch( "Duplicate",              &m_duplicate  , "duplicate/I");
    m_validationTree->Branch( "Perigee",                &m_perigeeInfo, "perigeeInfo/I");        
    m_validationTree->Branch( "Author",                 &m_author,      "authorChange/I");
    m_validationTree->Branch( "McPt",                   &m_pT_mc         , "ptmc/F");
    m_validationTree->Branch( "McCharge",               &m_charge_mc     , "chargemc/F");        
    m_validationTree->Branch( "NumberOfHits",           &m_numhits      , "hitsontrack/I");
    m_validationTree->Branch( "NumberBarcodes",         &m_numbarcodes  , "barcodesontrack/I");
    m_validationTree->Branch( "NumberOfHitsTruthLost",  &m_numtruthlost , "hitstruthlost/I");
    m_validationTree->Branch( "LeadingBarcode",         &m_leadingbarcode, "leadingbc/I");
    m_validationTree->Branch( "NumberOfHitsLBC",        &m_leadingnumhits, "numberofhitslbc/I");
    m_validationTree->Branch( "BarcodeDuplicates",      &m_barcodeDuplicates, "barcodedupes/I");      
    m_validationTree->Branch( "NumberOfSeeds",          &m_nseeds, "numberofseeds/I");
    m_validationTree->Branch( "SeedIndices",            m_seeds, "seeds[numberofseeds]/I");      
    
  }
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  // to get the brem truth
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL) << "Toll service not found !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = toolSvc->retrieveTool("Trk::TruthToTrack", m_truthToTrack);
  if(sc.isFailure()) {
     msg(MSG::FATAL) << "Cannot retrieve the TruthToTrack tool... Exiting" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not get PixelID helper !" << endmsg;
      return StatusCode::FAILURE;
    }
#endif
  
  return sc;
}
//==================================================================================================

StatusCode Trk::SimpleAmbiguityProcessorTool::finalize()
{
  ATH_MSG_INFO (name() << "::finalize() -- statistics:");
  StatusCode sc = AlgTool::finalize();
  return sc;
}

void Trk::SimpleAmbiguityProcessorTool::statistics()
{
  ATH_MSG_INFO (name() << " -- statistics:");
  std::streamsize ss = std::cout.precision();
  if (msgLvl(MSG::INFO)) {
    int iw=9;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of events processed      :   "<< m_Nevents << std::endl;
    std::cout << "  statistics by eta range          ------All---Barrel---Trans.--- Endcap---DBM--- " << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of candidates at input   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "  - candidates rejected score 0   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "  - candidates rejected as double :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  candidates with good score      :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + recovered after brem refit    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    }
    std::cout << "  candidates rejected score 0     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + rejected failed brem refit    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
      std::cout << "  + rejected brem refit score 0   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    }
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  number of normal fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + 2nd brem fit for failed fit   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
      std::cout << "  normal brem fits for electrons  :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    }
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  sum of succesful fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "  sum of failed fits              :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of subtracks created     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "  Number of candidates excluded   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of tracks accepted       :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  including number of brem fits   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::endl;
    }
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
	      << "    definition: ( 0.0 < Barrel < " << m_etabounds[iBarrel-1] << " < Transition < " << m_etabounds[iTransi-1]
	      << " < Endcap < " << m_etabounds[iEndcap-1] << " DBM )" << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
  }
  std::cout.precision (ss);
  return;
}

//==================================================================================================

/** helper function for statistics */

void Trk::SimpleAmbiguityProcessorTool::increment_by_eta(std::vector<int>& Ntracks, const Track* track, bool updateAll) {

  if (updateAll) Ntracks[Trk::SimpleAmbiguityProcessorTool::iAll] += 1;

  // test
  if (!track)
    {
       ATH_MSG_ERROR ("track pointer zero, should not happen!");
       return;
    }

  // use first parameter 
  if (!track->trackParameters())
    {
      ATH_MSG_WARNING ("No track parameters, needed for statistics code in Trk::SimpleAmbiguityProcessorTool!");
    }
  else
    {
      double eta = track->trackParameters()->front()->eta();
      if (fabs(eta) < m_etabounds[0]) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iBarrel];
      else if (fabs(eta) < m_etabounds[1]) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iTransi];
      else if (fabs(eta) < m_etabounds[2]) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iEndcap];
      else if ((fabs(eta) > m_etabounds[3]) && (fabs(eta) < m_etabounds[4])) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iDBM];
    }
}

//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection*  Trk::SimpleAmbiguityProcessorTool::process(const TrackCollection* tracks)
{
  
  using namespace std;

#ifdef SIMPLEAMBIGPROCNTUPLECODE
  const xAOD::EventInfo* eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure()) 
    msg(MSG::WARNING)<<"Could not get EventInfo object" << endmsg;  
  else {
    m_event = (int)eventInfo->eventNumber();
  }
  
  m_truthPIX = 0;
  m_truthSCT = 0;
  // retrieve the multi-truth maps
  if (evtStore()->contains<PRD_MultiTruthCollection>(m_truth_locationPixel) && evtStore()->retrieve(m_truthPIX,m_truth_locationPixel).isFailure())
    msg(MSG::WARNING)<<"Could not find TruthPixel" << endmsg;  
  
  if (evtStore()->contains<PRD_MultiTruthCollection>(m_truth_locationSCT) && evtStore()->retrieve(m_truthSCT,m_truth_locationSCT).isFailure())
    msg(MSG::WARNING)<<"Could not find TruthSCT" << endmsg;
  
  
  m_trackBarcodeMap.clear();
  m_barcodeTrackMap.clear();
  // fill the truth maps
  if (m_truthPIX && m_truthSCT){
    
    TrackCollection::const_iterator trackIt    = tracks->begin();
    TrackCollection::const_iterator trackItEnd = tracks->end();
    
    for ( ; trackIt != trackItEnd ; ++trackIt)
      {
	std::map<int,int> barcodeOccurence;
	std::map<int,const HepMC::GenParticle*> barcodeGenParticle;
	int numhits         = 0;
	int numbarcodes     = 0;
	int numtruthlost    = 0;
	int leadingbarcode  = 0;
	int leadingnumhits  = 0;  
	// prd iteration
	const DataVector<const MeasurementBase>* measurements = (*trackIt)->measurementsOnTrack();
	if (!measurements) continue;
	DataVector<const MeasurementBase>::const_iterator rotIter    = measurements->begin();
	DataVector<const MeasurementBase>::const_iterator rotIterEnd = measurements->end();
        
        
	for ( ; rotIter != rotIterEnd; ++rotIter ){
	  // get the prd from the ROT    
	  const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*rotIter);
	  if (!rot) continue;
	  const Trk::PrepRawData* prd = (*rot).prepRawData();
	  if (!prd) continue;
          
	  // PIXEL / SCT
	  const InDet::PixelCluster* pixCluster = dynamic_cast<const InDet::PixelCluster*>(prd);
	  const InDet::SCT_Cluster*  sctCluster  = pixCluster ? 0 : dynamic_cast<const InDet::SCT_Cluster*>(prd);
	  const InDet::SiCluster* siCluster = pixCluster;
	  siCluster = siCluster ? siCluster : sctCluster;
	  // get the right 
	  const PRD_MultiTruthCollection* truthColll = pixCluster ? m_truthPIX : m_truthSCT;
	  // one of the two worked
          
	  if (siCluster){
	    
	    ++numhits;            
	    // get the rdo list and loop over it
	    const std::vector<Identifier>& rdoList = siCluster->rdoList();
	    std::vector<Identifier>::const_iterator rdoIter    = rdoList.begin();
	    std::vector<Identifier>::const_iterator rdoIterEnd = rdoList.end();
            
	    for ( ; rdoIter != rdoIterEnd; ++rdoIter ){
	      // find the GenParticle link : Pixels                                
	      PRD_MultiTruthCollection::const_iterator hmcpIt      = m_truthPIX->end();
	      std::pair<PRD_MultiTruthCollection::const_iterator, PRD_MultiTruthCollection::const_iterator> itpixRange = truthColll->equal_range(*rdoIter);
	      for ( hmcpIt = itpixRange.first; hmcpIt != itpixRange.second; ++hmcpIt){
		// get the HepMCParticleLink
		HepMcParticleLink pLink = hmcpIt->second;
		// get vertex and direction
		const HepMC::GenParticle* particle = pLink.cptr();
		if (particle){
		  // get the particle barcode
		  int barcode = particle->barcode();
		  std::map<int,int>::iterator bcuIter = barcodeOccurence.find(barcode);
		  if (barcode && bcuIter == barcodeOccurence.end()) {
		    barcodeOccurence.insert(std::make_pair<int,int>(barcode,1));
		    barcodeGenParticle.insert(std::make_pair<int,const HepMC::GenParticle*>(barcode,particle));
		  }
		  else if (barcode) ++barcodeOccurence[barcode];
		  else ++numtruthlost; 
		} else ++numtruthlost;
                
	      } // loop over GenParticles
	    } // loop over rdos
	  } // siCluster check 
	} // prd Loop
	numbarcodes = barcodeOccurence.size();
	std::map<int,int>::iterator bcIter    = barcodeOccurence.begin();
	std::map<int,int>::iterator bcIterEnd = barcodeOccurence.end();
	for ( ; bcIter != bcIterEnd; ++bcIter){
	  if ( (*bcIter).second > leadingnumhits ){
	    leadingnumhits = (*bcIter).second;
	    leadingbarcode = (*bcIter).first;
	  }
	}
	if (leadingbarcode){
	  // get the charge and the truth pt
	  const HepMC::GenParticle* particle = barcodeGenParticle[leadingbarcode];
	  m_pT_mc = particle ?  particle->momentum().perp() : 0.;
          
	  int pdgCode = particle->pdg_id();
	  int absPdgCode = abs(pdgCode);
          
	  // get the charge: ap->charge() is used later
	  const HepPDT::ParticleData* ap =  m_particleDataTable->particle( absPdgCode);
	  if( ap) m_charge_mc = ap->charge();
	  // since the PDT table only has abs(PID) values for the charge
	  m_charge_mc *= (pdgCode > 0.) ?  1. : -1.;
	}
	// create the stat object
	TrackBarcodeStats tbcStats;
	tbcStats.numhits        =  numhits       ;
	tbcStats.numbarcodes    =  numbarcodes   ;
	tbcStats.numtruthlost   =  numtruthlost  ;
	tbcStats.leadingbarcode =  leadingbarcode;
	tbcStats.leadingnumhits =  leadingnumhits;
	// create the map entries
	m_trackBarcodeMap.insert(std::make_pair<const Trk::Track*,TrackBarcodeStats>(*trackIt,tbcStats));
	m_barcodeTrackMap.insert(std::make_pair<int,const Trk::Track*>(leadingbarcode,*trackIt));
        
      } // track loop
    
  } // MC case
  
#endif
  
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  StatusCode sc1;
  
  m_truthPIX  = 0;
  m_truthSCT  = 0;
  m_truthTRT  = 0;
  
  sc1 = evtStore()->retrieve(m_truthPIX,m_truth_locationPixel);
  if (sc1.isFailure()) 
    msg(MSG::WARNING)<<"Could not find TruthPixel"<<endmsg;  
  
  sc1 = evtStore()->retrieve(m_truthSCT,m_truth_locationSCT);
  if (sc1.isFailure()) 
    msg(MSG::WARNING)<<"Could not find TruthSCT"<<endmsg;

  sc1 = evtStore()->retrieve(m_truthTRT,m_truth_locationTRT);
  if (sc1.isFailure()) 
    msg(MSG::FATAL)<<"Could not find TruthTRT"<<endmsg;
#endif
  
  ++m_Nevents; // statistics

  // clear all caches etc.
  reset();
  
  //put tracks into maps etc
  ATH_MSG_DEBUG ("Adding input track candidates to list");
  addNewTracks(tracks);
 
  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat 

  ATH_MSG_DEBUG ("Solving Tracks");
  solveTracks();
  
  if (msgLvl(MSG::DEBUG)) dumpTracks(*m_finalTracks);
  
  // memory defragmantation fix. Cleaning before returning the result 
  m_prdSigSet.clear(); 
  m_trackScoreTrackMap.clear(); 
   
  return m_finalTracks;
}

void Trk::SimpleAmbiguityProcessorTool::reset()
{
  //this is a map which contains pointers to copies of all the input tracks
  m_trackScoreTrackMap.clear();

  //Signature Set
  m_prdSigSet.clear();
  
  // clear prdAssociationTool via selection tool
  m_selectionTool->reset();

  //final copy - ownership is passed out of algorithm
  m_finalTracks = new TrackCollection;
  
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  numOutliersDiff  = 0;
  numFirstFitLost  = 0;
  numSecondFitLost = 0;
  truthBefore      = 0;
  truthAfter       = 0;
#endif

  return;
}

//==================================================================================================
void Trk::SimpleAmbiguityProcessorTool::addNewTracks(const TrackCollection* tracks)
{
  using namespace std;

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  findTrueTracks(tracks);
#endif
 
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks->size());
 
  TrackCollection::const_iterator trackIt    = tracks->begin();
  TrackCollection::const_iterator trackItEnd = tracks->end();

#ifdef SIMPLEAMBIGPROCNTUPLECODE          
  if (!evtStore()->contains<TrackSeedMap>(m_trackSeedMapLocation)){
    msg(MSG::DEBUG)<<"Could not find TrackSeedMap, Seed validation needs to be run as well to have full output!" << endmsg;
    m_trackSeedMap = 0;	
  } else if (evtStore()->retrieve(m_trackSeedMap,m_trackSeedMapLocation).isFailure()){
    msg(MSG::WARNING)<<"Could not retrieve TrackSeedMap" << endmsg;		
    m_trackSeedMap = 0;	
  } else{
    msg(MSG::DEBUG)<<"Retrieved TrackSeedMap" << endmsg;	
  }
  if (m_trackTrackMap) delete m_trackTrackMap;
  m_trackTrackMap = new  std::map<const Trk::Track* , Trk::Track*> ;
#endif
 
  for ( ; trackIt != trackItEnd ; ++trackIt)
    {

#ifdef SIMPLEAMBIGPROCDEBUGCODE   
      numOutliersBefore = 0;
      numOutliersAfter  = 0;
#endif

      ATH_MSG_DEBUG ("Processing track candidate "<<*trackIt);
      // statistics
      increment_by_eta(m_Ncandidates,*trackIt);
    
      bool reject = false;
    
      // only fitted tracks get hole search, input is not fitted
      TrackScore score = m_scoringTool->score( **trackIt, true);

#ifdef SIMPLEAMBIGPROCNTUPLECODE   
      // get the object stored from the track map
      m_duplicate        = 0; 
      m_numbarcodes      = 0;
      m_numtruthlost     = 0;
      m_leadingbarcode   = 0;
      m_leadingnumhits   = 0;        
      std::map<const Trk::Track*, TrackBarcodeStats>::iterator tbcIt = m_trackBarcodeMap.find(*trackIt);
      
      if ( tbcIt !=  m_trackBarcodeMap.end() ){ 
	m_numhits          = (*tbcIt).second.numhits;
	m_numbarcodes      = (*tbcIt).second.numbarcodes;
	m_numtruthlost     = (*tbcIt).second.numtruthlost;
	m_leadingbarcode   = (*tbcIt).second.leadingbarcode;
	m_leadingnumhits   = (*tbcIt).second.leadingnumhits;
	// get the number of track having the same barcode
	m_barcodeDuplicates = m_barcodeTrackMap.count(m_leadingbarcode);
      }
      
      // reset for later filling
      m_score            = int(score);
      m_eta              = 0.;
      m_phi              = 0.;
      m_pt               = 0.;
      m_track            = long(*trackIt);
      m_author           = 0;
      m_accepted         = 0;
      m_perigeeInfo      = 0;
      
      // only if seed map has been found
      m_nseeds = 0;
      
      if ( m_trackSeedMap ){
	pair< TrackSeedMap::iterator,TrackSeedMap::iterator > seeds = m_trackSeedMap->equal_range((*trackIt));
	TrackSeedMap::const_iterator tsmIter = seeds.first;
	TrackSeedMap::const_iterator tsmIterEnd = seeds.second;
        
	for (int i=0; i<MAXSEEDSPERTRACK;++i) m_seeds[i]=0;
	for(; tsmIter!=tsmIterEnd;++tsmIter){
	  if(m_nseeds <  MAXSEEDSPERTRACK) m_seeds[m_nseeds] = (*tsmIter).second;
	  m_nseeds++;
	}
      }
#endif      

      // veto tracks with score 0
      if (score==0) { 
	ATH_MSG_DEBUG ("Candidate score is zero, reject it");
	// statistic
	increment_by_eta(m_NcandScoreZero,*trackIt);
	
	reject = true;
	
#ifdef SIMPLEAMBIGPROCNTUPLECODE 
	// (1) tracks can be rejected in the addNewTracks() method due to 0 score
	fillValidationTree(*trackIt);
	m_validationTree->Fill();
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
	if( isTrueTrack(*trackIt)){
	  msg(MSG::INFO)<< "the # of Outliers before is: " << numOutliersBefore << " and after is: " << numOutliersAfter<<endmsg;
	  if(numOutliersBefore != numOutliersAfter){
	    msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(*trackIt) << " Because of Outlier change" <<endmsg; 
	    numOutliersDiff++;
	  } else {
	    msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(*trackIt) << " Because of Zero Score" <<endmsg; 
	  }
	  prdTruth(*trackIt);
	  tsosTruth(*trackIt);
	}
#endif

      } else {

	ATH_MSG_DEBUG ("Track Score is "<< score);
	
	// double track rejection
	if (m_dropDouble) {
	  std::vector<const Trk::PrepRawData*> prds = m_selectionTool->getPrdsOnTrack(*trackIt);

	  // unfortunately PrepRawDataSet is not a set !
	  PrdSignature prdSig;
	  prdSig.insert( prds.begin(),prds.end() );

	  // we try to insert it into the set, if we fail (pair.second), it then exits already
	  if ( !(m_prdSigSet.insert(prdSig)).second ) {

	    ATH_MSG_DEBUG ("Double track, reject it !");
	    // statistic
	    increment_by_eta(m_NcandDouble,*trackIt);

	    reject = true;

#ifdef SIMPLEAMBIGPROCNTUPLECODE   
	    // (2) tracks can be rejected in the addNewTracks() method due to being a duplicate
	    m_duplicate = 1;
	    fillValidationTree(*trackIt);  	  
	    m_validationTree->Fill();
#endif
	  } else {
	    ATH_MSG_DEBUG ("Insert new track in PrdSignatureSet");
	  }
	}
      }
 
      if (!reject) {
	// this does a deep copy, we need to clean this
	const Track* track = new Track(**trackIt);
	
#ifdef SIMPLEAMBIGPROCNTUPLECODE  
	m_trackTrackMap->insert( std::pair< const Trk::Track* , Trk::Track*> (track,  (*trackIt)));
#endif 	
	
	// add track to map, map is sorted small to big ! set if fitted
	ATH_MSG_VERBOSE ("Track  ("<< track <<") has score "<<score);
	m_trackScoreTrackMap.insert( make_pair(-score, make_pair(track, !m_forceRefit)) );

#ifdef SIMPLEAMBIGPROCDEBUGCODE
	keepTrackOfTracks(*trackIt,track);
#endif

      }
    }
  
  ATH_MSG_DEBUG ("Number of tracks in map:"<<m_trackScoreTrackMap.size());
  
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  int numTrueMap = 0;
  TrackScoreMap::iterator itnext = m_trackScoreTrackMap.begin();
  TrackScoreMap::iterator itend = m_trackScoreTrackMap.end();
  for (; itnext != itend; ++itnext){
    if(isTrueTrack(itnext->second.first))
      numTrueMap++;
  }
  msg(MSG::INFO)<<"Number of TRUE tracks in map:"<< numTrueMap <<endmsg;
#endif
  
  return;
}

//==================================================================================================
  
void Trk::SimpleAmbiguityProcessorTool::addTrack(const Trk::Track* track, const bool fitted)
{
  using namespace std;

  // compute score
  TrackScore score;
  bool suppressHoleSearch = fitted ? m_suppressHoleSearch : true;
  score = m_scoringTool->score( *track, suppressHoleSearch );

  // do we accept the track ?
  if (score!=0)
    {
      ATH_MSG_DEBUG ("Track  ("<< track <<") has score "<<score);
      // statistic
      increment_by_eta(m_NscoreOk,track);

      // add track to map, map is sorted small to big !
      m_trackScoreTrackMap.insert( make_pair(-score, make_pair(track, fitted)) );

      return;
    }

  // do we try to recover the track ?
  if (score==0 && fitted && m_tryBremFit &&
      !track->info().trackProperties(Trk::TrackInfo::BremFit) &&
      track->trackParameters()->front()->pT() > m_pTminBrem &&
      (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
    {

      ATH_MSG_DEBUG ("Track score is zero, try to recover it via brem fit");

      // run track fit using electron hypothesis
      const Trk::Track* bremTrack = m_fitterTool->fit(*track,true,Trk::electron);

      if (!bremTrack)
	{
	  ATH_MSG_DEBUG ("Brem refit failed, drop track");
	  // statistic
	  increment_by_eta(m_NscoreZeroBremRefitFailed,track);
	  increment_by_eta(m_NfailedFits,track);

	  // clean up
	  delete(track);

	}
      else
	{
	  // clean up
	  delete(track);

	  // statistic
	  increment_by_eta(m_NgoodFits,bremTrack);

	  // rerun score
	  score = m_scoringTool->score( *bremTrack, suppressHoleSearch );

	  // do we accept the track ?
	  if (score!=0)
	    {
	      ATH_MSG_DEBUG ("Brem refit successful, recovered track  ("<< track <<") has score "<<score);
	      // statistics
	      increment_by_eta(m_NscoreZeroBremRefit,bremTrack);

	      // add track to map, map is sorted small to big !
	      m_trackScoreTrackMap.insert( make_pair(-score, make_pair(bremTrack, fitted)) );
	      return;
	    }
	  else
	    {
	      ATH_MSG_DEBUG ("Brem refit gave still track score zero, reject it");
	      // statistic
	      increment_by_eta(m_NscoreZeroBremRefitScoreZero,bremTrack);

	      // clean up
	      delete(bremTrack);
	    }
	}
    }
  else  
    {
      ATH_MSG_DEBUG ("Track score is zero, reject it");
      // statistic
      increment_by_eta(m_NscoreZero,track);

#ifdef SIMPLEAMBIGPROCNTUPLECODE
      m_accepted = 0;
      m_score    = 0;
      m_validationTree->Fill();
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
      if(isTrueTrack(track)){
	msg(MSG::INFO)<< "the # of Outliers before is: " << numOutliersBefore << " and after is: " << numOutliersAfter<<endmsg;
	if(numOutliersBefore != numOutliersAfter){
	  msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(track) << " Because of Outlier change" <<endmsg; 
	  numOutliersDiff++;
	}else {
	  msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(track) << " Because of Zero Score" <<endmsg; 
	}
	prdTruth(track);
	tsosTruth(track);
      }
#endif
      // clean up
      delete(track);
    }
  return;
}
//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::solveTracks()
{
  using namespace std;

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  n_trueFitFails = 0;
  n_fitFails     = 0;
#endif

  ATH_MSG_DEBUG ("Starting to solve tracks");

  // now loop as long as map is not empty
  while ( !m_trackScoreTrackMap.empty() )
    {
      // get current best candidate 
      TrackScoreMap::iterator itnext = m_trackScoreTrackMap.begin();

      // clean it out to make sure not to many shared hits
      ATH_MSG_VERBOSE ("--- Trying next track "<<itnext->second.first<<"\t with score "<<-itnext->first);
      const Trk::Track* cleanedTrack = m_selectionTool->getCleanedOutTrack( itnext->second.first , -(itnext->first));

      // cleaned track is input track and fitted
      if (cleanedTrack == itnext->second.first && itnext->second.second )
	{
		
#ifdef SIMPLEAMBIGPROCNTUPLECODE   
	  m_score       = int(-(itnext->first));
	  m_eta         = 0.;
	  m_phi         = 0.;
	  m_pt          = 0.;
	  m_track       = long(itnext->second.first);
	  m_author      = 0;
	  m_accepted    = 0;
	  m_perigeeInfo = 0;
	  m_duplicate   = 0;    
	  
	  m_nseeds = 0;
	  
	  if (m_trackSeedMap) {
	    // find the original track
	    std::map<const Trk::Track* , Trk::Track*>::iterator iter;
	    const Trk::Track * tmpTrack = itnext->second.first;
	    while((iter = m_trackTrackMap->find(tmpTrack)) != m_trackTrackMap->end()){
	      tmpTrack = iter->second;
	    }
	    pair< TrackSeedMap::iterator,TrackSeedMap::iterator > seeds = m_trackSeedMap->equal_range((tmpTrack));
            
	    TrackSeedMap::const_iterator tsmIter = seeds.first;
	    TrackSeedMap::const_iterator tsmIterEnd = seeds.second;
            
	    for(int i=0; i<MAXSEEDSPERTRACK;++i) m_seeds[i]=0;
	    for(; tsmIter!=tsmIterEnd;++tsmIter){
	      if(m_nseeds <  MAXSEEDSPERTRACK) m_seeds[m_nseeds] = (*tsmIter).second;
	      m_nseeds++;
	    }
	  }
#endif
		
	  // track can be kept as is and is already fitted
	  ATH_MSG_DEBUG ("Accepted track "<<itnext->second.first<<"\t has score "<<-(itnext->first));
	  // statistic
	  increment_by_eta(m_Naccepted,itnext->second.first);
	  if (m_tryBremFit && itnext->second.first->info().trackProperties(Trk::TrackInfo::BremFit))
	    increment_by_eta(m_NacceptedBrem,itnext->second.first);

	  // add track to PRD_AssociationTool
	  StatusCode sc = m_selectionTool->registerPRDs(itnext->second.first);
	  if (sc.isFailure()) msg(MSG::ERROR) << "addPRDs() failed" << endmsg;
	  // add to output list 
	  m_finalTracks->push_back( const_cast<Track*>(itnext->second.first) );

#ifdef SIMPLEAMBIGPROCNTUPLECODE 
	  // this track goes into the final selection, record it
	  m_accepted = 1;
	  fillValidationTree(itnext->second.first);
	  m_validationTree->Fill();
#endif

	  // don't forget to drop track from map
	  m_trackScoreTrackMap.erase(itnext);
	}
      else if ( cleanedTrack == itnext->second.first )
	{

#ifdef SIMPLEAMBIGPROCDEBUGCODE
	  numOutliersBefore = cleanedTrack->outliersOnTrack()->size();
#endif
	  // don't forget to drop track from map
	  m_trackScoreTrackMap.erase(itnext);
	  // track can be kept as is, but is not yet fitted
	  ATH_MSG_DEBUG ("Good track, but need to fit this track first, score, add it into map again and retry !");
	  refitTrack(cleanedTrack);
	  // delete original copy
	  delete (cleanedTrack);
	 }
      else if ( cleanedTrack )
	{
#ifdef SIMPLEAMBIGPROCNTUPLECODE  
	  m_trackTrackMap->insert( std::pair< const Trk::Track* , Trk::Track*> (cleanedTrack, const_cast<Trk::Track*>(itnext->second.first)));
#endif
	  
#ifdef SIMPLEAMBIGPROCDEBUGCODE
	  keepTrackOfTracks(itnext->second.first, cleanedTrack);
#endif
	  

	  // now delete original track
	  delete itnext->second.first;
	  // don't forget to drop track from map
	  m_trackScoreTrackMap.erase(itnext);

	  // stripped down version should be reconsidered
	  ATH_MSG_DEBUG ("Candidate excluded, add subtrack to map. Track "<<cleanedTrack);
	  // statistic
	  increment_by_eta(m_NsubTrack,cleanedTrack);

	  // track needs fitting !
	  addTrack( cleanedTrack, false);
	}
      else
	{

#ifdef SIMPLEAMBIGPROCNTUPLECODE 
	  // this is a track that did not pass the track selector tool
	  m_accepted = -1;  
	  fillValidationTree(itnext->second.first);
	  m_validationTree->Fill();
#endif
		
	  // track should be discarded
	  ATH_MSG_DEBUG ("Track "<< itnext->second.first << " is excluded, no subtrack, reject");
	  // statistic
	  increment_by_eta(m_NnoSubTrack,itnext->second.first);

	  delete (itnext->second.first);
	  // don't forget to drop track from map
	  m_trackScoreTrackMap.erase(itnext);
	}
    }
  
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<m_finalTracks->size());
  

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  TrackCollection::const_iterator  m_itFinal = m_finalTracks->begin();
  TrackCollection::const_iterator m_endFinal = m_finalTracks->end();
  for ( ; m_itFinal != m_endFinal ; ++m_itFinal) {
    if ( isTrueTrack(*m_itFinal) )
      truthAfter++;
  }
  msg(MSG::INFO)<<"Where, the number of true track on output is: "<<truthAfter <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE track failed fits is:  "<< n_trueFitFails <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE tracks rejected due to outliers is: "<< numOutliersDiff <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE tracks rejected after the first fit is: "<< numFirstFitLost <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE tracks rejected after the second fit is: "<< numSecondFitLost <<endmsg;
  
  if(truthBefore != truthAfter)
    msg(MSG::INFO)<<"The number of tracks lost this events is:  "<< truthBefore-truthAfter << endmsg;
  
  if (n_trueFitFails >0 && m_isBackTracking){
    msg(MSG::INFO) << "DOING THE BREM TRUTH" << endmsg;
    getBremTruth();//problem with statuscode
  }
  
  produceInputOutputConnection();
#endif

  return;
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::refitTrack( const Trk::Track* track)
{
  using namespace std;
  const Trk::Track* newTrack = 0;
  if (!m_suppressTrackFit)
    if (m_refitPrds) 
      {
	// simple case, fit PRD directly
	ATH_MSG_VERBOSE ("Refit track "<<track<<" from PRDs");
	newTrack = refitPrds (track);
      }
    else 
      {
	// ok, we fit ROTs
	ATH_MSG_VERBOSE ("Refit track "<<track<<" from ROTs");
	newTrack = refitRots (track);
      }
  else
    {
      double reXi2 = 0.; int nDF = 0;
      const DataVector<const TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
      DataVector<const TrackStateOnSurface>* vecTsos = new DataVector<const TrackStateOnSurface>();
      // loop over TSOS, copy TSOS and push into vector
      DataVector<const TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
      DataVector<const TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
      for ( ; iTsos != iTsosEnd ; ++iTsos) 
	{
	  const TrackStateOnSurface* newTsos = new TrackStateOnSurface(**iTsos);
	  vecTsos->push_back(newTsos);
	  if((*iTsos)->type(Trk::TrackStateOnSurface::Measurement))
	    {  //Get the chi2 and number of hits
	      if ((*iTsos)->fitQualityOnSurface()) {
		reXi2 += (*iTsos)->fitQualityOnSurface()->chiSquared();
		nDF   += (*iTsos)->fitQualityOnSurface()->numberDoF();
	      }
	    }
	}
      Trk::FitQuality* fq = new Trk::FitQuality(reXi2,nDF-5);
      Trk::TrackInfo info;
      info.addPatternRecoAndProperties(track->info());
      Trk::TrackInfo newInfo;
      newInfo.setPatternRecognitionInfo(Trk::TrackInfo::SimpleAmbiguityProcessorTool);
      info.addPatternReco(newInfo); 

      newTrack = new Trk::Track(info, vecTsos, fq);
    }

  if (newTrack!=0) 
    {
      ATH_MSG_DEBUG ("New track successfully fitted"<<newTrack);

#ifdef SIMPLEAMBIGPROCNTUPLECODE  
	m_trackTrackMap->insert( std::pair< const Trk::Track* , Trk::Track*> (newTrack, const_cast<Trk::Track*> (track)));
#endif
		
#ifdef SIMPLEAMBIGPROCDEBUGCODE
      keepTrackOfTracks(track, newTrack);
#endif

      addTrack( newTrack, true );
    }
  else {
#ifdef SIMPLEAMBIGPROCNTUPLECODE 
    // this is a track that did not survive the refit
    m_accepted = -2;  
    m_validationTree->Fill();
#endif
	
#ifdef SIMPLEAMBIGPROCDEBUGCODE
    msg(MSG::INFO) << "The Track: " << origTrack(track) << " failed to fit" << endmsg;
    n_fitFails++;
    msg(MSG::INFO) << "The total number of failed fits is now" << n_fitFails <<endmsg;
    if ( isTrueTrack(track)) {
      n_trueFitFails++;
      msg(MSG::INFO)<< "The total number of TRUE failed fits is now" << n_trueFitFails <<endmsg;
      prdTruth(track);
      tsosTruth(track);
    }
#endif
    
    ATH_MSG_DEBUG ("Fit failed !");
  }  
  
  return;
}

//==================================================================================================

const Trk::Track* Trk::SimpleAmbiguityProcessorTool::refitPrds( const Trk::Track* track)
{

  // get vector of PRDs
  std::vector<const Trk::PrepRawData*> prds = m_selectionTool->getPrdsOnTrack(track);

  if ( 0==prds.size() ) {
    msg(MSG::WARNING) << "No PRDs on track"<<endmsg;
    return 0;
  }
     
  ATH_MSG_VERBOSE ("Track "<<track<<"\t has "<<prds.size()<<"\t PRDs");

  const TrackParameters* par = track->perigeeParameters();
  if (par==0) {
    ATH_MSG_DEBUG ("Track ("<<track<<") has no perigee! Try any other ?");
    par = track->trackParameters()->front();
    if (par==0) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
      return 0;
    }
  }

  // refit using first parameter, do outliers
  const Trk::Track* newTrack = 0;

  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit))
    {
      // statistics
      increment_by_eta(m_NbremFits,track);

      ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
      newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);

    }
  else
    {
      // statistics
      increment_by_eta(m_Nfits,track);

      ATH_MSG_VERBOSE ("Normal track, refit");
      newTrack = m_fitterTool->fit(prds, *par, true, m_particleHypothesis);

      if (!newTrack && m_tryBremFit && par->pT() > m_pTminBrem &&
	  (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
	{
	  // statistics
	  increment_by_eta(m_NrecoveryBremFits,track);

	  ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
	  newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);
	}
    }
  
  if(newTrack)
    {
      // statistic
      increment_by_eta(m_NgoodFits,newTrack);

      //keeping the track of previously accumulated TrackInfo
      const Trk::TrackInfo old_info = track->info();
      newTrack->info().addPatternReco(old_info);
    }
  else
    {
      // statistic
      increment_by_eta(m_NfailedFits,track);
    }
  return newTrack;
}

//==================================================================================================

const Trk::Track* Trk::SimpleAmbiguityProcessorTool::refitRots( const Trk::Track* track)
{

  ATH_MSG_VERBOSE ("Refit track "<<track);

  // refit using first parameter, do outliers
  const Trk::Track* newTrack = 0;

  if (m_tryBremFit &&
      track->info().trackProperties(Trk::TrackInfo::BremFit))
    {
      // statistics
      increment_by_eta(m_NbremFits,track);

      ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
      newTrack = m_fitterTool->fit(*track, true, Trk::electron);
    }
  else
    {
      // statistics
      increment_by_eta(m_Nfits,track);

      ATH_MSG_VERBOSE ("Normal track, refit");
      newTrack = m_fitterTool->fit(*track, true, m_particleHypothesis);

      if (!newTrack && m_tryBremFit &&
	  track->trackParameters()->front()->pT() > m_pTminBrem &&
	  (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
	{
	  // statistics
	  increment_by_eta(m_NrecoveryBremFits,track);

	  ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
	  newTrack = m_fitterTool->fit(*track, true, Trk::electron);
	}
    }

  if(newTrack)
    {
      // statistic
      increment_by_eta(m_NgoodFits,newTrack);

      //keeping the track of previously accumulated TrackInfo
      const Trk::TrackInfo old_info = track->info();
      newTrack->info().addPatternReco(old_info);
    }
  else
    {
      // statistic
      increment_by_eta(m_NfailedFits,track);
    }
  return newTrack;
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::dumpTracks( const TrackCollection& tracks )
{

  ATH_MSG_VERBOSE ("Dumping tracks in collection");
  int num=0;
  TrackScore totalScore = 0;
  TrackCollection::const_iterator it    = tracks.begin();
  TrackCollection::const_iterator itEnd = tracks.end();
  for (; it != itEnd ; ++it)
    {
      // score track:
      const TrackScore score = m_scoringTool->score( **it, m_suppressHoleSearch );
      ATH_MSG_VERBOSE (num++<<"\tTrack :"<<*it<<"\tScore: "<<score);
      totalScore+=score;
    }
  ATH_MSG_DEBUG ("Total event score : "<<totalScore);
  return;
}


//==================================================================================================
//
//
//   FROM HERE EVERYTHING IS DEBUGGING CODE !!!
//
//
// Part I : Ntuple writing
//==================================================================================================

#ifdef SIMPLEAMBIGPROCNTUPLECODE
void Trk::SimpleAmbiguityProcessorTool::fillValidationTree(const Trk::Track* track) const
{     
   // keep track of the track pointer
    m_track = long(track);
   // the good guess : we perigee 
    const Trk::TrackParameters* tp = track->perigeeParameters();
    m_perigeeInfo = tp ? 1 : 0;
    if (!tp){
     // take the first track parameter estimate for the validation
        const DataVector<const Trk::TrackParameters>* tps = track->trackParameters();    
        if (tps && tps->size()) tp = (*tps)[0];
    }
   // fill pt / eta / phi
    m_pt  = tp ? float(tp->momentum().perp()) : m_pt;
    m_eta = tp ? float(tp->momentum().eta())  : m_eta;
    m_phi = tp ? float(tp->momentum().phi())  : m_phi;   
}
#endif

//==================================================================================================
// Part II : Truth association
//==================================================================================================
#ifdef SIMPLEAMBIGPROCDEBUGCODE

void Trk::SimpleAmbiguityProcessorTool::findTrueTracks(const TrackCollection* recTracks)
{

  numSharedTruth = 0;
  m_trueTracks.clear();
  m_trackHistory.clear();
  m_tracksShared.clear();

  msg(MSG::DEBUG) << "Acessing TrackTruthCollection " << endmsg;
  const TrackTruthCollection* truthMap  = 0;
  if (evtStore()->retrieve(truthMap , m_truthCollection).isFailure()) 
    msg(MSG::WARNING) << "No truth map present, abort TrueTrack search" << endmsg;


  
  std::map<int,std::pair<float,const Trk::Track*> > barcodeMap;
  float minProb =0.95;
  TrackCollection::const_iterator trackIt    = recTracks->begin();
  TrackCollection::const_iterator trackItEnd = recTracks->end();

  for (;trackIt!=trackItEnd;++trackIt)
    {
      msg(MSG::DEBUG) << "The Track is now " << *trackIt << endmsg;
	
      // initialise history tracing
      m_trackHistory.insert(std::make_pair(*trackIt,*trackIt));
	
      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(*trackIt));
      tracklink.setStorableObject(*recTracks);
      const ElementLink<TrackCollection> tracklink2=tracklink;
	
      // check if the track has a valid agreement in TrackToTruth
      TrackTruthCollection::const_iterator found = truthMap->find(tracklink2);
      if (found != truthMap->end() &&  
	  found->second.particleLink().isValid() &&
	  found->second.probability() > minProb    )
	{
	  if (!isSharedTrack(*trackIt)) addTrackToMap(*trackIt);//add track and pdrs to map
	  else numSharedTruth++;
	  int barcode=found->second.particleLink().barcode();
	  msg(MSG::DEBUG) << "The Barcode is: " << barcode << endmsg;
	  std::pair<int , std::pair<float , const Trk::Track*> >
	    insertion = std::make_pair(barcode,std::make_pair(found->second.probability(),*trackIt));
	  std::pair<std::map<int,std::pair<float,const Trk::Track*> >::iterator, bool > barcodeMapInsert =
	    barcodeMap.insert(insertion);
	  if (!(barcodeMapInsert.second) && insertion.second.first > barcodeMapInsert.first->second.first)
	    {
	      // if the barcode is already known and the new entry would be better, erase old and insert new
	      msg(MSG::DEBUG) << "Erasing Track:" << origTrack(barcodeMapInsert.first->second.second) <<endmsg; 		
	      barcodeMap.erase(barcodeMapInsert.first);
	      msg(MSG::DEBUG) << "Inserting Track:"<<  origTrack(insertion.second.second)<< endmsg;
	      barcodeMap.insert(insertion);
	    }
	  msg(MSG::DEBUG) << "The probability of " << origTrack(barcodeMapInsert.first->second.second) << "is "<< barcodeMapInsert.first->second.first <<endmsg; 
	}
    }
  
  // copy the true tracks in the TruthHistorySet:
  std::map<int,std::pair<float,const Trk::Track*> >::const_iterator  it = barcodeMap.begin();
  std::map<int,std::pair<float,const Trk::Track*> >::const_iterator end = barcodeMap.end();
  for (; it!=end; ++it) m_trueTracks.insert(it->second.second);
  truthBefore = m_trueTracks.size();
  msg(MSG::INFO) << "True Track set up with " << truthBefore << " true tracks." << endmsg;
  msg(MSG::INFO)<< "OF THE TRUE TRACKS " << numSharedTruth << " ARE SHARED" << endmsg;
  
  
}
//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::keepTrackOfTracks(const Trk::Track* oldTrack, const Trk::Track* newTrack)
{
  m_trackHistory.insert(std::make_pair(newTrack,oldTrack));
  if (m_trueTracks.find(oldTrack) != m_trueTracks.end() ) m_trueTracks.insert(newTrack);
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::produceInputOutputConnection()
{


  const TrackCollectionConnection* dmp;
  if (evtStore()->retrieve(dmp, m_resolvedTrackConnection).isFailure())
    {
      // output map: SiSpSeededTrack, ResolvedTrack  
      TrackCollectionConnection* siSP_ResolvedConnection = new TrackCollectionConnection();
      
      TrackCollection::const_iterator  itFinal = m_finalTracks->begin();
      TrackCollection::const_iterator endFinal = m_finalTracks->end();
      for ( ; itFinal != endFinal ; ++itFinal)
	{
	  std::map<const Trk::Track*, const Trk::Track*>::iterator pos = m_trackHistory.find(*itFinal);
	  while (pos->first != pos->second && pos != m_trackHistory.end())
	    pos = m_trackHistory.find(pos->second);
	  
	  if (pos == m_trackHistory.end())
	    msg(MSG::ERROR) << "Track not found in history" << endmsg;
	  else
	    siSP_ResolvedConnection->insert(std::make_pair(pos->second,*itFinal));
	  
	}
      
      StatusCode sc = evtStore()->record(siSP_ResolvedConnection, m_resolvedTrackConnection,false);
      
      if (sc.isFailure())
	msg(MSG::ERROR) << "Could not record trackCollectionConnecton" << endmsg;
      else
	msg(MSG::VERBOSE) << "Saved "<<siSP_ResolvedConnection->size()<<" track collection connections"<<endmsg; 
    }
}
//============================================================================================

const Trk::Track* Trk::SimpleAmbiguityProcessorTool::origTrack( const Trk::Track* track){
  
  std::map<const Trk::Track*, const Trk::Track*>::iterator m_pos = m_trackHistory.find(track);
  while (m_pos->first != m_pos->second && m_pos != m_trackHistory.end())
    m_pos = m_trackHistory.find(m_pos->second);
  
  if(m_pos == m_trackHistory.end())
    return 0;
  
  return m_pos->first;
}

//==================================================================================================
bool Trk::SimpleAmbiguityProcessorTool::isSharedTrack(const Trk::Track* Tr){

  int numSharedPRD = 0;
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();
  
  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if( m_tracksShared.find(prd) != m_tracksShared.end() ) numSharedPRD++;
  }
  if(numSharedPRD>=3) return true;
  else return false;
}
//===================================================================================						     

void Trk::SimpleAmbiguityProcessorTool::addTrackToMap(Trk::Track* Tr)
{
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();
  
  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if(prd) m_tracksShared.insert(std::make_pair(prd,Tr));
  }
}

//=================================================================================================
bool Trk::SimpleAmbiguityProcessorTool::isTrueTrack(const Trk::Track* track){
  std::set<const Trk::Track*>::const_iterator m_iter = m_trueTracks.find(track);
  if(m_iter != m_trueTracks.end())
    return true;
  else 
    return false;

}

//================================================================================
void Trk::SimpleAmbiguityProcessorTool::prdTruth(const Trk::Track* track){
  
  
  //geting the truth info about th prds
  std::vector<const Trk::PrepRawData*> prds = m_selectionTool->getPrdsOnTrack(track);
  std::vector<const Trk::PrepRawData*>::const_iterator prdit = prds.begin();
  std::vector<const Trk::PrepRawData*>::const_iterator prdite = prds.end();
  double m_eta = (*(track)->trackParameters()->begin())->eta();
  msg(MSG::INFO)<< "The eta of this track is " << m_eta << endmsg;   
  
  for( ; prdit != prdite; ++prdit){
    const InDet::SiCluster      * si = dynamic_cast<const InDet::SiCluster*>      (*prdit);
    const InDet::PixelCluster   * px = dynamic_cast<const InDet::PixelCluster*>   (*prdit);
    const InDet::TRT_DriftCircle* tr = dynamic_cast<const InDet::TRT_DriftCircle*>(*prdit);
    PRD_MultiTruthCollection::const_iterator mce;
    PRD_MultiTruthCollection::const_iterator mc;
    if     (px) {
      mc=m_truthPIX->find((*prdit)->identify()); 
      mce=m_truthPIX->end();
      if(mc==mce) {
	msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was pixel noise" << endmsg;
	continue;
      }
      msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was a pixel hit" << endmsg;
    }
    else if(si) {
      mc=m_truthSCT->find((*prdit)->identify()); 
      mce=m_truthSCT->end();
      if(mc==mce) {
	msg(MSG::INFO)<< "the hit " << *prdit << "On the track " <<origTrack(track) << "was si noise" << endmsg;	
	continue;
      }	
      msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was a si hit" << endmsg;
    }
    else if(tr) {
      mc=m_truthTRT->find((*prdit)->identify()); 
      mce=m_truthTRT->end();
      if(mc==mce) {
	msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was trt noise" << endmsg;	
	continue;
      }
      msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was a trt hit" << endmsg;
    }
    else {
      continue;
    }
    
    const HepMC::GenParticle* pa = mc->second.cptr(); 	
    int partid = pa->pdg_id();
    int partBc = pa->barcode();
    
    //HepLorentzVector m  = pa->momentum();
    double m_theta = pa->momentum().theta();
    msg(MSG::INFO)<< "the theta of the hit was " << m_theta << endmsg;
    msg(MSG::INFO)<< "The hit " << *prdit << "came from " << partid << " With a barcode of " << partBc << endmsg;
    
    
    
  }
}  

//====================================================================================
void Trk::SimpleAmbiguityProcessorTool::tsosTruth(const Trk::Track* track){

  
  const DataVector<const TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
  // loop over TSOS, copy TSOS and push into vector
  DataVector<const TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
  DataVector<const TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();   
  for(; iTsos != iTsosEnd; ++iTsos){
    msg(MSG::INFO)<< "the type of " << *iTsos << " is "<< (*iTsos)->type() << endmsg;
    const FitQualityOnSurface* fq = (*iTsos)->fitQualityOnSurface();
    if (fq)
      msg(MSG::INFO)<< "the chi2 of " << *iTsos << " is "<< fq->chiSquared() << endmsg;
    const MeasurementBase* mb = (*iTsos)->measurementOnTrack();
    if (mb){
      Identifier Id =  mb->associatedSurface().associatedDetectorElementIdentifier();
      msg(MSG::INFO)<< "the global r of the hit is " << mb->associatedSurface().center().perp() << endmsg;
      std::string detType = " unknown ";
      if (m_pixelId->is_pixel(Id))
	detType = " Pixel ";
      else if (m_pixelId->is_sct(Id))
	detType = " SCT ";
      else if (m_pixelId->is_trt(Id))
	detType = " TRT ";
      msg(MSG::INFO)<< "the surface is " << detType << endmsg; 
    }
  } 
}

//=======================================================================================
StatusCode Trk::SimpleAmbiguityProcessorTool::getBremTruth(){

  StatusCode sc;
  
  // Retrieve McEventCollection from StoreGate
  const McEventCollection* mcEventCollection = 0;
  
  sc = evtStore()->retrieve( mcEventCollection, m_generatedEventCollectionName );
  
  if ( sc.isFailure() ){
    return StatusCode::FAILURE;
  }
  
  // Loop over all events in StoreGate
  McEventCollection::const_iterator event = mcEventCollection->begin();
  
  for ( ; event != mcEventCollection->end(); ++event ){
    
    // Synchronise event number with reconstruction event number
    int eventNumber = (*event)->event_number();    
    msg(MSG::INFO) << "Event number: " << eventNumber - 1 << endmsg;

    // Determine the momentum of the original particle
    const double initialMomentum = originalMomentum( *event );
    msg(MSG::INFO) << "Initial momentum: " << initialMomentum << endmsg;

   

    // Calculate the total momentum loss as a result of brem
    double pLostByBrem = momentumLostByBrem( *event );
    msg(MSG::INFO) << "Total momentum lost by original particle due to Brem: " << pLostByBrem << " MeV" << endmsg;
   
    
    // Calculate the fraction of incident energy lost per vertex on track
    const std::vector<double> fractionOfIncidentEnergyLost = fractionOfIncidentMomentumLostPerVertex( *event );
    int countBrem = 0; //Si brem counter
    std::vector<double>::const_iterator fractionLostInLayer = fractionOfIncidentEnergyLost.begin();
    for ( ; fractionLostInLayer != fractionOfIncidentEnergyLost.end(); ++fractionLostInLayer ){
      msg(MSG::INFO) << "Fraction of incident energy lost at vertex: " << (*fractionLostInLayer) << endmsg;
      countBrem++;
    }
    
    // Determine the positions of the brem vertices
    const std::vector<HepPoint3D> positionsOfVertices = positionsOfBremVertices( *event );
    countBrem = 0; //Reinitialize brem counter
    std::vector<HepPoint3D>::const_iterator positionOfVertex = positionsOfVertices.begin();
    for ( ; positionOfVertex != positionsOfVertices.end(); ++positionOfVertex ){
      msg(MSG::INFO) << "(x, y, z) of vertex: " << positionOfVertex->perp() << endmsg;
      countBrem++;
    }
    
  }

  return StatusCode::SUCCESS;
}
//======================================================================================================
const double Trk::SimpleAmbiguityProcessorTool::originalMomentum( const HepMC::GenEvent* genEvent )
{

  // Loop over all particles in the event (info on this from GenEvent documentation)
  HepMC::GenEvent::particle_const_iterator particle = genEvent->particles_begin();

  // Calculate initial energy of electron. Should be the first particle
  HepMC::GenParticle* initialParticle = *( genEvent->particles_begin() );

  // Double check - should come from vertex barcode = -1 & have id 10001. Particle must also be defined
  //if ( !initialParticle || initialParticle->production_vertex()->barcode() != -1 || initialParticle->barcode() != 10001 )
  //  msg(MSG::WARNING) << "Inconsistency between initial particle and initial vertex" << endmsg;

  //Hep3Vector& initial3Momentum = initialParticle->momentum();
 
  double initialMomentum = initialParticle->momentum().mag();

  //  const Trk::TrackParameters* initialPerigeeParameters = m_truthToTrack->makePerigeeParameters(initialParticle);

  return initialMomentum;

}
//==================================================================================================
const double Trk::SimpleAmbiguityProcessorTool::momentumLostByBrem( const HepMC::GenEvent* genEvent ) const
{

 
  double bremPhotonEnergy(0.);
 
  // Loop over all verticies and determine which are associated to the original particle
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();
 
  for ( ; vertex != genEvent->vertices_end(); ++vertex ){
 
    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;
 
    // Loop over all particles in vertex. Determine if it is a vertex of the original particle
    HepMC::GenVertex::particle_iterator particleAtVertex = (*vertex)->particles_begin(HepMC::family);
 
    for ( ; particleAtVertex != (*vertex)->particles_end(HepMC::family); ++particleAtVertex ){
 
      if ( (*particleAtVertex)->pdg_id() == 22 )
	bremPhotonEnergy += (*particleAtVertex)->momentum().e();
 
    }
 
  }  

  return bremPhotonEnergy;

}
//=================================================================================================
const std::vector<double> Trk::SimpleAmbiguityProcessorTool::fractionOfIncidentMomentumLostPerVertex( const HepMC::GenEvent* genEvent ) const
{
  
  // Don't forget: Some of the 4-momentum of incident electron always transfered to nucleus. That's why no brem in vacuum.

  std::vector<double> fractionOfIncidentMomentumLostPerVertex;

  // Loop over all verticies and find those associated to the original track
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();

  for ( ; vertex != genEvent->vertices_end(); ++vertex ){

    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;

    // One incident particle only. THIS IS HARDWIRED TO ASSUME FIRST PARTICLE IS INCIDENT
    HepMC::GenVertex::particle_iterator incidentParticle = (*vertex)->particles_begin(HepMC::family);
    double incidentEnergy = (*incidentParticle)->momentum().e();

    // Loop over outgoing particles and extract the photon
    double photonEnergy(0.);

    HepMC::GenVertex::particles_out_const_iterator outgoingParticle = (*vertex)->particles_out_const_begin();

    for ( ; outgoingParticle != (*vertex)->particles_out_const_end(); ++outgoingParticle ){

      if ( (*outgoingParticle)->pdg_id() == 22 )
	photonEnergy = (*outgoingParticle)->momentum().e();

    }

    // Define the fractional energy loss
    double fractionalEnergyLoss = photonEnergy / incidentEnergy;

    if ( fractionalEnergyLoss != 0. )
      fractionOfIncidentMomentumLostPerVertex.push_back( fractionalEnergyLoss );

  }

  return fractionOfIncidentMomentumLostPerVertex;

}
//=================================================================================================
const std::vector<HepPoint3D> Trk::SimpleAmbiguityProcessorTool::positionsOfBremVertices( const HepMC::GenEvent* genEvent ) const
{

  std::vector<HepPoint3D> positionsOfVertices;

  // Loop over all verticies and find those associated to the original track
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();

  for ( ; vertex != genEvent->vertices_end(); ++vertex ){

    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;

    // Loop over vertices and find ones with photons radiated
    HepMC::GenVertex::particles_out_const_iterator outgoingParticle = (*vertex)->particles_out_const_begin();

    for ( ; outgoingParticle != (*vertex)->particles_out_const_end(); ++outgoingParticle ){

      if ( (*outgoingParticle)->pdg_id() == 22 ){
	//positionsOfVertices.push_back( (*vertex)->point3d() );
	HepPoint3D photonPoint3d((*vertex)->point3d().x(),(*vertex)->point3d().y(),(*vertex)->point3d().z());
	positionsOfVertices.push_back( photonPoint3d );
      }
    }

  }

  return positionsOfVertices;

}
//============================================================================================================
bool Trk::SimpleAmbiguityProcessorTool::vertexAssociatedWithOriginalTrack( HepMC::GenVertex* genVertex) const
{

  bool originalParticleFlag = false;

  // Loop over all particles in vertex. Determine if it is a vertex of the original particle
  HepMC::GenVertex::particle_iterator particleAtVertex = genVertex->particles_begin(HepMC::family);

  for ( ; particleAtVertex != genVertex->particles_end(HepMC::family); ++particleAtVertex ){
      
    int originalParticle = ( (*particleAtVertex)->barcode() - 10001 ) % 1000000;

    if (originalParticle == 0 )
      originalParticleFlag = true;

  }

  return originalParticleFlag;

}
#endif



