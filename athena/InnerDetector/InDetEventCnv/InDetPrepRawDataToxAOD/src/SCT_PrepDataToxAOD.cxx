/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_PrepDataToxAOD.cxx
//   Implementation file for class SCT_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#include "SCT_PrepDataToxAOD.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Collection.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "HepMC/GenParticle.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "CLHEP/Geometry/Point3D.h"



/////////////////////////////////////////////////////////////////////
//
//         Constructor with parameters:
//
/////////////////////////////////////////////////////////////////////
SCT_PrepDataToxAOD::SCT_PrepDataToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_incidentSvc("IncidentSvc", name),
  m_SCTHelper(0),
  m_firstEventWarnings(true)
{ 
  // --- Steering and configuration flags
  declareProperty("UseTruthInfo", m_useTruthInfo=false);
  declareProperty("WriteRDOinformation", m_writeRDOinformation =true);
  declareProperty("WriteSDOs", m_writeSDOs = true);
  declareProperty("WriteSiHits", m_writeSiHits = true);

  // --- Configuration keys
  declareProperty("SiClusterContainer",  m_clustercontainer = "SCT_Clusters");
  declareProperty("MC_SDOs", m_SDOcontainer = "SCT_SDO_Map");
  declareProperty("MC_Hits", m_sihitContainer = "SCT_Hits");
  declareProperty("PRD_MultiTruth", m_multiTruth = "PRD_MultiTruthSCT");

  // --- Services and Tools
}

/////////////////////////////////////////////////////////////////////
//
//        Initialize method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode SCT_PrepDataToxAOD::initialize()
{

  CHECK ( detStore()->retrieve(m_SCTHelper, "SCT_ID") );

  CHECK ( m_incidentSvc.retrieve() );
  // register to the incident service:
  m_incidentSvc->addListener( this, IncidentType::EndEvent);

  //make sure we don't write what we don't have
  if (not  m_useTruthInfo) {
    m_writeSDOs = false;
    m_writeSiHits = false;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
//
//        Execute method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode SCT_PrepDataToxAOD::execute() 
{     
  const SCT_RDO_Container * rdoContainer(0);
   
  // the cluster ambiguity map
  if ( m_writeRDOinformation ) {
    if(!evtStore()->contains<SCT_RDO_Container>("SCT_RDOs")){
      if (m_firstEventWarnings) {
	ATH_MSG_WARNING("RDO ASSOC: No SCT RDO container in StoreGate");
      }
    }
    else {
      if(evtStore()->retrieve(rdoContainer,"SCT_RDOs").isFailure()) {
        ATH_MSG_WARNING( "Failed to retrieve SCT RDO container" );
      }
    }
  
    if ( rdoContainer != 0){

      // get all the RIO_Collections in the container
      
      for( auto& collection: *rdoContainer ){

        //get all the RDOs in the collection
        for (auto& rdo : *collection) {

          if ( !rdo) {
            ATH_MSG_WARNING( "Null SCT RDO. Skipping it");
            continue;
          }

          Identifier rdoId = rdo->identify();
	  
          m_IDtoRAWDataMap.insert( std::pair< Identifier, const SCT_RDORawData*>( rdoId, rdo ) );      
        } // collection
      } // container
    } // Have container;
  }   
  ATH_MSG_DEBUG("Size of RDO map is "<<m_IDtoRAWDataMap.size());

  // Mandatory. This is needed and required if this algorithm is scheduled.
  const InDet::SCT_ClusterContainer* sctClusterContainer = 0;     
  if( evtStore()->retrieve(sctClusterContainer,m_clustercontainer).isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve SCT PrepDataContainer " << m_clustercontainer);
    return StatusCode::FAILURE;
  }

  // Optional. Normally only available in Hits files -- samples need to digitised and Hits need to be copied for this to work
  const SiHitCollection* sihitCollection = 0;
  if (m_writeSiHits) {
    if (evtStore()->contains<SiHitCollection>(m_sihitContainer)) {
      ATH_CHECK(evtStore()->retrieve(sihitCollection, m_sihitContainer));
    } else {
      if (m_firstEventWarnings) {
	ATH_MSG_WARNING("SiHit collection not available (" << m_sihitContainer << "). Skipping although requested.");
	sihitCollection = 0;
      }	
    }
  }

  // Optional. On RDO
  const InDetSimDataCollection* sdoCollection = 0;
  if (m_writeSDOs) {
    if ( evtStore()->contains<InDetSimDataCollection>(m_SDOcontainer) ) {
      ATH_CHECK(evtStore()->retrieve(sdoCollection, m_SDOcontainer));
    } else {
      ATH_MSG_WARNING("SDO container not available (" << m_SDOcontainer << "). Skipping although requested.");
      sdoCollection = 0;
    }
  }

  // Optional. On ESD and AOD
  const PRD_MultiTruthCollection* prdmtColl = 0;
  if (m_useTruthInfo) {
    if ( evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruth) ) {
      ATH_CHECK(evtStore()->retrieve(prdmtColl, m_multiTruth));
    } else {
      ATH_MSG_WARNING("MultiTruth container not available (" << m_multiTruth << "). Skipping although requested.");
      prdmtColl = 0;
    }
  }


  // Create the xAOD container and its auxiliary store:
  xAOD::TrackMeasurementValidationContainer* xaod = new xAOD::TrackMeasurementValidationContainer();
  CHECK( evtStore()->record( xaod, m_clustercontainer ) );
  xAOD::TrackMeasurementValidationAuxContainer* aux = new xAOD::TrackMeasurementValidationAuxContainer();
  CHECK( evtStore()->record( aux, m_clustercontainer + "Aux." ) );
  xaod->setStore( aux );
  
  std::vector<unsigned int>* offsets = new std::vector<unsigned int>( m_SCTHelper->wafer_hash_max(), 0 );
  CHECK( evtStore()->record( offsets, m_clustercontainer + "Offsets" ) );
  
  // Loop over the container
  unsigned int counter(0);
  for( const auto& clusterCollection : *sctClusterContainer){

    //Fill Offset container
    (*offsets)[clusterCollection->identifyHash()] = counter;
    
    // skip empty collections
    if( clusterCollection->empty() ) continue;

         
    // loop over collection and convert to xAOD
    for( auto& prd : *clusterCollection ){
      ++counter;
      
      Identifier clusterId = prd->identify();
      if ( !clusterId.is_valid() ) {
        ATH_MSG_WARNING("SCT cluster identifier is not valid!");
      }
              
      // create and add xAOD object
      xAOD::TrackMeasurementValidation* xprd = new xAOD::TrackMeasurementValidation();
      xaod->push_back(xprd);
  
      //Set Identifier
      xprd->setIdentifier( clusterId.get_compact() );

      //Set Global Position
      Amg::Vector3D gpos = prd->globalPosition();
      xprd->setGlobalPosition(gpos.x(),gpos.y(),gpos.z());

      //Set Local Position
      const Amg::Vector2D& locpos = prd->localPosition();
      float locY(0.);
      float locX = locpos.x();
      if ( !(std::isinf(locpos.y()) || std::isnan(locpos.y())) ){
        if (locpos.y()>=1e-07)
          locY=locpos.y();
      } else { 
        locY = -9999.;
      }

      // Set local error matrix
      xprd->setLocalPosition(locX,locY); 
      

      

      const Amg::MatrixX& localCov = prd->localCovariance();
      if(localCov.size() == 1){
        xprd->setLocalPositionError( localCov(0,0), 0., 0. ); 
      } else if(localCov.size() == 4){
        xprd->setLocalPositionError( localCov(0,0), localCov(1,1), localCov(0,1) );     
      } else {
        xprd->setLocalPositionError(0.,0.,0.);
      }
         
      // Set vector of hit identifiers
      std::vector< uint64_t > rdoIdentifierList;
      for( const auto &hitIdentifier : prd->rdoList() ){
        rdoIdentifierList.push_back( hitIdentifier.get_compact() );
      }
      xprd->setRdoIdentifierList(rdoIdentifierList);      

      //Add SCT specific information
      const InDet::SiWidth cw = prd->width();
      xprd->auxdata<int>("SiWidth") = (int)cw.colRow()[0];
      xprd->auxdata<int>("hitsInThirdTimeBin") = (int)(prd->hitsInThirdTimeBin());

      xprd->auxdata<int>("bec")          =   m_SCTHelper->barrel_ec(clusterId)   ;
      xprd->auxdata<int>("layer")        =   m_SCTHelper->layer_disk(clusterId)  ;   
      xprd->auxdata<int>("phi_module")   =   m_SCTHelper->phi_module(clusterId)  ;
      xprd->auxdata<int>("eta_module")   =   m_SCTHelper->eta_module(clusterId)  ;
      xprd->auxdata<int>("side")         =   m_SCTHelper->side(clusterId)        ;
   
      // Add the Detector element ID  --  not sure if needed as we have the informations above
      const InDetDD::SiDetectorElement* de = prd->detectorElement();
      uint64_t detElementId(0);
      if(de){
        Identifier detId = de->identify();
        if ( detId.is_valid() ) {
          detElementId = detId.get_compact();
        }
      }
      xprd->auxdata<uint64_t>("detectorElementID") = detElementId;
   
      //Add details about the individual hits 
      if(m_writeRDOinformation)
        addRDOInformation(xprd, prd);
   
      
      // Use the MultiTruth Collection to get a list of all true particle contributing to the cluster
      if(prdmtColl){
        std::vector<int> barcodes;
        //std::pair<PRD_MultiTruthCollection::const_iterator,PRD_MultiTruthCollection::const_iterator>;
        auto range = prdmtColl->equal_range(clusterId);
        for (auto i = range.first; i != range.second; ++i) {
           barcodes.push_back( i->second.barcode() );
        }
        xprd->auxdata< std::vector<int> >("truth_barcode") = barcodes;
      }

      // Use the SDO Collection to get a list of all true particle contributing to the cluster per readout element
      //  Also get the energy deposited by each true particle per readout element   
      if( sdoCollection ){
        addSDOInformation( xprd, prd,sdoCollection);
      }
    
      // Now Get the most detailed truth from the SiHits
      // Note that this could get really slow if there are a lot of hits and clusters
      if( sihitCollection  ){
        addSiHitInformation( xprd, prd, sihitCollection); 
      }
    }
  }
  ATH_MSG_DEBUG( " recorded SCT_PrepData objects: size " << xaod->size() );

  m_firstEventWarnings = false; //disable one-time warnings

  return StatusCode::SUCCESS;
}



void SCT_PrepDataToxAOD::addSDOInformation( xAOD::TrackMeasurementValidation* xprd,
                                            const InDet::SCT_Cluster* prd,
                                            const InDetSimDataCollection* sdoCollection ) const
{
  std::vector<int> sdo_word;
  std::vector< std::vector< int > > sdo_depositsBarcode;
  std::vector< std::vector< float > > sdo_depositsEnergy;
  // find hit
  for( const auto &hitIdentifier : prd->rdoList() ){
    auto pos = sdoCollection->find(hitIdentifier);
    if( pos != sdoCollection->end() ) {
      sdo_word.push_back( pos->second.word() ) ;
      
      std::vector< int > sdoDepBC;
      std::vector< float > sdoDepEnergy;
      for( auto deposit : pos->second.getdeposits() ){
        if(deposit.first){
          sdoDepBC.push_back( deposit.first->barcode());
        } else {
          sdoDepBC.push_back( -1 );   
        }
        sdoDepEnergy.push_back( deposit.second  );
        ATH_MSG_DEBUG(" SDO Energy Deposit " << deposit.second  ) ;
      }
      sdo_depositsBarcode.push_back( sdoDepBC );
      sdo_depositsEnergy.push_back( sdoDepEnergy );
    }
  }
  xprd->auxdata< std::vector<int> >("sdo_words")  = sdo_word;
  xprd->auxdata< std::vector< std::vector<int> > >("sdo_depositsBarcode")  = sdo_depositsBarcode;
  xprd->auxdata< std::vector< std::vector<float> > >("sdo_depositsEnergy") = sdo_depositsEnergy;
}



void  SCT_PrepDataToxAOD::addSiHitInformation( xAOD::TrackMeasurementValidation* xprd, 
                                               const InDet::SCT_Cluster* prd,
                                               const SiHitCollection* sihitCollection) const
                             
                             
{

  std::vector<SiHit> matchingHits = findAllHitsCompatibleWithCluster(  prd, sihitCollection );

  int numHits = matchingHits.size();

  std::vector<float> sihit_energyDeposit(numHits,0);
  std::vector<float> sihit_meanTime(numHits,0);
  std::vector<int>   sihit_barcode(numHits,0);
  
  std::vector<float> sihit_startPosX(numHits,0);
  std::vector<float> sihit_startPosY(numHits,0);
  std::vector<float> sihit_startPosZ(numHits,0);

  std::vector<float> sihit_endPosX(numHits,0);
  std::vector<float> sihit_endPosY(numHits,0);
  std::vector<float> sihit_endPosZ(numHits,0);

  int hitNumber(0);
  const InDetDD::SiDetectorElement* de = prd->detectorElement();
  if(de){
    for ( auto sihit : matchingHits ) {          
      sihit_energyDeposit[hitNumber] =  sihit.energyLoss() ;
      sihit_meanTime[hitNumber] =  sihit.meanTime() ;
      sihit_barcode[hitNumber] =  sihit.particleLink().barcode() ;
    
      // Convert Simulation frame into reco frame
      const HepGeom::Point3D<double>& startPos=sihit.localStartPosition();

      Amg::Vector2D pos= de->hitLocalToLocal( startPos.z(), startPos.y() );
      sihit_startPosX[hitNumber] =  pos[0];
      sihit_startPosY[hitNumber] =  pos[1];
      sihit_startPosZ[hitNumber] =  startPos.x();
 

      const HepGeom::Point3D<double>& endPos=sihit.localEndPosition();
      pos= de->hitLocalToLocal( endPos.z(), endPos.y() );
      sihit_endPosX[hitNumber] =  pos[0];
      sihit_endPosY[hitNumber] =  pos[1];
      sihit_endPosZ[hitNumber] =  endPos.x();
      ++hitNumber;
    }
  }

  xprd->auxdata<std::vector<float> >("sihit_energyDeposit") = sihit_energyDeposit;
  xprd->auxdata<std::vector<float> >("sihit_meanTime") = sihit_meanTime;
  xprd->auxdata<std::vector<int> >("sihit_barcode") = sihit_barcode;
  
  xprd->auxdata<std::vector<float> >("sihit_startPosX") = sihit_startPosX;
  xprd->auxdata<std::vector<float> >("sihit_startPosY") = sihit_startPosY;
  xprd->auxdata<std::vector<float> >("sihit_startPosZ") = sihit_startPosZ;

  xprd->auxdata<std::vector<float> >("sihit_endPosX") = sihit_endPosX;
  xprd->auxdata<std::vector<float> >("sihit_endPosY") = sihit_endPosY;
  xprd->auxdata<std::vector<float> >("sihit_endPosZ") = sihit_endPosZ;


}





std::vector<SiHit> SCT_PrepDataToxAOD::findAllHitsCompatibleWithCluster( const InDet::SCT_Cluster* prd, 
                                                                         const SiHitCollection* collection) const
{
  ATH_MSG_VERBOSE( "Got " << collection->size() << " SiHits to look through" );
  std::vector<SiHit>  matchingHits;
    
  // Check if we have detector element  --  needed to find the local position of the SiHits
  const InDetDD::SiDetectorElement* de = prd->detectorElement();
  if(!de)
    return matchingHits;

  std::vector<const SiHit* >  multiMatchingHits;
  
  for ( const auto&  siHit : *collection) {
    // Check if it is a SCT hit
    if( !siHit.isSCT() )
      continue;
     
    //Check if it is on the correct module
    Identifier clusterId = prd->identify();
  
    if( m_SCTHelper->barrel_ec(clusterId) != siHit.getBarrelEndcap() ||
        m_SCTHelper->layer_disk(clusterId)!= siHit.getLayerDisk()    ||  
        m_SCTHelper->phi_module(clusterId)!= siHit.getPhiModule()    ||
        m_SCTHelper->eta_module(clusterId)!= siHit.getEtaModule()    ||
        m_SCTHelper->side(clusterId)      != siHit.getSide()  ) 
       continue;

    // Now we have all hits in the module that match lets check to see if they match the cluster
    // Must be within +/- 1 hits of any hit in the cluster to be included
    ATH_MSG_DEBUG("Hit is on the same module");
    
    HepGeom::Point3D<double>  averagePosition =  siHit.localStartPosition() + siHit.localEndPosition();
    averagePosition *= 0.5;
    Amg::Vector2D pos = de->hitLocalToLocal( averagePosition.z(), averagePosition.y() );
    InDetDD::SiCellId diode = de->cellIdOfPosition(pos);
   
    for( const auto &hitIdentifier : prd->rdoList() ){
      //if( abs( int(diode.etaIndex()) - m_SCTHelper->eta_index( hitIdentifier ) ) <=1 ) 
      //if( abs( int(diode.phiIndex() - m_SCTHelper->phi_index( hitIdentifier ) ) <=1 ) 
      ATH_MSG_DEBUG("Truth Strip " <<  diode.phiIndex() << " Cluster Strip " <<   m_SCTHelper->strip( hitIdentifier ) );

      if( abs( int(diode.phiIndex()) -  m_SCTHelper->strip( hitIdentifier ) ) <=1)
      {
        multiMatchingHits.push_back(&siHit);   
        break;   
      }     
    }
  }
  
  
  //Now we will now make 1 SiHit for each true particle if the SiHits "touch" other 
  std::vector<const SiHit* >::iterator siHitIter  = multiMatchingHits.begin();
  std::vector<const SiHit* >::iterator siHitIter2 = multiMatchingHits.begin();
  ATH_MSG_DEBUG( "Found " << multiMatchingHits.size() << " SiHit " );
  for ( ; siHitIter != multiMatchingHits.end(); siHitIter++) {
    const SiHit* lowestXPos = *siHitIter;
    const SiHit* highestXPos = *siHitIter;


    // We will merge these hits
    std::vector<const SiHit* > ajoiningHits;
    ajoiningHits.push_back( *siHitIter );
  
    siHitIter2 = siHitIter+1;    
    while ( siHitIter2 != multiMatchingHits.end() ) {
    // Need to come from the same truth particle 
      if( (*siHitIter)->particleLink().barcode() != (*siHitIter2)->particleLink().barcode() ){
        ++siHitIter2;
        continue;
      }
       
      // Check to see if the SiHits are compatible with each other.
      if (fabs((highestXPos->localEndPosition().x()-(*siHitIter2)->localStartPosition().x()))<0.00005 &&
          fabs((highestXPos->localEndPosition().y()-(*siHitIter2)->localStartPosition().y()))<0.00005 &&
          fabs((highestXPos->localEndPosition().z()-(*siHitIter2)->localStartPosition().z()))<0.00005 )
      {
        highestXPos = *siHitIter2;
        ajoiningHits.push_back( *siHitIter2 );
        // Dont use hit  more than once
        siHitIter2 = multiMatchingHits.erase( siHitIter2 );
      }else if (fabs((lowestXPos->localStartPosition().x()-(*siHitIter2)->localEndPosition().x()))<0.00005 &&
                fabs((lowestXPos->localStartPosition().y()-(*siHitIter2)->localEndPosition().y()))<0.00005 &&
                fabs((lowestXPos->localStartPosition().z()-(*siHitIter2)->localEndPosition().z()))<0.00005)
      {
        lowestXPos = *siHitIter2;
        ajoiningHits.push_back( *siHitIter2 );
        // Dont use hit  more than once
        siHitIter2 = multiMatchingHits.erase( siHitIter2 );
      } else {
        ++siHitIter2;
      }
    }
    
    if( ajoiningHits.size() == 0){
      ATH_MSG_WARNING("This should really never happen");
      continue;
    }else if(ajoiningHits.size() == 1){
      // Copy Si Hit ready to return
      matchingHits.push_back( *ajoiningHits[0] );
      continue;
    } else {
    //  Build new SiHit and merge information together.  
      ATH_MSG_DEBUG("Merging " << ajoiningHits.size() << " SiHits together." );
      
      
      float energyDep(0);
      float time(0);
      for( auto& siHit :  ajoiningHits){
        energyDep += siHit->energyLoss();
        time += siHit->meanTime();    
      }
      time /= (float)ajoiningHits.size();
       
      matchingHits.push_back(  SiHit(lowestXPos->localStartPosition(), 
                                     highestXPos->localEndPosition(),
                                     energyDep,
                                     time,
                                     (*siHitIter)->particleLink().barcode(),
                                     1, // 0 for pixel 1 for SCT
                                     (*siHitIter)->getBarrelEndcap(),
                                     (*siHitIter)->getLayerDisk(),
                                     (*siHitIter)->getEtaModule(),
                                     (*siHitIter)->getPhiModule(),
                                     (*siHitIter)->getSide() ) );
    }
  } 
  

  return matchingHits;
  
}

void SCT_PrepDataToxAOD::addRDOInformation(xAOD::TrackMeasurementValidation* xprd, 
                                           const InDet::SCT_Cluster* prd) const{


  std::vector<int> strip;
  std::vector<int> timebin;
  std::vector<int> groupsize;
  
  for( const auto &hitIdentifier : prd->rdoList() ){
    auto result = m_IDtoRAWDataMap.find(hitIdentifier);
    if( result != m_IDtoRAWDataMap.end() ){
      const SCT_RDORawData *sctRdo = result->second;
      const SCT3_RawData* rdo3 = dynamic_cast<const SCT3_RawData*>(sctRdo);
      int tbin(-1);
      int gs(-1);
      if (rdo3!=0){
        tbin = rdo3->getTimeBin();
        gs = rdo3->getGroupSize();
      }
      timebin.push_back( tbin);
      groupsize.push_back( gs);
      strip.push_back(m_SCTHelper->strip(sctRdo->identify())); 
    } else {
      timebin.push_back( -1 );
      strip.push_back( -1 );
      groupsize.push_back( -1 );     
    }
  }
  
  xprd->auxdata< std::vector<int> >("rdo_strip") = strip;
  xprd->auxdata< std::vector<int> >("rdo_timebin") = timebin;
  xprd->auxdata< std::vector<int> >("rdo_groupsize") = groupsize;
  
}

void SCT_PrepDataToxAOD::handle(const Incident& inc) {
  
  /// clear map of RDOs<->identifiers
  if ( m_writeRDOinformation && inc.type() == IncidentType::EndEvent ){
    ATH_MSG_VERBOSE("'EndEvent' incident caught. Refreshing Cache.");
    m_IDtoRAWDataMap.clear();
  }     
}




/////////////////////////////////////////////////////////////////////
//
//        Finalize method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode SCT_PrepDataToxAOD::finalize()
{
  return StatusCode::SUCCESS;
}
