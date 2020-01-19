/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LongLivedParticleDPDMaker/RPVLLTestRates.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperMetaData/SkimDecisionCollection.h" 
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include <iostream>


//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

RPVLLTestRates::RPVLLTestRates(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_EventCounter(0),
  m_tHistSvc("THistSvc",name),
  m_myTree(0),
  m_skimPassHist(0),
  m_twoDskimHist(0),
  m_runNum(0),
  m_lumiBlock(0),
  m_evtNum(0)
  //  m_filterPassed(0)
{
}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

RPVLLTestRates::~RPVLLTestRates() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize

StatusCode RPVLLTestRates::initialize() {

  m_EventCounter=0;

  StatusCode sc = m_tHistSvc.retrieve();
  if (sc.isFailure()) return StatusCode::FAILURE;

  m_myTree= new TTree("myTree","myTree");
  sc = m_tHistSvc->regTree("/AANT/myTree",m_myTree);
  if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to book TTree"<<endmsg;

  m_myTree->Branch("RunNumber",&m_runNum,"RunNumber/I");
  m_myTree->Branch("LumiBlock",&m_lumiBlock,"LumiBlock/I");
  m_myTree->Branch("EventNumber",&m_evtNum,"EventNumber/I");

  return StatusCode::SUCCESS;
} 

StatusCode RPVLLTestRates::finalize() {
  //  delete m_filterPassed;
  return StatusCode::SUCCESS;
}		 

StatusCode RPVLLTestRates::execute() {

  if (m_EventCounter==0) {

    ////////// first event! ////////////////////////
    const SkimDecisionCollection *SDcoll = 0;
    StatusCode sc = evtStore()->retrieve(SDcoll, "StreamDESDM_RPVLL_SkimDecisionsContainer");
    if (sc.isSuccess()) {
      std::cout<<"nick - booking skimPasshist"<<std::endl;
      m_skimPassHist = new TH1F("skim","skim",SDcoll->size(), 0.,(float)SDcoll->size());
      m_twoDskimHist = new TH2F("skim2D","skim",SDcoll->size(),0.,(float)SDcoll->size(),SDcoll->size(), 0.,(float)SDcoll->size());
      sc = m_tHistSvc->regHist("/AANT/skimPass",m_skimPassHist);
      if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to book TH1"<<endmsg;
      sc = m_tHistSvc->regHist("/AANT/skimPass2D",m_twoDskimHist);
      if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to book TH1"<<endmsg;
      
      m_filterPassed.resize(SDcoll->size());
      for (unsigned int i=0; i< m_filterPassed.size(); ++i) m_filterPassed[i]=0;
      SkimDecisionCollection::const_iterator itr=SDcoll->begin(), itrEnd = SDcoll->end();
      int filterCount = 0;
      for(; itr != itrEnd; itr++) {
	filterCount++;
	std::string name = (*itr)->getName();
	const char* cname = name.c_str();
	m_skimPassHist->GetXaxis()->SetBinLabel(filterCount,cname);
	m_twoDskimHist->GetXaxis()->SetBinLabel(filterCount,cname);
	m_twoDskimHist->GetYaxis()->SetBinLabel(filterCount,cname);
	
	m_myTree->Branch(name.c_str(),&(m_filterPassed[filterCount-1]),(name+"/I").c_str());
      }
    } 
  }

  m_EventCounter++;

  const EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve event info" );
  }
  m_runNum     = eventInfo->event_ID()->run_number();
  m_evtNum   = eventInfo->event_ID()->event_number();
  m_lumiBlock     = eventInfo->event_ID()->lumi_block();
  
  
  //// these are the ones that are useful for RPVLL filters
  const SkimDecisionCollection *SDcoll = 0;
  sc = evtStore()->retrieve(SDcoll, "StreamDESDM_RPVLL_SkimDecisionsContainer");
  int isAc=0;

  if (sc.isSuccess()){
    SkimDecisionCollection::const_iterator itr=SDcoll->begin(), itrEnd = SDcoll->end();
    int filterCount = 0;
    for(; itr != itrEnd; itr++) {
      isAc = (*itr)->isAccepted();
      m_skimPassHist->Fill((float)filterCount+0.5,isAc);
      m_filterPassed[filterCount] = isAc;
      std::string name = (*itr)->getName();
      msg(MSG::INFO) << "SkimDecName = " << name <<" isAccepted: "<< isAc << endmsg;
      
      if (isAc>0) {
	int filterCount2=0;
	SkimDecisionCollection::const_iterator itr2=SDcoll->begin(), itrEnd2 = SDcoll->end();
	for(; itr2 != itrEnd2; itr2++) {
	  int isAc2 = (*itr2)->isAccepted();
	  m_twoDskimHist->Fill((float)filterCount+0.5,filterCount2+0.5,isAc2);
	  filterCount2++;
	  
	}
      }
      filterCount++;
    }
  } 
  else {
    msg(MSG::WARNING )<< "No SkimDecisionCollection was found: key = DESDM_RPVLL_SkimDecisionsContaine"<< endmsg;
  }
  
  
  m_myTree->Fill();

  return StatusCode::SUCCESS;
}
