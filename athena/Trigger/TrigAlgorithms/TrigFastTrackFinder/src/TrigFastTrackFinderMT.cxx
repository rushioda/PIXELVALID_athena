/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// TrigFastTrackFinder.cxx
// -------------------------------
// ATLAS Collaboration
//
// package created 16/04/2013 by Dmitry Emeliyanov (see ChangeLog for more details)
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <algorithm>
#include <memory>
#include <array>

#include <tbb/parallel_for.h>
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h" 

#include "TrkParameters/TrackParameters.h" 
#include "TrkTrack/Track.h" 
#include "TrkTrack/TrackInfo.h" 

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

#include "IRegionSelector/IRegSelSvc.h"

#include "TrigInDetEvent/TrigSiSpacePointBase.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "TrigInDetPattRecoEvent/TrigL2TimeoutException.h"
#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"


#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"

#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigInDetToolInterfaces/ITrigSpacePointConversionTool.h"
#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"
//#include "TrigInDetToolInterfaces/ITrigL2ResidualCalculator.h"
//#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigZFinder.h"

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "../src/TrigFastTrackFinderMT.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "CxxUtils/make_unique.h"

TrigFastTrackFinderMT::TrigFastTrackFinderMT(const std::string& name, ISvcLocator* pSvcLocator) : 

  AthAlgorithm(name, pSvcLocator), 
  m_numberingTool("TrigL2LayerNumberTool"), 
  m_spacePointTool("TrigSpacePointConversionTool"),
  m_TrigL2SpacePointTruthTool("TrigL2SpacePointTruthTool"),
  //m_trigL2ResidualCalculator("TrigL2ResidualCalculator"),
  m_trackMaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_trigInDetTrackFitter("TrigInDetTrackFitter"),
  m_trigZFinder("TrigZFinder"),
  m_trackSummaryTool("Trk::ITrackSummaryTool/ITrackSummaryTool"),
  m_ftkDataProviderSvc("TrigFTK_DataProviderSvc",name),
  m_roiCollectionKey(""),
  m_shift_x(0.0),
  m_shift_y(0.0),
  m_doCloneRemoval(true),
  m_ftkMode(false),
  m_ftkRefit(false),
  m_useBeamSpot(true),
  m_nfreeCut(5), 
  m_iBeamCondSvc(nullptr),
  m_nTracks(0),
  m_nPixSPsInRoI(0),
  m_nSCTSPsInRoI(0),
  m_currentStage(-1),
  m_outputCollectionSuffix(""),
  m_countTotalRoI(0),
  m_countRoIwithEnoughHits(0),
  m_countRoIwithTracks(0),
  m_nSignalPresent(0),
  m_nSignalDetected(0),
  m_nSignalTracked(0),
  m_nSignalClones(0),
  m_minSignalSPs(0),
  m_pixelId(0),
  m_sctId(0),
  m_idHelper(0),
  m_particleHypothesis(Trk::pion),
  m_useNewLayerNumberScheme(false)
{

  /** Doublet finding properties. */
  declareProperty("Doublet_FilterRZ",            m_tcs.m_doubletFilterRZ = true);
  declareProperty("DoubletDR_Max",            m_tcs.m_doublet_dR_Max = 270.0);
  declareProperty("SeedRadBinWidth",            m_tcs.m_seedRadBinWidth = 2.0);

  /** Triplet finding properties. */

  declareProperty("Triplet_D0Max",            m_tcs.m_tripletD0Max      = 4.0);
  declareProperty("Triplet_D0_PPS_Max",       m_tcs.m_tripletD0_PPS_Max = 1.7);
  declareProperty("Triplet_nMaxPhiSlice",     m_tcs.m_nMaxPhiSlice = 53);
  declareProperty("Triplet_MaxBufferLength",     m_tcs.m_maxTripletBufferLength = 3);
  declareProperty("TripletDoPSS",            m_tcs.m_tripletDoPSS = false);


  declareProperty( "VertexSeededMode",    m_vertexSeededMode = false);
  declareProperty( "doZFinder",           m_doZFinder = true);

  declareProperty( "doFastZVertexSeeding",           m_doFastZVseeding = true);
  declareProperty( "zVertexResolution",           m_tcs.m_zvError = 10.0);

  declareProperty("Triplet_MinPtFrac",        m_tripletMinPtFrac = 0.3);
  declareProperty("pTmin",                    m_pTmin = 1000.0);
  declareProperty("TrackInitialD0Max",            m_initialD0Max      = 10.0);

  declareProperty("doSeedRedundancyCheck",            m_checkSeedRedundancy = false);

  declareProperty( "MinHits",               m_minHits = 5 );

  declareProperty( "OutputCollectionSuffix",m_outputCollectionSuffix = "");
 
  declareProperty( "UseBeamSpot",           m_useBeamSpot = true);
  declareProperty( "FreeClustersCut"   ,m_nfreeCut      );
  declareProperty( "SpacePointProviderTool", m_spacePointTool  );
  declareProperty( "LayerNumberTool", m_numberingTool  );

  declareProperty( "initialTrackMaker", m_trackMaker);
  declareProperty( "trigInDetTrackFitter",   m_trigInDetTrackFitter );
  declareProperty( "trigZFinder",   m_trigZFinder );

  declareProperty("TrackSummaryTool", m_trackSummaryTool);
  declareProperty( "TrigL2SpacePointTruthTool", m_TrigL2SpacePointTruthTool);
  declareProperty( "retrieveBarCodes", m_retrieveBarCodes = false);
  declareProperty( "SignalBarCodes", m_vSignalBarCodes);
  declareProperty( "MinSignalSPs", m_minSignalSPs = 3);
  declareProperty( "doResMon",       m_doResMonitoring = true);

  declareProperty("doCloneRemoval", m_doCloneRemoval = true);

  declareProperty("FTK_Mode",            m_ftkMode = false);
  declareProperty("FTK_DataProviderService",             m_ftkDataProviderSvc);
  declareProperty("FTK_Refit",           m_ftkRefit = false);

  declareProperty("useNewLayerNumberScheme", m_useNewLayerNumberScheme = false);

  declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");
  declareProperty("Tracks", m_outputTracksKey = std::string("FTF_Tracks"), "FTF tracks");

  // declare monitoring histograms

  //declareMonitoredStdContainer("trk_pt",         m_trk_pt);
  //declareMonitoredStdContainer("trk_a0",         m_trk_a0);
  //declareMonitoredStdContainer("trk_z0",         m_trk_z0);
  //declareMonitoredStdContainer("trk_phi0",       m_trk_phi0);
  //declareMonitoredStdContainer("trk_eta",        m_trk_eta);
  //declareMonitoredStdContainer("trk_chi2dof",    m_trk_chi2dof);
  //declareMonitoredStdContainer("trk_nSiHits",    m_trk_nSiHits);
  //declareMonitoredStdContainer("trk_nPIXHits",   m_trk_nPIXHits);
  //declareMonitoredStdContainer("trk_nSCTHits",   m_trk_nSCTHits);
  //declareMonitoredStdContainer("trk_a0beam",     m_trk_a0beam);
  //declareMonitoredStdContainer("trk_dPhi0",      m_trk_dPhi0);
  //declareMonitoredStdContainer("trk_dEta" ,      m_trk_dEta);

  //declareMonitoredVariable("roi_nSeeds",m_nSeeds);
  //declareMonitoredVariable("roi_nTracks",m_nTracks);
  //declareMonitoredVariable("roi_nSPsPIX",m_nPixSPsInRoI);
  //declareMonitoredVariable("roi_nSPsSCT",m_nSCTSPsInRoI);
  //declareMonitoredVariable("roi_lastStageExecuted",m_currentStage);
  //declareMonitoredVariable("roi_eta", m_roiEta);
  //declareMonitoredVariable("roi_etaWidth", m_roiEtaWidth);
  //declareMonitoredVariable("roi_phi", m_roiPhi);
  //declareMonitoredVariable("roi_phiWidth", m_roiPhiWidth);
  //declareMonitoredVariable("roi_z", m_roiZ);
  //declareMonitoredVariable("roi_zWidth", m_roiZ_Width);
  //declareMonitoredVariable("roi_nSPs", m_roi_nSPs);
  //declareMonitoredVariable("time_PattRecoOnly",m_timePattReco);

  //// Z-vertexing 
  //declareMonitoredVariable("roi_nZvertices",m_nZvertices);
  //declareMonitoredStdContainer("roi_zVertices",m_zVertices);
  ////Spacepoints
  //declareMonitoredStdContainer("sp_x" ,m_sp_x);
  //declareMonitoredStdContainer("sp_y" ,m_sp_y);
  //declareMonitoredStdContainer("sp_z" ,m_sp_z);
  //declareMonitoredStdContainer("sp_r" ,m_sp_r);

  //Unbiased residuals
  //declareMonitoredStdContainer("hit_IBLPhiResidual",m_iblResPhi);
  //declareMonitoredStdContainer("hit_IBLEtaResidual",m_iblResEta);
  //declareMonitoredStdContainer("hit_IBLPhiPull",    m_iblPullPhi);
  //declareMonitoredStdContainer("hit_IBLEtaPull",    m_iblPullEta);
  //declareMonitoredStdContainer("hit_PIXBarrelPhiResidual",m_pixResPhiBarrel);
  //declareMonitoredStdContainer("hit_PIXBarrelEtaResidual",m_pixResEtaBarrel);
  //declareMonitoredStdContainer("hit_PIXBarrelPhiPull",    m_pixPullPhiBarrel);
  //declareMonitoredStdContainer("hit_PIXBarrelEtaPull",    m_pixPullEtaBarrel);
  //declareMonitoredStdContainer("hit_SCTBarrelResidual",   m_sctResBarrel);
  //declareMonitoredStdContainer("hit_SCTBarrelPull",       m_sctPullBarrel);
  //declareMonitoredStdContainer("hit_PIXEndCapPhiResidual",m_pixResPhiEC);
  //declareMonitoredStdContainer("hit_PIXEndCapEtaResidual",m_pixResEtaEC);
  //declareMonitoredStdContainer("hit_PIXEndCapPhiPull",    m_pixPullPhiEC);
  //declareMonitoredStdContainer("hit_PIXEndCapEtaPull",    m_pixPullEtaEC);
  //declareMonitoredStdContainer("hit_SCTEndCapResidual",   m_sctResEC);
  //declareMonitoredStdContainer("hit_SCTEndCapPull",       m_sctPullEC);
  
}

//--------------------------------------------------------------------------

TrigFastTrackFinderMT::~TrigFastTrackFinderMT() {}

//-----------------------------------------------------------------------

StatusCode TrigFastTrackFinderMT::initialize() {

  ATH_MSG_DEBUG("TrigFastTrackFinderMT::initialize() "  << PACKAGE_VERSION);

  ATH_CHECK( m_roiCollectionKey.initialize() );
  ATH_CHECK( m_outputTracksKey.initialize() );

  if(m_ftkMode) {
    ATH_CHECK(m_ftkDataProviderSvc.retrieve());
  } else {
    ATH_CHECK(m_trackSummaryTool.retrieve());
    ATH_MSG_DEBUG(" TrigFastTrackFinder : MinHits set to " << m_minHits);
    if (m_useBeamSpot) {
      ATH_CHECK(service("BeamCondSvc", m_iBeamCondSvc));
    }
    
    ATH_CHECK(m_numberingTool.retrieve());
    
    ATH_CHECK(m_spacePointTool.retrieve());
    
    ATH_CHECK(m_trackMaker.retrieve());
    ATH_CHECK(m_trigInDetTrackFitter.retrieve());
    
    if (m_doZFinder) {
      ATH_CHECK(m_trigZFinder.retrieve());
    }

    //ATH_CHECK(m_trigL2ResidualCalculator.retrieve());
  }

  //Get ID helper
  //ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID").isFailure());
  //ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID").isFailure());

  //ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID").isFailure()); 

  if (m_retrieveBarCodes) {
    m_nSignalPresent=0;
    m_nSignalDetected=0;
    m_nSignalTracked=0;
    m_nSignalClones=0;
    ATH_CHECK(m_TrigL2SpacePointTruthTool.retrieve());
  }
  
  ATH_MSG_DEBUG(" doResMon " << m_doResMonitoring);
  ATH_MSG_DEBUG(" Initialized successfully"); 
  return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------

StatusCode TrigFastTrackFinderMT::beginRun()
{
  ATH_MSG_DEBUG("At BeginRun of " << name());

  //getting magic numbers from the layer numbering tool

  m_tcs.m_maxBarrelPix    = m_numberingTool->offsetBarrelSCT();
  m_tcs.m_minEndcapPix    = m_numberingTool->offsetEndcapPixels(); 
  m_tcs.m_maxEndcapPix    = m_numberingTool->offsetEndcapSCT();
  m_tcs.m_maxSiliconLayer = m_numberingTool->maxSiliconLayerNum();
  m_tcs.m_layerGeometry.clear();

  if(m_useNewLayerNumberScheme) {
    const std::vector<TRIG_INDET_SI_LAYER>* pVL = m_numberingTool->layerGeometry();
    std::copy(pVL->begin(),pVL->end(),std::back_inserter(m_tcs.m_layerGeometry));
  }

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

StatusCode TrigFastTrackFinderMT::execute() {

  ATH_MSG_VERBOSE("TrigFastTrackFinderMT::execHLTAlgorithm()");

  clearMembers();

  // 2. Retrieve beam spot and magnetic field information 
  //

  m_shift_x=0.0;
  m_shift_y=0.0;
  if(m_useBeamSpot && m_iBeamCondSvc) {
    getBeamSpot();

  }
  else {
    m_vertex = Amg::Vector3D(0.0,0.0,0.0);
  }
  

  
  
  // 4. RoI preparation/update 
  SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey);
  ATH_CHECK(roiCollection.isValid());
  TrigRoiDescriptorCollection::const_iterator roi = roiCollection->begin();
  TrigRoiDescriptorCollection::const_iterator roiE = roiCollection->end();
  TrigRoiDescriptor internalRoI;
  for (; roi != roiE; ++roi) {
    internalRoI.push_back(*roi);
  }
  internalRoI.manageConstituents(false);//Don't try to delete RoIs at the end
  m_currentStage = 1;
  m_countTotalRoI++;

  SG::WriteHandle<TrackCollection> outputTracks(m_outputTracksKey);
  //outputTracks = CxxUtils::make_unique<TrackCollection>();
  if(m_ftkMode) {
    outputTracks = CxxUtils::make_unique<TrackCollection>(*(m_ftkDataProviderSvc->getTracksInRoi(internalRoI, m_ftkRefit)));//This is a view container
    if ( m_ftkRefit ) {
      ATH_MSG_DEBUG("FTK_DataProviderSvc returned " << outputTracks->size() << " refitted tracks");
    } else {
      ATH_MSG_DEBUG("FTK_DataProviderSvc returned " << outputTracks->size() << " raw tracks");
    }
    if (outputTracks->size()> 0) {
      m_countRoIwithEnoughHits++;
      m_countRoIwithTracks++;
      // fill vectors of quantities to be monitored
      fillMon(*outputTracks);
    }
    m_nTracks=outputTracks->size();

    return StatusCode::SUCCESS;
  }
  else {
    StatusCode sc(StatusCode::SUCCESS);
    m_tcs.roiDescriptor = &internalRoI;
  
    std::vector<TrigSiSpacePointBase> convertedSpacePoints;
    convertedSpacePoints.reserve(5000);
    sc = m_spacePointTool->getSpacePoints( internalRoI, convertedSpacePoints, m_nPixSPsInRoI, m_nSCTSPsInRoI);
    
    ////Record spacepoint x and y
    //for(std::vector<TrigSiSpacePointBase>::const_iterator spIt = convertedSpacePoints.begin(); spIt != convertedSpacePoints.end(); ++spIt) {
    //  m_sp_x.push_back((*spIt).original_x());
    //  m_sp_y.push_back((*spIt).original_y());
    //  m_sp_z.push_back((*spIt).z());
    //  m_sp_r.push_back((*spIt).r());
    //}
    /*    
    std::vector<int> lCounter;
    for(int l=0;l<50;l++) lCounter.push_back(0);
    for(std::vector<TrigSiSpacePointBase>::const_iterator spIt = convertedSpacePoints.begin(); spIt != convertedSpacePoints.end(); ++spIt) {
      if((*spIt).layer()>31 || (*spIt).layer()<0) {
	std::cout<<"Wrong layer "<<(*spIt).layer()<<" z="<<(*spIt).z()<<" r="<<(*spIt).r()<<std::endl;
      }
      else lCounter[(*spIt).layer()]++;
    }
    int nTotal=0;
    for(int l=0;l<50;l++) {
      nTotal+=lCounter[l];
      std::cout<<"L="<<l<<" nSP="<<lCounter[l]<<std::endl;
    }
    std::cout<<"Total "<<nTotal<<" spacepoints out of "<<convertedSpacePoints.size()<<std::endl;

    //  m_sp_x.push_back((*spIt).original_x());
    //  m_sp_y.push_back((*spIt).original_y());
    //  m_sp_z.push_back((*spIt).z());
    //  m_sp_r.push_back((*spIt).r());
    //}
    */

    if(sc.isFailure()) { 
      ATH_MSG_WARNING("REGTEST / Failed to retrieve offline spacepoints ");
      outputTracks = CxxUtils::make_unique<TrackCollection>();
      return sc;
    }
    
    
    m_roi_nSPs = convertedSpacePoints.size();    
    
    if( m_roi_nSPs >= m_minHits ) {
      ATH_MSG_DEBUG("REGTEST / Found " << m_roi_nSPs << " space points.");
      m_countRoIwithEnoughHits++;
    }
    else {
      ATH_MSG_DEBUG("No tracks found - too few hits in ROI to run " << m_roi_nSPs);
        outputTracks = CxxUtils::make_unique<TrackCollection>();
        return StatusCode::SUCCESS;
    }

    m_currentStage = 2;
    
    std::unique_ptr<TrigRoiDescriptor> superRoi = CxxUtils::make_unique<TrigRoiDescriptor>();
    
    if (m_doZFinder) {
      m_tcs.m_vZv.clear();
      superRoi->setComposite(true);

      TrigVertexCollection* vertexCollection = m_trigZFinder->findZ( convertedSpacePoints, internalRoI);

      ATH_MSG_DEBUG("vertexCollection->size(): " << vertexCollection->size());

      for (auto vertex : *vertexCollection) {
        ATH_MSG_DEBUG("REGTEST / ZFinder vertex: " << *vertex);
        float z      = vertex->z();
        float zMinus = z - 7.0;
        float zPlus  = z + 7.0;
        TrigRoiDescriptor* roi =  new TrigRoiDescriptor(internalRoI.eta(), internalRoI.etaMinus(), internalRoI.etaPlus(), 
            internalRoI.phi(), internalRoI.phiMinus(), internalRoI.phiPlus(), z, zMinus, zPlus);
        superRoi->push_back(roi);
        m_zVertices.push_back(z);
        m_tcs.m_vZv.push_back(z);
      }
      m_tcs.roiDescriptor = superRoi.get();
      ATH_MSG_DEBUG("REGTEST / superRoi: " << *superRoi);
      delete vertexCollection;
    }

    m_currentStage = 3;
        
    if (m_retrieveBarCodes) {
      std::vector<int> vBar;
      m_TrigL2SpacePointTruthTool->getBarCodes(convertedSpacePoints,vBar);
      
      //for(auto barCode : vBar) std::cout<<"SP bar code = "<<barCode<<std::endl;
    } 
    
    m_tcs.m_tripletPtMin = m_tripletMinPtFrac*m_pTmin;
    ATH_MSG_VERBOSE("m_tcs.m_tripletPtMin: " << m_tcs.m_tripletPtMin);
    ATH_MSG_VERBOSE("m_pTmin: " << m_pTmin);
    
    
    std::map<int, int> nGoodRejected;
    std::map<int, int> nGoodAccepted;
    std::map<int, int> nGoodTotal;
    if(m_retrieveBarCodes) {
      for(auto barCode : m_vSignalBarCodes) {
        nGoodRejected.insert(std::pair<int,int>(barCode,0));
        nGoodAccepted.insert(std::pair<int,int>(barCode,0));
        nGoodTotal.insert(std::pair<int,int>(barCode,0));
      }
    }
    
    int iSeed=0;

    
    
    TRIG_TRACK_SEED_GENERATOR seedGen(m_tcs);

    seedGen.loadSpacePoints(convertedSpacePoints);

    if (m_doZFinder && m_doFastZVseeding) seedGen.createSeedsZv();
    else seedGen.createSeeds();
    
    std::vector<TrigInDetTriplet*> triplets;
    seedGen.getSeeds(triplets);
    
    ATH_MSG_DEBUG("number of triplets: " << triplets.size());
    
    m_currentStage = 4;
    
    
    // 8. Combinatorial tracking
    
    std::vector<int> vTBarCodes(triplets.size(),-1);
    
    if(m_retrieveBarCodes) {
      assignTripletBarCodes(triplets, vTBarCodes);
    }
    
    std::vector<std::tuple<bool, double,Trk::Track*>> qualityTracks; //bool used for later filtering
    qualityTracks.reserve(triplets.size());
    
    m_nSeeds  = 0;
    iSeed=0;
    
    long int trackIndex=0;
    
    if(m_checkSeedRedundancy) m_siClusterMap.clear();
    
    bool PIX = true;
    bool SCT = true;
    
    m_trackMaker->newTrigEvent(PIX,SCT);
    
    for(unsigned int tripletIdx=0;tripletIdx!=triplets.size();tripletIdx++) {
      
      TrigInDetTriplet* seed = triplets[tripletIdx];
      
      const Trk::SpacePoint* osp1 = seed->s1().offlineSpacePoint();
      const Trk::SpacePoint* osp2 = seed->s2().offlineSpacePoint();
      const Trk::SpacePoint* osp3 = seed->s3().offlineSpacePoint();
      
      if(m_checkSeedRedundancy) {
      //check if clusters do not belong to any track
        std::vector<Identifier> clusterIds;
        extractClusterIds(osp1, clusterIds);
        extractClusterIds(osp2, clusterIds);
        extractClusterIds(osp3, clusterIds);
        if(usedByAnyTrack(clusterIds, m_siClusterMap)) {
          continue;
        }
      }
      
      std::list<const Trk::SpacePoint*> spList = {osp1, osp2, osp3};
      
      bool trackFound=false;
      
      ++m_nSeeds;
      
      const std::list<Trk::Track*>& tracks = m_trackMaker->getTracks(spList);
      
      for(std::list<Trk::Track*>::const_iterator t=tracks.begin(); t!=tracks.end(); ++t) {
        if((*t)) {
          float d0 = (*t)->perigeeParameters()==0 ? 10000.0 : (*t)->perigeeParameters()->parameters()[Trk::d0]; 
          if (fabs(d0) > m_initialD0Max) {
            ATH_MSG_DEBUG("REGTEST / Reject track with d0 = " << d0 << " > " << m_initialD0Max);
            qualityTracks.push_back(std::make_tuple(false,0,(*t)));//Flag track as bad, but keep in vector for later deletion
            continue;
          }
          if(m_checkSeedRedundancy) {
            //update clusterMap 
            updateClusterMap(trackIndex++, (*t), m_siClusterMap);
          }
          if(m_doCloneRemoval) {
            qualityTracks.push_back(std::make_tuple(true, -trackQuality((*t)), (*t)));
          }
          else {
            qualityTracks.push_back(std::make_tuple(true, 0, (*t)));
          }
        }
      }  
      iSeed++;
      ATH_MSG_VERBOSE("Found "<<tracks.size()<<" tracks using triplet");
      if(!tracks.empty()) {
        trackFound = true;
      }

      if(m_retrieveBarCodes) {
        bool goodTriplet=false;
        int foundBarCode=-1;

        for(auto barCode : m_vSignalBarCodes) {
          if (vTBarCodes[tripletIdx] == barCode) {
            foundBarCode=barCode;
            goodTriplet=true;break;
          }
        }

        if(goodTriplet) {
          (*nGoodTotal.find(foundBarCode)).second++;
          if(trackFound) (*nGoodAccepted.find(foundBarCode)).second++;
          else (*nGoodRejected.find(foundBarCode)).second++;
        }
      }
    }

    m_trackMaker->endEvent();
    for(auto& seed : triplets) delete seed;

    //clone removal
    if(m_doCloneRemoval) {
      filterSharedTracks(qualityTracks);
    }

    TrackCollection* initialTracks = new TrackCollection;
    initialTracks->reserve(qualityTracks.size());
    for(const auto& q : qualityTracks) {
      if (std::get<0>(q)==true) {
        initialTracks->push_back(std::get<2>(q));
      }
      else {
        delete std::get<2>(q);
      }
    }
    qualityTracks.clear();

    ATH_MSG_DEBUG("After clone removal "<<initialTracks->size()<<" tracks left");


    m_currentStage = 5;



    if (m_retrieveBarCodes) {
      //reco. efficiency analysis
      calculateRecoEfficiency(convertedSpacePoints, nGoodTotal, nGoodAccepted);
    }


    TrackCollection* fittedTracks = m_trigInDetTrackFitter->fit(*initialTracks, m_particleHypothesis);
    delete initialTracks;

    if( fittedTracks->empty() ) {
      ATH_MSG_DEBUG("REGTEST / No tracks fitted");
    }

    for (auto fittedTrack = fittedTracks->begin(); fittedTrack!=fittedTracks->end(); ++fittedTrack) {
      (*fittedTrack)->info().setPatternRecognitionInfo(Trk::TrackInfo::FastTrackFinderSeed);
      ATH_MSG_VERBOSE("Updating fitted track: " << **fittedTrack);
      m_trackSummaryTool->updateTrack(**fittedTrack);
      ATH_MSG_VERBOSE("Updated track: " << **fittedTrack);
    }
    outputTracks = CxxUtils::make_unique<TrackCollection>(*fittedTracks);
    //outputTracks = CxxUtils::make_unique<TrackCollection>();

    if( outputTracks->empty() ) {
      ATH_MSG_DEBUG("REGTEST / No tracks reconstructed");
    }
    m_currentStage = 6;

    //monitor Z-vertexing

    m_nZvertices=m_zVertices.size();

    //monitor number of tracks
    m_nTracks=outputTracks->size();
    ATH_MSG_DEBUG("REGTEST / Found " << m_nTracks << " tracks");
    if( !outputTracks->empty() )
      m_countRoIwithTracks++;

    ///////////// fill vectors of quantities to be monitored
    fillMon(*outputTracks);

    m_currentStage = 7;

    return StatusCode::SUCCESS;
  }
}

double TrigFastTrackFinderMT::trackQuality(const Trk::Track* Tr) {

  DataVector<const Trk::TrackStateOnSurface>::const_iterator  
    m  = Tr->trackStateOnSurfaces()->begin(), 
       me = Tr->trackStateOnSurfaces()->end  ();

  double quality = 0. ;
  const double W       = 17.;

  for(; m!=me; ++m) {
    const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
    if(!fq) continue;

    double x2 = fq->chiSquared();
    double q;
    if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
    else                     q =      (W-x2    );
    if(q < 0.) q = 0.;
    quality+=q;
  }
  return quality;
}

void TrigFastTrackFinderMT::filterSharedTracks(std::vector<std::tuple<bool, double,Trk::Track*>>& QT) {

  std::set<const Trk::PrepRawData*> clusters;

  const Trk::PrepRawData* prd[100];

  std::sort(QT.begin(), QT.end(),
      [](const std::tuple<bool, double, Trk::Track*>& lhs, const std::tuple<bool, double, Trk::Track*>& rhs) {
      return std::get<1>(lhs) < std::get<1>(rhs); } );

  for (auto& q : QT) {
    DataVector<const Trk::MeasurementBase>::const_iterator 
      m  = std::get<2>(q)->measurementsOnTrack()->begin(), 
         me = std::get<2>(q)->measurementsOnTrack()->end  ();

    int nf = 0, nc = 0; 
    for(; m!=me; ++m ) {

      const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
      if(pr) {
        ++nc;
        if(clusters.find(pr)==clusters.end()) {prd[nf++]=pr; if(nf==100) break;}
      }
    }
    if((nf >= m_nfreeCut) || (nf == nc) ) {
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]);
    }
    else  {
      std::get<0>(q) = false;
    }
  }
}

//---------------------------------------------------------------------------

StatusCode TrigFastTrackFinderMT::finalize()
{

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("TrigFastTrackFinderMT::finalize() - TrigFastTrackFinder Statistics: ");
  ATH_MSG_INFO("RoI processed: " <<  m_countTotalRoI);
  ATH_MSG_INFO("RoI with enough SPs : " <<  m_countRoIwithEnoughHits);
  ATH_MSG_INFO("RoI with Track(s)  Total/goodZvertex/badZvertex: " << m_countRoIwithTracks);
  if (m_retrieveBarCodes) {
    ATH_MSG_INFO("Number of signal tracks present " << m_nSignalPresent);
    ATH_MSG_INFO("Number of signal seeds  found   " << m_nSignalDetected);
    ATH_MSG_INFO("Number of signal tracks found   " << m_nSignalTracked);
    if(m_nSignalPresent!=0) {
      ATH_MSG_INFO("Track seeding  efficiency      " << (100.0*m_nSignalDetected/m_nSignalPresent) <<" % ");
      ATH_MSG_INFO("Track seeding  redundancy      " << (100.0*m_nSignalClones/m_nSignalPresent) << " %");
      ATH_MSG_INFO("Track finding efficiency       " << (100.0*m_nSignalTracked/m_nSignalPresent) << " %");
    }
  }
  ATH_MSG_INFO("=========================================================");

  return StatusCode::SUCCESS;
}

void TrigFastTrackFinderMT::updateClusterMap(long int trackIdx, const Trk::Track* pTrack, std::map<Identifier, std::vector<long int> >& clusterMap) {
  //loop over clusters

  for(auto tMOT = pTrack->measurementsOnTrack()->begin(); tMOT != pTrack->measurementsOnTrack()->end(); ++tMOT) { 

    const InDet::SiClusterOnTrack* siCLOT = dynamic_cast<const InDet::SiClusterOnTrack*>(*tMOT); 
    if (siCLOT==nullptr) continue;
    const InDet::SiCluster* siCL = dynamic_cast<const InDet::SiCluster*>(siCLOT->prepRawData()); 
    if (siCL==nullptr) continue;
    Identifier id = siCL->identify(); 
    clusterMap[id].push_back(trackIdx);
    //no sorting is needed as the vectors are sorted by the algorithm design
    //due to monotonically increasing trackIdx
    // std::map<Identifier, std::vector<long int> >::iterator itm = clusterMap.find(id);
    //std::sort((*itm).second.begin(),(*itm).second.end());
    //std::copy((*itm).second.begin(),(*itm).second.end(),std::ostream_iterator<long int>(std::cout," "));
    //std::cout<<std::endl;
  }
}

void TrigFastTrackFinderMT::extractClusterIds(const Trk::SpacePoint* pSP, std::vector<Identifier>& vIds) {
  const InDet::SiCluster* pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().first);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
  //check second cluster : SCT uv clusters only !
  pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().second);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
}

bool TrigFastTrackFinderMT::usedByAnyTrack(const std::vector<Identifier>& vIds, std::map<Identifier, std::vector<long int> >& clusterMap) {

  std::vector<long int> xSection;
  //initializing
  std::map<Identifier, std::vector<long int> >::iterator itm0 = clusterMap.find(*vIds.begin());
  if(itm0 == clusterMap.end()) return false;
  xSection.reserve((*itm0).second.size());
  std::copy((*itm0).second.begin(), (*itm0).second.end(), std::back_inserter(xSection));
  std::vector<Identifier>::const_iterator it = vIds.begin();it++;
  for(;it!=vIds.end();++it) {
    std::map<Identifier, std::vector<long int> >::iterator itm1 = clusterMap.find(*it);
    if(itm1 == clusterMap.end()) return false;
    std::vector<long int> tmp;
    std::set_intersection(xSection.begin(), xSection.end(), (*itm1).second.begin(),(*itm1).second.end(), std::back_inserter(tmp));
    if(tmp.empty()) return false;
    //update xSection
    xSection.clear();
    xSection.reserve(tmp.size());
    std::copy(tmp.begin(), tmp.end(), std::back_inserter(xSection));
  }
  return !xSection.empty();
}

int TrigFastTrackFinderMT::findBarCodeInData(int barCode, const std::vector<TrigSiSpacePointBase>& vSP) {
  int nFound=0;
  std::set<int> layerSet;
  for(auto sp : vSP) {
    if(barCode==sp.barCode()) {
      nFound++;
      layerSet.insert(sp.layer());
    }
  }
  if(int(layerSet.size())<m_minSignalSPs) {//less than N unique layers
    nFound=0;
  }
  return nFound;
}

void TrigFastTrackFinderMT::showBarCodeInData(int barCode, const std::vector<TrigSiSpacePointBase>& vSP) {
  for(auto sp : vSP) {
    if(barCode==sp.barCode()) {
      ATH_MSG_DEBUG("L="<<sp.layer()<<" r="<<sp.r()<<" z="<<sp.z());
    }
  }
}

int TrigFastTrackFinderMT::findBarCodeInTriplets(int barCode, const std::vector<std::shared_ptr<TrigInDetTriplet>>& vTR) {
  int nFound=0;
  for(auto tr : vTR) {
    bool found = (barCode == tr->s1().barCode()) && (barCode == tr->s2().barCode()) && (barCode == tr->s3().barCode());
    if(found) {
      nFound++;
    }
  }
  return nFound;
}

void TrigFastTrackFinderMT::assignTripletBarCodes(const std::vector<std::shared_ptr<TrigInDetTriplet>>& vTR, std::vector<int>& vBar) {
  int iTR=0;
  for(auto tr : vTR) {
    bool good = (tr->s1().barCode() == tr->s2().barCode()) && (tr->s3().barCode() == tr->s2().barCode());
    good = good && (tr->s1().barCode() > 0);
    if(good) {
      vBar[iTR] = tr->s1().barCode();
    }
    iTR++;
  }
}

void TrigFastTrackFinderMT::assignTripletBarCodes(const std::vector<TrigInDetTriplet*>& vTR, std::vector<int>& vBar) {
  int iTR=0;
  for(auto tr : vTR) {
    bool good = (tr->s1().barCode() == tr->s2().barCode()) && (tr->s3().barCode() == tr->s2().barCode());
    good = good && (tr->s1().barCode() > 0);
    if(good) {
      vBar[iTR] = tr->s1().barCode();
    }
    iTR++;
  }
}

void TrigFastTrackFinderMT::getBeamSpot() {
  m_vertex = m_iBeamCondSvc->beamPos();
  ATH_MSG_VERBOSE("Beam spot position " << m_vertex);
  double xVTX = m_vertex.x();
  double yVTX = m_vertex.y();
  double zVTX = m_vertex.z();
  double tiltXZ = m_iBeamCondSvc->beamTilt(0);
  double tiltYZ = m_iBeamCondSvc->beamTilt(1);
  m_shift_x = xVTX - tiltXZ*zVTX;//correction for tilt
  m_shift_y = yVTX - tiltYZ*zVTX;//correction for tilt
  ATH_MSG_VERBOSE("Beam center position:  " << m_shift_x <<"  "<< m_shift_y);
}

void TrigFastTrackFinderMT::clearMembers() {
  m_nTracks = 0;
  m_zVertices.clear();

  m_trk_pt.clear();
  m_trk_a0.clear();
  m_trk_z0.clear();
  m_trk_phi0.clear();
  m_trk_eta.clear();
  m_trk_chi2dof.clear();
  m_trk_nSiHits.clear();
  m_trk_nPIXHits.clear();
  m_trk_nSCTHits.clear();
  m_trk_a0beam.clear();
  m_trk_dPhi0.clear();
  m_trk_dEta.clear();

  //m_sp_x.clear();
  //m_sp_y.clear();
  //m_sp_z.clear();
  //m_sp_r.clear();

  m_iblResPhi.clear();
  m_iblResEta.clear();
  m_iblPullPhi.clear();
  m_iblPullEta.clear();
  m_pixResPhiBarrel.clear();
  m_pixResEtaBarrel.clear();
  m_pixPullPhiBarrel.clear();
  m_pixPullEtaBarrel.clear();
  m_sctResBarrel.clear();
  m_sctPullBarrel.clear();
  m_pixResPhiEC.clear();
  m_pixResEtaEC.clear();
  m_pixPullPhiEC.clear();
  m_pixPullEtaEC.clear();
  m_sctResEC.clear();
  m_sctPullEC.clear();



  m_nPixSPsInRoI=0;
  m_nSCTSPsInRoI=0;
  m_currentStage=0;
  m_roi_nSPs=0;
  m_nZvertices=0;
}

void TrigFastTrackFinderMT::calculateRecoEfficiency(const std::vector<TrigSiSpacePointBase>& convertedSpacePoints,
    const std::map<int,int>& nGoodTotal, 
    const std::map<int,int>& nGoodAccepted) {

  //reco. efficiency analysis
  for(auto barCode : m_vSignalBarCodes) {
    int nSignalSPs = findBarCodeInData(barCode, convertedSpacePoints);
    if(nSignalSPs<m_minSignalSPs) continue;
    m_nSignalPresent+=1;
    int nSignalTracks = (*nGoodTotal.find(barCode)).second;
    if(nSignalTracks==0) {
      continue;
    }
    m_nSignalDetected+=1;
    m_nSignalClones+=nSignalTracks;

    int nGoodTripletsAccepted = (*nGoodAccepted.find(barCode)).second;
    if(nGoodTripletsAccepted==0) continue;
    m_nSignalTracked+=1;
  } 
}

void TrigFastTrackFinderMT::fillMon(const TrackCollection& tracks) {
  size_t size = tracks.size();
  m_trk_pt.reserve(size);
  m_trk_a0.reserve(size);
  m_trk_z0.reserve(size);
  m_trk_phi0.reserve(size);
  m_trk_eta.reserve(size);
  m_trk_chi2dof.reserve(size);
  m_trk_nSiHits.reserve(size);
  m_trk_nPIXHits.reserve(size);
  m_trk_nSCTHits.reserve(size);
  m_trk_a0beam.reserve(size);
  m_trk_dPhi0.reserve(size);
  m_trk_dEta.reserve(size);
  for (auto track : tracks) {
    const Trk::TrackParameters* trackPars = track->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }

    if(trackPars->covariance()==nullptr) {
      continue;
    }

    float a0 = trackPars->parameters()[Trk::d0]; 
    m_trk_a0.push_back(a0);
    float z0 = trackPars->parameters()[Trk::z0]; 
    m_trk_z0.push_back(z0);
    float phi0 = trackPars->parameters()[Trk::phi0]; 
    m_trk_phi0.push_back(phi0);
    m_trk_a0beam.push_back(a0+m_shift_x*sin(phi0)-m_shift_y*cos(phi0));
    float dPhi0 = HLT::wrapPhi(phi0 - m_roiPhi);
    m_trk_dPhi0.push_back(dPhi0);
    float theta = trackPars->parameters()[Trk::theta]; 
    float eta = -log(tan(0.5*theta)); 
    m_trk_eta.push_back(eta);
    m_trk_dEta.push_back(eta - m_roiEta);

    float qOverP = trackPars->parameters()[Trk::qOverP]; 
    if (qOverP==0) {
      ATH_MSG_DEBUG("REGTEST / q/p == 0, adjusting to 1e-12");
      qOverP = 1e-12;
    }
    float pT=sin(theta)/qOverP;
    m_trk_pt.push_back(pT);

    const Trk::FitQuality* fq = track->fitQuality();
    float chi2 = 1e8;
    if (fq) {
      ATH_MSG_VERBOSE("Fitted chi2: " << fq->chiSquared());
      ATH_MSG_VERBOSE("Fitted ndof: " << fq->numberDoF());
      if(fq->numberDoF()!=0) {
        chi2 = fq->chiSquared()/fq->numberDoF();
      }
    }
    m_trk_chi2dof.push_back(chi2);
    int nPix=0, nSct=0;

    for(auto tSOS = track->trackStateOnSurfaces()->begin();  
        tSOS!=track->trackStateOnSurfaces()->end(); ++tSOS) { 
      if ((*tSOS)->type(Trk::TrackStateOnSurface::Perigee) == false) {
        const Trk::FitQualityOnSurface* fq =  (*tSOS)->fitQualityOnSurface(); 
        if(!fq) continue; 
        int nd = fq->numberDoF(); 
        if(nd==2) nPix++;
        if(nd==1) nSct++;
      }
    }
    if (m_ftkMode) {
      const Trk::TrackSummary* summary = track->trackSummary();
      if( summary != nullptr){
        nPix = summary->get(Trk::numberOfPixelHits);
        nSct = summary->get(Trk::numberOfSCTHits);
      }
    }
    m_trk_nPIXHits.push_back(nPix); 
    m_trk_nSCTHits.push_back(nSct/2); 
    m_trk_nSiHits.push_back(nPix + nSct/2); 

    ATH_MSG_DEBUG("REGTEST / track npix/nsct/phi0/pt/eta/d0/z0/chi2: " <<
        nPix   << " / "  << 
        nSct/2 << " / "  << 
        phi0   << " / "  << 
        pT     << " / " << 
        eta    << " / " << 
        a0     << " / " <<
        z0     << " / " <<
        chi2);
    // tighter selection for unbiased residuals
    bool goodTrack = std::fabs(pT)>1000. && (nPix + nSct/2) > 3 && nSct > 0;
    if (goodTrack && m_doResMonitoring) {
      runResidualMonitoring(*track);
    }
  }
}

void TrigFastTrackFinderMT::runResidualMonitoring(const Trk::Track& /*track*/) {
  //std::vector<TrigL2HitResidual> vResid;
  //vResid.clear();
  //StatusCode scRes = m_trigL2ResidualCalculator->getUnbiassedResiduals(track,vResid);
  //if(!scRes.isSuccess()) return;
  //for(std::vector<TrigL2HitResidual>::iterator it=vResid.begin();it!=vResid.end();++it) {
  //  switch(it->regionId()) {
  //    case Region::PixBarrel :
  //      m_pixResPhiBarrel.push_back(it->phiResidual());
  //      m_pixPullPhiBarrel.push_back(it->phiPull());
  //      m_pixResEtaBarrel.push_back(it->etaResidual());
  //      m_pixPullEtaBarrel.push_back(it->etaPull());
  //      break;
  //    case Region::PixEndcap :
  //      m_pixResPhiEC.push_back(it->phiResidual());
  //      m_pixPullPhiEC.push_back(it->phiPull());
  //      m_pixResEtaEC.push_back(it->etaResidual());
  //      m_pixPullEtaEC.push_back(it->etaPull());
  //      break;
  //    case Region::SctBarrel :
  //      m_sctResBarrel.push_back(it->phiResidual());
  //      m_sctPullBarrel.push_back(it->phiPull());
  //      break;
  //    case Region::SctEndcap :
  //      m_sctResEC.push_back(it->phiResidual());
  //      m_sctPullEC.push_back(it->phiPull());
  //      break;
  //    case Region::IBL :
  //      if (m_tcs.m_maxSiliconLayer==20) {
  //        m_iblResPhi.push_back(it->phiResidual());
  //        m_iblPullPhi.push_back(it->phiPull());
  //        m_iblResEta.push_back(it->etaResidual());
  //        m_iblPullEta.push_back(it->etaPull());
  //      }
  //      else {//No IBL, fill pixel histograms instead
  //        m_pixResPhiBarrel.push_back(it->phiResidual());
  //        m_pixPullPhiBarrel.push_back(it->phiPull());
  //        m_pixResEtaBarrel.push_back(it->etaResidual());
  //        m_pixPullEtaBarrel.push_back(it->etaPull());
  //      }
  //      break;
  //    case Region::Undefined :
  //      ATH_MSG_DEBUG("Undefined ID region");
  //      break;
  //  }
  //}
}

