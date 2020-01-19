///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class FileCutFlowSvc
// Authors: Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////

#include "FileCutFlowSvc.h"

// STL include
#include <algorithm>

// #include "FillEBCFromFlat.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/FileIncident.h"
#include "StoreGate/StoreClearedIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include <TTree.h>
#include <TFile.h>

// EDM includes
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventStreamInfo.h"


FileCutFlowSvc::FileCutFlowSvc(const std::string& name,
                       ISvcLocator* pSvcLocator ) :
  AthService(name, pSvcLocator),
  m_outMetaDataStore("StoreGateSvc/MetaDataStore", name),
  m_inMetaDataStore("StoreGateSvc/InputMetaDataStore", name),
  m_eventStore("StoreGateSvc", name),
  m_completeCollName(""),
  m_incompleteCollName(""),
  m_skimmingCycle(0),
  m_fileCollName("CutBookkeepersFile"),
  m_inputStream(""),
  m_alreadyDeterminedCycleNumber(false)
{
  declareProperty("OutputCollName",           m_completeCollName="CutBookkeepers",
    "DEFUNCT - handled by tool now");
  declareProperty("OutputIncompleteCollName", m_incompleteCollName = "IncompleteCutBookkeepers",
    "DEFUNCT - handled by tool now");
  declareProperty("SkimmingCycle",            m_skimmingCycle = 0, "DEFUNCT - handled automatically");
  declareProperty("InputStream",              m_inputStream = "N/A", "The name of the input file stream");
  assert( pSvcLocator );
}



FileCutFlowSvc::~FileCutFlowSvc()
{
}



StatusCode
FileCutFlowSvc::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() << " - package version " << PACKAGE_VERSION );

  //Get output MetaData StoreGate
  ATH_CHECK( m_outMetaDataStore.retrieve() );
  //Get input MetaData StoreGate
  ATH_CHECK( m_inMetaDataStore.retrieve() );
  //Get Event StoreGate
  ATH_CHECK( m_eventStore.retrieve() );

  // Align File name with complete name
  m_fileCollName = m_completeCollName + "File";

  //Get IncidentSvc
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
  incSvc->addListener(this, IncidentType::BeginInputFile, 60); // pri has to be < 100 to be after MetaDataSvc.
  incSvc->addListener(this, IncidentType::EndInputFile, 50); // pri has to be > 10 to be before MetaDataSvc.
  incSvc->addListener(this, "MetaDataStop", 50);

  xAOD::CutBookkeeperContainer* fileBook(NULL);
  fileBook = new xAOD::CutBookkeeperContainer();
  ATH_CHECK( recordCollection( fileBook, m_fileCollName) );
  if (m_outMetaDataStore->retrieve(m_completeBook,m_fileCollName).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve handle to cutflowsvc bookkeeper");
    //return StatusCode::RECOVERABLE;
  }

  // Determine the skimming cycle number that we should use now from the input file
  ATH_MSG_VERBOSE("Have currently the cycle number = " << m_skimmingCycle );
  ATH_CHECK( this->determineCycleNumberFromInput(m_completeCollName) );
  ATH_MSG_VERBOSE("Will use cycle number = " << m_skimmingCycle );

  return StatusCode::SUCCESS;
}


StatusCode
FileCutFlowSvc::stop()
{
  return StatusCode::SUCCESS;
}


StatusCode
FileCutFlowSvc::finalize()
{
  ATH_MSG_DEBUG( "Finalizing " << name() << " - package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}



StatusCode
FileCutFlowSvc::queryInterface( const InterfaceID& riid, void** ppvi )
{
  // valid placeholder?
  if ( 0 == ppvi ) { return StatusCode::FAILURE ; }  // RETURN
  if ( ICutFlowSvc::interfaceID() == riid ) {
    *ppvi = static_cast<ICutFlowSvc*>(this);
    addRef(); // NB! : inrement the reference count!
    return StatusCode::SUCCESS;                     // RETURN
  }
  // Interface is not directly availible: try out a base class
  return AthService::queryInterface( riid, ppvi );
}


CutIdentifier FileCutFlowSvc::registerFilter( const std::string& name,
                                          const std::string& description )
{
  ATH_MSG_DEBUG("calling registerFilter(" << name << ", " << description << ")" );

  xAOD::CutBookkeeper* tmpEBK = new xAOD::CutBookkeeper();
  tmpEBK->setName(name);
  tmpEBK->setDescription(description);
  tmpEBK->setInputStream(m_inputStream);
  tmpEBK->setCycle(m_skimmingCycle);
  CutIdentifier cutID = tmpEBK->uniqueIdentifier();

  // Let's see if an CutBookkeeper of this name already exists
  ATH_MSG_VERBOSE("in registerFilter(" << name << ", " << description << "): "
                  << "Going to search if this CutBookkeeper already exists" );
  bool existsAlready=false;
  for ( std::size_t i=0; i<m_completeBook->size(); ++i ) {
    xAOD::CutBookkeeper* ebk = m_completeBook->at(i);
    if( tmpEBK->isEqualTo( ebk) ) {
      ATH_MSG_DEBUG("The CutBookkeeper with name " << name
                    << " and cutID " << cutID << " already exists... not adding!" );
      existsAlready = true;
      cutID = ebk->uniqueIdentifier();
      break;
    }
  }

  ATH_MSG_VERBOSE("in registerFilter(" << name << ", " << description << "): "
                  << "Found it: " << existsAlready );

  // If this CutBookkeeper already exists, delete the temporary
  // and return the existing cutID
  if ( existsAlready ) {
    delete tmpEBK;
    return cutID;
  }

  // If it is a new CutBookkeeper, add it to the container
  ATH_MSG_DEBUG( "You are DECLARING a new filter of name " << name
                 << " and cutID " << cutID );
  m_completeBook->push_back(tmpEBK);

  ATH_MSG_VERBOSE("done calling registerFilter(" << name << ", " << description << ")" );
  return cutID;
}




CutIdentifier FileCutFlowSvc::registerCut( const std::string& name,
                                       const std::string& description,
                                       CutIdentifier originCutID )
{
  ATH_MSG_DEBUG("calling registerCut(" << name << ", " << description
                << ", " << originCutID << ")" );

  // Get the CutBookkeeper of the origin Filter Algorithm/Tool
  xAOD::CutBookkeeper *originEBK = this->getCutBookkeeper(originCutID);
  if ( !originEBK ) {
    ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << originCutID );
    return 0;
  }

  // Call the registerFilter method and get the correct CutBookkeeper
  // from the returned cutID
  CutIdentifier cutID = this->registerFilter( name, description );
  xAOD::CutBookkeeper* ebk = this->getCutBookkeeper(cutID);
  if ( !ebk ) {
    ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << cutID );
    return 0;
  }
  originEBK->addUsedOther( ebk );

  return cutID;
}




// This method is probably only called by the DecionsSvc
CutIdentifier FileCutFlowSvc::registerTopFilter( const std::string& name,
                                             const std::string& description,
                                             unsigned int logic,
                                             const std::string& outputStream ) 
{
  ATH_MSG_DEBUG("calling registerTopFilter(" << name << ", " << description
                << ", " << logic << ", " << outputStream << ")" );
  // Call the registerFilter method and get the correct CutBookkeeper
  // from the returned cutID
  CutIdentifier cutID = this->registerFilter( name, description );
  xAOD::CutBookkeeper* ebk = this->getCutBookkeeper(cutID);
  if ( !ebk ) {
    ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << cutID );
    return 0;
  }

  //Then set the logic and outputStream of the relevant CutBookkeeper
  // toFind->setDescription(description);
  ebk->setCutLogic(xAOD::CutBookkeeper::CutLogic(logic));
  ebk->setTopFilter(true);
  ebk->addOutputStreamForAllUsed(outputStream);

  return cutID;
}



CutIdentifier FileCutFlowSvc::declareUsedOtherFilter( const std::string& name,
                                                  CutIdentifier originCutID )
{
  ATH_MSG_DEBUG("calling declareUsedOtherFilter(" << name << ", " << originCutID << ")" );

  // Get the CutBookkeeper of the origin cut
  xAOD::CutBookkeeper *originEBK = this->getCutBookkeeper(originCutID);
  if ( !originEBK ) {
    ATH_MSG_ERROR( "(file: " << __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << originCutID );
    return 0;
  }

  // Create a temporary CutBookkeeper object
  xAOD::CutBookkeeper* tmpEBK = new xAOD::CutBookkeeper();
  // tmpEBK->makePrivateStore();
  tmpEBK->setName(name);
  tmpEBK->setInputStream(m_inputStream);
  tmpEBK->setCycle(m_skimmingCycle);
  CutIdentifier cutID = tmpEBK->uniqueIdentifier();

  // See if the CutBookkeeper already exists or not
  bool existsAlready = false;
  for ( std::size_t i=0; i<m_completeBook->size(); ++i ) {
    xAOD::CutBookkeeper* ebk = m_completeBook->at(i);
    if( tmpEBK->isEqualTo( ebk ) ) {
      originEBK->addUsedOther( ebk );
      cutID = ebk->uniqueIdentifier();
      existsAlready = true;
    }
  }

  // If this CutBookkeeper already exists, delete the temporary
  // and return the existing cutID
  if ( existsAlready ) {
    delete tmpEBK;
    return cutID;
  }

  // Otherwise, add the new one to the collection
  tmpEBK->setDescription( "Registered by origin filter" );
  originEBK->addUsedOther( tmpEBK );
  m_completeBook->push_back( tmpEBK );

  return cutID;
}



void
FileCutFlowSvc::setFilterDescription( CutIdentifier cutID,
                                  const std::string& descr )
{
  ATH_MSG_DEBUG("calling setFilterDescription(" << cutID << ", " << descr << ")" );

  xAOD::CutBookkeeper* ebk = this->getCutBookkeeper(cutID);
  ebk->setDescription(descr);
  return;
}




void
FileCutFlowSvc::addEvent( CutIdentifier cutID )
{
  ATH_MSG_INFO("calling addEvent(" << cutID << ")" );

  double evtWeight=1.0;

  const xAOD::EventInfo* evtInfo = 0;
  StatusCode sc = m_eventStore->retrieve(evtInfo);
  if ( sc.isFailure() || NULL == evtInfo ) {
    ATH_MSG_WARNING("Could not retrieve EventInfo from StoreGate  ");
    evtWeight=-1000.;
  } else {
    // Only try to access the mcEventWeight is we are running on Monte Carlo, duhhh!
    if ( evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) {
      evtWeight = evtInfo->mcEventWeight();
    }
  }

  addEvent(cutID,evtWeight);

  return;
}



void
FileCutFlowSvc::addEvent( CutIdentifier cutID, double weight)
{
  ATH_MSG_INFO("calling addEvent(" << cutID << ", " << weight << ")" );

  xAOD::CutBookkeeper* eb = this->getCutBookkeeper(cutID);
  if ( !eb ) {
    ATH_MSG_INFO("Could not find eb");

    // Iterate over the complete bookkeepers and update the cutID-to-bookkeeper map
    ATH_MSG_DEBUG( "addEvent: Going to re-populate the map. Have "
                   << m_completeBook->size() << " CutBookkeepers"
                   << " and skimming cycle " << m_skimmingCycle
                   << " and input Stream name " << m_inputStream );
    xAOD::CutBookkeeperContainer::iterator iter    = m_completeBook->begin();
    xAOD::CutBookkeeperContainer::iterator iterEnd = m_completeBook->end();
    for ( ; iter != iterEnd; ++iter ) {
      xAOD::CutBookkeeper* ebk = *iter;
      CutIdentifier cutID = ebk->uniqueIdentifier();
      ATH_MSG_VERBOSE( "addEvent: Have CutBookkeeper with"
                       << " skimming cycle " << ebk->cycle()
                       << " and input Stream name " << ebk->inputStream()
                       << " and logic " << ebk->cutLogic()
                       << " isTopFilter " << ebk->isTopFilter()
                       << " and cutID " << cutID
                       << " and name " << ebk->name() );
      if ( m_skimmingCycle == ebk->cycle() ) {
        if ( m_inputStream == ebk->inputStream() ) {
          CutIDMap_t::iterator mapIter = m_ebkMap.find(cutID);
          ATH_MSG_DEBUG( "BeginInputFile: Have CutBookkeeper with"
                         << " cutID " << cutID
                         <<  " and name " << ebk->name() );
          if ( mapIter != m_ebkMap.end() ) { // we found this CutBookkeeper in the existing map
            (*mapIter).second = ebk;
          }
        }
      }
    } // End: Loop over all complete CutBookkeepers
    eb = this->getCutBookkeeper(cutID);
  }
  if ( !eb ) {
    ATH_MSG_ERROR ( "Got a zero pointer to an CutBookkeeper with cutID="
                    << cutID << "! This should never happen..." );
    return;
  }
  ATH_MSG_VERBOSE( "addEvent: have cutID " << cutID
                   << " and CutBookkeeper name " << eb->name() );
  eb->addNAcceptedEvents(1);
  eb->addSumOfEventWeights(weight);
  eb->addSumOfEventWeightsSquared(weight*weight);
  return;
}



//__________________________________________________________________________
void FileCutFlowSvc::handle( const Incident& inc )
{
  ATH_MSG_VERBOSE( "Start incident " << inc.type() );

  //OPENING NEW INPUT FILE
  //Things to do:
  // 1) note that a file is currently opened
  // 2) Load CutBookkeepers from input file
  //    2a) if incomplete from input, directly propagate to output
  //    2b) if complete from input, wait for EndInputFile to decide what to do in output

  if ( inc.type() == IncidentType::BeginInputFile ) {
    // Check the stream name
    const EventStreamInfo* esi;
    if (m_inMetaDataStore->retrieve(esi).isFailure()) {
      ATH_MSG_WARNING("No EventStreamInfo taking stream from property InputStream");
    }
    else {
      std::string inputstream = *(esi->getProcessingTags().begin());
      if (m_inputStream.empty()) {m_inputStream=inputstream;}
      else if (m_inputStream!=inputstream) {
        const FileIncident* finc = dynamic_cast<const FileIncident*>(&inc);
        ATH_MSG_FATAL("File " << finc->fileName() 
                              << " stream " << inputstream 
                              << " does not match previous file " 
                              << m_inputStream);
        return;
      }
    }

    // Clear the file bookkeeper
    if (m_completeBook.isValid()) {
      // Reset existing container
      for (xAOD::CutBookkeeperContainer::iterator it = m_completeBook->begin();
           it != m_completeBook->end(); ++it) {
        (*it)->setNAcceptedEvents(0);
        (*it)->setSumOfEventWeights(0);
        (*it)->setSumOfEventWeightsSquared(0);
      }
    }
  }

  // Clean up the bookkeeper before output
  //if ( inc.type() == "MetaDataStop" || inc.type() == "EndInputFile") {
  if ( inc.type() == "MetaDataStop" ) {
    if (m_completeBook.isValid()) {
      // Reset existing container
      for (xAOD::CutBookkeeperContainer::iterator it = m_completeBook->begin();
           it != m_completeBook->end(); ++it) {
      }
    }
    const xAOD::CutBookkeeperContainer* fileBook(NULL);
    if( !(m_outMetaDataStore->retrieve(fileBook, m_fileCollName) ).isSuccess() ) {
      ATH_MSG_WARNING( "Could not get " << m_fileCollName 
                       << " CutBookkeepers from output MetaDataStore" );
    }
    else {
      const SG::IConstAuxStore* fileBookAux = fileBook->getConstStore();
      if (m_outMetaDataStore->removeDataAndProxy(fileBook).isFailure()) {
        ATH_MSG_ERROR("Unable to remove " << m_fileCollName);
      }
      if (m_outMetaDataStore->removeDataAndProxy(fileBookAux).isFailure()) {
        ATH_MSG_ERROR("Unable to remove " << m_fileCollName);
      }
    }
  }

  ATH_MSG_DEBUG( "End incident " << inc.type() );
  return;
}


void
FileCutFlowSvc::print()
{
  return;
}


StatusCode FileCutFlowSvc::determineCycleNumberFromInput( const std::string& collName )
{
  ATH_MSG_DEBUG("calling determineCycleNumberFromInput('" << collName
                  << "')... have currently cycle number = " << m_skimmingCycle );

  // Try to get CutBookkeepers from the input file
  if ( m_inMetaDataStore->contains<xAOD::CutBookkeeperContainer>(collName) ) {
    ATH_MSG_VERBOSE("Found xAOD::CutBookkeeperContainer in input MetaStore with name: " << collName);

    // There can always only be a single object in the input store. As the store
    // is connected to just a single input file.
    const xAOD::CutBookkeeperContainer* constColl = 0;
    ATH_CHECK( m_inMetaDataStore->retrieve( constColl, collName ) );
    xAOD::CutBookkeeperContainer* tmpColl = const_cast<xAOD::CutBookkeeperContainer*>(constColl);
    if ( !(tmpColl->hasStore()) ) {
      ATH_MSG_VERBOSE("Setting store of xAOD::CutBookkeeperContainer");
      // Get also the auxilliary store
      // const SG::IConstAuxStore* auxColl = 0;
      const xAOD::CutBookkeeperAuxContainer* auxColl = 0;
      ATH_CHECK( m_inMetaDataStore->retrieve(auxColl, collName+"Aux.") );
      tmpColl->setConstStore( auxColl );
    }
    // Now, iterate over all CutBookkeepers and search for the highest cycle number
    int maxCycle=0;
    for ( std::size_t i=0; i<tmpColl->size(); ++i ) {
      // Get the current old EBK
      const xAOD::CutBookkeeper* cbk = tmpColl->at(i);
      int inCycle = cbk->cycle();
      if (inCycle > maxCycle) maxCycle = inCycle;
    }
    m_skimmingCycle = std::max(m_skimmingCycle,maxCycle+1);
  }

  ATH_MSG_DEBUG("done calling determineCycleNumberFromInput('" << collName
                  << "')... have now cycle number = " << m_skimmingCycle );
  return StatusCode::SUCCESS;
}



StatusCode
FileCutFlowSvc::recordCollection( xAOD::CutBookkeeperContainer * coll,
                              const std::string &collName) {
  ATH_MSG_VERBOSE("calling recordCollection(..., " << collName << ")" );

  //Expected that FileCutFlowSvc is the only one allowed to record *coll
  if( m_outMetaDataStore->contains<xAOD::CutBookkeeperContainer>(collName) ) {
    ATH_MSG_ERROR( "xAOD::CutBookkeeperContainer " << collName
      << " already exists in output MetaData store. This is unexpected." );
    return StatusCode::FAILURE;
  }

  // Record the container
  ATH_CHECK( m_outMetaDataStore->record(coll, collName) );
  ATH_MSG_VERBOSE("Recorded xAOD::CutBookkeeperContainer " << collName);


  // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore
  xAOD::CutBookkeeperAuxContainer* auxCont = new xAOD::CutBookkeeperAuxContainer;
  coll->setStore( auxCont ); //gives it a new associated aux container

  // Record the aux container
  ATH_CHECK( m_outMetaDataStore->record( auxCont, collName+"Aux." ) );
  ATH_MSG_VERBOSE( "Recorded xAOD::CutBookkeeperContainer " << collName << "Aux." );

  return StatusCode::SUCCESS;
}



xAOD::CutBookkeeper*
FileCutFlowSvc::getCutBookkeeper( const CutIdentifier cutID ) {
  xAOD::CutBookkeeperContainer::iterator it = m_completeBook->begin();
  xAOD::CutBookkeeperContainer::iterator ite = m_completeBook->end();
  while (it != ite) {
    if ((*it)->uniqueIdentifier()==cutID) return (*it);
    ++it;
  }
  return 0;
}

