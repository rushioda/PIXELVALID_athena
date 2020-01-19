/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// sTgcDigitizationTool
// ------------
// Authors:  Nectarios Benekos  <nectarios.benekos@cern.ch>
//           Jiaming Yu  <jiaming.yu@cern.ch>  
////////////////////////////////////////////////////////////////////////////////

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
 
//Outputs
#include "MuonDigitContainer/sTgcDigitContainer.h"

//sTGC digitization includes
#include "sTGC_Digitization/sTgcDigitizationTool.h"
#include "sTGC_Digitization/sTgcDigitMaker.h"

//Gaudi - Core
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"
#include "AIDA/IHistogram1D.h"
#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

//Geometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"

//Pile-up
#include "PileUpTools/PileUpMergeSvc.h"

//Truth
#include "CLHEP/Units/PhysicalConstants.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"

//Random Numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include <TFile.h>
#include <TH2.h>
#include <TH1.h>

namespace Trk{
   class PlaneSurface;
}

using namespace MuonGM;

typedef struct {
  uint16_t bcTag; // 001 --> PREVIOUS BC, 010 --> CURRENT BC, 100 --> NEXT BC ==> if digitTime inside the overlap of the PREVIOUS and CURRENT BC, bcTag = 011 (=3)
  float charge;  // for pad/wire response, charge = 0. 
  int channelType; // 0 --> pad, 1 --> strip,  2 --> wire
  MuonSimData::Deposit Dep;
  double Edep; 
  int keep;  // a flag used to label this digit object is kept or not (could be removed by the electronics threshold / deadtime) 0 --> do not keep, 1 --> keep if the strip is turned on by neighborOn mode; 2 --> keep because of a signal over threshold 
} structDigitType; 

typedef std::pair<float, structDigitType> tempDigitType; // pair<float digitTime, structDigitType>  

typedef struct {
  int readLevel;
  float neighborOnTime;
} structNeighborOn;
typedef std::map<Identifier,std::pair<structNeighborOn, std::vector<tempDigitType> > > tempDigitCollectionType; // map<ReadoutElementID, pair< read or not,  all DigitObject with the identical ReadoutElementId but at different time>>; for the int(read or not) : 0 --> do not read this strip, 1 --> turned on by neighborOn mode; 2 --> this channel has signal over threshold
typedef std::map<IdentifierHash, tempDigitCollectionType> tempDigitContainerType; // use IdentifierHashId, similar structure as the <sTgcDigitCollection>

inline bool sort_EarlyToLate(tempDigitType a, tempDigitType b){
  return a.first < b.first;
}

/*******************************************************************************/
sTgcDigitizationTool::sTgcDigitizationTool(const std::string& type, const std::string& name, const IInterface* parent)
  : PileUpToolBase(type, name, parent),
    m_mergeSvc(0), 
    m_rndmEngine(0),
    m_rndmSvc("AtRndmGenSvc", name),
    m_rndmEngineName("MuonDigitization"),
    m_sgSvc("StoreGateSvc", name), 
    m_activeStore(0), 
    m_hitIdHelper(0), 
    m_digitContainer(0),
    m_idHelper(0),
    m_mdManager(0),
    m_digitizer(0),
    m_thpcsTGC(0),
    m_sdoContainer(0), 
    m_inputHitCollectionName("sTGCSensitiveDetector"),
    m_outputDigitCollectionName("sTGC_DIGITS"),
    m_outputSDO_CollectionName("sTGC_SDO"),
    m_doToFCorrection(0),
    m_doChannelTypes(3),
    m_deadtimeStrip(50.),
    m_deadtimePad(5.),
    m_timeWindowPad(30.),
    m_timeWindowStrip(30.)

    //m_file(0),
    //m_SimHitOrg(0),
    //m_SimHitMerged(0),
    //m_SimHitDigitized(0),
    //m_SimHitDigitizedwPad(0),
    //m_SimHitDigitizedwoPad(0),
    //m_kineticEnergy(0),
    //m_EnergyDeposit(0)


{
  declareInterface<IMuonDigitizationTool>(this);
  declareProperty("RndmSvc",                 m_rndmSvc,                                             "Random Number Service used in Muon digitization");
  declareProperty("RndmEngine",              m_rndmEngineName,                                      "Random engine name");
  declareProperty("MCStore",                 m_sgSvc,                                               "Simulated Data Event Store"); 
  declareProperty("InputObjectName",         m_inputHitCollectionName    = "sTGCSensitiveDetector", "name of the input object");
  declareProperty("OutputObjectName",        m_outputDigitCollectionName = "sTGC_DIGITS",           "name of the output object");
  declareProperty("OutputSDOName",           m_outputSDO_CollectionName  = "sTGC_SDO"); 
  declareProperty("doToFCorrection",         m_doToFCorrection); 
  declareProperty("doChannelTypes",          m_doChannelTypes); 
  declareProperty("DeadtimeElectronicsStrip",m_deadtimeStrip); 
  declareProperty("DeadtimeElectronicsPad",  m_deadtimePad); 
  declareProperty("timeWindowPad",           m_timeWindowPad); 
  declareProperty("timeWindowStrip",         m_timeWindowStrip); 
}
/*******************************************************************************/
// member function implementation
//--------------------------------------------
StatusCode sTgcDigitizationTool::initialize() {

  StatusCode status(StatusCode::SUCCESS);

  ATH_MSG_INFO (" sTgcDigitizationTool  retrieved");
  ATH_MSG_INFO ( "Configuration  sTgcDigitizationTool" );
  ATH_MSG_INFO ( "RndmSvc                " << m_rndmSvc             );
  ATH_MSG_INFO ( "RndmEngine             " << m_rndmEngineName      );
  ATH_MSG_INFO ( "MCStore                " << m_sgSvc               );
  ATH_MSG_INFO ( "InputObjectName        " << m_inputHitCollectionName );
  ATH_MSG_INFO ( "OutputObjectName       " << m_outputDigitCollectionName );
  ATH_MSG_INFO ( "OutputSDOName          " << m_outputSDO_CollectionName  );

  // initialize transient event store 
  if(m_sgSvc.retrieve().isFailure()) { 
    ATH_MSG_FATAL("Could not retrieve StoreGateSvc!");
    return StatusCode::FAILURE; 
  } 

  status = service("ActiveStoreSvc", m_activeStore);
  if(!status.isSuccess()) { 
    msg(status.isFailure() ? MSG::FATAL : MSG::ERROR) << "Could not get active store service" << endreq; 
    return status;
  }

  // retrieve MuonDetctorManager from DetectorStore
  status = detStore()->retrieve(m_mdManager);
  if(status.isFailure()) return status;
  ATH_MSG_DEBUG("Retrieved MuonDetectorManager from DetectorStore.");
  
  //initialize the sTgcIdHelper
  m_idHelper = m_mdManager->stgcIdHelper();
  if(!m_idHelper) {
    ATH_MSG_WARNING("sTgcIdHelper could not be retrieved.");
    return status;
  }
  
  // sTgcHitIdHelper
  m_hitIdHelper = sTgcHitIdHelper::GetHelper();

  // check the input object name
  if(m_inputHitCollectionName=="") {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Input objects: '" << m_inputHitCollectionName << "'");
  }

  // check the output object name
  if(m_outputDigitCollectionName=="") {
    ATH_MSG_FATAL("Property OutputObjectName not set !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Output digits: '" << m_outputDigitCollectionName << "'");
  }
  
  // initialize class to execute digitization 
  m_digitizer = new sTgcDigitMaker(m_hitIdHelper, m_mdManager);
  m_digitizer->setMessageLevel(static_cast<MSG::Level>(msgLevel()));
  if(!m_rndmSvc.retrieve().isSuccess()) {
    ATH_MSG_FATAL(" Could not initialize Random Number Service");
    return StatusCode::FAILURE;
  } 
    
  // getting our random numbers stream
  ATH_MSG_DEBUG("Getting random number engine : <" << m_rndmEngineName << ">");
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if(m_rndmEngine==0) {
    ATH_MSG_FATAL("Could not find RndmEngine : " << m_rndmEngineName);
    return StatusCode::FAILURE;
  }
		
  status = m_digitizer->initialize(m_rndmEngine, m_doChannelTypes);
  if(status.isFailure()) {
    ATH_MSG_FATAL("Fail to initialize sTgcDigitMaker");
    return status;
  }

  // initialize digit container
  m_digitContainer = new sTgcDigitContainer(m_idHelper->detectorElement_hash_max());
  m_digitContainer->addRef();


  // initialize digit parameters
  //m_noiseFactor = 0.09;
  m_readoutThreshold = 0.05; 
  m_neighborOnThreshold = 0.01;
  m_saturation = 1.75; // = 3500. / 2000.;
  //m_ADC = 0.00171; // = m_saturation / (TMath::Power(2, 10));
  //m_ADC = 0.02734; // = m_saturation / (TMath::Power(2, 6));
  //m_deadtimeStrip = 50.; // 50ns deadtime of electronics after peak found (for strip readout) 
  //m_deadtimePad = 5.; // 50ns deadtime of electronics after peak found (for strip readout) 
  m_timeWindowOffsetPad    = 0.;
  m_timeWindowOffsetStrip   = 25.;
  //m_timeWindowPad          = 30.; // TGC  29.32; // 29.32 ns = 26 ns +  4 * 0.83 ns
  //m_timeWindowStrip         = 30.; // TGC  40.94; // 40.94 ns = 26 ns + 18 * 0.83 ns
  m_bunchCrossingTime       = 24.95; // 24.95 ns =(40.08 MHz)^(-1)
  m_timeJitterElectronicsPad = 2.; //ns
  m_timeJitterElectronicsStrip= 2.; //ns

  //m_file = new TFile("sTGC_Digit_plot.root", "RECREATE");
  //m_SimHitOrg = new TH2F("SimHitOrg", "OrgSimHit;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitMerged = new TH2F("SimHitMerged", "SimHitMerged;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitDigitized = new TH2F("SimHitDigitized", "SimHitDigitized;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitDigitizedwPad = new TH2F("SimHitDigitizedwPad", "SimHitDigitizedwPad;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitDigitizedwoPad = new TH2F("SimHitDigitizedwoPad", "SimHitDigitizedwoPad;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_kineticEnergy = new TH1F("kineticEnergy", "kineticEnergy;kineticEnergy (GeV);", 1000, 0., 200.);
  //m_EnergyDeposit = new TH1F("EnergyDeposit", "EnergyDeposit;EnergyDeposit (MeV);", 1000, 0., 0.01);

  return status;
}
/*******************************************************************************/ 
StatusCode sTgcDigitizationTool::prepareEvent(unsigned int nInputEvents) {

  ATH_MSG_DEBUG("sTgcDigitizationTool::prepareEvent() called for " << nInputEvents << " input events" );
  m_STGCHitCollList.clear();
  //m_thpcsTGC = new TimedHitCollection<GenericMuonSimHit>();
  
  //Perform null check on m_thpcsTGC. If pointer is not null throw error
  //  if(!m_thpcsTGC) { 
  //      m_thpcsTGC = new TimedHitCollection<GenericMuonSimHit>();
  //}else{
  //	ATH_MSG_ERROR ( "m_thpcsTGC is not null" );
  //	return StatusCode::FAILURE;	
  //}

  return StatusCode::SUCCESS;
}
/*******************************************************************************/

#ifdef ATHENA_20_20
StatusCode sTgcDigitizationTool::processBunchXing(int bunchXing,
						  PileUpEventInfo::SubEvent::const_iterator bSubEvents,
						  PileUpEventInfo::SubEvent::const_iterator eSubEvents) {
#else
StatusCode sTgcDigitizationTool::processBunchXing(int bunchXing,
						  SubEventIterator bSubEvents,
						  SubEventIterator eSubEvents) {
#endif
  ATH_MSG_DEBUG ( "sTgcDigitizationTool::in processBunchXing()" );
  if(!m_thpcsTGC) {
    m_thpcsTGC = new TimedHitCollection<GenericMuonSimHit>();
  }
#ifdef ATHENA_20_20
  PileUpEventInfo::SubEvent::const_iterator iEvt = bSubEvents;
#else
  SubEventIterator iEvt = bSubEvents;
#endif
  //loop on event and sub-events for the current bunch Xing
  for (; iEvt!=eSubEvents; ++iEvt) {
#ifdef ATHENA_20_20
    StoreGateSvc& seStore = *iEvt->pSubEvtSG;
#else
    StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
#endif
    PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());
    ATH_MSG_VERBOSE( "SubEvt EventInfo from StoreGate " << seStore.name() << " :"
                     << " bunch crossing : " << bunchXing );
//                     << " time offset : " << iEvt->time()
//                     << " event number : " << iEvt->ptr()->eventNumber()
//                     << " run number : " << iEvt->ptr()->runNumber() );
    const GenericMuonSimHitCollection* seHitColl(nullptr);
    if (!seStore.retrieve(seHitColl,m_inputHitCollectionName).isSuccess()) {
      ATH_MSG_ERROR ( "SubEvent sTGC SimHitCollection not found in StoreGate " << seStore.name() );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE ( "sTGC SimHitCollection found with " << seHitColl->size() << " hits" );
    //Copy hit Collection
    GenericMuonSimHitCollection* sTGCHitColl = new GenericMuonSimHitCollection("sTGCSensitiveDetector");
    GenericMuonSimHitCollection::const_iterator i = seHitColl->begin();
    GenericMuonSimHitCollection::const_iterator e = seHitColl->end();
	   
    // Read hits from this collection
    for (; i!=e; ++i){
      sTGCHitColl->Emplace(*i);
    }
    m_thpcsTGC->insert(thisEventIndex, sTGCHitColl);
    //store these for deletion at the end of mergeEvent
    m_STGCHitCollList.push_back(sTGCHitColl);
  }

  return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::getNextEvent() {

  ATH_MSG_DEBUG ( "sTgcDigitizationTool::getNextEvent()" );
	 
  if (!m_mergeSvc) {
    const bool CREATEIF(true);
    if (!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() ||
	0 == m_mergeSvc) {
      ATH_MSG_ERROR ("Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }

  // initialize pointer
  //m_thpcsTGC = 0;
 
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<GenericMuonSimHitCollection>::type TimedHitCollList;
 
  //this is a list<info<time_t, DataLink<GenericMuonSimHitCollection> > >
  TimedHitCollList hitCollList;
	 
  if (!(m_mergeSvc->retrieveSubEvtsData(m_inputHitCollectionName, hitCollList).isSuccess()) ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  }
  if (hitCollList.size()==0) {
    ATH_MSG_ERROR ( "TimedHitCollList has size 0" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( hitCollList.size() << " sTGC SimHitCollections with key " << m_inputHitCollectionName << " found" );
  }
	 
  // create a new hits collection
  //m_thpcsTGC = new TimedHitCollection<GenericMuonSimHit>() ;
  
  //Perform null check on m_thpcsTGC. If pointer is not null throw error
  if(!m_thpcsTGC) { 
        m_thpcsTGC = new TimedHitCollection<GenericMuonSimHit>();
  }else{
 	ATH_MSG_ERROR ( "m_thpcsTGC is not null" );
	return StatusCode::FAILURE;	
  }
	 
  //now merge all collections into one
  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());
  while (iColl != endColl) {
    const GenericMuonSimHitCollection* p_collection(iColl->second);  
    m_thpcsTGC->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "sTGC SimHitCollection found with " << p_collection->size() << " hits"  );
    ++iColl;
  }
  
  return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::mergeEvent() {

  StatusCode status = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ( "sTgcDigitizationTool::in mergeEvent()" );

  // Cleanup and record the Digit container in StoreGate
  status = recordDigitAndSdoContainers();
  if(!status.isSuccess()) {
    ATH_MSG_FATAL("sTgcDigitizationTool::recordDigitAndSdoContainers failed.");
    return StatusCode::FAILURE; 
  }
    
  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization Failed" );
    //return StatusCode::FAILURE;
  }

  // reset the pointer (delete null pointer should be safe)
  delete m_thpcsTGC; 
  m_thpcsTGC = 0;

	
  std::list<GenericMuonSimHitCollection*>::iterator STGCHitColl = m_STGCHitCollList.begin();
  std::list<GenericMuonSimHitCollection*>::iterator STGCHitCollEnd = m_STGCHitCollList.end();
  while(STGCHitColl!=STGCHitCollEnd) {
    delete (*STGCHitColl);
    ++STGCHitColl;
  }
  m_STGCHitCollList.clear(); 


  return status;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::recordDigitAndSdoContainers() { 
  // cleanup digit container
  m_digitContainer->cleanup();
  
  // record the digit container in StoreGate
  m_activeStore->setStore(&*m_sgSvc);
  StatusCode status = m_sgSvc->record(m_digitContainer, m_outputDigitCollectionName);
  if(status.isFailure()) {
    ATH_MSG_FATAL("Unable to record sTGC digit container in StoreGate");
    return status;
  }
  ATH_MSG_DEBUG("sTgcDigitContainer recorded in StoreGate.");
  
  // create and record the SDO container in StoreGate
  m_sdoContainer = new MuonSimDataCollection();
  status = m_sgSvc->record(m_sdoContainer, m_outputSDO_CollectionName);
  if(status.isFailure())  {
    ATH_MSG_FATAL("Unable to record sTGC SDO collection in StoreGate");
    return status;
  } else {
    ATH_MSG_DEBUG("sTgcSDOCollection recorded in StoreGate.");
  }
  
  return status;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::digitize() {
  return this->processAllSubEvents(); 
} 
/*******************************************************************************/
StatusCode sTgcDigitizationTool::processAllSubEvents() {
  // 
  StatusCode status = StatusCode::SUCCESS;
  //m_thpcsTGC = new TimedHitCollection<GenericMuonSimHit>();
  ATH_MSG_DEBUG (" sTgcDigitizationTool::processAllSubEvents()" );
  
  status = recordDigitAndSdoContainers();
  /*if(!status.isSuccess()) {
    ATH_MSG_FATAL("sTgcDigitizationTool::recordDigitAndSdoContainers failed.");
    return StatusCode::FAILURE;
  }
  */
  //merging of the hit collection in getNextEvent method    	
  if (0 == m_thpcsTGC) {
    status = getNextEvent();
    if (StatusCode::FAILURE == status) {
      ATH_MSG_INFO ( "There are no sTGC hits in this event" );
      return status;
    }
  }
  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization() Failed" );
  }   
  
  // reset the pointer (delete null pointer should be safe)
  delete m_thpcsTGC; 
  m_thpcsTGC = 0;


  return status;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::finalize() {
  ATH_MSG_INFO("sTgcDigitizationTool::finalize() ---- m_digitContainer->digit_size() = "<<m_digitContainer->digit_size()); 
  m_digitContainer->release(); 
  delete m_digitizer; 
  m_digitizer = 0;

  //m_SimHitOrg->Write();
  //m_SimHitMerged->Write();
  //m_SimHitDigitized->Write();
  //m_SimHitDigitizedwPad->Write();
  //m_SimHitDigitizedwoPad->Write();
  //m_kineticEnergy->Write();
  //m_EnergyDeposit->Write();

  //m_file->Close();

  return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::doDigitization() {
  
  ATH_MSG_DEBUG ("sTgcDigitizationTool::doDigitization()" );
    
  TimedHitCollection<GenericMuonSimHit>::const_iterator i, e; 

  //int hash_max = m_idHelper->detectorElement_hash_max(); 

  tempDigitContainerType tempDigitContainer;

  sTgcDigitCollection* digitCollection = 0;

  ATH_MSG_DEBUG("create PRD container of size " << m_idHelper->detectorElement_hash_max());

  IdContext tgcContext = m_idHelper->module_context();

  // nextDetectorElement-->sets an iterator range with the hits of current detector element , returns a bool when done
  while(m_thpcsTGC->nextDetectorElement(i, e)) {    
    std::map< Identifier, std::pair< std::pair<double, Amg::Vector3D>, const GenericMuonSimHit*> > merged_SimHit;
    // merge hits

  
    int nhits = 0;
    while(i != e){ 
      TimedHitPtr<GenericMuonSimHit> phit = *i++;
      const GenericMuonSimHit& hit = *phit;     
      //m_SimHitOrg->Fill(hit.globalPosition().x(), hit.globalPosition().y());
      //m_kineticEnergy->Fill(hit.kineticEnergy()/1000.);

      // 50MeV cut on kineticEnergy of the particle 
      if(hit.kineticEnergy()<50.) continue;
      nhits++;
 
      sTgcSimIdToOfflineId simToOffline(*m_idHelper);
      const int idHit = hit.GenericId();
      Identifier layid = simToOffline.convert(idHit);

      std::string stationName= m_idHelper->stationNameString(m_idHelper->stationName(layid));
      int isSmall = stationName[2] == 'S';
      //int stationEta = m_idHelper->stationEta(layid);
      //int stationPhi  = m_idHelper->stationPhi(layid);
      int multiPlet = m_idHelper->multilayer(layid);
      int gasGap = m_idHelper->gasGap(layid);
 
      const MuonGM::sTgcReadoutElement* detEL = m_mdManager->getsTgcReadoutElement(layid);
      if( !detEL ){
        msg(MSG::WARNING) << "Failed to retrieve detector element for: isSmall " << isSmall << " eta " << m_idHelper->stationEta(layid) << " phi " << m_idHelper->stationPhi(layid) << " ml " << m_idHelper->multilayer(layid)  << endreq;
	continue;
      }

      // project the hit position to wire surface (along the incident angle)
      // Amg::Transform3D GTOL = detEL->absTransform().inverse();
      Amg::Vector3D HPOS(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
      // Amg::Vector3D LPOS = GTOL*HPOS;
     
      const Amg::Vector3D GLOBAL_ORIG(0., 0., 0.);
      const Amg::Vector3D GLOBAL_Z(0., 0., 1.);
      const Amg::Vector3D GLODIRE(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
    
      int surfHash_wire =  detEL->surfaceHash(gasGap, 2);
      const Trk::PlaneSurface&  SURF_WIRE = detEL->surface(surfHash_wire);

      Amg::Vector3D LOCAL_Z = SURF_WIRE.transform().inverse()*GLOBAL_Z - SURF_WIRE.transform().inverse()*GLOBAL_ORIG;
      Amg::Vector3D LOCDIRE = SURF_WIRE.transform().inverse()*GLODIRE - SURF_WIRE.transform().inverse()*GLOBAL_ORIG;
 
      Amg::Vector3D LPOS = SURF_WIRE.transform().inverse() * HPOS;

      double scale = 0.;
      if (std::abs(std::abs(LOCAL_Z.x())-1.)<1e-5 && std::abs(LOCAL_Z.y())<1e-5 && std::abs(LOCAL_Z.z())<1e-5)      scale = -LPOS.x() / LOCDIRE.x();
      else if (std::abs(LOCAL_Z.x())<1e-5 && std::abs(std::abs(LOCAL_Z.y())-1.)<1e-5 && std::abs(LOCAL_Z.z())<1e-5) scale = -LPOS.y() / LOCDIRE.y();
      else if (std::abs(LOCAL_Z.x())<1e-5 && std::abs(LOCAL_Z.y())<1e-5 && std::abs(std::abs(LOCAL_Z.z())-1.)<1e-5) scale = -LPOS.z() / LOCDIRE.z();
      else msg(MSG::ERROR) << " Wrong scale! " << endreq;

      Amg::Vector3D HITONSURFACE_WIRE = LPOS + scale * LOCDIRE;
      Amg::Vector3D G_HITONSURFACE_WIRE = SURF_WIRE.transform() * HITONSURFACE_WIRE;

      //msg(MSG::VERBOSE) << "project to wire surface : "<< "global position : " << G_HITONSURFACE_WIRE.x() <<"  "<< G_HITONSURFACE_WIRE.y() <<"  "<< G_HITONSURFACE_WIRE.z() << endreq; 
      //msg(MSG::VERBOSE) << "project to wire  surface : "<< "local position : " << HITONSURFACE_WIRE.x() <<"  "<< HITONSURFACE_WIRE.y() <<"  "<< HITONSURFACE_WIRE.z() << endreq; 


      // check the strip id this hit is pointing to
      int surfHash_strip = detEL->surfaceHash(gasGap, 1);
      const Trk::PlaneSurface&  SURF_STRIP = detEL->surface(surfHash_strip);
      Amg::Vector3D hitOnSurface_strip = SURF_STRIP.transform().inverse()*G_HITONSURFACE_WIRE;
      Amg::Vector2D POSONSURF_STRIP(hitOnSurface_strip.x(),hitOnSurface_strip.y());
  
      Identifier STRIP_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, 1, 1, true);// find the a strip id
      bool insideBounds = SURF_STRIP.insideBounds(POSONSURF_STRIP);
      if(!insideBounds) { 
        if(MSG::DEBUG) msg(MSG::DEBUG) << "Outside of the strip surface boundary : " <<  m_idHelper->print_to_string(STRIP_ID) << "; local position x = "<< POSONSURF_STRIP.x() << "  y = "<<POSONSURF_STRIP.y() <<endreq; 
        continue;
      }
      
      int stripNumber = detEL->stripNumber(POSONSURF_STRIP, STRIP_ID);
      if( stripNumber == -1 ){
        msg(MSG::ERROR) <<"Failed to obtain strip number " << m_idHelper->print_to_string(STRIP_ID) << endreq;
        msg(MSG::ERROR) <<" pos " << POSONSURF_STRIP << endreq;
        //stripNumber = 1;
      }
      bool isValid = 0;
      STRIP_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, 1, stripNumber, true, &isValid);
      if(!isValid) continue;

      std::pair<std::pair<double, Amg::Vector3D>, const GenericMuonSimHit* >& entry = merged_SimHit[STRIP_ID];
      if(entry.first.first<0) entry.first.first = 0;
      entry.first.first += hit.depositEnergy();
      entry.first.second = G_HITONSURFACE_WIRE;
      if(!entry.second) entry.second = &hit;

    } // end of while(i != e)

    // end of merge hits
 
    ATH_MSG_DEBUG("sTgcDigitizationTool::doDigitization next element");
    
    // Loop over the hits:
    typedef std::map< Identifier, std::pair< std::pair<double, Amg::Vector3D>, const GenericMuonSimHit*> >::iterator it_mergedSimHit; 

    for(it_mergedSimHit it_SimHit = merged_SimHit.begin(); it_SimHit!=merged_SimHit.end(); it_SimHit++ ) {
      double depositEnergy = it_SimHit->second.first.first; 
      if(depositEnergy<0) {
        msg(MSG::ERROR) << "Invalid depositEnergy value " << depositEnergy <<endreq;
        continue;
      }
      const GenericMuonSimHit temp_hit = *((it_SimHit->second).second);

      const GenericMuonSimHit hit(temp_hit.GenericId(), temp_hit.globalTime(), temp_hit.globalpreTime(), 
				  //temp_hit.globalPosition(),
				  it_SimHit->second.first.second, 
				  temp_hit.localPosition(),
				  temp_hit.globalPrePosition(), 
				  temp_hit.localPrePosition(),
				  temp_hit.particleEncoding(),
				  temp_hit.kineticEnergy(),
				  temp_hit.globalDirection(),
				  depositEnergy,
				  temp_hit.StepLength(),
				  temp_hit.trackNumber());

      //m_SimHitMerged->Fill(hit.globalPosition().x(), hit.globalPosition().y());
      //m_EnergyDeposit->Fill(depositEnergy);

      float globalHitTime = hit.globalTime();
      float tof = hit.globalPosition().mag()/CLHEP::c_light;
      float bunchTime = globalHitTime - tof;

      sTgcDigitCollection* digiHits = 0;
      digiHits = m_digitizer->executeDigi(&hit, globalHitTime);
      if(!digiHits) continue;

      //m_SimHitDigitized->Fill(hit.globalPosition().x(), hit.globalPosition().y());

      sTgcDigitCollection::const_iterator it_digiHits;
     // bool hasPad = false;
      for(it_digiHits=digiHits->begin(); it_digiHits!=digiHits->end(); ++it_digiHits) {

	/**
	   NOTE: 
	   ----- 
	   Since not every hit might end up resulting in a
	   digit, this construction might take place after the hit loop 
	   in a loop of its own! 
	*/
	
	// make new sTgcDigit
	Identifier newDigitId = (*it_digiHits)->identify();
	float newTime    = (*it_digiHits)->time();
        int newChannelType   = m_idHelper->channelType((*it_digiHits)->identify());
       // if(newChannelType==0) hasPad = true;

	//uint16_t newBcTag    = (*it_digiHits)->bcTag();
        float timeJitterElectronicsStrip = CLHEP::RandGauss::shoot(m_rndmEngine, 0, m_timeJitterElectronicsStrip);
        float timeJitterElectronicsPad = CLHEP::RandGauss::shoot(m_rndmEngine, 0, m_timeJitterElectronicsPad);
        if(newChannelType==1) newTime += timeJitterElectronicsStrip;
        else newTime += timeJitterElectronicsPad;
	uint16_t newBcTag    = bcTagging(newTime+bunchTime, newChannelType);

        if(m_doToFCorrection) newTime += bunchTime;
        else newTime += globalHitTime;

        float newCharge = -1.; 
        if(newChannelType==1) newCharge      = (*it_digiHits)->charge();
	Identifier elemId    = m_idHelper->elementID(newDigitId);
	
        if(newChannelType!=0 && newChannelType!=1 && newChannelType!=2) {
	  ATH_MSG_WARNING( "Wrong channelType " << newChannelType );
        }

	IdentifierHash coll_hash;
	
        m_idHelper->get_detectorElement_hash(elemId, coll_hash); 
      
        std::vector<tempDigitType>& newTempDigit = tempDigitContainer[coll_hash][newDigitId].second;
        tempDigitContainer[coll_hash][newDigitId].first.readLevel = 0;
        tempDigitContainer[coll_hash][newDigitId].first.neighborOnTime = -1.;

        MuonSimData::Deposit deposit(hit.particleLink(), MuonMCData(hit.globalPosition().x(), hit.globalPosition().y()));
        structDigitType newStructDigit = {newBcTag, newCharge, newChannelType, deposit, 1e6 * hit.depositEnergy(), 0}; 
        newTempDigit.push_back(std::make_pair(newTime, newStructDigit));

      } // end of loop digiHits
 
      delete digiHits;
      digiHits = 0;
      //if(hasPad) m_SimHitDigitizedwPad->Fill(hit.globalPosition().x(), hit.globalPosition().y());
      //else m_SimHitDigitizedwoPad->Fill(hit.globalPosition().x(), hit.globalPosition().y()); 

    }// end of loop(merged_SimHit) 
    merged_SimHit.clear();   
  }//while(m_thpcsTGC->nextDetectorElement(i, e))

  //after looping all the hit in a single event, add noise factor,simulation VMM "neighborOn" mode and cut at a threshold for each readout element, remove the readout elements do not pass the threshold
  m_activeStore->setStore(&*m_sgSvc);
  //int count_coll = 0;

  for(tempDigitContainerType::iterator it_coll = tempDigitContainer.begin(); it_coll != tempDigitContainer.end(); ++it_coll){

    for(tempDigitCollectionType::iterator it_REID = it_coll->second.begin(); it_REID != it_coll->second.end(); ++it_REID){

      Identifier newDigitId = it_REID->first;
      //msg(MSG::VERBOSE) << "   Digit Id= " << m_idHelper->show_to_string(newDigitId) << endreq;

      sort(it_REID->second.second.begin(), it_REID->second.second.end(), sort_EarlyToLate);

      float deadtimeBlockStrip = -9999.;
      float deadtimeBlockPad = -9999.;

      // apply electronics deadtime
      for(std::vector<tempDigitType>::iterator it_digit = it_REID->second.second.begin(); it_digit != it_REID->second.second.end(); ++it_digit){

        if(it_digit->second.channelType!=1) {
          if( (it_digit->first - deadtimeBlockPad)> m_deadtimePad ) {
            if(it_REID->second.first.readLevel<2) it_REID->second.first.readLevel = 2;
	    it_digit->second.keep = 2;
	    deadtimeBlockPad = it_digit->first;
          }
        } // end of pad/wire
        else if((it_digit->first - deadtimeBlockStrip)> m_deadtimeStrip || it_REID->second.first.readLevel<2){ 

          if( it_digit->second.charge>m_readoutThreshold ) {
	    it_digit->second.keep = 2; 
	    deadtimeBlockStrip = it_digit->first;
            
            if(it_REID->second.first.readLevel<2) it_REID->second.first.readLevel = 2;
            // "neighborOn" mode of VMM
            Identifier parentId = m_idHelper->parentID(newDigitId);
            int multiPlet = m_idHelper->multilayer(newDigitId);
            int gasGap = m_idHelper->gasGap(newDigitId);
            int channelType = it_digit->second.channelType;
            int stripNumber = m_idHelper->channel(newDigitId);
            bool isValid = false;
            Identifier neighborId = m_idHelper->channelID(parentId, multiPlet, gasGap, channelType, stripNumber+1, true, &isValid);
            if(isValid){
              tempDigitCollectionType::iterator it_neighborREID = it_coll->second.find(neighborId);
              if(it_neighborREID != it_coll->second.end()) { if(it_neighborREID->second.first.readLevel<1) {it_neighborREID->second.first.readLevel = 1; it_neighborREID->second.first.neighborOnTime = it_digit->first;}} 
            }
            isValid = false;
            neighborId = m_idHelper->channelID(parentId, multiPlet, gasGap, channelType, stripNumber-1, true, &isValid);
            if(isValid){
              tempDigitCollectionType::iterator it_neighborREID = it_coll->second.find(neighborId);
              if(it_neighborREID != it_coll->second.end()) { if(it_neighborREID->second.first.readLevel<1) {it_neighborREID->second.first.readLevel = 1;it_neighborREID->second.first.neighborOnTime = it_digit->first;}} 
            }
          } // end if not in the dead time block 
          else if( it_digit->second.charge>m_neighborOnThreshold ){
            it_digit->second.keep = 1;  
            deadtimeBlockStrip = it_digit->first;
          }
        } // end of strip

        //msg(MSG::VERBOSE) << "              "
	//		  << " digittime = "       << it_digit->first 
	//		  << " BC tag = "          << it_digit->second.bcTag 
	//		  << " charge = "          << it_digit->second.charge 
	//		  << " channelType = "     << it_digit->second.channelType
	//		  << " keep this digit = " << it_digit->second.keep
	//		  << endreq;
        // 
      } // end of loop all signals(at different time) of the same ReadoutElementID

    } // end of loop for all the ReadoutElementID

  } // end of loop tempDigitContainer(hash_max)


  for(tempDigitContainerType::iterator it_coll = tempDigitContainer.begin(); it_coll != tempDigitContainer.end(); ++it_coll){
    
    IdentifierHash coll = it_coll->first;
    msg(MSG::VERBOSE) << "coll = "<< coll << endreq;  
    digitCollection = new sTgcDigitCollection(it_coll->second.begin()->first, coll);

    for(tempDigitCollectionType::iterator it_REID = it_coll->second.begin(); it_REID != it_coll->second.end(); ++it_REID){
      int readLevel = it_REID->second.first.readLevel;
      float neighborOnTime = it_REID->second.first.neighborOnTime;
      if(!readLevel) continue;
      
      Identifier newDigitId = it_REID->first;

      std::vector<MuonSimData::Deposit> deposits;

      for(std::vector<tempDigitType>::iterator it_digit = it_REID->second.second.begin(); it_digit != it_REID->second.second.end(); ++it_digit){

        int kept = it_digit->second.keep;
        if(!kept) continue; 
        else if(readLevel==2 && kept<2) continue;
        else if(readLevel==1 && kept==1) {
          if(std::abs(it_digit->first - neighborOnTime)>m_timeJitterElectronicsStrip*3.) continue; // 3 sigma ~ 99.7%
        }

        float newDigitTime = it_digit->first;
        uint16_t newBcTag = it_digit->second.bcTag;
        float newCharge  = it_digit->second.charge;
        int newChannelType = it_digit->second.channelType;
        MuonSimData::Deposit newDep = it_digit->second.Dep;

        if(newChannelType!=1) digitCollection->push_back(new sTgcDigit(newDigitId, newBcTag, newDigitTime, -1));
        else {
          if(newCharge>m_saturation) newCharge = m_saturation; 
          //newCharge = (float)((int)(newCharge/m_ADC));
          digitCollection->push_back(new sTgcDigit(newDigitId, newBcTag, newDigitTime, newCharge)); 
        }

        deposits.push_back(newDep);
      } // end of loop for all the digit object of the same ReadoutElementID

      //Record the SDO collection in StoreGate
      m_sdoContainer->insert ( std::make_pair ( newDigitId, MuonSimData(deposits, it_REID->second.second.begin()->second.Edep) ) );
      deposits.clear();

    } // end of loop for all the ReadoutElementID

    if(digitCollection->size()){
      ATH_MSG_VERBOSE("push the collection to m_digitcontainer : HashId = " << digitCollection->identifierHash() );
      if(m_digitContainer->addCollection(digitCollection, digitCollection->identifierHash()).isFailure()){
        ATH_MSG_WARNING("Failed to add collection with hash " << digitCollection->identifierHash());        
      }
      for(sTgcDigitCollection::iterator it_stgcDigit_final = digitCollection->begin(); it_stgcDigit_final != digitCollection->end(); ++it_stgcDigit_final) {
  
        ATH_MSG_VERBOSE("push to StoreGate : Digit Id= " << m_idHelper->show_to_string((*it_stgcDigit_final)->identify())
			<< " BC tag = "    << (*it_stgcDigit_final)->bcTag() 
			<< " digitTime = " << (*it_stgcDigit_final)->time() 
			<< " charge_6bit = "    << (*it_stgcDigit_final)->charge_6bit()
			<< " charge_10bit = "    << (*it_stgcDigit_final)->charge_10bit()
			<< " channelType = " << m_idHelper->channelType((*it_stgcDigit_final)->identify())) ; 
      }
    }
    else {
      delete digitCollection;
      digitCollection = 0;
    }

  } // end of loop tempDigitContainer(hash_max)

  return StatusCode::SUCCESS;
}

/*******************************************************************************/
uint16_t sTgcDigitizationTool::bcTagging(const float digitTime, const int channelType) const {

  uint16_t bctag = 0;

  double offset, window;
  if(channelType == 1) { //strips 
    offset = m_timeWindowOffsetStrip;
    window = m_timeWindowStrip;
  }
  else { // wire gangs or pads
    offset = m_timeWindowOffsetPad;
    window = m_timeWindowPad;
  }

  if(-m_bunchCrossingTime+offset < digitTime && digitTime < -m_bunchCrossingTime+offset+window) {
    bctag = (bctag | 0x1);
  }
  if(                     offset < digitTime && digitTime <                      offset+window) {
    bctag = (bctag | 0x2);
  }
  if(+m_bunchCrossingTime+offset < digitTime && digitTime < +m_bunchCrossingTime+offset+window) {
    bctag = (bctag | 0x4);
  }

  return bctag;
}

