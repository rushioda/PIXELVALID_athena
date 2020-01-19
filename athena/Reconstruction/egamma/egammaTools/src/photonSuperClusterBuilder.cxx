/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "photonSuperClusterBuilder.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
//
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
//
#include "FourMomUtils/P4Helpers.h"
//
#include "egammaInterfaces/IegammaSwTool.h"
//
#include <vector>

using CLHEP::GeV;

//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
photonSuperClusterBuilder::photonSuperClusterBuilder(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  egammaSuperClusterBuilder(type, name, parent)
{

  //Containers
  declareProperty("InputEgammaRecContainerName", m_inputEgammaRecContainerName = "egammaRecCollection");

  declareProperty("SuperPhotonRecCollectionName", m_photonSuperRecCollectionName = "PhotonSuperRecCollection");

  declareProperty("SuperClusterCollestionName",  m_outputPhotonSuperClusters  = "PhotonSuperClusters");
  //other options
  declareProperty("AddClustersInWindow", m_addClustersInWindow = true,  
		  "add the topoclusters in window");

  declareProperty("AddClustersMatchingVtx", m_addClustersMatchingVtx = true, 
		  "add the topoclusters matching conversion vertex");

  declareProperty("UseOnlyLeadingVertex", m_useOnlyLeadingVertex = true, 
		  "use only the leading vertex for matching");

  declareProperty("UseOnlySi", m_useOnlySi = true, 
		  "use only vertices/tracks with silicon tracks for adding sec. clusters (Mix not considered Si)");

  declareProperty("AddClustrsMatchingVtxTracks", m_addClustersMatchingVtxTracks = true, 
		  "add the topoclusters matching conversion vertex tracks");

  declareProperty("UseOnlyLeadingTrack", m_useOnlyLeadingTrack = true, 
		  "use only the leading track for matching");
  
  // Declare interface & properties.
  declareInterface<IphotonSuperClusterBuilder>(this);
}

//Destructor.
photonSuperClusterBuilder::~photonSuperClusterBuilder() {}

StatusCode photonSuperClusterBuilder::initialize() {
  ATH_MSG_DEBUG(" Initializing photonSuperClusterBuilder");
  return egammaSuperClusterBuilder::initialize();
}

StatusCode photonSuperClusterBuilder::finalize() {
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////////////////////////////////

StatusCode photonSuperClusterBuilder::execute(){

  //Retrieve input egammaRec container.
  const EgammaRecContainer *egammaRecs = 0;
  StatusCode sc=evtStore()->retrieve(egammaRecs, m_inputEgammaRecContainerName );
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve "<< m_inputEgammaRecContainerName);
    return StatusCode::FAILURE;
  }

  //Have to register cluster container in order to properly get cluster links.
  xAOD::CaloClusterContainer *outputClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(), 
											     m_outputPhotonSuperClusters, 
  											     msg());
  if (!outputClusterContainer) {
    ATH_MSG_ERROR("Could not make supercluster container : "<< m_outputPhotonSuperClusters);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Made supercluster container : " << m_outputPhotonSuperClusters);
  }
  //Create the new Photon Super Cluster based EgammaRecContainer
  EgammaRecContainer *newEgammaRecs = new EgammaRecContainer();
  if (evtStore()->record(newEgammaRecs, m_photonSuperRecCollectionName).isFailure()) {
    ATH_MSG_ERROR("Could not record egammaRecContainer " << m_photonSuperRecCollectionName);
    return StatusCode::FAILURE;
  } 

  return executeImpl(egammaRecs, newEgammaRecs, outputClusterContainer);
}

StatusCode photonSuperClusterBuilder::hltExecute(const EgammaRecContainer *egammaRecs,
                                                   EgammaRecContainer *newEgammaRecs,
                                                   xAOD::CaloClusterContainer *outputClusterContainer) {

  return executeImpl(egammaRecs, newEgammaRecs, outputClusterContainer);
}

StatusCode photonSuperClusterBuilder::executeImpl(const EgammaRecContainer *egammaRecs,
                                                    EgammaRecContainer *newEgammaRecs,
                                                    xAOD::CaloClusterContainer *outputClusterContainer) {
  //Loop over input egammaRec objects, build superclusters.
  std::vector<bool> isUsed (egammaRecs->size(),0);

  for (std::size_t i = 0 ; i < egammaRecs->size();++i) {    
    //Used to revert status of topos 
    //in case we fail to make a supercluser.
    std::vector<bool> isUsedRevert(isUsed);
    const auto egRec=egammaRecs->at(i);
    const auto egClus = egRec->caloCluster();
    //First some basic seed cuts
    if(isUsed.at(i)){
      continue;      
    }
    double emFrac(0.);
    if (!egClus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
      ATH_MSG_WARNING("NO ENG_FRAC_EM moment available" );
    }
    if (egClus->et()*emFrac < m_EtThresholdCut){
      continue;
    }

    //Passed preliminary custs
    ATH_MSG_DEBUG("=========================================================");
    ATH_MSG_DEBUG("Creating supercluster egammaRec photon object");
    ATH_MSG_DEBUG("Using cluster Et = " << egClus->et()  << " EMFraction " << emFrac << " EM Et " << egClus->et()*emFrac);
    //
    //So it is used
    isUsed.at(i)=1;     
    //Start accumulating
    std::vector<const xAOD::CaloCluster*>  accumulatedClusters;
    accumulatedClusters.push_back(egClus);

    //////////////////////////////
    //Core Logic goes here
    ATH_MSG_DEBUG("Find secondary clusters");

    // for stats
    int nWindowClusters = 0;
    int nExtraClusters = 0;

    const std::vector<std::size_t> secondaryClusters = 
      SearchForSecondaryClusters(i, egammaRecs, isUsed, nWindowClusters, nExtraClusters);

    for (auto secClus : secondaryClusters) {
      const auto secRec = egammaRecs->at(secClus);
      accumulatedClusters.push_back(secRec->caloCluster());
      // no need to add vertices
    }
    //
    //End of core Logic 

    ///////////////////////////////////////////////////////////////////////////////
    //Create the new cluster 

    ATH_MSG_DEBUG("Set up new Cluster");
    auto egType = (egRec->getNumberOfVertices() > 0) ? 
      xAOD::EgammaParameters::convertedPhoton :
      xAOD::EgammaParameters::unconvertedPhoton;

    xAOD::CaloCluster* newCluster = CreateNewCluster(accumulatedClusters, egType);

    if (!newCluster) {
      ATH_MSG_DEBUG("Creating a new cluster failed");
      //Revert status of constituent clusters.
      isUsed = isUsedRevert;
      continue;
    }

    //push back the new photon super cluster 
    outputClusterContainer->push_back(newCluster);

    ///////////////////////////////////////////////////////
    //Now create the new eg Rec 
    egammaRec *newEgRec = new egammaRec(*egRec);

    //Add the cluster link to the super cluster
    ElementLink< xAOD::CaloClusterContainer > clusterLink(newCluster, *outputClusterContainer);
    std::vector< ElementLink<xAOD::CaloClusterContainer> > phCluster {clusterLink};
    newEgRec->setCaloClusters(phCluster);

    // will rerun conversion finding, so conversion vertices are not copied

    //push it back
    newEgammaRecs->push_back(newEgRec);

    ATH_MSG_DEBUG("Finished making photon egammaRec object");
  } //End loop on egammaRecs
  return StatusCode::SUCCESS;
}

std::vector<std::size_t> 
photonSuperClusterBuilder::SearchForSecondaryClusters(std::size_t photonInd,
						      const EgammaRecContainer *egammaRecs,
						      std::vector<bool>& isUsed,
						      int& nWindowClusters,
						      int& nExtraClusters) const
{

  std::vector<std::size_t> secondaryClusters;
  if (!egammaRecs) {
    ATH_MSG_WARNING("photon egammaRec container is null! Returning an empty vector ...");
    return secondaryClusters;
  }

  const auto seedPhoton = egammaRecs->at(photonInd);
  const auto seedCaloClus = seedPhoton->caloCluster();
  if (!seedCaloClus) {
    ATH_MSG_WARNING("The seed egammaRec does not have a cluster");
    return secondaryClusters;
  }

  // let's determine some things about the seed
  std::vector<const xAOD::Vertex*> seedVertices;
  std::vector<xAOD::EgammaParameters::ConversionType> seedVertexType;
  std::vector<const xAOD::TrackParticle*> seedVertexTracks; // tracks from conversion vertex
  
  auto numVertices = seedPhoton->getNumberOfVertices();
  if (m_useOnlyLeadingVertex && numVertices > 0) {
    numVertices = 1;
  }

  for (std::size_t vx = 0; vx < numVertices; ++vx) {
    const auto vertex = seedPhoton->vertex(vx);
    const auto convType = xAOD::EgammaHelpers::conversionType(vertex);
    seedVertices.push_back(vertex);
    seedVertexType.push_back(convType);
    const bool addTracks = !m_useOnlySi || 
      convType == xAOD::EgammaParameters::singleSi ||
      convType == xAOD::EgammaParameters::doubleSi;
    if (addTracks) {
      for (unsigned int tp=0; tp < vertex->nTrackParticles(); ++tp){
	seedVertexTracks.push_back(vertex->trackParticle(tp));
      }
    }
  }

  // Now loop over the potential secondary clusters
  // Note: start with index after seed cluster: depends on sorted nature of egammaRecs
  for (std::size_t i = photonInd + 1; i < egammaRecs->size(); ++i) {
    
    //First some basic seed cuts
    if(isUsed.at(i)){
      continue;      
    }
    
    const auto egRec = egammaRecs->at(i);
    const auto caloClus = egRec->caloCluster();
    if (!caloClus) {
      ATH_MSG_WARNING("The potentially secondary egammaRec does not have a cluster");
      continue;
    } 
    // Now perform a number of tests to see if the cluster should be added
    bool addCluster = false;
    if (m_addClustersInWindow && 
	MatchesInWindow(seedCaloClus, caloClus)) {
      ATH_MSG_DEBUG("Cluster  with Et : " << caloClus->et()<< " matched in window");
      nWindowClusters++;
      addCluster = true;
    } 

    // should do "else if" if we want nWindowCluster and nExtraCluster to not both increment
    if (m_addClustersMatchingVtx && 
	MatchesVtx(seedVertices, seedVertexType, egRec)) {
      ATH_MSG_DEBUG("conversion vertices match");
      addCluster = true;
      nExtraClusters++;
    } else if (m_addClustersMatchingVtxTracks &&
	       MatchesVtxTrack(seedVertexTracks, egRec)) {
      ATH_MSG_DEBUG("conversion track match");
      addCluster = true;
      nExtraClusters++;
    }

    if (addCluster) {
      secondaryClusters.push_back(i);
      isUsed[i] = true;
    }      
  }
  ATH_MSG_DEBUG("Found: " << secondaryClusters.size()<< " secondaries");
  return secondaryClusters;
}


bool photonSuperClusterBuilder::MatchesVtx(const std::vector<const xAOD::Vertex*>& seedVertices,
					   const std::vector<xAOD::EgammaParameters::ConversionType>& seedVertexType,
					   const egammaRec *egRec) const
{

  auto numTestVertices = egRec->getNumberOfVertices();
  if (m_useOnlyLeadingVertex && numTestVertices > 0) {
    numTestVertices = 1;
  }
  
  for (size_t seedVx = 0; seedVx < seedVertices.size(); ++seedVx) {
    if (!m_useOnlySi || 
	seedVertexType[seedVx] == xAOD::EgammaParameters::singleSi ||
	seedVertexType[seedVx] == xAOD::EgammaParameters::doubleSi) {
      
      for (size_t testVx = 0; testVx < numTestVertices; ++testVx) {
	if (seedVertices[seedVx] == egRec->vertex(testVx)) {
	  return true;
	}
      }
    }
  }
  return false;
}

bool photonSuperClusterBuilder::MatchesVtxTrack(const std::vector<const xAOD::TrackParticle*>& seedVertexTracks,
						const egammaRec *egRec) const
{
  auto numTestTracks = egRec->getNumberOfTrackParticles();
  if (m_useOnlyLeadingTrack && numTestTracks > 0) {
    numTestTracks = 1;
  }
  for (size_t seedTk = 0; seedTk < seedVertexTracks.size(); ++seedTk) {
    // selected tracks alread are just Si if we are only looking at Si tracks
    for (size_t testTk = 0; testTk < numTestTracks; ++testTk) {
      if (seedVertexTracks[seedTk] == egRec->trackParticle(testTk)) {
	return true;
      }
    }
  }
  return false;
}
