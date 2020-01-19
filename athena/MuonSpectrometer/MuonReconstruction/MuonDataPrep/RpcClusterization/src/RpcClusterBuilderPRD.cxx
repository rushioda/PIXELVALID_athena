/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
    August 2003. Andrea Di Simone.
    Based on CscClusterBuilder by Ketevi A. Assamagan
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "RpcClusterization/RpcClusterBuilderPRD.h"

#include  <algorithm>
#include  <cmath>
#include  <cassert>
#include <fstream>
#include <strstream>

//#include "TrkEventPrimitives/ErrorMatrix.h"
//#include "TrkEventPrimitives/CovarianceMatrix.h"


using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

RpcClusterBuilderPRD::RpcClusterBuilderPRD(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_temp_coll(NULL), m_rpcClusterContainer(NULL),
  m_activeStore(NULL), m_EvtStore(NULL), m_muonMgr(NULL), m_rpcId(NULL)
{

  // Declare the properties
    declareProperty("InputCollectionName",  m_colKeyIn = "RPC_Measurements");  // StoreGate key for RPC clusters
    declareProperty("CollectionName",  m_colKey = "rpcClusters");  // StoreGate key for RPC clusters
    declareProperty("ClusterTimeSpread",  m_timeSpread = 15.);
}


StatusCode RpcClusterBuilderPRD::initialize(){

  StatusCode sc;

  // Store Gate active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get ActiveStoreSvc ");
    return sc ;
  }

  // Store Gate transient event store
  sc = serviceLocator()->service("StoreGateSvc", m_EvtStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get StoreGateSvc ");
    return sc ;
  }

  // initialize the detectore store service and GeoModel
  StoreGateSvc* detStore=0;
  sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("DetectorStore service not found !");
    return sc;
  } 
  
  sc = detStore->retrieve( m_muonMgr );
  if (sc.isFailure()) {
      ATH_MSG_ERROR(" Cannot retrieve MuonGeoModel ");
      return sc;
  }
  
  m_rpcId = m_muonMgr->rpcIdHelper();
   
  

  // Create an empty cluster container
  //  m_rpcClusterContainer = new  Muon::RpcPrepDataContainer(m_rpcId->module_hash_max()); 
  //  m_rpcClusterContainer->addRef();
  

  return sc;

}

StatusCode RpcClusterBuilderPRD::execute() {
 
  StatusCode sc;

  m_rpcClusterContainer = new  Muon::RpcPrepDataContainer(m_rpcId->module_hash_max()); 

  //  m_rpcClusterContainer->cleanup();
  sc = m_EvtStore->record(m_rpcClusterContainer,m_colKey);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Cannot record RPC Cluster Container ");
    return StatusCode::FAILURE;
  }


  sc=fill_rpcClusterContainer();

  if(sc.isFailure()) ATH_MSG_WARNING("couldn't build clusters for this event");

  // try to retrieve the collection just created and print it out
  //  if (!sc.isFailure())
  //  sc = retrieve_rpcClusterContainer();

  return sc;

}

StatusCode RpcClusterBuilderPRD::finalize() {
 
   //delete m_digit_position;

   return StatusCode::SUCCESS;

}

StatusCode RpcClusterBuilderPRD::fill_rpcClusterContainer() {

  StatusCode sc;

  const Muon::RpcPrepDataContainer* container;
  sc = m_EvtStore->retrieve(container,m_colKeyIn);
  if (sc.isFailure()) {
    ATH_MSG_WARNING(" Cannot retrieve RPC Digit Container with key " << m_colKeyIn.c_str() );
    return StatusCode::SUCCESS;
  }

  Muon::RpcPrepDataContainer::const_iterator rpcCollection=container->begin();
  Muon::RpcPrepDataContainer::const_iterator lastCollection=container->end();

  //  sc = m_EvtStore->retrieve(rpcCollection, lastCollection);
  //  if (sc.isFailure()) {
  //  ATH_MSG_ERROR(" Cannot retrieve RPC Digit collections ");
  //  return StatusCode::SUCCESS;
  // }
  
  //  const MuonDetElemHash& hashF = container->hashFunc();
  //  int n = hashF.max(); 

  for (  ; rpcCollection!=lastCollection; ++rpcCollection ){
    
    const Muon::RpcPrepDataCollection * theCollection= *rpcCollection;

    if(theCollection->size()>0){
      
      
      // create temp collection for clusters 	 
      Identifier elementId = m_rpcId->elementID(theCollection->identify()); 	 
      IdContext rpcContext = m_rpcId->module_context(); 	 
      IdentifierHash rpcHashId; 	 
      
      if (m_rpcId->get_hash(elementId, rpcHashId, &rpcContext)) { 	 
	ATH_MSG_ERROR("Unable to get RPC hash id from CSC PreDataCollection collection id"
	    << "context begin_index = " << rpcContext.begin_index()
	    << " context end_index  = " << rpcContext.end_index()
	    << " the identifier is "); 	 
	elementId.show(); 	 
      } 	 
      
      m_temp_coll = new Muon::RpcPrepDataCollection(rpcHashId); 	 
      m_temp_coll->setIdentifier(elementId); 	 
      m_coll_vect.push_back(m_temp_coll);
      
      // build the patterns
      
      if(buildPatterns(theCollection)) buildClusters(elementId);
      
    }
    //clear map at each collection
    m_digits.clear();
  }
  
  //before returning, store collections into container:

  std::vector<Muon::RpcPrepDataCollection*>::iterator coll_begin=m_coll_vect.begin();
  std::vector<Muon::RpcPrepDataCollection*>::iterator coll_end=m_coll_vect.end();
  
  for(unsigned int k=0;k<m_coll_vect.size();k++){

    sc = m_rpcClusterContainer->addCollection(m_coll_vect[k], m_coll_vect[k]->identifyHash());
    if (sc.isFailure()) 
      ATH_MSG_ERROR("Couldn't record RpcPrepDataCollection with key=" << m_colKey
	  << " in StoreGate!");

  }

//   for(;coll_begin!=coll_end;++coll_begin){
    
//     sc = m_rpcClusterContainer->addCollection(*coll_begin, (*coll_begin)->identifyHash());
//     if (sc.isFailure()) 
//       ATH_MSG_ERROR("Couldn't record RpcPrepDataCollection with key=" << m_colKey
// 	  << " in StoreGate!");
    
//   }
  
  m_coll_vect.clear();
  
  return StatusCode::SUCCESS;
}

int RpcClusterBuilderPRD::buildPatterns(const Muon::RpcPrepDataCollection* rpcCollection) {

  // here we loop over the digits in the collection and fill the patterns

  //const  Muon::RpcPrepDataCollection rpcCollection = *iter; 

  vector<Muon::RpcPrepData*> theDigits= rpcCollection->stdcont();
    
  Identifier eleId=rpcCollection->identify();

  // loop over digits in collection
  
  for(unsigned int j=0;j< theDigits.size();j++){

    Muon::RpcPrepData* rpcDigit = theDigits[j];
    
    if(!(rpcDigit->triggerInfo())){ // remove trigger patterns. temporary
      
      Identifier id=rpcDigit->identify();
      int nstrip=m_rpcId->strip(id);
      int doubletZ=m_rpcId->doubletZ(id);
      int doubletPhi=m_rpcId->doubletPhi(id);
      int gasGap=m_rpcId->gasGap(id);
      int measPhi=m_rpcId->measuresPhi(id);
      
      // use Id of first strip to identify a panel.
      
      Identifier panelId=m_rpcId->channelID(eleId,doubletZ,doubletPhi,gasGap,measPhi,1);
      
      if(m_digits.find(panelId)==m_digits.end()){ // first hit on this panel
	
	pattern newPatt;
	newPatt[nstrip]=rpcDigit;
	m_digits[panelId]=newPatt;
	
      } else { // use existing pattern
	
	if(m_digits[panelId].find(nstrip)==m_digits[panelId].end()){ // no hits on this strip before

	  m_digits[panelId][nstrip]=rpcDigit;

	} else if(m_digits[panelId][nstrip]->time()> rpcDigit->time()){

	  m_digits[panelId][nstrip]=rpcDigit; // if more than one digit, keep only the one with lowest time	  

	}
	
	
      }
    }
  }
  
  return m_digits.size();
  
}

void RpcClusterBuilderPRD::buildClusters(Identifier elementId) {

  // loop over existing patterns

  map<Identifier, pattern >::iterator patt_it=m_digits.begin();

  for(;patt_it!=m_digits.end();++patt_it){ 

    Identifier panelId=(*patt_it).first; // this is the panel id

    int measphi=m_rpcId->measuresPhi(panelId);

    //int clusteropen=0;
    
    //const RpcReadoutElement* descriptor=m_muonMgr->getRpcReadoutElement(elementId);
    
    map<int, Muon::RpcPrepData*, less<int> >::iterator dig_it=m_digits[panelId].begin();

    vector<Identifier> theIDs;
    vector<int> theAmbFlags;
    int lastStrip=-999;
    float lastTime=-999;
    float mintime=0;
    Amg::Vector3D localPosition(0,0,0);
    Amg::Vector3D globalPosition(0,0,0);
    unsigned int count=0;
    //    vector<Trk::PrepRawData*> digitsInCluster;
    
    // get the ID from the first digit in collection
    
    for(;dig_it!=m_digits[panelId].end();++dig_it){ // loop over patterns

      const RpcReadoutElement* descriptor=m_muonMgr->getRpcReadoutElement((*dig_it).second->identify());
    
      if(lastStrip==-999){ // first hit of a cluster..
	
	//std::cout<<"    opening a cluster at strip "<< (*dig_it).first<<std::endl;
	//clusteropen=1;
	lastStrip=(*dig_it).first;
	theIDs.push_back((*dig_it).second->identify());
	theAmbFlags.push_back((*dig_it).second->ambiguityFlag());
	//	digitsInCluster.push_back((*dig_it).second);
	globalPosition+=descriptor->stripPos((*dig_it).second->identify());
	localPosition+=descriptor->localStripPos((*dig_it).second->identify());
	mintime=(*dig_it).second->time();
	lastTime=(*dig_it).second->time();
	
      } else if(abs(lastStrip-(*dig_it).first)==1 &&
		abs(lastTime-(*dig_it).second->time())<m_timeSpread){ // still on the same cluster, with acceptable time spread

	//std::cout<<"    adding to a cluster the strip "<< (*dig_it).first<<std::endl;;
	lastStrip=(*dig_it).first;
	theIDs.push_back((*dig_it).second->identify());
	theAmbFlags.push_back((*dig_it).second->ambiguityFlag());
	//	digitsInCluster.push_back((*dig_it).second);
	globalPosition+=descriptor->stripPos((*dig_it).second->identify());
	localPosition+=descriptor->localStripPos((*dig_it).second->identify());
	if(mintime>(*dig_it).second->time()) mintime=(*dig_it).second->time(); //keep the lowest time in the cluster
	lastTime=(*dig_it).second->time();	
	
      } else { // close the cluster
	//std::cout<<"    closing the cluster "<<std::endl;

	// determine ambiguity flag:
	// if one of the strips has abiguity=1, then all the cluster inherits it
	// otherwise we assign to the cluster the max of the abiguities of the strips

	int clusAmbFlag=-1;
	bool hasGoldenStrip=false;

	//	std::cout<<"deciding amb flag"<<std::endl;
	for(unsigned int k=0;k<theAmbFlags.size();++k){
	  clusAmbFlag=max(clusAmbFlag,theAmbFlags[k]);
	  //	  std::cout<<theAmbFlags[k]<< " "<<clusAmbFlag<<std::endl;
	  if(theAmbFlags[k]==1) hasGoldenStrip=true;
	}
	if(hasGoldenStrip) clusAmbFlag=1;

	//	std::cout<<"decision is "<<clusAmbFlag<<std::endl;

	localPosition=localPosition*(1/(float)theIDs.size());
	globalPosition=globalPosition*(1/(float)theIDs.size());
	float width=descriptor->StripWidth(measphi);
	AmgSymMatrix(1) mat ;
	mat(0,0) = 1.;
	double err = theIDs.size()*width/sqrt((double)12);
	mat *= err*err;
	const Amg::MatrixX * errClusterPos = new Amg::MatrixX(mat);
	//Trk::CovarianceMatrix * cov = new Trk::CovarianceMatrix(mat);
	//const Trk::ErrorMatrix * errClusterPos = new Trk::ErrorMatrix(cov);

	//DataVector<Trk::PrepRawData>* DV = new DataVector<Trk::PrepRawData>(SG::VIEW_ELEMENTS);
	//	for(int k=0;k<digitsInCluster.size();++k){
	//	  DV->push_back(digitsInCluster[k]);
	//	}

	IdContext rpcContext = m_rpcId->module_context();
	IdentifierHash rpcHashId;
	m_rpcId->get_hash(elementId, rpcHashId, &rpcContext);
        
	Amg::Vector2D pointLocPos ;
	descriptor->surface(panelId).globalToLocal(globalPosition,globalPosition,pointLocPos);
	Muon::RpcPrepData* newCluster = new Muon::RpcPrepData(theIDs[0],
							      rpcHashId,
							      pointLocPos,
							      theIDs,
							      //   DV,
							      errClusterPos,
							      //width,
							      // new Trk::GlobalPosition(globalPosition),
							      descriptor,
							      mintime,
							      false,
							      clusAmbFlag);
	push_back(newCluster);
	
	// clear all the numbers and restart a new cluster
	
	lastStrip=(*dig_it).first;
	theIDs.clear();
	theAmbFlags.clear();
	//digitsInCluster.clear();
	theIDs.push_back((*dig_it).second->identify());
	theAmbFlags.push_back((*dig_it).second->ambiguityFlag());
	//std::cout<<"    opening2 a cluster at strip "<< (*dig_it).first<<std::endl;
	//	digitsInCluster.push_back((*dig_it).second);
	globalPosition=descriptor->stripPos((*dig_it).second->identify());
	localPosition=descriptor->localStripPos((*dig_it).second->identify());
	mintime=(*dig_it).second->time();
	lastTime=(*dig_it).second->time();	

      }
      
      
      // if we are at the end, close the custer anyway and fill it with what we have found
      
      if(count==m_digits[panelId].size()-1&&theIDs.size()>0){



	// determine ambiguity flag:
	// if one of the strips has abiguity=1, then all the cluster inherits it
	// otherwise we assign to the cluster the max of the abiguities of the strips
	
	int clusAmbFlag=-1;
	bool hasGoldenStrip=false;

	//	std::cout<<"deciding amb flag"<<std::endl;
	for(unsigned int k=0;k<theAmbFlags.size();++k){
	  clusAmbFlag=max(clusAmbFlag,theAmbFlags[k]);
	  //	  std::cout<<theAmbFlags[k]<< " "<<clusAmbFlag<<std::endl;
	  if(theAmbFlags[k]==1) hasGoldenStrip=true;
	}
	if(hasGoldenStrip) clusAmbFlag=1;

	//	std::cout<<"decision is "<<clusAmbFlag<<std::endl;
	
	IdContext rpcContext = m_rpcId->module_context();
	IdentifierHash rpcHashId;
	m_rpcId->get_hash(elementId, rpcHashId, &rpcContext);
	
	//std::cout<<"    closing2 the cluster "<<std::endl;
	localPosition=localPosition*(1/(float)theIDs.size());
	globalPosition=globalPosition*(1/(float)theIDs.size());
	double width=descriptor->StripWidth(measphi);
	AmgSymMatrix(1) mat;
	mat(0,0) = 1.;
	double err = theIDs.size()*width/sqrt((double)12);
	mat *= err*err;
	const Amg::MatrixX * errClusterPos = new Amg::MatrixX(mat);
	//Trk::CovarianceMatrix * cov = new Trk::CovarianceMatrix(mat); 
	//const Trk::ErrorMatrix * errClusterPos = new Trk::ErrorMatrix(cov);
	//	DataVector<Trk::PrepRawData>* DV = new DataVector<Trk::PrepRawData>(SG::VIEW_ELEMENTS);
	///	for(int k=0;k<digitsInCluster.size();++k){
	//	  DV->push_back(digitsInCluster[k]);
	//	}
	
	Amg::Vector2D pointLocPos ;
	descriptor->surface(panelId).globalToLocal(globalPosition,globalPosition,pointLocPos);
	Muon::RpcPrepData* newCluster = new Muon::RpcPrepData(theIDs[0],
							      rpcHashId,
							      pointLocPos,
							      theIDs,
							      // DV,
							      errClusterPos,
							      // width,
							      // new Trk::GlobalPosition(globalPosition),
							      descriptor,
							      mintime,
							      false,
							      clusAmbFlag);
	push_back(newCluster);

      }
      
      
      count++;
      
    }
  // outfile.close();
  
  }

}

void RpcClusterBuilderPRD::push_back(Muon::RpcPrepData *& newCluster){

  //  StatusCode status = StatusCode::SUCCESS;
  
  m_temp_coll->push_back(newCluster);


  //  Identifier elementId = m_rpcId->elementID(newCluster->identify());
  // IdContext rpcContext = m_rpcId->module_context();
 //  Muon::RpcPrepDataContainer::KEY key = m_rpcClusterContainer->key(elementId);

//    if (!m_EvtStore->contains<Muon::RpcPrepDataCollection>(key)) {
//       IdentifierHash rpcHashId;
//       if (m_rpcId->get_hash(elementId, rpcHashId, &rpcContext)) {
//          ATH_MSG_ERROR("Unable to get RPC hash id from RPC PreDataCollection collection id" 
// 	     << "context begin_index = " << rpcContext.begin_index()
// 	     << " context end_index  = " << rpcContext.end_index()
// 	     << " the identifier is ");
//          elementId.show();
//       }
//       Muon::RpcPrepDataCollection * newCollection = new Muon::RpcPrepDataCollection(rpcHashId);
//       newCollection->setIdentifier(elementId);
//       newCollection->push_back(newCluster);
//       status = m_rpcClusterContainer->addCollection(newCollection, newCollection->identify());
//       if (status.isFailure()) 
//          ATH_MSG_ERROR("Couldn't record RpcPrepdataCollection with key=" << key 
// 	     << " in StoreGate!");
//    } else {  
//       Muon::RpcPrepDataCollection * oldCollection;
//       status = m_EvtStore->retrieve(oldCollection, key);
//       if (status.isFailure())
// 	   ATH_MSG_ERROR("Couldn't retrieve RpcDigitCollection with key=" 
// 	     << key << " from StoreGate!");
//       oldCollection->push_back(newCluster);
//    }
}

StatusCode RpcClusterBuilderPRD::retrieve_rpcClusterContainer() const {

  StatusCode sc;

  typedef Muon::RpcPrepDataCollection::const_iterator cluster_iterator;

  const Muon::RpcPrepDataContainer* PRDcontainer;
  sc = m_EvtStore->retrieve(PRDcontainer,m_colKey);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Cannot retrieve RPC PrepData Cluster Container ");
    return StatusCode::FAILURE;
  }

 for (Muon::RpcPrepDataContainer::const_iterator container_iterator=PRDcontainer->begin();
                                 container_iterator != PRDcontainer->end();
                                 ++container_iterator) {

    const Muon::RpcPrepDataCollection* collection = *container_iterator;
    ATH_MSG_INFO("Size of the collection is " << collection->size() );
    if (collection->size() > 0) {
       ATH_MSG_INFO("**************************************************************");
       cluster_iterator beginCluster = collection->begin();
       cluster_iterator endCluster   = collection->end();
       for ( ; beginCluster!=endCluster; ++beginCluster) {
          const Muon::RpcPrepData* cluster = *beginCluster;
          Amg::Vector3D position = cluster->globalPosition();
          ATH_MSG_INFO("RPC Cluster ID, Position (mm), size = " 
	      << m_rpcId->show_to_string(cluster->identify()) << " ["
	      << setiosflags(ios::fixed) << setprecision(3) << setw(12) << position.x() 
	      << setiosflags(ios::fixed) << setprecision(3) << setw(12) << position.y() 
	      << setiosflags(ios::fixed) << setprecision(3) << setw(12) << position.z() 
	    //  << setiosflags(ios::fixed) << setprecision(3) << setw(12) << cluster->width()
	      << setiosflags(ios::fixed) << setprecision(3) << setw(12) << cluster->rdoList().size()
	      << " ]"); 
       }
       ATH_MSG_INFO("Number of Clusters in the collection is " << collection->size() );
       ATH_MSG_INFO("**************************************************************");
    }
 } 

  return StatusCode::SUCCESS;
}
