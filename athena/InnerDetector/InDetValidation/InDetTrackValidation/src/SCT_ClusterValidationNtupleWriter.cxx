/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ClusterValidationNtupleWriter.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "InDetTrackValidation/SCT_ClusterValidationNtupleWriter.h"
// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/LocalPosition.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetIdentifier/SCT_ID.h"
//#include "InDetTrackValidation/SCT_ClusterStruct.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_Cabling/SCT_OnlineId.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "DataModel/DataVector.h"

#include "xAODEventInfo/EventInfo.h"

#include "InDetPrepRawData/SCT_Cluster.h"

 #include "EventPrimitives/EventPrimitivesHelpers.h"

//using HepGeom::Point3D;
using CLHEP::GeV;

InDet::SCT_ClusterValidationNtupleWriter::SCT_ClusterValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator):
        AthAlgorithm(name,pSvcLocator),
        m_riocontainer(0),
        //m_inputTrackCollection("Tracks")
        //        m_fullNtupleName("/NTUPLES/FILE1/FitterValidation/TrackStatistics"),
        mjo_riocontainername("SCT_Clusters"),
        m_dataObjectName("SCT_RDOs"),
	m_spacePointContainerName("SCT_SpacePoints"),
	m_inputTrackCollection("CombinedInDetTracks"),
        m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc",name),
        m_cabling("SCT_CablingSvc",name),
        m_ntupleFileName("/NTUPLES/FILE1"), 
        m_ntupleDirName("FitterValidation"),
        m_ntupleTreeName("RIOs"),
	m_fillCluster(false),
        m_fillRDO(true),
	m_fillSpacePoint(false),
        m_fillBSErrs(true),
	m_doHitsOnTracks(false),
        m_nt(0),
        m_rioLoc1(0),
        m_rioSurfaceX(0),
        m_rioSurfaceY(0),
        m_rioSurfaceZ(0),
        m_SctBarrelEndcap(0),
        m_SctLayerDisk(0),
        m_SctEtaModule(0),
        m_SctPhiModule(0),
        m_SctSide(0),
        m_SctDeltaPhi(0),
        m_SctHitErr(0),
	m_nSP(0),
	m_sp_bec(0),
	m_sp_layer(0),
	m_sp_eta(0),
	m_sp_phi(0),
	m_sp_x(0),
	m_sp_y(0),
	m_sp_z(0),
	m_nRDOs(0),
        m_sct_rdoGroupSize(0),
	m_sct_rdoIsOnTrack(0),
        m_sct_layer(0),
        m_sct_eta(0),
        m_sct_phi(0),
        m_sct_side(0),
        m_sct_barrelec(0),
        m_sct_firstStrip(0),
        m_sct_numHitsInWafer(0),
        m_sct_waferHash(0),
	m_sct_tbin(0),
	m_sct_rodid(0),
	m_sct_channel(0),
	m_scterr_bec(0),
	m_scterr_layer(0),
	m_scterr_eta(0),
	m_scterr_phi(0),
	m_scterr_side(0),
	m_scterr_rodid(0),
	m_scterr_channel(0),
	m_scterr_type(0)

{
    declareProperty("SCT_ClusterContainer", mjo_riocontainername);
    declareProperty("SCT_RDOContainer", m_dataObjectName);
    declareProperty("SCT_SpacePointContainer", m_spacePointContainerName);
    declareProperty("SCT_InputTrackCollection", m_inputTrackCollection);
    declareProperty("NtupleFileName", m_ntupleFileName);
    declareProperty("NtupleDirectoryName", m_ntupleDirName);
    declareProperty("NtupleTreeName", m_ntupleTreeName);
    declareProperty("ByteStreamErrService", m_byteStreamErrSvc);
    declareProperty("FillCluster",        m_fillCluster);
    declareProperty("FillSpacePoint",        m_fillSpacePoint);
    declareProperty("FillRDO",        m_fillRDO);
    declareProperty("FillBSErrs",        m_fillBSErrs);
    declareProperty("DoHitsOnTracks",        m_doHitsOnTracks);
}

InDet::SCT_ClusterValidationNtupleWriter::~SCT_ClusterValidationNtupleWriter() {}

StatusCode InDet::SCT_ClusterValidationNtupleWriter::initialize() {

    if (detStore()->retrieve(m_sctid, "SCT_ID").isFailure()) {
      ATH_MSG_FATAL ("Could not get SCT ID helper");
      return StatusCode::FAILURE;
    }

    if (m_fillBSErrs) {
      if (m_byteStreamErrSvc.retrieve().isFailure()) {
        ATH_MSG_ERROR("Failed to retrieve service " << m_byteStreamErrSvc);
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Retrieved service " << m_byteStreamErrSvc);
      }
      if (m_cabling.retrieve().isFailure()) {
        ATH_MSG_ERROR("Failed to retrieve service " << m_cabling);
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Retrieved service " << m_cabling);
      }
    }


    // ---------------------------
    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    StatusCode sc =  service("THistSvc", tHistSvc);
    if ( sc.isFailure() ) {
        ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
        return sc;
    }

    // ---------------------------
    // create tree and register it to THistSvc
    m_nt = new TTree(TString(m_ntupleTreeName), "SCT_Clusters");
    // NB: we must not delete the tree, this is done by THistSvc
    std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
    sc = tHistSvc->regTree(fullNtupleName, m_nt);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Unable to register TTree : " << fullNtupleName );
        return sc;
    }
    //-----------------
    // add items
    // event info:

    m_nt->Branch("SCT_RunNumber",          &m_runNumber,     "run_number/I");      
    m_nt->Branch("SCT_EventNumber",        &m_eventNumber,   "event_number/I");      
    m_nt->Branch("SCT_LumiBlock",          &m_lumiBlock,     "lumi_block/I");      
    m_nt->Branch("SCT_TimeStamp",          &m_timeStamp,     "time_stamp/I");      
    m_nt->Branch("SCT_BunchCrossing",      &m_bunchCrossing, "bunch_crossing/I");      
    m_nt->Branch("SCT_nRIOs",              &m_nRIOs,         "Number_of_RIOs/I");
    
    // RIO info:
    m_rioLoc1       = new std::vector<float>();
    m_rioSurfaceX   = new std::vector<float>();
    m_rioSurfaceY   = new std::vector<float>();
    m_rioSurfaceZ   = new std::vector<float>();
    // SCT module info
    m_SctBarrelEndcap= new  std::vector<int>();
    m_SctLayerDisk   = new  std::vector<int>();
    m_SctEtaModule   = new  std::vector<int>();
    m_SctPhiModule   = new  std::vector<int>();
    m_SctSide        = new  std::vector<int>();
    m_SctDeltaPhi    = new  std::vector<float>();
    m_SctHitErr      = new  std::vector<float>();
    // SCT RDO info
    m_sct_rdoGroupSize            = new  std::vector<int>();
    m_sct_rdoIsOnTrack            = new  std::vector<int>();
    m_sct_layer                   = new  std::vector<int>();
    m_sct_eta                     = new  std::vector<int>();
    m_sct_phi                     = new  std::vector<int>();
    m_sct_side                    = new  std::vector<int>();
    m_sct_barrelec                = new  std::vector<int>();
    m_sct_firstStrip              = new  std::vector<int>();
    m_sct_numHitsInWafer          = new  std::vector<int>();
    m_sct_waferHash               = new  std::vector<int>();
    m_sct_tbin                    = new  std::vector<int>();
    m_sct_rodid                   = new  std::vector<int>();
    m_sct_channel                 = new  std::vector<int>();
    // SCT SpacePoint info
    m_sp_bec                      = new  std::vector<int>();
    m_sp_layer                    = new  std::vector<int>();
    m_sp_eta                      = new  std::vector<int>();
    m_sp_phi                      = new  std::vector<int>();
    m_sp_x                        = new  std::vector<float>();
    m_sp_y                        = new  std::vector<float>();
    m_sp_z                        = new  std::vector<float>();
    // SCT BS error info
    m_scterr_bec                  = new  std::vector<int>();
    m_scterr_layer                = new  std::vector<int>();
    m_scterr_eta                  = new  std::vector<int>();
    m_scterr_phi                  = new  std::vector<int>();
    m_scterr_side                 = new  std::vector<int>();
    m_scterr_rodid                = new  std::vector<int>();
    m_scterr_channel              = new  std::vector<int>();
    m_scterr_type                 = new  std::vector<int>();

    if (m_fillCluster) {
      m_nt->Branch("SCT_DriftRadius", &m_rioLoc1); 
      m_nt->Branch("SCT_SurfaceX",    &m_rioSurfaceX); 
      m_nt->Branch("SCT_SurfaceY",    &m_rioSurfaceY); 
      m_nt->Branch("SCT_SurfaceZ",    &m_rioSurfaceZ); 
      
      m_nt->Branch("SCT_BarrelEndcap", &m_SctBarrelEndcap);
      m_nt->Branch("SCT_LayerDisk",    &m_SctLayerDisk);
      m_nt->Branch("SCT_EtaModule",    &m_SctEtaModule);
      m_nt->Branch("SCT_PhiModule",    &m_SctPhiModule);
      m_nt->Branch("SCT_Side",         &m_SctSide);
      m_nt->Branch("SCT_DeltaPhi",     &m_SctDeltaPhi);
      m_nt->Branch("SCT_HitErr",       &m_SctHitErr);
    }
    if (m_fillSpacePoint) {
      m_nt->Branch("SP_SCT_nSpacePoints", &m_nSP);
      m_nt->Branch("SP_SCT_BarrelEndcap", &m_sp_bec);
      m_nt->Branch("SP_SCT_EtaModule",    &m_sp_eta);
      m_nt->Branch("SP_SCT_PhiModule",    &m_sp_phi);
      m_nt->Branch("SP_SCT_Layer",        &m_sp_layer);
      m_nt->Branch("SP_SCT_X",            &m_sp_x);
      m_nt->Branch("SP_SCT_Y",            &m_sp_y);
      m_nt->Branch("SP_SCT_Z",            &m_sp_z);
    }
    if (m_fillRDO) {
      m_nt->Branch("RDO_SCT_nRDOs",                  &m_nRDOs);
      m_nt->Branch("RDO_SCT_RDO_Group_Size",         &m_sct_rdoGroupSize);
      m_nt->Branch("RDO_SCT_Layer",                  &m_sct_layer);
      m_nt->Branch("RDO_SCT_Eta",                    &m_sct_eta);
      m_nt->Branch("RDO_SCT_Phi",                    &m_sct_phi);
      m_nt->Branch("RDO_SCT_Side",                   &m_sct_side);
      m_nt->Branch("RDO_SCT_Barrelec",               &m_sct_barrelec);
      m_nt->Branch("RDO_SCT_FirstStrip",             &m_sct_firstStrip);
      m_nt->Branch("RDO_SCT_NumHitsInWafer",         &m_sct_numHitsInWafer);
      m_nt->Branch("RDO_SCT_WaferHash",              &m_sct_waferHash);
      m_nt->Branch("RDO_SCT_TimeBin",                &m_sct_tbin);
      m_nt->Branch("RDO_SCT_RODId",                  &m_sct_rodid);
      m_nt->Branch("RDO_SCT_Channel",                &m_sct_channel);
      if (m_doHitsOnTracks) {
	m_nt->Branch("RDO_SCT_isOnTrack",              &m_sct_rdoIsOnTrack);
      }
    }
    if (m_fillBSErrs) {
      m_nt->Branch("SCTErr_TotalNumBSErrs",         &m_totalNumErrors,"numBSerrs/I");
      m_nt->Branch("SCTErr_BEC",                    &m_scterr_bec);
      m_nt->Branch("SCTErr_Layer",                  &m_scterr_layer);
      m_nt->Branch("SCTErr_Eta",                    &m_scterr_eta);
      m_nt->Branch("SCTErr_Phi",                    &m_scterr_phi);
      m_nt->Branch("SCTErr_Side",                   &m_scterr_side);
      m_nt->Branch("SCTErr_RODId",                  &m_scterr_rodid);
      m_nt->Branch("SCTErr_Channel",                &m_scterr_channel);
      m_nt->Branch("SCTErr_Type",                   &m_scterr_type);
    }

    return sc;
}

StatusCode InDet::SCT_ClusterValidationNtupleWriter::execute() {

    StatusCode sc = StatusCode::SUCCESS;

    //-------------
    // get some event properties    
    const xAOD::EventInfo* eventInfo;
    sc = evtStore()->retrieve(eventInfo);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Could not retrieve event info" );
    }
    m_runNumber     = eventInfo->runNumber(); 
    m_eventNumber   = eventInfo->eventNumber();
    m_lumiBlock     = eventInfo->lumiBlock();
    m_timeStamp     = eventInfo->timeStamp();
    m_bunchCrossing = eventInfo->bcid();

    // reset variables
    m_nRIOs = 0;
    m_rioLoc1->clear();
    m_rioSurfaceX->clear();
    m_rioSurfaceY->clear();
    m_rioSurfaceZ->clear();
    m_SctBarrelEndcap->clear();
    m_SctLayerDisk->clear();
    m_SctEtaModule->clear();
    m_SctPhiModule->clear();
    m_SctSide->clear();
    m_SctDeltaPhi->clear();
    m_SctHitErr->clear();
    
    m_nRDOs = 0;
    m_sct_rdoGroupSize->clear();
    m_sct_rdoIsOnTrack->clear();
    m_sct_layer->clear();
    m_sct_eta->clear();
    m_sct_phi->clear();
    m_sct_side->clear();
    m_sct_barrelec->clear();
    m_sct_firstStrip->clear();
    m_sct_numHitsInWafer->clear();
    m_sct_waferHash->clear();
    m_sct_tbin->clear();
    m_sct_rodid->clear();
    m_sct_channel->clear();
    
    m_nSP = 0;
    m_sp_bec->clear();
    m_sp_layer->clear();
    m_sp_eta->clear();
    m_sp_phi->clear();
    m_sp_x->clear();
    m_sp_y->clear();
    m_sp_z->clear();

    m_totalNumErrors = 0;
    m_scterr_bec->clear();
    m_scterr_layer->clear();
    m_scterr_eta->clear();
    m_scterr_phi->clear();
    m_scterr_side->clear();
    m_scterr_rodid->clear();
    m_scterr_channel->clear();
    m_scterr_type->clear();

    // Retrieve containers which are required

    // Container with SCT RIOs
    m_riocontainer = 0;
    if ( m_fillCluster) {
      if(!evtStore()->contains<SCT_ClusterContainer>(mjo_riocontainername)){
	ATH_MSG_DEBUG("No PrepRawDataContainer in StoreGate");
      }
      else {
	sc = evtStore()->retrieve(m_riocontainer, mjo_riocontainername);
	if(sc.isFailure()) {
	  ATH_MSG_DEBUG("Could not get PrepRawDataContainer");
	}
      }
    }

    // SpacePoint container
    const SpacePointContainer* p_spContainer = 0;
    if (m_fillSpacePoint) {
      if(!evtStore()->contains<SpacePointContainer>(m_spacePointContainerName)){
	ATH_MSG_DEBUG("No SCT space-point container in StoreGate");
      }
      else {
	sc = evtStore()->retrieve(p_spContainer,m_spacePointContainerName);
	if(sc.isFailure()) {
	  ATH_MSG_DEBUG("Could not get SpacePointContainer");
	}
      }
    }

    // RDO container
    typedef SCT_RDORawData SCTRawDataType;
    const SCT_RDO_Container* p_rdocontainer = 0;
    std::vector<Identifier> RDOsOnTracks;
    if(m_fillRDO) {
      if(!evtStore()->contains<SCT_RDO_Container>(m_dataObjectName)){
	ATH_MSG_DEBUG("No SCT RDO container in StoreGate");
      }
      else {
	sc =  evtStore()->retrieve(p_rdocontainer,m_dataObjectName);
	if(sc.isFailure()) {
	  ATH_MSG_DEBUG( "Failed to retrieve SCT RDO container" );
	}
      }
      if (m_doHitsOnTracks) {
	//Track container
	const DataVector<Trk::Track>* tracks;
	sc = evtStore()->retrieve(tracks,m_inputTrackCollection);
	if (sc.isFailure() ) {
	  ATH_MSG_ERROR("Track container not found");
	}  else {
	  // assemble list of RDOs on Tracks	  
	  for (int i=0; i<(int)tracks->size(); i++) {
	    const Trk::Track *track=(*tracks)[i];
	    if (track == 0) {
	      ATH_MSG_ERROR("no pointer to track!");
	      break;
	    }
	    // Get pointer to track state on surfaces
	    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();
	    if (trackStates == 0){
	      ATH_MSG_WARNING("for current track is TrackStateOnSurfaces == Null, no data will be written for this track");
	    } else {
	      // Loop over all track states on surfaces
	      for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin(); it!=trackStates->end(); it++) {
		// Get pointer to RIO of right type
		const InDet::SiClusterOnTrack *clus =
		  dynamic_cast<const InDet::SiClusterOnTrack*>((*it)->measurementOnTrack());
		if (clus){
		  // Get Pointer to prepRawDataObject
		  const InDet::SiCluster *RawDataClus
		    = dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
		  if (RawDataClus==0){
		    ATH_MSG_WARNING("SiCluster WITHOUT prepRawData!!!!");
		    break;
		  }
		  // if Cluster is in SCT ...
		  if (RawDataClus->detectorElement()->isSCT()) {
		    
		    const std::vector<Identifier>& rdoList = RawDataClus->rdoList();
		    RDOsOnTracks.insert(RDOsOnTracks.end(),rdoList.begin(),rdoList.end());
		  }
		}
	      }
	    }
	  }
	}	
      } // end of m_doHitsOnTracks
    } /// end of m_fillRDO

    
    // Fill clusters
    if (m_fillCluster && m_riocontainer !=0) {

      int RIOindex = 0;

      // -----------------------
      // get all the RIO_Collections in the container
      InDet::SCT_ClusterContainer::const_iterator containerIterator = m_riocontainer->begin();
      for( ; containerIterator != m_riocontainer->end(); containerIterator++){
        ATH_MSG_DEBUG( "There are "  << (*containerIterator)->size() << " entries in the PrepRawDataCollection" );
	
        // ---------------------------------
        //get all the RIOs in the collection

        InDet::SCT_ClusterCollection::const_iterator rioIterator = (*containerIterator)->begin();
        for (; rioIterator != (*containerIterator)->end(); rioIterator++) {
	  // get the surface center of the RIO
	  if (!(*rioIterator)) {
	    ATH_MSG_WARNING( "NULL pointer to RIO" );
	    continue;
	  }
	  if (!((*rioIterator)->detectorElement())) {
	    ATH_MSG_WARNING( "in RIO: NULL pointer to detElement" );
	    continue;
	  }
	  
	  if (m_nRIOs >=500000) {
	    ATH_MSG_WARNING( "reached max. number of RIO entries in the ntuple" );
	    m_nt->Fill();
	    return StatusCode::SUCCESS;
	    
	  }
	  const Amg::Vector3D surfaceGlobPos = (*rioIterator)->detectorElement()->center( (*rioIterator)->identify() ) ;
	  // fill surface global position
	  m_rioSurfaceX->push_back(float(surfaceGlobPos.x()));
	  m_rioSurfaceY->push_back(float(surfaceGlobPos.y()));
	  m_rioSurfaceZ->push_back(float(surfaceGlobPos.z()));
	  // get the local position of the RIO
	  const Amg::Vector2D locPos = (*rioIterator)->localPosition();
	  // fill local position
	  m_rioLoc1->push_back(float(locPos[Trk::locX]));
	  
	  Identifier clId = (*rioIterator)->identify();  
	  const SCT_Cluster& cluster = **rioIterator;
	  InDet::SiWidth  width = cluster.width();
	  m_SctDeltaPhi->push_back(float(width.phiR()));
	  m_SctLayerDisk->push_back(int(m_sctid->layer_disk(clId)));
	  m_SctEtaModule->push_back(int(m_sctid->eta_module(clId)));
	  m_SctPhiModule->push_back(int(m_sctid->phi_module(clId)));
	  m_SctSide->push_back(int(m_sctid->side(clId)));
	  m_SctBarrelEndcap->push_back(int(m_sctid->barrel_ec(clId))) ; //endcap or barrel
	  //determine hit error 
//             bool SctCluster_is_1dim = (cluster.localParameters().parameterKey() == 1);
            /*if (SctCluster_is_1dim)*/ 
	  m_SctHitErr->push_back( Amg::error(cluster.localCovariance(),1));
//             else {
//               HepSymMatrix CovMat;
//               CovMat = cluster.localErrorMatrix();covariance();
//               if (CovMat.num_col() == 2) {
//               //ATH_MSG_DEBUG( "hit CovMat:" << CovMat[0,0]  << " " << CovMat[0,1] << " " );
//               //ATH_MSG_DEBUG( "           " << CovMat[1,0]  << " " << CovMat[1,1] << " " );
//                 const double hitErr = sqrt(CovMat[0][0]);
//               }
//               m_SctHitErr->push_back(hitErr);
//  
//             }
// 
            // count the RIO
            RIOindex++;
        }
      }
      m_nRIOs = RIOindex;
    }

    //////// fill SpacePoint stuff if required////////////////////////////////////////////////////////////////

    if (m_fillSpacePoint && p_spContainer != 0) {
      DataVector<Trk::SpacePoint>::const_iterator p_sp;
      std::pair<IdentifierHash, IdentifierHash> sp_clusInfo;
      //loop over SCT space points collections
      for(SpacePointContainer::const_iterator it=p_spContainer->begin(); it!=p_spContainer->end(); ++it) {
	const SpacePointCollection *colNext=&(**it);
	if (!colNext) continue;
	
	//loop over SpacePoints
	for(p_sp=colNext->begin(); p_sp!=colNext->end(); ++p_sp) {
	  m_nSP++;
	  const Trk::SpacePoint& sp = **p_sp;
	  // Getting [layer/phi] from the one of the two clusters
	  sp_clusInfo = sp.elementIdList();
	  IdentifierHash first_clus = sp_clusInfo.first;
	  Identifier waferId = m_sctid->wafer_id(first_clus);
	  m_sp_bec->push_back(m_sctid->barrel_ec(waferId));
	  m_sp_layer->push_back(m_sctid->layer_disk(waferId));
	  m_sp_phi->push_back(m_sctid->phi_module(waferId));
	  m_sp_eta->push_back(m_sctid->eta_module(waferId));
	  ///get the position in real space
	 // HepPoint3D point = sp.globalPosition();
	  Amg::Vector3D point = sp.globalPosition();
	  m_sp_x->push_back(point.x());
	  m_sp_y->push_back(point.y());
	  m_sp_z->push_back(point.z());
	}
      }
    }
    //// Get the RDO information /////////////////////////////////////////////////////////////////////
    if (m_fillRDO && p_rdocontainer !=0) {

      SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin();
      SCT_RDO_Container::const_iterator lastCol = p_rdocontainer->end();
      for( ; col_it!= lastCol ; ++col_it){
        const InDetRawDataCollection<SCTRawDataType>* SCT_Collection(*col_it);
        
        if (!SCT_Collection) continue;  // select only SCT RDOs
        Identifier theWaferIdOfTheRDOCollection =  SCT_Collection->identify();
        unsigned int theHashOfTheRDOCollection= m_sctid->wafer_hash(theWaferIdOfTheRDOCollection);
	/// get online rodid and channel no. from ID hash using cabling    
	uint32_t onlineID = m_cabling->getOnlineIdFromHash(theHashOfTheRDOCollection);
	SCT_OnlineId online(onlineID);
	uint32_t rod = online.rod();
	uint32_t fibre = online.fibre();

        DataVector<SCTRawDataType>::const_iterator p_rdo=SCT_Collection->begin() ;
        DataVector<SCTRawDataType>::const_iterator end_rdo = SCT_Collection->end();
        int totalNumberOfStrips=0;
        int thisBec = m_sctid->barrel_ec(theWaferIdOfTheRDOCollection);
        int thisLayerDisk = m_sctid->layer_disk(theWaferIdOfTheRDOCollection);
        int thisPhi = m_sctid->phi_module(theWaferIdOfTheRDOCollection);
        int thisEta = m_sctid->eta_module(theWaferIdOfTheRDOCollection);
        int thisSide = m_sctid->side(theWaferIdOfTheRDOCollection);

	m_nRDOs += SCT_Collection->size();
        for(; p_rdo!=end_rdo; ++p_rdo){

          //  SCT RDO Group Size;
          m_sct_rdoGroupSize->push_back((*p_rdo)->getGroupSize());
          totalNumberOfStrips=totalNumberOfStrips+((*p_rdo)->getGroupSize());

          Identifier SCT_Identifier = (*p_rdo)->identify();

          //  SCT_ClusterStruct cluster;
          m_sct_firstStrip->push_back(m_sctid->strip(SCT_Identifier));

          //Identifier wafer_id =  m_sctid->wafer_id(SCT_Identifier); 
 
          m_sct_layer->push_back(thisLayerDisk);
          m_sct_eta->push_back(thisEta);
          m_sct_phi->push_back(thisPhi);
          m_sct_side->push_back(thisSide);
          m_sct_barrelec->push_back(thisBec);
          m_sct_numHitsInWafer->push_back(totalNumberOfStrips);
          m_sct_waferHash->push_back(theHashOfTheRDOCollection);
	  SCT3_RawData* rdo3 = dynamic_cast<SCT3_RawData*>(*p_rdo);
	  if (rdo3!=0) {
	    m_sct_tbin->push_back(rdo3->getTimeBin());
	  } else {
	    m_sct_tbin->push_back(-1);
	  }
	  m_sct_rodid->push_back(rod);
	  m_sct_channel->push_back(fibre);

	  if (m_doHitsOnTracks) {
	    int isOnTrack = 0;
	    if (find(RDOsOnTracks.begin(),RDOsOnTracks.end(),SCT_Identifier) != RDOsOnTracks.end())
	      isOnTrack = 1;
	    m_sct_rdoIsOnTrack->push_back(isOnTrack);
	  }
        }
      }
    }

    /// Fill ByteStream errors block if requested.
    if (m_fillBSErrs) {
      m_totalNumErrors = 0;
      /** types of errors are defined in the enum in ISCT_ByteStreamErrorsSvc.
       * At the moment there are 15 different types of BS error.
       */
      for (int type=0; type < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++type) { 
	std::set<IdentifierHash>* errorSet = m_byteStreamErrSvc->getErrorSet(type);
	if (errorSet != 0) {
	  int eta=0,phi=0,bec=0,layer=0,side=0;
	  std::set<IdentifierHash>::iterator it = errorSet->begin();
	  std::set<IdentifierHash>::iterator itEnd = errorSet->end();
	  m_totalNumErrors += errorSet->size();
	  for (; it != itEnd; ++it) {
	    Identifier itId = m_sctid->wafer_id(*it);
	    layer = m_sctid->layer_disk(itId);
	    side = m_sctid->side(itId);
	    eta = m_sctid->eta_module(itId);
	    phi = m_sctid->phi_module(itId);
	    bec = m_sctid->barrel_ec(itId);
	    m_scterr_type->push_back(type);
	    m_scterr_bec->push_back(bec);
	    m_scterr_layer->push_back(layer);
	    m_scterr_eta->push_back(eta);
	    m_scterr_phi->push_back(phi);
	    m_scterr_side->push_back(side);

	  /// get online rodid and channel no. from ID hash using cabling    
	    uint32_t onlineID = m_cabling->getOnlineIdFromHash(*it);
	    SCT_OnlineId online(onlineID);
	    uint32_t rod = online.rod();
	    uint32_t fibre = online.fibre();
	    m_scterr_rodid->push_back((int)rod);
	    m_scterr_channel->push_back((int)fibre);
	  }
	}
      }
    }
    m_nt->Fill();
    return StatusCode::SUCCESS;
}


StatusCode InDet::SCT_ClusterValidationNtupleWriter::finalize() {
    ATH_MSG_INFO( "SCT_ClusterValidationNtupleWriter finalize()" );
    delete m_rioLoc1;
    delete m_rioSurfaceX;
    delete m_rioSurfaceY;
    delete m_rioSurfaceZ;
    delete m_SctBarrelEndcap;
    delete m_SctLayerDisk;
    delete m_SctEtaModule;
    delete m_SctPhiModule;
    delete m_SctSide;
    delete m_SctDeltaPhi;
    delete m_SctHitErr;
    delete m_sp_bec;
    delete m_sp_layer;
    delete m_sp_eta;
    delete m_sp_phi;
    delete m_sp_x;
    delete m_sp_y;
    delete m_sp_z;
    delete m_sct_rdoGroupSize;
    delete m_sct_rdoIsOnTrack;
    delete m_sct_layer;
    delete m_sct_eta;
    delete m_sct_phi;
    delete m_sct_side;
    delete m_sct_barrelec;
    delete m_sct_firstStrip;
    delete m_sct_numHitsInWafer;
    delete m_sct_waferHash;
    delete m_sct_tbin;
    delete m_sct_rodid;
    delete m_sct_channel;
    delete m_scterr_bec;
    delete m_scterr_layer;
    delete m_scterr_eta;
    delete m_scterr_phi;
    delete m_scterr_side;
    delete m_scterr_rodid;
    delete m_scterr_channel;
    delete m_scterr_type;

    m_rioLoc1=0;
    m_rioSurfaceX=0;
    m_rioSurfaceY=0;
    m_rioSurfaceZ=0;
    m_SctBarrelEndcap=0;
    m_SctLayerDisk=0;
    m_SctEtaModule=0;
    m_SctPhiModule=0;
    m_SctSide=0;
    m_SctDeltaPhi=0;
    m_SctHitErr=0;
    m_sp_bec=0;
    m_sp_layer=0;
    m_sp_eta=0;
    m_sp_phi=0;
    m_sp_x=0;
    m_sp_y=0;
    m_sp_z=0;
    m_sct_rdoGroupSize=0;
    m_sct_rdoIsOnTrack=0;
    m_sct_layer=0;
    m_sct_eta=0;
    m_sct_phi=0;
    m_sct_side=0;
    m_sct_barrelec=0;
    m_sct_firstStrip=0;
    m_sct_numHitsInWafer=0;
    m_sct_waferHash=0;
    m_sct_tbin=0;
    m_sct_rodid=0;
    m_sct_channel=0;
    m_scterr_bec=0;
    m_scterr_layer=0;
    m_scterr_eta=0;
    m_scterr_phi=0;
    m_scterr_side=0;
    m_scterr_rodid=0;
    m_scterr_channel=0;
    m_scterr_type=0;

    return StatusCode::SUCCESS;
}
