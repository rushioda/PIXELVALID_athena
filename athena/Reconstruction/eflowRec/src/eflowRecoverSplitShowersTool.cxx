/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME: eflowRecoverSplitShowersTool.cxx
PACKAGE: offline/Reconstruction/eflowRec

AUTHOR: Thomas Velz, Mark Hodgkinson
CREATED: 16 January 2014

********************************************************************/

#include "eflowRec/eflowRecoverSplitShowersTool.h"
#include "eflowRec/eflowUtil.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCaloObjectMaker.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/IEFlowCellEOverPTool.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowSubtractor.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

using namespace eflowSubtract;

eflowRecoverSplitShowersTool::eflowRecoverSplitShowersTool(const std::string& type,const std::string& name,const IInterface* parent):
AthAlgTool(type, name, parent),
m_debug(0),
m_eflowCaloObjectContainer(0),
m_rCell(0.75),
m_windowRms(0.032),
m_theEOverPTool("eflowCellEOverPTool",this),
m_matchingTool("PFTrackClusterMatchingTool/RcvrSpltMatchingTool", this),
m_binnedParameters(new eflowEEtaBinnedParameters()),
m_integrator(new eflowLayerIntegrator(m_windowRms, 1.0e-3, 3.0)),
m_subtractionSigmaCut(1.5),
m_recoverIsolatedTracks(false),
m_nTrackClusterMatches(0),
m_useUpdated2015ChargedShowerSubtraction(true)
{
  declareInterface<eflowRecoverSplitShowersTool>(this);
  declareProperty("SubtractionSigmaCut",m_subtractionSigmaCut);
  declareProperty("eflowCellEOverPTool", m_theEOverPTool,"Energy Flow E/P Values and Shower Parameters Tool");
  declareProperty("PFTrackClusterMatchingTool", m_matchingTool, "The track-cluster matching tool");
  declareProperty("RecoverIsolatedTracks",m_recoverIsolatedTracks,"Whether to recover isolated tracks also");
  declareProperty("useUpdated2015ChargedShowerSubtraction",m_useUpdated2015ChargedShowerSubtraction,"Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density region");
  eflowRingSubtractionManager::setRMaxAndWeightRange(m_rCell, 1.0e6);
}

eflowRecoverSplitShowersTool::~eflowRecoverSplitShowersTool() {}

StatusCode eflowRecoverSplitShowersTool::initialize(){

  // tool service
  IToolSvc* myToolSvc;
  if ( service("ToolSvc",myToolSvc).isFailure() ) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_matchingTool.retrieve().isFailure()){
    msg(MSG::WARNING) << "Couldn't retrieve PFTrackClusterMatchingTool." << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_theEOverPTool.retrieve().isFailure()){
    msg(MSG::WARNING) << "Cannot find eflowEOverPTool" << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_theEOverPTool->execute(m_binnedParameters).isFailure()){
    msg(MSG::WARNING) << "Could not execute eflowCellEOverPTool " << endmsg;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

void eflowRecoverSplitShowersTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer*, eflowRecClusterContainer*){

  msg(MSG::DEBUG) << "Executing eflowRecoverSplitShowersTool" << endmsg;

  m_eflowCaloObjectContainer = theEflowCaloObjectContainer;

  recycleTracksClusters();

  /* Add each track to its matching cluster's eflowCaloObject */
  int const nOriginalObj = matchAndCreateEflowCaloObj();

  /* Extrapolate tracks, match clusters, do shower simulation, run cell subtraction */
  performRecovery(nOriginalObj);

}

StatusCode eflowRecoverSplitShowersTool::finalize(){

  msg(MSG::INFO) << "Produced " << m_nTrackClusterMatches << " track-cluster matches." << endmsg;

  delete m_binnedParameters;
  delete m_integrator;

  return StatusCode::SUCCESS;

}
void eflowRecoverSplitShowersTool::recycleTracksClusters() {
  getTracksToRecover();
  getClustersToConsider();

}

void eflowRecoverSplitShowersTool::getClustersToConsider() {

  m_clustersToConsider.clear();

  eflowCaloObjectContainer::iterator  itEFCalObject = m_eflowCaloObjectContainer->begin();
  eflowCaloObjectContainer::iterator endEFCalObject = m_eflowCaloObjectContainer->end();
  for (; itEFCalObject != endEFCalObject; ++itEFCalObject) {
    eflowCaloObject* thisEflowCaloObject = *itEFCalObject;

    if (thisEflowCaloObject->nClusters() == 0) { continue; }

    for(unsigned i=0; i<thisEflowCaloObject->nClusters(); ++i){
        /* Skip empty clusters (subtraction remnants) */
        const CaloClusterCellLink* theCellLink = thisEflowCaloObject->efRecCluster(i)->getCluster()->getCellLinks();
        if (0 == (int)theCellLink->size()){ continue; }

        thisEflowCaloObject->efRecCluster(i)->clearTrackMatches();
        m_clustersToConsider.push_back(thisEflowCaloObject->efRecCluster(i));
        thisEflowCaloObject->clearClusters();
        thisEflowCaloObject->clearLinks();
    }
  }

  std::sort(m_clustersToConsider.begin(),m_clustersToConsider.end(),eflowRecCluster::SortDescendingPt());
}

void eflowRecoverSplitShowersTool::getTracksToRecover() {

  m_tracksToRecover.clear(); int iEFCalOb=0;
  eflowCaloObjectContainer::iterator  itEFCalObject = m_eflowCaloObjectContainer->begin();
  eflowCaloObjectContainer::iterator endEFCalObject = m_eflowCaloObjectContainer->end();
  for (; itEFCalObject != endEFCalObject; ++itEFCalObject, ++iEFCalOb) {
    eflowCaloObject* thisEflowCaloObject = *itEFCalObject;

    /* Skip isolated tracks if flag set */
    if (!m_recoverIsolatedTracks && thisEflowCaloObject->nClusters() == 0) {
      unsigned int nTrk = thisEflowCaloObject->nTracks();
      // But make sure we get eflowObjects from them
      for (unsigned int iTrk = 0; iTrk < nTrk; ++iTrk) {
	eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrk);
	if (!thisEfRecTrack->isSubtracted()) thisEfRecTrack->setSubtracted();
      }
      continue;
    }

    /* Add all tracks on the eflowCaloObject that haven't been subtracted yet*/
    std::vector<eflowRecTrack*> updatedTracks; updatedTracks.clear();
    unsigned int nTracks = thisEflowCaloObject->nTracks();

    /* For cluster-only CaloObjects */
    if(nTracks == 0) continue;

    /* For track-only and track-cluster CaloObjects */
    for (unsigned int iTrack = 0; iTrack < nTracks; ++iTrack){
      eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrack);
      if (thisEfRecTrack->isSubtracted()){
        updatedTracks.push_back(thisEfRecTrack);
        continue;
      }
      thisEfRecTrack->clearClusterMatches();
      m_tracksToRecover.push_back(thisEfRecTrack);
    }
    thisEflowCaloObject->clearTracks();
    if (!updatedTracks.empty()) {
      thisEflowCaloObject->addTracks(updatedTracks);
      updatedTracks.clear();
    } else {
      thisEflowCaloObject->clearLinks();
    }
  }

  std::sort(m_tracksToRecover.begin(),m_tracksToRecover.end(),eflowRecTrack::SortDescendingPt());

}

int eflowRecoverSplitShowersTool::matchAndCreateEflowCaloObj() {
  /* Cache the original number of eflowCaloObjects */
  const int nCaloObj = m_eflowCaloObjectContainer->size();

  std::vector<eflowRecTrack*>::iterator endEfRecTrack = m_tracksToRecover.end();
  /* loop tracks in m_tracksToRecover and do matching */
  for (std::vector<eflowRecTrack*>::iterator itEfRecTrack = m_tracksToRecover.begin();
      itEfRecTrack != endEfRecTrack; ++itEfRecTrack) {
    eflowRecTrack* thisEfRecTrack = *itEfRecTrack;
    /* No point to do anything if bin does not exist */
    if (!thisEfRecTrack->hasBin()) {
      eflowCaloObject* thisEflowCaloObject = new eflowCaloObject();
      thisEflowCaloObject->addTrack(thisEfRecTrack);
      m_eflowCaloObjectContainer->push_back(thisEflowCaloObject);
      continue;
    }
    if (1 == m_debug) {
      const xAOD::TrackParticle* track = thisEfRecTrack->getTrack();
      std::cout << "Recovering charged EFO with e,eta and phi " << track->e() << ", "
                << track->eta() << " and " << track->phi() << std::endl;
    }
    /* Get list of matched clusters */
    std::vector<eflowRecCluster*> matchedClusters = m_matchingTool->doMatches(thisEfRecTrack, m_clustersToConsider, -1);
    if (matchedClusters.empty()) { continue; }

    m_nTrackClusterMatches += matchedClusters.size();
    /* Matched cluster: create TrackClusterLink and add it to both the track and the cluster (eflowCaloObject will be created later) */
    std::vector<eflowRecCluster*>::const_iterator itCluster = matchedClusters.begin();
    std::vector<eflowRecCluster*>::const_iterator endCluster = matchedClusters.end();
    for (; itCluster != endCluster; ++itCluster) {
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack,
                                                                                   (*itCluster));
      thisEfRecTrack->addClusterMatch(trackClusterLink);
      (*itCluster)->addTrackMatch(trackClusterLink);
    }
  }

  /* Create all eflowCaloObjects that contain eflowRecCluster */
  eflowCaloObjectMaker makeCaloObject;
  int nCaloObjects = makeCaloObject.makeTrkCluCaloObjects(m_tracksToRecover, m_clustersToConsider,
                                                          m_eflowCaloObjectContainer);
  msg(MSG::DEBUG) << "eflowRecoverSplitShowersTool created " << nCaloObjects << " CaloObjects." << endmsg;

  /* integrate cells; determine FLI; eoverp */
  for (unsigned int iCalo = nCaloObj; iCalo < m_eflowCaloObjectContainer->size(); ++iCalo) {
    eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainer->at(iCalo);
    thisEflowCaloObject->simulateShower(m_integrator, m_binnedParameters, m_useUpdated2015ChargedShowerSubtraction);
  }
  return nCaloObj;
}

void eflowRecoverSplitShowersTool::performSubtraction(eflowCaloObject* thisEflowCaloObject) {
  for (unsigned iTrack = 0; iTrack < thisEflowCaloObject->nTracks(); ++iTrack) {
    eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrack);
    /* Get matched cluster via Links */
    std::vector<eflowRecCluster*> matchedClusters;
    matchedClusters.clear();
    std::vector<eflowTrackClusterLink*> links = thisEfRecTrack->getClusterMatches();
    std::vector<eflowTrackClusterLink*>::iterator itLink = links.begin();
    std::vector<eflowTrackClusterLink*>::iterator endLink = links.end();
    for (; itLink != endLink; ++itLink) {
      matchedClusters.push_back((*itLink)->getCluster());
    }
    /* Do subtraction */
    std::vector<xAOD::CaloCluster*> clusterSubtractionList;
    std::vector<eflowRecCluster*>::const_iterator itCluster = matchedClusters.begin();
    std::vector<eflowRecCluster*>::const_iterator endCluster = matchedClusters.end();
    for (; itCluster != endCluster; ++itCluster) {
      clusterSubtractionList.push_back(
          (*itCluster)->getClusterForModification(eflowCaloObject::getClusterContainerPtr()));
    }
    if (getSumEnergy(clusterSubtractionList) - thisEfRecTrack->getEExpect() < m_subtractionSigmaCut
        * sqrt(thisEfRecTrack->getVarEExpect())) {
      /* Check if we can annihilate right away */
      Subtractor::annihilateClusters(clusterSubtractionList);
    } else {
      /* Subtract the track from all matched clusters */
      Subtractor::subtractTracksFromClusters(thisEfRecTrack, clusterSubtractionList);
      /* Annihilate the cluster(s) if the remnant is small (i.e. below k*sigma) */
      if (getSumEnergy(clusterSubtractionList) < m_subtractionSigmaCut
          * sqrt(thisEfRecTrack->getVarEExpect())) {
        Subtractor::annihilateClusters(clusterSubtractionList);
      }
    }
    /* Flag tracks as subtracted */
    thisEfRecTrack->setSubtracted();
  }
}

void eflowRecoverSplitShowersTool::performRecovery(int const nOriginalObj) {
  unsigned int nEFCaloObs = m_eflowCaloObjectContainer->size();
  for (unsigned int iCalo = nOriginalObj; iCalo < nEFCaloObs; ++iCalo) {
    eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainer->at(iCalo);

    performSubtraction(thisEflowCaloObject);
  }

}


double eflowRecoverSplitShowersTool::getSumEnergy(const std::vector<xAOD::CaloCluster*>& clusters) {
  double result = 0.0;
  std::vector<xAOD::CaloCluster*>::const_iterator  itCluster = clusters.begin();
  std::vector<xAOD::CaloCluster*>::const_iterator endCluster = clusters.end();
  for (; itCluster != endCluster; ++itCluster) {
    result += (*itCluster)->e();
  }
  return result;
}

void eflowRecoverSplitShowersTool::printClusterList(std::vector<xAOD::CaloCluster*>& clusters, std::string prefix) {
  std::vector<xAOD::CaloCluster*>::iterator firstMatchedClus = clusters.begin();
  std::vector<xAOD::CaloCluster*>::iterator lastMatchedClus = clusters.end();
  for (; firstMatchedClus != lastMatchedClus; ++firstMatchedClus) {
    std::cout << prefix << " have cluster with E, eta and phi of " << (*firstMatchedClus)->e()
              << ", " << (*firstMatchedClus)->eta() << " and " << (*firstMatchedClus)->phi()
              << std::endl;
  }
}
