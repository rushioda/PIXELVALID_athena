/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCaloObjectBuilder.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  24th January, 2005

********************************************************************/

#include "eflowRec/eflowCaloObjectBuilder.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowISubtractionAlgTool.h"
#include "eflowRec/eflowRecoverSplitShowersTool.h"
#include "eflowRec/eflowLCCalibTool.h"
#include "eflowRec/eflowObjectCreatorTool.h"

#include "FourMomUtils/P4DescendingSorters.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/ListItem.h"

#include <iostream>
using namespace std;


eflowCaloObjectBuilder::eflowCaloObjectBuilder(const std::string& name,  ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_eflowClustersOutputName("PFOClusters_JetETMiss"),
  m_eflowCaloObjectsName("eflowCaloObjects01"),
  m_eflowRecTracksName("eflowRecTracks01"),
  m_eflowRecClustersName("eflowRecClusters01"),
  m_storeGate(nullptr),
  m_tools(this)
{
  /* The following properties can be specified at run-time (declared in jobOptions file) */
  /* Tool configuration */
  declareProperty( "PrivateToolList",  m_tools, "List of Private AlgTools" );
  /* Name if eflow cluster container to be created */
  declareProperty("EflowClustersOutputName",m_eflowClustersOutputName);
  /* Name of eflow Calo Object to be retrieved */
  declareProperty("EflowCaloObjectsName",m_eflowCaloObjectsName);
  /* Name of eflow Rec Track to be retrieved */
  declareProperty("EflowRecTracksName",m_eflowRecTracksName);
  /* Name of eflow Rec Cluster to be retrieved */
  declareProperty("EflowRecClustersName",m_eflowRecClustersName);
}

eflowCaloObjectBuilder::~eflowCaloObjectBuilder() { }

StatusCode eflowCaloObjectBuilder::initialize() {
  
  msg(MSG::DEBUG) << "Initialising eflowCaloObjectBuilder " << endmsg;

  if (service("StoreGateSvc", m_storeGate).isFailure()) {
    msg(MSG::WARNING) << "Unable to retrieve pointer to StoreGateSvc" << endmsg;
    return StatusCode::SUCCESS;
  } 

  /* Tool service */
  IToolSvc* myToolSvc;
  if ( service("ToolSvc",myToolSvc).isFailure() ) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endmsg;
    return StatusCode::SUCCESS;
  }

  if ( m_tools.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Failed to retrieve " << m_tools << endmsg;
    return StatusCode::SUCCESS;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_tools << endmsg;
  }
  
  // print the list of tools - taken from JetRec/JetAlgorithm.cxx
  printTools();

  return StatusCode::SUCCESS;
}

StatusCode eflowCaloObjectBuilder::execute(){

  msg(MSG::DEBUG) << "Executing eflowCaloObjectBuilder" << endmsg;
  StatusCode sc;

  /* Retrieve the eflowCaloObject container, return if not existing */
  const eflowCaloObjectContainer* caloObjectContainer;
  sc = m_storeGate->retrieve(caloObjectContainer,m_eflowCaloObjectsName);
  if(sc.isFailure() || !caloObjectContainer ){
    msg(MSG::WARNING) <<" no eflow calo object container" <<endmsg; 
    return StatusCode::SUCCESS;
  }

  /* Retrieve the eflowRecTrack container, return if not existing */
  const eflowRecTrackContainer* recTrackContainer;
  sc = m_storeGate->retrieve(recTrackContainer,m_eflowRecTracksName);
  if(sc.isFailure() || !recTrackContainer ){
    msg(MSG::WARNING) <<" no eflow rec track container" <<endmsg;
    return StatusCode::SUCCESS;
  }

  /* Retrieve the eflowRecCluster container, return if not existing */
  const eflowRecClusterContainer* recClusterContainer;
  sc = m_storeGate->retrieve(recClusterContainer,m_eflowRecClustersName);
  if(sc.isFailure() || !recClusterContainer ){
    msg(MSG::WARNING) <<" no eflow rec cluster container" <<endmsg;
    return StatusCode::SUCCESS;
  }

  /* Run the AlgTools
   * --> CellLevelSubtractionTool, RecoverSplitShowersTool */
  ToolHandleArray< eflowISubtractionAlgTool >::iterator itAlgTool = m_tools.begin();
  ToolHandleArray< eflowISubtractionAlgTool >::iterator lastAlgTool  = m_tools.end();
  for (; itAlgTool != lastAlgTool; ++itAlgTool) {
    (*itAlgTool)->execute(const_cast<eflowCaloObjectContainer*>(caloObjectContainer),
                          const_cast<eflowRecTrackContainer*>(recTrackContainer),
                          const_cast<eflowRecClusterContainer*>(recClusterContainer));
  }

  /* Sort clusters by pt TODO: should this be done somewhere else? */
  //Do we need to sort the objects at all?
  //newCaloClusterContainer->sort(P4Sorters::Descending::Pt());

  /* Clear track-cluster links */
  eflowTrackClusterLink::clearInstances();

  return StatusCode::SUCCESS;

}

StatusCode eflowCaloObjectBuilder::finalize() { return StatusCode::SUCCESS; }

void eflowCaloObjectBuilder::printTools() {
  // print the list of tools - taken from JetRec/JetAlgorithm.cxx
  msg(MSG::INFO) << " " << endmsg;
  msg(MSG::INFO) << "List of tools in execution sequence of eflowCaloObjectBuilder:" << endmsg;
  msg(MSG::INFO) << "------------------------------------" << endmsg;
  msg(MSG::INFO) << " " << endmsg;
  ToolHandleArray<eflowISubtractionAlgTool>::iterator itTool = m_tools.begin();
  ToolHandleArray<eflowISubtractionAlgTool>::iterator lastTool = m_tools.end();
  unsigned int toolCtr = 0;
  for (; itTool != lastTool; itTool++) {
    toolCtr++;
    msg(MSG::INFO) << std::setw(2) << std::setiosflags(std::ios_base::right) << toolCtr << ".) "
    << std::resetiosflags(std::ios_base::right) << std::setw(36) << std::setfill('.')
    << std::setiosflags(std::ios_base::left) << (*itTool)->type() << std::setfill('.')
    << (*itTool)->name() << std::setfill(' ') << endmsg;
  }
  msg(MSG::INFO) << " " << endmsg;
  msg(MSG::INFO) << "------------------------------------" << endmsg;
}
