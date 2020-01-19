/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_StrawStatus.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "TRT_CalibAlgs/TRT_StrawStatus.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"

// #include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "xAODEventInfo/EventInfo.h"

//#include "GaudiKernel/ServiceHandle.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_ConditionsServices/ITRT_HWMappingSvc.h"
#include "TRT_ConditionsServices/ITRT_DCS_ConditionsSvc.h"

#include "VxVertex/VxContainer.h"

#include "TRT_TrackHoleSearch/TRTTrackHoleSearchTool.h"
#include "TrkParameters/TrackParameters.h"

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

int last_lumiBlock0=-99;

//================ Constructor =================================================

InDet::TRT_StrawStatus::TRT_StrawStatus(const std::string& name, ISvcLocator* pSvcLocator)
:
AthAlgorithm(name,pSvcLocator),
m_nBarrelStraws(1642), m_nEndcapStraws(3840), m_nAllStraws(5482),
m_nBarrelBoards(9), m_nEndcapBoards(20), m_nAllBoards(29),
m_nBarrelChips(104), m_nEndcapChips(240), m_nAllChips(344),
m_nBarrelPads(208), m_nEndcapPads(480), m_nAllPads(688),
m_nEvents(0), m_runNumber(0),
m_TRTHelper(0),
m_mapSvc("TRT_HWMappingSvc",name),
m_DCSSvc("TRT_DCS_ConditionsSvc",name),
m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc", name), // use this service to retrieve barrel and end-cap straw number later on, as well as DTMROC,.. 
m_TRTStrawStatusSummarySvc("TRT_StrawStatusSummarySvc", name),
m_trt_hole_finder("TRTTrackHoleSearchTool"),
m_updator("Trk::KalmanUpdator/TrkKalmanUpdator"),
m_locR_cut(1.4),
m_tracksName("CombinedInDetTracks"),
m_fileName("TRT_StrawStatusOutput"),
m_skipBusyEvents(0), // for cosmics - reject events that are either showers or noise bursts
m_printDetailedInformation(0) // print the information on mapping as well as which straws are declared dead etc. 
{
    declareProperty("tracksCollection", m_tracksName);
    declareProperty("outputFileName", m_fileName);
    declareProperty("skipBusyEvents", m_skipBusyEvents);        
    declareProperty("trt_hole_finder",          m_trt_hole_finder);
    declareProperty("HWMapSvc", m_mapSvc);
    declareProperty("InDetTRT_DCS_ConditionsSvc",m_DCSSvc);
    declareProperty("locR_cut",                 m_locR_cut );
    declareProperty("printDetailedInformation", m_printDetailedInformation); 
    clear();  
}

//================ Destructor =================================================

InDet::TRT_StrawStatus::~TRT_StrawStatus()
{}


//================ Initialisation =================================================

StatusCode InDet::TRT_StrawStatus::initialize()
{
    // Code entered here will be executed once at program start.
    
    StatusCode sc = detStore()->retrieve(m_TRTHelper, "TRT_ID");
    if ( sc.isFailure() ) {
        msg(MSG::ERROR) << "Unable to retrieve TRT ID Helper." << endreq;
        return sc;        
    } else {
        msg(MSG::INFO) << "retrieved m_TRTHelper " << m_TRTHelper << endreq;  
    }
    
    sc = m_TRTStrawNeighbourSvc.retrieve() ;
    if (sc.isFailure()) {
        msg(MSG::ERROR) << "Could not find TRTStrawNeighbourSvc " << endreq;    
        return sc;
    } else {
        msg(MSG::INFO) << "retrieved TRTStrawNeighbourSvc " << m_TRTStrawNeighbourSvc << endreq;
    }
    
    if ( m_trt_hole_finder.retrieve().isFailure() ){
        ATH_MSG_FATAL( "Failed to retrieve the TRTTrackHoleSearchTool." );
        return StatusCode::FAILURE;
    } else {
        msg(MSG::INFO) << "retrieved TRTTrackHoleSearchTool " << m_trt_hole_finder << endreq;
    }       
   if(m_updator.retrieve().isFailure()) {
	   ATH_MSG_FATAL( "Could not retrieve measurement updator tool." );
	   return StatusCode::FAILURE;
	} else {
		msg(MSG::INFO) << "retrieved Trk::KalmanUpdator to calculate unbiased track states " << m_updator << endreq; 
	}
    sc = m_mapSvc.retrieve();
    if (sc.isFailure()) {
        msg(MSG::ERROR) << "Failed to retrieve m_mapSvc." << endreq;
           return sc;
    } else {
        msg(MSG::INFO) << "retrieved m_mapSvc " << m_mapSvc << endreq;
    }
    sc = m_DCSSvc.retrieve();
        if (sc.isFailure()){
        msg(MSG::ERROR) << "Failed to retrieve m_DCSSvc" << endreq;
        return sc;
    } else {
        msg(MSG::INFO) << "retrieved m_DCSSvc " << m_DCSSvc << endreq;
    }
    msg(MSG::INFO) << "initialize() successful in " << name() << ", retrieved: ..., locR_cut = " << m_locR_cut << " mm." << endreq;
    return sc;
}

//================ Finalisation =================================================

StatusCode InDet::TRT_StrawStatus::finalize(){
    reportResults();
    if (m_printDetailedInformation) printDetailedInformation();
    // Code entered here will be executed once at the end of the program run.
    return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode InDet::TRT_StrawStatus::execute(){
    const xAOD::EventInfo *eventInfo = 0;
    StatusCode sc = evtStore()->retrieve(eventInfo); 
    if ( sc.isFailure() ) {
        msg(MSG::ERROR) << "Unable to retrieve Event Info " << endreq;
        return sc;
    } 
    int runNumber = (int) eventInfo->runNumber();  
    if (runNumber != m_runNumber) {
        if (m_nEvents) { reportResults(); clear(); }
        m_runNumber = runNumber;
    } 
    int lumiBlock0 =eventInfo->lumiBlock();
    const TRT_RDO_Container* rdoContainer; // container of all TRT hits
    sc = evtStore()->retrieve(rdoContainer, "TRT_RDOs");
    if ( sc.isFailure() ) {
        msg(MSG::ERROR) << "no TRT_RDO container available " << endreq;
        return sc;
    }
    const DataVector<Trk::Track> *trkCollection; // track collection
    sc = evtStore()->retrieve( trkCollection, m_tracksName );
    if ( sc.isFailure() ) {
        msg(MSG::ERROR) << "Could not find Tracks Collection: " << m_tracksName << endreq;
        return sc;
    }   
    
    //================ Event selection  
    
    const VxContainer* vxContainer(0); // require at least one primary vertex with at least three tracks
    sc = evtStore()->retrieve(vxContainer,"VxPrimaryCandidate");
    if ( sc.isFailure() ) { msg(MSG::ERROR) << "vertex container missing!" << endreq; 
    } else {
        int countVertices(0);
        for (VxContainer::const_iterator it = vxContainer->begin() ; it != vxContainer->end() ; ++it ) {
            if ( (*it)->vxTrackAtVertex()->size() >= 3 ) countVertices++;
        }
        if (countVertices < 1)  return sc; 
    }
    
    if (m_skipBusyEvents) { // cosmic running
        int countRDOhitsInEvent(0);
        for (TRT_RDO_Container::const_iterator rdoIt = rdoContainer->begin(); rdoIt != rdoContainer->end(); ++rdoIt) {
            const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
            if (TRTCollection==0) continue;
            for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); ++trtIt) {
                countRDOhitsInEvent++;
            }
        }
        if (countRDOhitsInEvent>100000) {
            if (msgLvl(MSG::INFO)) msg() << "N RDO hits in event greater than 100000: " << countRDOhitsInEvent << ", exiting" << endreq;
            return sc;
        }
        
        if (trkCollection->size() > 10) {
            if (msgLvl(MSG::INFO)) msg() << "N tracks greater than 10: " << trkCollection->size() << ", exiting" << endreq;
            return sc;
        }
    }
    
    //================ End event selection         
    
    //================ Loop over all tracks, accumulate hits on track, also find holes on track
    
    std::vector<Identifier> holeIdentifiers;
    std::vector<Identifier> holeIdentifiersWithHits; // holes on straws that have hits, it is just that the hit was not associalted to a track
    for ( DataVector<Trk::Track>::const_iterator trackIt = trkCollection->begin(); trackIt != trkCollection->end(); trackIt++ ) {
        const Trk::Track *track = *trackIt;
        //=== select track
        const Trk::Perigee* perigee = (*trackIt)->perigeeParameters();
        if ( perigee == 0 ) { msg(MSG::ERROR) << "Trk::Perigee missing" << endreq; continue; }
        if ( fabs(perigee->pT())/CLHEP::GeV < 1. ) continue; // 1 GeV pT cut
        
        const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**trackIt).trackStateOnSurfaces();
        if ( trackStates == 0 ) { msg(MSG::ERROR) << "Trk::TrackStateOnSurface empty" << endreq; continue; }
        
        int n_pixel_hits(0), n_sct_hits(0), n_trt_hits(0);  // count hits, require minimal number of all hits 
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ ) {
            if ( *trackStatesIt == 0 ) { msg(MSG::ERROR) << "*trackStatesIt == 0" << endreq; continue; }

            if ( !((*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement)) ) continue; // this skips outliers
            
            if ( dynamic_cast<const InDet::TRT_DriftCircleOnTrack*> ( (*trackStatesIt)->measurementOnTrack() )  ) n_trt_hits++;
            else if ( dynamic_cast<const InDet::SCT_ClusterOnTrack*>     ( (*trackStatesIt)->measurementOnTrack() )  ) n_sct_hits++;
            else if( dynamic_cast<const InDet::PixelClusterOnTrack*>    ( (*trackStatesIt)->measurementOnTrack() )  ) n_pixel_hits++;
        }           
        if (n_pixel_hits<2 || n_sct_hits < 6 || n_trt_hits<15) continue; // end count hits      
        
        //=== end select track
        
        //=== loop over all hits on track, accumulate them
        
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ ) {
            
            if ( *trackStatesIt == 0 ) { msg(MSG::ERROR) << "*trackStatesIt == 0" << endreq; continue; }

            if ( !((*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement)) ) continue; // this skips outliers

            const InDet::TRT_DriftCircleOnTrack *driftCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() );
            if ( driftCircleOnTrack == 0 ) continue; // not TRT measurement - this way, keep both hits and outliers
            
			const Trk::TrackStateOnSurface& hit = **trackStatesIt;
			
			const Trk::TrackParameters* unbiased_track_parameters = m_updator->removeFromState( *(hit.trackParameters()), hit.measurementOnTrack()->localParameters(), hit.measurementOnTrack()->localCovariance());
			
			double unbiased_locR = unbiased_track_parameters->parameters()[Trk::locR];
			if ( fabs(unbiased_locR) > m_locR_cut ) continue; // same cut as the default hole search cut
    
            const InDet::TRT_DriftCircle *driftCircle = driftCircleOnTrack->prepRawData();
            if ( driftCircle == 0 ) { msg(MSG::ERROR) << "driftCircle == 0" << endreq; continue; }
            
            Identifier id = driftCircle->identify();
            int index[6]; myStrawIndex(id, index); // side, layer, phi, straw_layer, straw_within_layer, straw_index
            m_accumulateHits[(index[0]>0)?0:1][index[2]][index[5]][1]++; // accumulate hits on track
            if (driftCircle->highLevel()) m_accumulateHits[(index[0]>0)?0:1][index[2]][index[5]][3]++; // accumulate hits on track
            
        } // end trackStatesIt loop
        
        // add holeIdentifiers - fill vector
        
        const DataVector<const Trk::TrackStateOnSurface>* holes = m_trt_hole_finder->getHolesOnTrack( *track );
        if ( holes==0 ) continue; // no holes found
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = holes->begin(); trackStatesIt != holes->end(); trackStatesIt++ ) {
            
            if ( !(*trackStatesIt)->type(   Trk::TrackStateOnSurface::Hole  )  ) { msg(MSG::ERROR) << "m_trt_hole_finder returned something that is not a hole" << endreq; continue; }
            
            const Trk::TrackParameters* track_parameters = (*trackStatesIt)->trackParameters();
            if (!track_parameters) { msg(MSG::WARNING) << "m_trt_hole_finder track_parameters missing" << endreq; continue; }
            
            Identifier id = track_parameters->associatedSurface().associatedDetectorElementIdentifier();
            if ( !(m_TRTHelper->is_trt(id)) ) { msg(MSG::ERROR) << "m_trt_hole_finder returned something that is not a TRT hole" << endreq; continue; }

            // add se same 1.4 mm locR selection, in case it is not on by default 
            if ( fabs( track_parameters->parameters()[Trk::locR] ) > m_locR_cut ) continue;
    
            holeIdentifiers.push_back( id );
        } // end add holeIdentifiers
        
    } // end trackIt loop   
    
    //================ End loop over all tracks
    
    //================ Loop over all hits - it includes hits from dead straws that are masked off in drift circle creation
    
    for (TRT_RDO_Container::const_iterator rdoIt = rdoContainer->begin(); rdoIt != rdoContainer->end(); ++rdoIt) {
        const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
        if (TRTCollection==0) continue;
        for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); trtIt++) {
            Identifier id = (*trtIt)->identify();
            int index[6]; myStrawIndex(id, index); // side, layer, phi, straw_layer, straw_within_layer, straw_index
            m_accumulateHits[(index[0]>0)?0:1][index[2]][index[5]][0]++; // accumulate all hits 
            if ((*trtIt)->highLevel()) m_accumulateHits[(index[0]>0)?0:1][index[2]][index[5]][2]++; // accumulate TR hits
            
            if (std::find(holeIdentifiers.begin(), holeIdentifiers.end(), id) != holeIdentifiers.end())  // a hole was found on the same straw, but hits is there
                holeIdentifiersWithHits.push_back( id );            
        }  
    }
    
    //================ End loop over all hits 

    //================ End loop over all holes, each time also save whether the straw with a hole had a hit
    
    for (unsigned int i=0; i<holeIdentifiers.size(); i++) {
        
        Identifier id = holeIdentifiers[i];

        int index[6]; myStrawIndex(id, index); // side, layer, phi, straw_layer, straw_within_layer, straw_index
        
        m_accumulateHits[(index[0]>0)?0:1][index[2]][index[5]][4]++;
        
        if (std::find(holeIdentifiersWithHits.begin(), holeIdentifiersWithHits.end(), id) != holeIdentifiersWithHits.end())
            m_accumulateHits[(index[0]>0)?0:1][index[2]][index[5]][5]++;
    }
    
    //================ End loop over all hits 

    //===== searching for HV lines with voltage < 1490 V
    if (lumiBlock0 != last_lumiBlock0){
        float theValue;
        int chanNum;
        char fileName_mapping[300]; 
        sprintf(fileName_mapping, "%s.%07d_Voltage_trips.txt", m_fileName.c_str(), m_runNumber);
        FILE *fmapping = fopen(fileName_mapping, "a");
        //StatusCode 
        sc = StatusCode::SUCCESS;//for compatibility with Rel 17
        
        //===== Loop over all HV lines
        for (chanNum=1;chanNum<1281;chanNum++){
            theValue = 9999.;
            std::string folderName = (chanNum <= 640)?"/TRT/DCS/HV/ENDCAPA":"/TRT/DCS/HV/ENDCAPC";
            short int old_chanNum = (chanNum<=640)?chanNum:(chanNum-640);
            sc = m_DCSSvc->getValue(folderName,old_chanNum,theValue);
            if (theValue<1490.){
                fprintf(fmapping,"Low voltage on line %i\n",chanNum);
            } else if (theValue == 9999.){ //if was not updated with last mDCSSvc->getValue(...)
                fprintf(fmapping,"Unknown voltage on line %i\n",chanNum);
            }
        }
        //~scc;//for compatibility with Rel 17
        fclose(fmapping);
    }

    m_nEvents++;
    last_lumiBlock0 = lumiBlock0;
    if (m_nEvents%1000==0 && msgLvl(MSG::DEBUG)) reportResults();
    return sc;
}

//============================================================================================

void InDet::TRT_StrawStatus::clear() {
    m_nEvents = 0;
    for (int i=0; i<2; i++) for (int j=0; j<32; j++) for (int k=0; k<m_nAllStraws; k++) for (int m=0; m<6; m++)
        m_accumulateHits[i][j][k][m] = 0;
    return;
}

void InDet::TRT_StrawStatus::reportResults() {
    msg(MSG::INFO) << "InDet::TRT_StrawStatus::reportResults() for " << m_nEvents << " events." << endreq;
    char fileName[300]; sprintf(fileName, "%s.%07d_newFormat.txt", m_fileName.c_str(), m_runNumber);
    FILE *f = fopen(fileName, "w");
    fprintf(f, "%d %d %d %d %d %d %d %d %d \n", 0, 0, 0, 0, 0, 0, 0, 0, m_nEvents);
    for (int i=0; i<2; i++) for (int j=0; j<32; j++) for (int k=0; k<m_nAllStraws; k++) {
        int side = (i>0)?-1:1;
        if (k>=1642) side *= 2;
        fprintf(f, "%d %d %d", side, j, k);
        for (int m=0; m<6; m++) fprintf(f, " %d", m_accumulateHits[i][j][k][m]);
        fprintf(f, "\n");   
    }
    fclose(f);
    return;
}  

void InDet::TRT_StrawStatus::printDetailedInformation() {
    msg(MSG::INFO) << "InDet::TRT_StrawStatus::printDetailedInformation() " << endreq;
    char fileName[300]; sprintf(fileName, "%s.%07d_printDetailedInformation.txt", m_fileName.c_str(), m_runNumber);
    FILE *f = fopen(fileName, "w");
    for (std::vector<Identifier>::const_iterator it = m_TRTHelper->straw_layer_begin(); it != m_TRTHelper->straw_layer_end(); it++  ) {
        for (int i=0; i<=m_TRTHelper->straw_max( *it); i++) {
            Identifier id = m_TRTHelper->straw_id( *it, i);
            int index[6];
            myStrawIndex(id, index);
            int chip, HVpad;    
            m_TRTStrawNeighbourSvc->getChip(id, chip);
            m_TRTStrawNeighbourSvc->getPad(id, HVpad);
            static int printStatusCount(0);
            if (!printStatusCount) {
                msg(MSG::INFO) << "if the code crashes on the next line, there is a problem with m_TRTStrawStatusSummarySvc not being loaded " << endreq;
                msg(MSG::INFO) << "in that case, running with reco turned on normally solves the problem, know of no better solution at the moment" << endreq;
                                msg(MSG::INFO) << "if you do not need the detailed print information, you can also just set printDetailedInformation to 0 to avoid this crash" << endreq; 
                printStatusCount++;
            }
            int status = m_TRTStrawStatusSummarySvc->get_status( id );  
            int statusTemporary = m_TRTStrawStatusSummarySvc->getStatus( id );
            int statusPermanent = m_TRTStrawStatusSummarySvc->getStatusPermanent( id ); 
            for (int j=0; j<6; j++) fprintf(f, "%d ", index[j]);
            fprintf(f, "%d %d %d %d %d\n", chip, HVpad, status, statusTemporary, statusPermanent);
        }
    }           
    fclose(f);
    return; 
}

void InDet::TRT_StrawStatus::myStrawIndex(Identifier id, int *index) {
    int side = m_TRTHelper->barrel_ec(id);
    int layerNumber = m_TRTHelper->layer_or_wheel(id);
    int strawLayerNumber = m_TRTHelper->straw_layer(id);
    int strawNumber = m_TRTHelper->straw(id);        
    int straw(0);
    
    const int numberOfStraws[74] = { 0, 15, 31, 47, 63, 79, 96, 113, 130, 147, 164, 182, 200, 218, 236, 254, 273, 292, 311, 329, // layer 0, 329 straws, strawlayers 0-18
        348, 368, 388, 408, 428, 448, 469, 490, 511, 532, 553, 575, 597, 619, 641, 663, 686, 709, 732, 755, 778, 802, 826, 849, // layer 1, 520 straws, strawLayers 0-23
        872, 896, 920, 944, 968, 993, 1018, 1043, 1068, 1093, 1119, 1145, 1171, 1197, 1223, 1250, 1277, 1304, 1331, 1358, 1386, 1414, 1442, 1470, 1498, 1527, 1556, 1585, 1614, 1642 }; // layer 2
    
    if (abs(side)==1) { // barrel unique straw number
        if (layerNumber==1) strawLayerNumber+= 19;
        else if (layerNumber==2) strawLayerNumber+= 43;
        straw = ( numberOfStraws[strawLayerNumber+1] - strawNumber -1 );
    } else { // end-cap unique straw number
        int board = layerNumber;
        if (board<6) { board *= 2; if (strawLayerNumber>7) board++; }
        else { board += 6; }
        straw = board * 192 + strawNumber * 8 + strawLayerNumber % 8 ;
        straw += 1642;          
    }
    index[0] = side;
    index[1] = layerNumber;
    index[2] = m_TRTHelper->phi_module(id);
    index[3] = strawLayerNumber;
    index[4] = strawNumber;
    index[5] = straw;       
    return;
}
