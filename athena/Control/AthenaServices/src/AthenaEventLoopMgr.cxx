/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define  GAUDISVC_EVENTLOOPMGR_CPP

#include <cassert>
#include <ios>
#include <iostream>
#include <fstream> /* ofstream */
#include <iomanip>

#include "AthenaKernel/ITimeKeeper.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"

#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataSelectionAgent.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/Algorithm.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"

#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "ClearStorePolicy.h"

#include "AthenaEventLoopMgr.h"
#include "PersistentDataModel/AthenaAttributeList.h"

#include "CxxUtils/make_unique.h"

//=========================================================================
// Standard Constructor
//=========================================================================
AthenaEventLoopMgr::AthenaEventLoopMgr(const std::string& nam, 
				       ISvcLocator* svcLoc)
  : MinimalEventLoopMgr(nam, svcLoc), 
    m_incidentSvc ( "IncidentSvc",  nam ), 
    m_eventStore( "StoreGateSvc", nam ), 
    m_evtSelector(0), m_evtContext(0),
    m_histoDataMgrSvc( "HistogramDataSvc",         nam ), 
    m_histoPersSvc   ( "HistogramPersistencySvc",  nam ), 
    m_activeStoreSvc ( "ActiveStoreSvc",           nam ),
    m_pITK(0), 
    m_currentRun(0), m_firstRun(true), m_tools(this), m_nevt(0), m_writeHists(false),
    m_msg( msgSvc(), nam ),
    m_nev(0), m_proc(0), m_useTools(false), 
    m_eventContext(nullptr),
    m_chronoStatSvc( "ChronoStatSvc", nam )

{
  declareProperty("EvtStore", m_eventStore, "The StoreGateSvc instance to interact with for event payload" );
  declareProperty("EvtSel", m_evtsel, 
		  "Name of Event Selector to use. If empty string (default) "
		  "take value from ApplicationMgr");
  declareProperty("HistogramPersistency", m_histPersName="",
		  "Histogram persistency technology to use: ROOT, HBOOK, NONE. "
		  "By default (empty string) get property value from "
		  "ApplicationMgr");
  declareProperty("TimeKeeper", m_timeKeeperName, 
		  "Name of TimeKeeper to use. NONE or empty string (default) "
		  "means no time limit control on event loop");
  m_timeKeeperName.declareUpdateHandler(&AthenaEventLoopMgr::setupTimeKeeper, this);
  declareProperty("HistWriteInterval",    m_writeInterval=0 ,
		  "histogram write/update interval");
  declareProperty("FailureMode",          m_failureMode=1 , 
		  "Controls behaviour of event loop depending on return code of"
		  " Algorithms. 0: all non-SUCCESSes terminate job. "
		  "1: RECOVERABLE skips to next event, FAILURE terminates job "
		  "(DEFAULT). 2: RECOVERABLE and FAILURE skip to next events");
  declareProperty("EventPrintoutInterval", m_eventPrintoutInterval=1,
                  "Print event heartbeat printouts every m_eventPrintoutInterval events");
  declareProperty("UseDetailChronoStat",m_doChrono=false);
  declareProperty("ClearStorePolicy",
		  m_clearStorePolicy = "EndEvent",
		  "Configure the policy wrt handling of when the "
		  "'clear-the-event-store' event shall happen: at EndEvent "
		  "(default as it is makes things easier for memory management"
		  ") or at BeginEvent (easier e.g. for interactive use)");
  declareProperty("PreSelectTools",m_tools,"AlgTools for event pre-selection")->
declareUpdateHandler( &AthenaEventLoopMgr::setupPreSelectTools, this ); ;
  

}

//=========================================================================
// Standard Destructor
//=========================================================================
AthenaEventLoopMgr::~AthenaEventLoopMgr()   
{
}

//=========================================================================
// implementation of IAppMgrUI::initalize
//=========================================================================
StatusCode AthenaEventLoopMgr::initialize()    
{

  // configure our MsgStream
  m_msg.setLevel( m_outputLevel.value() );

  m_msg << MSG::INFO << "Initializing " << name()
	<< " - package version " << PACKAGE_VERSION << endreq ;
 

  StatusCode sc = MinimalEventLoopMgr::initialize();
  if ( !sc.isSuccess() ) 
  {
    m_msg << MSG::ERROR
	<< "Failed to initialize base class MinimalEventLoopMgr"
	<< endreq;
    return sc;
  } else {
    // re-configure our MsgStream: 
    // yes... do it twice b/c this is how we do it...
    m_msg.setLevel( m_outputLevel.value() );
  }

//-------------------------------------------------------------------------
// Setup access to event data services
//-------------------------------------------------------------------------

  sc = m_eventStore.retrieve();
  if( !sc.isSuccess() )  
  {
    m_msg << MSG::FATAL 
	  << "Error retrieving pointer to StoreGateSvc"
	  << endreq;
    return sc;
  }

//--------------------------------------------------------------------------
// Get the references to the services that are needed by the ApplicationMgr 
// itself
//--------------------------------------------------------------------------
  sc = m_incidentSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    m_msg << MSG::FATAL << "Error retrieving IncidentSvc." << endreq;
    return sc;
  }

//--------------------------------------------------------------------------
// Access Property Manager interface:
//--------------------------------------------------------------------------
  SmartIF<IProperty> prpMgr(serviceLocator());
  if ( !prpMgr.isValid() ) 
  {
    m_msg << MSG::FATAL 
	  << "IProperty interface not found in ApplicationMgr." 
	  << endreq;
    return StatusCode::FAILURE;
  }


//--------------------------------------------------------------------------
// Set up the Histogram Service
//--------------------------------------------------------------------------
  sc = m_histoDataMgrSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    m_msg << MSG::FATAL 
	  << "Error retrieving HistogramDataSvc" 
	  << endreq;
    return sc;
  }
    
  const std::string& histPersName(m_histPersName.value());
  if ( histPersName.length() == 0 )    
  {
    setProperty(prpMgr->getProperty("HistogramPersistency")).ignore();
  }

  if ( histPersName != "NONE" )   {

    m_histoPersSvc = IConversionSvc_t( "HistogramPersistencySvc", 
				       this->name() );

    if( !sc.isSuccess() )  {
      m_msg << MSG::WARNING 
	    << "Histograms cannot not be saved - though required." 
	    << endreq;
    } else {

      IService *is = 0;
      if (histPersName == "ROOT") {
	sc = serviceLocator()->service("RootHistSvc", is);
      } else if ( histPersName == "HBOOK" ) {
	sc = serviceLocator()->service("HbookHistSvc", is);
      }

      if (sc.isFailure()) {
	m_msg << MSG::ERROR 
	      << "could not locate actual Histogram persistency service"
	      << endreq;
      } else {
	Service *s = dynamic_cast<Service*>(is);
	if (s == 0) {
	  m_msg << MSG::ERROR << "Could not dcast HistPersSvc to a Service"
		<< endreq;
	} else {
	  const Property &prop = s->getProperty("OutputFile");
	  std::string val;
	  try {
	    const StringProperty &sprop = dynamic_cast<const StringProperty&>( prop );

	    val = sprop.value();

	  } catch (...) {
	    m_msg << MSG::VERBOSE
		  << "could not dcast OutputFile property to a StringProperty."
		  << " Need to fix Gaudi."
		  << endreq;

	    val = prop.toString();

	    //	    val.erase(0,val.find(":")+1);
	    //	    val.erase(0,val.find("\"")+1);
	    //	    val.erase(val.find("\""),val.length());
	  }

	  if (val != "" && 
	      val != "UndefinedROOTOutputFileName" && 
	      val != "UndefinedHbookOutputFileName" ) {
	    m_writeHists = true;
	  }

	}
      }
    }
    

  }  else { if (m_msg.level() <= MSG::DEBUG) { m_msg << MSG::DEBUG 
						     << "Histograms saving not required." 
						     << endreq; } }
//-------------------------------------------------------------------------
// Setup EventSelector service
//-------------------------------------------------------------------------
  const std::string& selName(m_evtsel.value());
  // the evt sel is usually specified as a property of ApplicationMgr
  if (selName.empty()) 
    sc = setProperty(prpMgr->getProperty("EvtSel"));
  if (sc.isFailure()) m_msg << MSG::WARNING << "Unable to set EvtSel property" << endreq;

  // We do not expect a Event Selector necessarily being declared
  if( !selName.empty() && selName != "NONE") {
    IEvtSelector* theEvtSel(0);
    StatusCode sc(serviceLocator()->service( selName, theEvtSel ));
    if( sc.isSuccess() && ( theEvtSel != m_evtSelector ) ) {
      // Event Selector changed (or setup for the first time)
      m_evtSelector = theEvtSel;
      
      // reset iterator
      if (m_evtSelector->createContext(m_evtContext).isFailure()) {
	m_msg  << MSG::FATAL << "Can not create the event selector Context." 
	       << endreq;
	return StatusCode::FAILURE;
      }
      if (m_msg.level() <= MSG::INFO) {
	INamedInterface* named (dynamic_cast< INamedInterface* >(theEvtSel));
	if (0 != named) {
	  m_msg << MSG::INFO 
		<< "Setup EventSelector service " << named->name( ) 
		<< endreq;
	}
      }
    } else if (sc.isFailure()) {
      m_msg  << MSG::FATAL << "No valid event selector called " << selName 
	     << endreq;
      return StatusCode::FAILURE;
    }
  }  
//-------------------------------------------------------------------------
// Setup TimeKeeper service
//-------------------------------------------------------------------------
  // the time keeper may one day be specified as a property of ApplicationMgr
  //  setProperty(prpMgr->getProperty("TimeKeeper"));

//-------------------------------------------------------------------------
// Setup 'Clear-Store' policy
//-------------------------------------------------------------------------
  try {
    setClearStorePolicy( m_clearStorePolicy );
  } catch(...) {
    return StatusCode::FAILURE;
  }
//-------------------------------------------------------------------------
// Make sure the ActiveStoreSvc is initialized.
// We don't use this, but want to be sure that it gets created
// during initialization, to avoid heap fragmentation.
//-------------------------------------------------------------------------
  sc = m_activeStoreSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    m_msg << MSG::FATAL << "Error retrieving ActiveStoreSvc." << endreq;
    return sc;
  }

  // create the EventContext object
  m_eventContext = new EventContext();


  // Listen to the BeforeFork incident
  m_incidentSvc->addListener(this,"BeforeFork",0);

  return sc;
}

//=========================================================================
// property handlers
//=========================================================================
void 
AthenaEventLoopMgr::setupTimeKeeper(Property&) {
  const std::string& tkName(m_timeKeeperName.value());
  // We do not expect a TimeKeeper necessarily being declared  
  if( tkName != "NONE" && tkName.length() != 0) {
    if (!(serviceLocator()->service( tkName, m_pITK, true)).isSuccess()) 
      m_msg << MSG::ERROR << "TimeKeeper not found." << endreq;
    else m_msg << MSG::INFO 
	       << "No TimeKeeper selected. "
	       << "No time limit control on event loop." 
	       << endreq;
  }
}

void 
AthenaEventLoopMgr::setClearStorePolicy(Property&) {
  const std::string& policyName = m_clearStorePolicy.value();

  if ( policyName != "BeginEvent" &&
       policyName != "EndEvent" ) {

    m_msg  << MSG::FATAL 
	   << "Unknown policy [" << policyName 
	   << "] for the 'ClearStore-policy !"
	   << endreq
	   << "Valid values are: BeginEvent, EndEvent"
	   << endreq;
    throw GaudiException("Can not setup 'ClearStore'-policy",
			 name(),
			 StatusCode::FAILURE);
  }

  return;
}

void
AthenaEventLoopMgr::setupPreSelectTools(Property&) {

  m_toolInvoke.clear();
  m_toolReject.clear();
  m_toolAccept.clear();

  m_tools.retrieve().ignore();
  if(m_tools.size() > 0) {
    m_useTools=true;
    m_toolInvoke.resize(m_tools.size());
    m_toolReject.resize(m_tools.size());
    m_toolAccept.resize(m_tools.size());

    tool_iterator firstTool = m_tools.begin();
    tool_iterator lastTool  = m_tools.end();
    unsigned int toolCtr = 0;
    for ( ; firstTool != lastTool; firstTool++ )
      {
	// reset statistics
	m_toolInvoke[toolCtr] = 0;
	m_toolReject[toolCtr] = 0;
	m_toolAccept[toolCtr] = 0;
        toolCtr++;
      }
  }

  return;

}

//=========================================================================
// implementation of IAppMgrUI::finalize
//=========================================================================
StatusCode AthenaEventLoopMgr::finalize()    
{

  StatusCode sc = MinimalEventLoopMgr::finalize();
  if (sc.isFailure()) 
  {
    m_msg << MSG::ERROR 
	  << "Error in Algorithm Finalize" 
	  << endreq;
  }

  StatusCode sc2 = writeHistograms(true);
  if (sc2.isFailure()) 
  {
    m_msg << MSG::ERROR 
	  << "Error in writing Histograms"
	  << endreq;
  }

  // Release all interfaces (ignore StatusCodes)
  m_histoDataMgrSvc.release().ignore();
  m_histoPersSvc.release().ignore();
  m_activeStoreSvc.release().ignore();

  m_evtSelector   = releaseInterface(m_evtSelector);
  m_incidentSvc.release().ignore();

  delete m_evtContext; m_evtContext = 0;

  delete m_eventContext; m_eventContext = 0;

  if(m_useTools) {
    tool_iterator firstTool = m_tools.begin();
    tool_iterator lastTool  = m_tools.end();
    unsigned int toolCtr = 0;
    m_msg << MSG::INFO
	  << "Summary of AthenaEvtLoopPreSelectTool invocation: (invoked/success/failure)" << endreq;
    m_msg << MSG::INFO
	   << "-----------------------------------------------------" << endreq;

    for ( ; firstTool != lastTool; firstTool++ ) {
      m_msg  << MSG::INFO
             << std::setw(2)     << std::setiosflags(std::ios_base::right)
             << toolCtr+1 << ".) " << std::resetiosflags(std::ios_base::right)
             << std::setw(48) << std::setfill('.')
             << std::setiosflags(std::ios_base::left)
             << (*firstTool)->name() << std::resetiosflags(std::ios_base::left)
             << std::setfill(' ')
             << " ("
             << std::setw(6) << std::setiosflags(std::ios_base::right)
             << m_toolInvoke[toolCtr]
             << "/"
             << m_toolAccept[toolCtr]
             << "/"
             << m_toolReject[toolCtr]
             << ")"
             << endreq;
      toolCtr++;
    }
  }
  return ( sc.isFailure() || sc2.isFailure() ) ? StatusCode::FAILURE :
    StatusCode::SUCCESS;

}

//=========================================================================
// write out the histograms
//=========================================================================
StatusCode AthenaEventLoopMgr::writeHistograms(bool force) {


  StatusCode sc (StatusCode::SUCCESS);
  
  if ( 0 != m_histoPersSvc && m_writeHists ) {
    DataSelectionAgent agent;
    StatusCode iret = m_histoDataMgrSvc->traverseTree( &agent );
    if( iret.isFailure() ) {
      sc = iret;
      m_msg << MSG::ERROR 
	    << "Error while traversing Histogram data store" 
	    << endreq;
    }
    
    IDataSelector* objects = agent.selectedObjects();
    // skip /stat entry!
    if ( objects->size() > 0 ) {
      int writeInterval(m_writeInterval.value());
      IDataSelector::iterator i;
      for ( i = objects->begin(); i != objects->end(); i++ ) {
	StatusCode iret(StatusCode::SUCCESS);
	
	if ( m_nevt == 1 || force || 
	     (writeInterval != 0 && m_nevt%writeInterval == 0) ) {
	  
	  bool crt(false);
	  
	  IOpaqueAddress* pAddr = (*i)->registry()->address();
	  if (pAddr == 0) {
	    iret = m_histoPersSvc->createRep(*i, pAddr);
	    if ( iret.isSuccess() ) {
	      (*i)->registry()->setAddress(pAddr);
	      crt = true;
	    } else {
	      m_msg << MSG::ERROR << "calling createRep for " 
		    << (*i)->registry()->identifier() << endreq;
	    }	       
	  }
	  
	  if (iret.isSuccess()) {
	    assert(pAddr != 0);
	    iret = m_histoPersSvc->updateRep(pAddr, *i);
	    
	    if (iret.isSuccess() && crt == true) {
	      iret = m_histoPersSvc->fillRepRefs(pAddr,*i);
	    }
	  }
	  
	}
	
	if ( iret.isFailure() ) {
	  sc = iret;
	}
	
      }    // end of loop over Objects
      
      if (force || (writeInterval != 0 && m_nevt%writeInterval == 0) ) {
	if (m_msg.level() <= MSG::DEBUG) { m_msg << MSG::DEBUG << "committing Histograms" << endreq; }
	m_histoPersSvc->conversionSvc()->commitOutput("",true).ignore();
      }
      
    }       // end of objects->size() > 0
    
  }
  
  return sc;
}

//=========================================================================
// Run the algorithms beginRun hook
//=========================================================================
StatusCode AthenaEventLoopMgr::beginRunAlgorithms(const EventInfo& event) {

  // Fire BeginRun "Incident"
  m_incidentSvc->fireIncident(EventIncident(event, name(),"BeginRun"));

  // Call the execute() method of all top algorithms 
  for ( ListAlg::iterator ita = m_topAlgList.begin(); 
        ita != m_topAlgList.end();
        ita++ ) 
  {
    const StatusCode& sc = (*ita)->sysBeginRun(); 
    if ( !sc.isSuccess() ) {
      m_msg << MSG::INFO  << "beginRun of algorithm "
	    << (*ita)->name() << " failed with StatusCode::" << sc
	    << endreq;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Run the algorithms endRun hook
//=========================================================================
StatusCode AthenaEventLoopMgr::endRunAlgorithms() {

  // Fire EndRun Incident
  m_incidentSvc->fireIncident(Incident(name(),"EndRun"));

  // Call the execute() method of all top algorithms 
  for ( ListAlg::iterator ita = m_topAlgList.begin(); 
        ita != m_topAlgList.end();
        ita++ ) 
  {
    const StatusCode& sc = (*ita)->sysEndRun();
    if ( !sc.isSuccess() ) {
      m_msg << MSG::INFO  << "endRun of algorithm "
	    << (*ita)->name() << " failed with StatusCode::" << sc
	    << endreq;
      return sc;
    }  
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Call sysInitialize() on all algorithms and output streams
//=========================================================================
StatusCode AthenaEventLoopMgr::initializeAlgorithms() {

  // Initialize the list of Algorithms. Note that existing Algorithms
  // are protected against multiple initialization attempts.
  ListAlg::iterator ita;
  for ( ita = m_topAlgList.begin(); ita != m_topAlgList.end(); ++ita )
    {
      StatusCode sc = (*ita)->sysInitialize();
      if( sc.isFailure() )
	{
	  m_msg << MSG::ERROR
		<< "Unable to initialize Algorithm: "
		<< (*ita)->name()
		<< endreq;
	  return sc;
	}

      Algorithm* alg = dynamic_cast<Algorithm*>( (IAlgorithm*)(*ita) );
      if (alg != 0) {
        alg->setContext( m_eventContext );
      } else {
        m_msg << MSG::ERROR
              << "Unable to dcast IAlgorithm " << (*ita)->name() 
              << " to Algorithm"
              << endreq;
        return StatusCode::FAILURE;
      }
    }

  // Initialize the list of Output Streams. Note that existing Output Streams
  // are protected against multiple initialization attempts.
  for (ita = m_outStreamList.begin(); ita != m_outStreamList.end(); ++ita )
    {
      StatusCode sc = (*ita)->sysInitialize();
      if( sc.isFailure() ) {
	m_msg << MSG::ERROR
	      << "Unable to initialize Output Stream: "
	      << (*ita)->name()
	      << endreq;
	return sc;
      }
    }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Run the algorithms for the current event
//=========================================================================
StatusCode AthenaEventLoopMgr::executeAlgorithms() {

  // Call the execute() method of all top algorithms 
  for ( ListAlg::iterator ita = m_topAlgList.begin(); 
        ita != m_topAlgList.end();
        ita++ ) 
  {
    (*ita)->resetExecuted(); 
    const StatusCode& sc = (*ita)->sysExecute(); 
    if ( !sc.isSuccess() ) {
      m_msg << MSG::INFO  << "Execution of algorithm "
	    << (*ita)->name() << " failed with StatusCode::" << sc
	    << endreq;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}


//=========================================================================
// executeEvent(void* par)
//=========================================================================
StatusCode AthenaEventLoopMgr::executeEvent(void* /*par*/)    
{
  const EventInfo* pEvent(0);
  std::unique_ptr<EventInfo> pEventPtr;
  if ( m_evtContext )
  { // Deal with the case when an EventSelector is provided
    // Retrieve the Event object
    const AthenaAttributeList* pAttrList = eventStore()->tryConstRetrieve<AthenaAttributeList>();
    if ( pAttrList != 0) { // Try making EventID-only EventInfo object from in-file TAG
      try {
        unsigned int runNumber = (*pAttrList)["RunNumber"].data<unsigned int>();
        unsigned long long eventNumber = (*pAttrList)["EventNumber"].data<unsigned long long>();
        unsigned int eventTime = (*pAttrList)["EventTime"].data<unsigned int>();
        unsigned int eventTimeNS = (*pAttrList)["EventTimeNanoSec"].data<unsigned int>();
        unsigned int lumiBlock = (*pAttrList)["LumiBlockN"].data<unsigned int>();
        unsigned int bunchId = (*pAttrList)["BunchId"].data<unsigned int>();
        pEventPtr = CxxUtils::make_unique<EventInfo>
          (new EventID(runNumber, eventNumber, eventTime, eventTimeNS, lumiBlock, bunchId), (EventType*)0);
        pEvent = pEventPtr.get();
        
      } catch (...) {
      }
/* FIXME: PvG, not currently written
      if ( pEvent != 0 ) { // Try adding EventType information
        try {
          float eventWeight = (*pAttrList)["EventWeight"].data<float>();
          const EventType* pType = new EventType();
          pEvent->setEventType(pType);
          pEvent->event_type()->set_mc_event_weight(eventWeight);
        } catch (...) {
          pEvent->setEventType(0);
        }
      }
*/
    }
    if ( pEvent == 0 ) {
      StatusCode sc = eventStore()->retrieve(pEvent);
      if( !sc.isSuccess() ) {
        m_msg << MSG::ERROR 
	      << "Unable to retrieve Event root object" << endreq;
        return (StatusCode::FAILURE);
      }
    }
  }
  else 
  {
    // With no iterator it's up to us to create an EventInfo
    pEventPtr = CxxUtils::make_unique<EventInfo>
      (new EventID(1,m_nevt), new EventType());
    pEvent = pEventPtr.get();
    pEventPtr->event_ID()->set_lumi_block( m_nevt );
    StatusCode sc = eventStore()->record(std::move(pEventPtr),"");
    if( !sc.isSuccess() )  {
      m_msg << MSG::ERROR 
	    << "Error declaring event data object" << endreq;
      return (StatusCode::FAILURE);
    } 
  }
  assert(pEvent);

  // m_eventContext->setEventID( EventIDBase(pEvent->event_ID()->run_number(), 
  //                                         pEvent->event_ID()->event_number(),
  //                                         pEvent->event_ID()->time_stamp(),
  //                                         pEvent->event_ID()->time_stamp_ns_offset(),
  //                                         pEvent->event_ID()->lumi_block(),
  //                                         pEvent->event_ID()->bunch_crossing_id()) );

  m_eventContext->setEventID( *((EventIDBase*) pEvent->event_ID()) );
  m_eventContext->setEvt (m_nev);

  m_eventContext->setProxy( eventStore()->hiveProxyDict() );
  Gaudi::Hive::setCurrentContext( m_eventContext );


  /// Fire begin-Run incident if new run:
  if (m_firstRun || (m_currentRun != pEvent->event_ID()->run_number()) ) {
    // Fire EndRun incident unless this is the first run
    if (!m_firstRun) {
      if (!(this->endRunAlgorithms()).isSuccess()) return (StatusCode::FAILURE);
    }
    m_firstRun=false;
    m_currentRun = pEvent->event_ID()->run_number();

    m_msg << MSG::INFO
	  << "  ===>>>  start of run " << m_currentRun << "    <<<==="
	  << endreq;
 
    if (!(this->beginRunAlgorithms(*pEvent)).isSuccess()) return (StatusCode::FAILURE);
  }

  bool toolsPassed=true;
  bool eventFailed = false;
  // Call any attached tools to reject events early
  unsigned int toolCtr=0;
  if(m_useTools) {
    tool_iterator theTool = m_tools.begin();
    tool_iterator lastTool  = m_tools.end();
    while(toolsPassed && theTool!=lastTool ) 
      {
        toolsPassed = (*theTool)->passEvent(pEvent);
	m_toolInvoke[toolCtr]++;
        {toolsPassed ? m_toolAccept[toolCtr]++ : m_toolReject[toolCtr]++;}
        toolCtr++;
        theTool++;
      }
  }


  uint64_t evtNumber = pEvent->event_ID()->event_number();
  bool doEvtHeartbeat(m_eventPrintoutInterval.value() > 0 && 
                      0 == (m_nev % m_eventPrintoutInterval.value()));
  if (doEvtHeartbeat)  {
   if(!m_useTools) m_msg << MSG::INFO
	<< "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun 
	<< " " << m_nev << " events processed so far  <<<===" << endreq;
   else m_msg << MSG::INFO
	<< "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun 
	<< " " << m_nev << " events read and " << m_proc 
        << " events processed so far  <<<===" << endreq;   
  }

  // Reset the timeout singleton
  resetTimeout(Athena::Timeout::instance());
  if(toolsPassed) {
  // Fire BeginEvent "Incident"
  m_incidentSvc->fireIncident(EventIncident(*pEvent, name(),"BeginEvent"));

  // An incident may schedule a stop, in which case is better to exit before the actual execution.
  if ( m_scheduledStop ) {
    m_msg << MSG::ALWAYS
	  << "A stopRun was requested by an incidentListener. "
	  << "Do not process this event." 
	  << endreq;
    return (StatusCode::SUCCESS);
  }

  // Execute Algorithms
  //  StatusCode sc = MinimalEventLoopMgr::executeEvent(par);

  // Call the execute() method of all top algorithms 
  StatusCode sc = executeAlgorithms();

  if(!sc.isSuccess()) {
    eventFailed = true; 

 /// m_failureMode 1, 
 ///    RECOVERABLE: skip algorithms, but do not terminate job
 ///    FAILURE: terminate job 
    if (m_failureMode == 1 && sc.isRecoverable() ) {
      m_msg << MSG::WARNING
	    << "RECOVERABLE error returned by algorithm. "
	    << "Skipping remaining algorithms." << std::endl
	    << "\tNo output will be written for this event, "
	    << "but job will continue to next event"
	    << endreq;
      eventFailed = false;
    }

 /// m_failureMode 2: skip algorithms, but do not terminate job
    if (m_failureMode >= 2) {
      m_msg << MSG::INFO
	    << "Skipping remaining algorithms." << std::endl
	    << "\tNo output will be written for this event, "
	    << "but job will continue to next event"
	    << endreq;
      eventFailed = false;
    }

  }  else {

    // Call the execute() method of all output streams 
    for (ListAlg::iterator ito = m_outStreamList.begin(); 
	 ito != m_outStreamList.end(); ito++ ) {
      (*ito)->resetExecuted();
      sc = (*ito)->sysExecute(); 
      if( !sc.isSuccess() ) {
	eventFailed = true; 
      } 
    }
    
    writeHistograms().ignore();

  }

  // Fire EndEvent "Incident"
  m_incidentSvc->fireIncident(EventIncident(*pEvent, name(),"EndEvent"));
  ++m_proc;
  }  // end of toolsPassed test
  ++m_nev;
  if (doEvtHeartbeat) {
   if(!m_useTools) m_msg << MSG::INFO
	<< "  ===>>>  done processing event #" << evtNumber << ", run #" << m_currentRun 
	<< " " << m_nev << " events processed so far  <<<===" << endreq;
   else m_msg << MSG::INFO
	<< "  ===>>>  done processing event #" << evtNumber << ", run #" << m_currentRun 
	<< " " << m_nev << " events read and " << m_proc 
        << " events processed so far <<<===" << endreq;
   std::ofstream outfile( "eventLoopHeartBeat.txt");
   if ( !outfile ) {
     m_msg << MSG::ERROR << " unable to open: eventLoopHeartBeat.txt" << endreq;
   } else {
     outfile << "  done processing event #" << evtNumber << ", run #" << m_currentRun 
	     << " " << m_nev << " events read so far  <<<===" << std::endl;
     outfile.close();
   }  

  }

  //------------------------------------------------------------------------
  // Check if there was an error processing current event
  //------------------------------------------------------------------------
  return eventFailed?StatusCode::FAILURE:StatusCode::SUCCESS;

}

//=========================================================================
// implementation of IEventProcessor::executeRun
//=========================================================================
StatusCode AthenaEventLoopMgr::executeRun(int maxevt)
{
  if (!(this->nextEvent(maxevt)).isSuccess()) return StatusCode::FAILURE;
  m_incidentSvc->fireIncident(Incident(name(),"EndEvtLoop"));
  return this->endRunAlgorithms();
}

//=========================================================================
// implementation of IAppMgrUI::nextEvent
//=========================================================================
StatusCode AthenaEventLoopMgr::nextEvent(int maxevt)   
{
  // make nextEvent(0) a dummy call
  if (0 == maxevt) return StatusCode::SUCCESS;

  static int        total_nevt = 0;

  // the current 'clear-store' policy
  static const ClearStorePolicy::Type s_clearStore = 
    clearStorePolicy( m_clearStorePolicy.value(), m_msg );


  // These following two initialization loops are performed here in case new
  // Top level Algorithms or Output Streams have been created interactively
  // at run-time instead of configuration time. Note also that it is possible
  // that some newly created Algorithms are still not initialized as a result
  // of these loops (e.g. where the membership of an existing Sequencer is 
  // changed at run-time. In this case, the Algorithm::sysExecute() function 
  // will ensure that the Algorithm is correctly initialized. This mechanism 
  // actually makes loops redundant, but they do  provide a well defined 
  // location for the initialization to take place in the non-interactive case.

  StatusCode sc = initializeAlgorithms();
  if(sc.isFailure())
    return sc;

  // loop over events if the maxevt (received as input) is different from -1.
  // if evtmax is -1 it means infinite loop (till time limit that is)
  //  int nevt(0);
  bool noTimeLimit(false);


  while((maxevt == -1 || m_nevt < maxevt) && 
	(noTimeLimit = (m_pITK == 0 || m_pITK->nextIter()) ) ) {

   if(m_doChrono && total_nevt>0) m_chronoStatSvc->chronoStart("EventLoopMgr"); //start after first event
   if(m_doChrono && total_nevt>0) m_chronoStatSvc->chronoStart("EventLoopMgr_preexec");

    // Check if there is a scheduled stop issued by some algorithm/sevice
    if ( m_scheduledStop ) {
      m_scheduledStop = false;
      m_msg << MSG::ALWAYS 
	    << "A stopRun was requested. Terminating event loop." << endreq;
      break;
    }

   

    ++m_nevt; ++total_nevt; 
    //-----------------------------------------------------------------------
    // Clear the event store, if used in the event loop
    //-----------------------------------------------------------------------
    if( s_clearStore == ClearStorePolicy::BeginEvent &&
	0 != total_nevt ) {
      sc = eventStore()->clearStore();
      if( !sc.isSuccess() ) {
	m_msg << MSG::ERROR
	      << "Clear of Event data store failed" << endreq;
        m_incidentSvc->fireIncident(Incident(name(),"EndEvtLoop"));
	return sc;
      }
    }



    //-----------------------------------------------------------------------
    // we need an EventInfo Object to fire the incidents. 
    //-----------------------------------------------------------------------
    if ( m_evtContext )
    {   // Deal with the case when an EventSelector is provided

      IOpaqueAddress* addr = 0;

      sc = m_evtSelector->next(*m_evtContext);

      if ( !sc.isSuccess() )
      {
        // This is the end of the loop. No more events in the selection
        m_msg << MSG::INFO 
	      << "No more events in event selection " << endreq;
	sc = StatusCode::SUCCESS;
        break;
      }

      if (m_evtSelector->createAddress(*m_evtContext, addr).isFailure()) {
        m_msg << MSG::ERROR
	      << "Could not create an IOpaqueAddress" << endreq;
        break;
      }
      

      // Most iterators provide the IOA of an event header (EventInfo, DataHeader)
      if (0 != addr) {
	//create its proxy
	sc = eventStore()->recordAddress(addr);
	if( !sc.isSuccess() ) {
	  m_msg << MSG::WARNING 
		<< "Error declaring Event object" << endreq;
	  continue;
	}
      } 
      if ((sc=eventStore()->loadEventProxies()).isFailure()) {
	m_msg << MSG::ERROR 
	      << "Error loading Event proxies" << endreq;
	continue;
      } 
    }
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr_preexec");
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStart("EventLoopMgr_execute");
    // Execute event for all required algorithms
    sc = executeEvent(0);
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr_execute");
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStart("EventLoopMgr_postexec");
    if( !sc.isSuccess() )
    {
      m_msg << MSG::ERROR 
	    << "Terminating event processing loop due to errors" << endreq;
      break;
    }

    //-----------------------------------------------------------------------
    // Clear the event store, if used in the event loop
    //-----------------------------------------------------------------------
    if ( s_clearStore == ClearStorePolicy::EndEvent ) {
      sc = eventStore()->clearStore();
      if( !sc.isSuccess() ) {
        m_msg << MSG::ERROR 
	      << "Clear of Event data store failed" << endreq;
	break;
      }
    }
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr_postexec");
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr");
  } //event loop

   

  return (noTimeLimit ? sc : StatusCode::FAILURE);

}


//=========================================================================
// Seek to a given event.
// The event selector must support the IEventSeek interface for this to work.
//=========================================================================
StatusCode AthenaEventLoopMgr::seek (int evt)
{
  IEventSeek* is = dynamic_cast<IEventSeek*> (m_evtSelector);
  if (is == 0) {
    m_msg << MSG::ERROR << "Seek failed; unsupported by event selector"
	  <<endreq;
    return StatusCode::FAILURE;
  }
  StatusCode sc = is->seek(evt);
  if (sc.isSuccess()) {
    m_nevt = evt;
    if (m_evtSelector->createContext(m_evtContext).isFailure()) {
      m_msg  << MSG::FATAL << "Can not create the event selector Context."
             << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    m_msg << MSG::ERROR << "Seek failed." << endreq;
  }
  return sc;
}


//=========================================================================
// Return the current event count.
//=========================================================================
int AthenaEventLoopMgr::curEvent() const
{
  return m_nevt;
}

//=========================================================================
// Return the collection size
//=========================================================================
int AthenaEventLoopMgr::size()
{
  ICollectionSize* cs = dynamic_cast<ICollectionSize*> (m_evtSelector);
  if (cs == 0) {
    m_msg << MSG::ERROR << "Collection size unsupported by event selector"
	  <<endreq;
    return -1;
  }
  return cs->size();
}

void AthenaEventLoopMgr::handle(const Incident& inc)
{
  if(inc.type()!="BeforeFork")
    return;

  if(!m_evtContext || !m_firstRun) {
    m_msg << MSG::WARNING << "Skipping BeforeFork handler. Either no event selector is provided or begin run has already passed" << endreq;
  }

  // Initialize Algorithms and Output Streams
  StatusCode sc = initializeAlgorithms();
  if(sc.isFailure()) {
    m_msg << MSG::ERROR << "Failed to initialize Algorithms" << endreq;
    return; 
  }

  // Construct EventInfo
  const EventInfo* pEvent(0);
  IOpaqueAddress* addr = 0;
  sc = m_evtSelector->next(*m_evtContext);
  if(!sc.isSuccess()) {
    m_msg << MSG::INFO << "No more events in event selection " << endreq;
    return;
  }
  sc = m_evtSelector->createAddress(*m_evtContext, addr);
  if (sc.isFailure()) {
    m_msg << MSG::ERROR << "Could not create an IOpaqueAddress" << endreq;
    return; 
  }
  if (0 != addr) {
    //create its proxy
    sc = eventStore()->recordAddress(addr);
    if(!sc.isSuccess()) {
      m_msg << MSG::ERROR << "Error declaring Event object" << endreq;
      return;
    }
  } 
  
  if(eventStore()->loadEventProxies().isFailure()) {
    m_msg << MSG::WARNING << "Error loading Event proxies" << endreq;
    return;
  }

  // Retrieve the Event object
  sc = eventStore()->retrieve(pEvent);
  if(!sc.isSuccess()) {
    m_msg << MSG::ERROR << "Unable to retrieve Event root object" << endreq;
    return;
  }

  sc = beginRunAlgorithms(*pEvent);
  if (!sc.isSuccess()) {
    m_msg << MSG::ERROR << "beginRunAlgorithms() failed" << endreq;
    return;
  } 

  m_firstRun=false;
  m_currentRun = pEvent->event_ID()->run_number();

  // Clear Store
  const ClearStorePolicy::Type s_clearStore = clearStorePolicy( m_clearStorePolicy.value(), m_msg );
  if(s_clearStore==ClearStorePolicy::EndEvent) {
    sc = eventStore()->clearStore();
    if(!sc.isSuccess()) {
      m_msg << MSG::ERROR << "Clear of Event data store failed" << endreq;
    }
  }
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
AthenaEventLoopMgr::queryInterface(const InterfaceID& riid, 
				   void** ppvInterface) 
{
  if ( IEventSeek::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEventSeek*>(this);
  }
  else if ( ICollectionSize::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ICollectionSize*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
StoreGateSvc* 
AthenaEventLoopMgr::eventStore() const {
  return &*m_eventStore;
}
