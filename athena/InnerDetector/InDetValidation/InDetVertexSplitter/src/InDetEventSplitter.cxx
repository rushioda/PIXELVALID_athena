/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////////
/// Author: Peter V. Loscutoff (ploscutoff@gmail.com
/// February 2009
/////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/DataHandle.h"

/// head file
#include "InDetEventSplitter.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"

#include <algorithm>
#include <math.h>
#include <map>
#include <sstream>
#include <cstdlib>
#include <ctime>

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

InDet::InDetEventSplitter::InDetEventSplitter(const std::string& name,
  ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  isOdd(false),
  addToVx(0),
  eventN(0){

  /// switches to control the analysis through job options
  declareProperty("TPBContainerName", m_tpbContainerName = "TrackParticleCandidate");
  declareProperty("TrackContainerName", m_trackContainerName = "Tracks");
  declareProperty("MaxVertexNumber", m_maxVtx = 1); //this should not be changed until a more robust handling of multi vertices is implemented
  declareProperty("PrimaryOnly",m_priOnly = true);  //this should not be changed presently
  declareProperty("UseTrackParticleBase",m_savetpb = true); //is this needed?
  declareProperty("RandomSeed",m_rndSeed = 7);
}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

InDet::InDetEventSplitter::~InDetEventSplitter() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate

StatusCode InDet::InDetEventSplitter::initialize() {
  
  std::srand(m_rndSeed);
  isOdd = false;
  addToVx = 1;
  eventN = 0;

  for (int i = 1; i <=m_maxVtx; i++){
    std::stringstream ss;
    ss << "odd_" << i << "_Tracks";
    m_trackKeys.push_back(ss.str());
    ss.str("");
    ss << "even_" << i << "_Tracks";
    m_trackKeys.push_back(ss.str()); 
    ss.str("");
    ss << "all_" << i << "_Tracks";
    m_trackKeys.push_back(ss.str());
    ss.str("");
  }

  ATH_MSG_INFO ("Initializing InDetEventSplitter");
  
  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode InDet::InDetEventSplitter::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode InDet::InDetEventSplitter::execute() {

  ATH_MSG_DEBUG("in execute()");

  StatusCode sc = StatusCode::SUCCESS;

  sc = split_vertices();
  if (sc.isFailure()) {
     ATH_MSG_ERROR("InDetEventSplitter Failed");
     return sc;
  }

  return sc;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode InDet::InDetEventSplitter::split_vertices() {

  ATH_MSG_DEBUG("in split_vertices()");

  StatusCode sc = StatusCode::SUCCESS;

  const Rec::TrackParticleContainer* tpbTES=0;
  const TrackCollection* trkTES=0;

  if (m_savetpb){
    sc=evtStore()->retrieve( tpbTES, m_tpbContainerName);
    if( sc.isFailure()  ||  !tpbTES ) {
      ATH_MSG_WARNING("No TrackParticleBase container found in TDS tried " << m_tpbContainerName);
      return StatusCode::SUCCESS;
    }  
    ATH_MSG_DEBUG("TrackParticleCandidate Collection successfully retrieved");
  }
  else {
    sc=evtStore()->retrieve( trkTES, m_trackContainerName);
    if( sc.isFailure()  ||  !trkTES ) {
      ATH_MSG_WARNING("No TrackCollection container found in TDS tried " << m_trackContainerName);
      return StatusCode::SUCCESS;
    }  
    ATH_MSG_DEBUG("TrackParticleCandidate Collection successfully retrieved");
  }  

  std::map<std::string,TrackCollection*> trackmap;
  std::map<std::string,Trk::TrackParticleBaseCollection*> tpbmap;

//  We need to create every container for each event, even if we don't write to them

  std::vector<std::string>::iterator key_iter = m_trackKeys.begin();
  for (; key_iter != m_trackKeys.end(); key_iter++){
    TrackCollection* tempTracks = 0;
    trackmap[(*key_iter)] = tempTracks;
    if (evtStore()->contains<TrackCollection>((*key_iter)) &&
	(evtStore()->retrieve(trackmap[(*key_iter)],(*key_iter))).isSuccess()){
    } else {
      trackmap[(*key_iter)] = new TrackCollection;
    }
  }  

  for (key_iter = m_trackKeys.begin(); key_iter != m_trackKeys.end(); key_iter++){
    Trk::TrackParticleBaseCollection* tempTpbs = 0;
    tpbmap[(*key_iter)] = tempTpbs;
    if (evtStore()->contains<Trk::TrackParticleBaseCollection>((*key_iter)) &&
	(evtStore()->retrieve(tpbmap[(*key_iter)],(*key_iter))).isSuccess()){
    } else {
      tpbmap[(*key_iter)] = new Trk::TrackParticleBaseCollection;
    }
  }  

  //We need to add an approprate fraction of unfit tracks to the half and full vertex collections
  //lets pull in the full list of tracks  

  if (m_savetpb){
    Rec::TrackParticleContainer::const_iterator tpbItr = tpbTES->begin();
    Rec::TrackParticleContainer::const_iterator tpbItrE = tpbTES->end();
    //we loop over that list
    for (; tpbItr != tpbItrE; tpbItr++){
      //it looks like our track collection is actually sorted by the vertex that they're in
      //which means that just alternating odd vs even is equivalent to splitting the vertex first, then splitting the remining
      //instead, we will just put in rand() call
      isOdd = std::rand() % 2;
      std::string oeNameString;
      if (isOdd)  oeNameString = "odd";
      if (!isOdd) oeNameString = "even";
      std::stringstream sss;
      sss << oeNameString << "_" << addToVx << "_Tracks";
      std::string oecontainerName = sss.str();   		  	
      std::string allNameString = "all";
      sss.str("");
      sss << allNameString << "_" << addToVx << "_Tracks";
      std::string allcontainerName = sss.str();  
      Trk::TrackParticleBase *trkCopy1 = new Trk::TrackParticleBase((*(*tpbItr)));
      Trk::TrackParticleBase *trkCopy2 = new Trk::TrackParticleBase((*(*tpbItr)));
      ATH_MSG_DEBUG("found a trackparticlebase, with momentum "<<(*tpbItr)->definingParameters().momentum()<<" giving it the key: "<< oecontainerName);	
      tpbmap[oecontainerName]->push_back(trkCopy1);
      ATH_MSG_DEBUG("found a trackparticlebase, with momentum "<<(*tpbItr)->definingParameters().momentum()<<" giving it the key: "<< allcontainerName);	
      tpbmap[allcontainerName]->push_back(trkCopy2);	
      
      addToVx++;
      if (addToVx > m_maxVtx) addToVx = 1;
    }
  }
  
  
  if (!m_savetpb){
    std::cout<<"NotYet Implemented"<<std::endl;
  }

  if (m_savetpb){
        for (key_iter = m_trackKeys.begin(); key_iter != m_trackKeys.end() ; key_iter++){
      if(evtStore()->record(tpbmap[(*key_iter)],(*key_iter),false).isFailure() ){
	ATH_MSG_ERROR("Could not save the "<< (*key_iter));
      }}}
  else{
    for (key_iter = m_trackKeys.begin(); key_iter != m_trackKeys.end() ; key_iter++){
      if(evtStore()->record(trackmap[(*key_iter)],(*key_iter),false).isFailure() ){
	ATH_MSG_ERROR("Could not save the "<< (*key_iter));
      }}}

    
  ATH_MSG_DEBUG("split_vertices() succeeded");
  
  eventN++;
  return StatusCode::SUCCESS;
}

