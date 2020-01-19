/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamEventStorageInputSvc.h"

#include "DumpFrags.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "ByteStreamData/ByteStreamUserMetadata.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "EventStorage/pickDataReader.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/Property.h"

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/Token.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
//#include "xAODEventInfo/EventInfo.h"
//#include "xAODEventInfo/EventAuxInfo.h"

#include "eformat/HeaderMarker.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Issue.h"
#include "eformat/Problem.h"
#include "eformat/Version.h"
#include "eformat/Status.h"
#include "eformat/old/util.h"

#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>


// Constructor.
ByteStreamEventStorageInputSvc::ByteStreamEventStorageInputSvc(const std::string& name,
		ISvcLocator* svcloc) : ByteStreamInputSvc(name, svcloc),
	//m_totalEventCounter(0),
	m_re(0),
        m_eventStatus(0),
	m_reader(0),
        m_evtInFile(0),
        m_eventOffset(0),
	m_sgSvc("StoreGateSvc", name),
	m_mdSvc("StoreGateSvc/InputMetaDataStore", name),
	m_incidentSvc("IncidentSvc", name),
        m_attlistsvc("ByteStreamAttListMetadataSvc", name),
        m_robProvider("ROBDataProviderSvc", name),
	m_sequential(false),
	m_fileCount(0) {
   declareProperty("FullFileName", m_vExplicitFile);
   declareProperty("EnableSequential", m_sequential);
   declareProperty("DumpFlag", m_dump = false);
   declareProperty("WaitSecs", m_wait = 0);
   declareProperty("ValidateEvent", m_valEvent = true);
   declareProperty("ProcessBadEvent", m_procBadEvent = false); // Defunct property, backward compatibility only
   declareProperty("MaxBadEvents", m_maxBadEvts = -1); // Defunct property, backward compatibility only

   declareProperty("EventStore", m_sgSvc);
   declareProperty("MetaDataStore", m_mdSvc);
   declareProperty("AttributeListKeys", m_keys);
}
//------------------------------------------------------------------------------
ByteStreamEventStorageInputSvc::~ByteStreamEventStorageInputSvc() {
   delete m_re; m_re = 0;
   delete m_reader; m_reader = 0;
}
//------------------------------------------------------------------------------
StatusCode ByteStreamEventStorageInputSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!ByteStreamInputSvc::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize ByteStreamInputSvc base class.");
      return(StatusCode::FAILURE);
   }

   // Retrieve InputMetaDataStore
   if (!m_mdSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get InputMetaDataStore.");
      return(StatusCode::FAILURE);
   }
   // Retrieve IncidentSvc
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve AttListSvc
   if (!m_attlistsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get metadata AttListSvc.");
      return(StatusCode::FAILURE);
   }
   if (!m_robProvider.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get rob data provider");
      return(StatusCode::FAILURE);
   }
   // Initialize stores for user metadata
   if (m_keys.size()>0) {
     StatusCode sc = m_attlistsvc->readInit(m_keys);
     if (sc.isFailure()) {
       msg() << MSG::WARNING << "readInit for AttributeList service failed" << endreq;
     }
   }

   // Check if defunct properties set, and give instructions
   if (m_procBadEvent != false) ATH_MSG_WARNING("ProcessBadEvent property has been moved to EventSelector, please use svgMgr.EventSelector.ProcessBadEvent instead");
   if (m_maxBadEvts != -1) ATH_MSG_WARNING("MaxBadEvents property has been moved to EventSelector, please use svgMgr.EventSelector.MaxBadEvents instead");

   // Retrieve StoreGateSvc
   if (!m_sgSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get StoreGateSvc.");
      return(StatusCode::FAILURE);
   }

   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode ByteStreamEventStorageInputSvc::stop() {
   ATH_MSG_DEBUG("Calling ByteStreamInputSvc::stop()");
   // close moved to EventSelector for explicit coupling with incident
   //if (m_reader != 0) closeBlockIterator(false);
   return(StatusCode::SUCCESS);
}
//------------------------------------------------------------------------------
StatusCode ByteStreamEventStorageInputSvc::finalize() {
   // delete the old event
   releaseCurrentEvent();
   if (!m_sgSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release StoreGateSvc");
   }
   if (!m_incidentSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release IncidentSvc");
   }
   if (!m_robProvider.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release rob data provider");
   }
   if (!m_mdSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release InputMetaDataStore");
   }
   return(ByteStreamInputSvc::finalize());
}
//------------------------------------------------------------------------------
long ByteStreamEventStorageInputSvc::positionInBlock()
{
  return m_evtOffsets.size();
}
//------------------------------------------------------------------------------
// Load both default and user freemetadata into store
bool ByteStreamEventStorageInputSvc::loadMetadata()
{
  bool good = true;

  // Get ByteStream Metadata into Input MetaData Store
  std::vector<std::string> fullFmd = m_reader->freeMetaDataStrings();
  std::vector<std::string> user, nonuser;
  for (std::vector<std::string>::iterator it=fullFmd.begin(); it != fullFmd.end(); ++it) {
     if (it->substr(0,5)=="UMD::") user.push_back(*it);
     else                          nonuser.push_back(*it);
  }
  // default goes into ByteStreamMetadata
  std::bitset<64> word1;
  std::bitset<64> word2;
  for (unsigned int i=0; i<64; ++i) {
     word1[i] = m_reader->detectorMask()[i];
     word2[i] = m_reader->detectorMask()[i+64];
  }
  ByteStreamMetadata* metadata = new ByteStreamMetadata(m_reader->runNumber(), m_reader->eventsInFile(),
	  m_reader->maxEvents(), m_reader->recEnable(), m_reader->triggerType(), word1.to_ulong(), word2.to_ulong(),
	  m_reader->beamType(), m_reader->beamEnergy(), m_reader->GUID(), m_reader->stream(),
	  m_reader->projectTag(), m_reader->lumiblockNumber(), nonuser);
  StatusCode status = m_mdSvc->record(metadata, "ByteStreamMetadata");
  if (!status.isSuccess()) {
     delete metadata; metadata = 0;
     ATH_MSG_WARNING("Unable to record Input MetaData for ByteStream");
     good = false;
  }
  else ATH_MSG_DEBUG("Recorded ByteStreamMetadata in InputMetadataStore");
  // user goes into ByteStreamUserMetadata
  for (std::vector<std::string>::const_iterator it=user.begin(); it != user.end(); ++it) {
     // find the beginning of the data
     std::string::size_type keybegin = it->rfind(ByteStreamFreeMetadataString::foldersep())+ByteStreamFreeMetadataString::foldersep().size();
     bool dataok(true);
     ByteStreamFreeMetadataString bsfms;
     // If dataset found, then make the FMS
     if (keybegin!=0) {
        dataok = bsfms.fromString(it->substr(keybegin));
     }
     // Find the beginning of the folder
     std::string::size_type folderbegin = it->find(ByteStreamFreeMetadataString::foldersep());
     std::string folder("UNKNOWN");
     // If folder found, then set string to that
     if (folderbegin!=std::string::npos) {
        folderbegin += ByteStreamFreeMetadataString::foldersep().size();
        folder = it->substr(folderbegin,keybegin-folderbegin-ByteStreamFreeMetadataString::foldersep().size());
     }
     // If data is ok
     if (dataok) {
        ByteStream::FreeMetadata* umeta;
        // Check if already in storegate and use that one if there, otherwise create one
        if (m_mdSvc->contains<ByteStream::FreeMetadata>(folder)) {
           StatusCode sc = m_mdSvc->retrieve(umeta,folder);
           if (sc.isSuccess()) {
              umeta->push_back(bsfms);
           }
        }
        else {
           umeta = new ByteStream::FreeMetadata();
           umeta->push_back(bsfms);
           StatusCode screc = m_mdSvc->record(umeta,folder);
           if (!screc.isSuccess()) {
              delete umeta; 
              ATH_MSG_WARNING("Unable to record User MetaData for ByteStream");
              good = false;
           }
        }
     }
     else good = false;
  }

  // Rebuild attributeLists with user metadata
  if (m_keys.size()>0) {
    StatusCode sc = m_attlistsvc->fromBSMetadata(m_keys);
    if (sc.isFailure()) {
       msg() << MSG::WARNING << "Conversion failed for AttributeList service" << endreq;
    }
  }

  ATH_MSG_DEBUG(" run parameters  =  \n "
     << " run number  " << m_reader->runNumber() << "\n"
     << " max events  " << m_reader->maxEvents() << "\n"
     << " rec enable  " << m_reader->recEnable() << "\n"
     << " trigger_type " << m_reader->triggerType() << "\n"
     << " detector mask  " << m_reader->detectorMask() << "\n"
     << " beam type " << m_reader->beamType() << "\n"
     << " beam energy " << m_reader->beamEnergy());

  std::vector<std::string> vMetaData = m_reader->freeMetaDataStrings();
  ATH_MSG_DEBUG("Number of Free meta data strings: " << vMetaData.size());
  for (unsigned int i = 0; i < vMetaData.size(); ++i) {
    ATH_MSG_DEBUG("MetaData[" << i << "]:" << vMetaData[i]);
  }

  return good;
}

const RawEvent* ByteStreamEventStorageInputSvc::previousEvent() {
  // Load data buffer from file
  char *buf;
  unsigned int eventSize;
  if (readerReady()) {
    //get current event position (cast to long long until native tdaq implementation)
    m_evtInFile--;
    m_eventOffset = m_evtOffsets.at(m_evtInFile);
    DRError ecode = m_reader->getData(eventSize,&buf,m_evtOffsets.at(m_evtInFile -1));
    if (DRWAIT == ecode && m_wait > 0) {
      do {
        // wait for n seconds
        ATH_MSG_DEBUG("Waiting for input:  " << m_wait << " seconds");
        int result = usleep((int)(m_wait*1000000));
        if (result != 0) {
          ATH_MSG_ERROR("System Error while running sleep");
          return 0;
        }
      } while(m_reader->getData(eventSize,&buf,m_eventOffset) == DRWAIT);
    } else if (DROK != ecode) {
      ATH_MSG_ERROR("Error reading next event");
      throw ByteStreamExceptions::readError();
    }
    ATH_MSG_DEBUG("Event Size " << eventSize);
  }
  else {
    ATH_MSG_ERROR("DataReader not ready. Need to getBlockIterator first");
    return 0;
  }

  // initialize before building RawEvent
  releaseCurrentEvent();
 
  // Use buffer to build FullEventFragment
  try {
    buildFragment(buf,eventSize,true);
  }
  catch (...) {
    // rethrow any exceptions
    throw;
  }

  if (m_re==0) {
    ATH_MSG_ERROR("Failure to build fragment");
    return 0;
  }

  // Set it for the data provider
  m_robProvider->setNextEvent(m_re);
  m_robProvider->setEventStatus(m_eventStatus);

  // dump
  if (m_dump) {
    DumpFrags::dump(m_re);
  }
  return(m_re);
}
//------------------------------------------------------------------------------
// Read the next event.
const RawEvent* ByteStreamEventStorageInputSvc::nextEvent() {

  // Load data buffer from file
  char *buf;
  unsigned int eventSize;
  if (readerReady()) {
    DRError ecode;
    // Check if have moved back from high water mark
    m_evtInFile++; // increment iterator
    if (m_evtInFile+1 > m_evtOffsets.size()) { 
      //get current event position (cast to long long until native tdaq implementation)
      ATH_MSG_DEBUG("nextEvent _above_ high water mark");
      m_eventOffset = (long long)m_reader->getPosition();
      m_evtOffsets.push_back(m_eventOffset);
      ecode = m_reader->getData(eventSize,&buf);
    }
    else {
      // Load from previous offset
      ATH_MSG_DEBUG("nextEvent below high water mark");
      m_eventOffset = m_evtOffsets.at(m_evtInFile-1);
      ecode = m_reader->getData(eventSize,&buf,m_eventOffset);
    }
    if (DRWAIT == ecode && m_wait > 0) {
      do {
        // wait for n seconds
        ATH_MSG_DEBUG("Waiting for input:  " << m_wait << " seconds");
        int result = usleep((int)(m_wait*1000000));
        if (result != 0) {
          ATH_MSG_ERROR("System Error while running sleep");
          return 0;
        }
      } while(m_reader->getData(eventSize,&buf) == DRWAIT);
    } else if (DROK != ecode) {
      ATH_MSG_ERROR("Error reading next event");
      throw ByteStreamExceptions::readError();
    }
    ATH_MSG_DEBUG("Event Size " << eventSize);
  }
  else {
    ATH_MSG_ERROR("DataReader not ready. Need to getBlockIterator first");
    return 0;
  }

  // initialize before building RawEvent
  releaseCurrentEvent();
 
  // Use buffer to build FullEventFragment
  try {
    buildFragment(buf,eventSize,true);
  }
  catch (...) {
    // rethrow any exceptions
    throw;
  }

  if (m_re==0) {
    ATH_MSG_ERROR("Failure to build fragment");
    return 0;
  }

  // Set it for the data provider
  m_robProvider->setNextEvent(m_re);
  m_robProvider->setEventStatus(m_eventStatus);

  //++m_totalEventCounter;

  // dump
  if (m_dump) {
    DumpFrags::dump(m_re);
  }
  return(m_re);
}

void ByteStreamEventStorageInputSvc::validateEvent()
{
  if (m_valEvent) {
    // check validity
    std::vector<eformat::FragmentProblem> p;
    m_re->problems(p);
    if (!p.empty()) {
      m_eventStatus += 0x01000000;
      // bad event
      ATH_MSG_WARNING("Failed to create FullEventFragment");
      for (std::vector<eformat::FragmentProblem>::const_iterator i = p.begin(), iEnd = p.end();
	        i != iEnd; i++) {
        ATH_MSG_WARNING(eformat::helper::FragmentProblemDictionary.string(*i));
      }
      releaseCurrentEvent();
      throw ByteStreamExceptions::badFragmentData();
    }
    if (!ROBFragmentCheck()) {
      m_eventStatus += 0x02000000;
      // bad event
      releaseCurrentEvent();
      ATH_MSG_ERROR("Skipping bad event");
      throw ByteStreamExceptions::badFragmentData();
    }
  } 
  else {
    ATH_MSG_DEBUG("Processing event without validating.");
  }
}

void ByteStreamEventStorageInputSvc::buildFragment(void* data, uint32_t eventSize, bool validate)
{
  OFFLINE_FRAGMENTS_NAMESPACE::DataType* fragment = reinterpret_cast<OFFLINE_FRAGMENTS_NAMESPACE::DataType*>(data);
  if (validate) {
    // check fragment type
    const uint32_t headWord = fragment[0];
    ATH_MSG_DEBUG("First word of the fragment " << MSG::hex << headWord << MSG::dec);
    // format version
    const uint32_t formatVersion = eformat::helper::Version(fragment[3]).major_version();
    ATH_MSG_DEBUG("Format version " << MSG::hex << formatVersion << MSG::dec);
    // error message
    if ((formatVersion != eformat::MAJOR_DEFAULT_VERSION) && 
        (formatVersion != eformat::MAJOR_V24_VERSION) && 
        (formatVersion != eformat::MAJOR_V30_VERSION) && 
        (formatVersion != eformat::MAJOR_V40_VERSION) && 
        (formatVersion != eformat::MAJOR_V31_VERSION) ) {
      ATH_MSG_ERROR("unsupported Format Version : " << MSG::hex << formatVersion << MSG::dec);
    }

    if (eformat::FULL_EVENT == headWord || 0xcc1234cc == headWord) { // ROS = 0xcc1234cc
      try {
        // convert old fragment
        if (formatVersion != eformat::MAJOR_DEFAULT_VERSION) {
          // 100 for increase of data-size due to header conversion
          uint32_t newEventSize = eventSize + 1000;
          OFFLINE_FRAGMENTS_NAMESPACE::DataType* newFragment = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[newEventSize];
          eformat::old::convert(fragment, newFragment, newEventSize);
          // delete old fragment
          delete [] fragment; fragment = 0;
          // set new pointer
          fragment = newFragment;
        }
      } catch (eformat::Issue& ex) {
        // bad event
        ATH_MSG_WARNING(ex.what());
        releaseCurrentEvent();
        ATH_MSG_ERROR("Skipping bad event");
        throw ByteStreamExceptions::badFragment();
      }
    }
    else {
      // Unknown fragment
      ATH_MSG_FATAL("Unknown Header work in input fragment " << MSG::hex << headWord);
      throw ByteStreamExceptions::badFragment();
    }
  } 
  // This is a FullEventFragment
  // make a new FEFrag in memory from it
  m_eventStatus = 0;
  if (fragment[5] > 0) {
    m_eventStatus += eformat::helper::Status(fragment[6]).specific();
    m_eventStatus += (eformat::helper::Status(fragment[6]).generic() & 0x000000ff) << 16;
  }

  // This is a FullEventFragment
  // make a new RawEvent in memory from it
  m_re = new RawEvent(fragment);
  ATH_MSG_DEBUG("Made an FullEventFragment from ES " << fragment);

}

//__________________________________________________________________________
StatusCode ByteStreamEventStorageInputSvc::generateDataHeader()
{
  // get file GUID
  m_fileGUID = m_reader->GUID();
  // reader returns -1 when end of the file is reached
  if (m_eventOffset != -1) {
    ATH_MSG_DEBUG("ByteStream File GUID:" << m_fileGUID);
    ATH_MSG_DEBUG("ByteStream Event Position in File: " << m_eventOffset);
    // Created data header element with BS provenance information
    Token* token = new Token();
    token->setDb(m_fileGUID);
    token->setTechnology(0x00001000);
    token->setOid(Token::OID_t(0LL, m_eventOffset));
    DataHeaderElement Dhe(ClassID_traits<DataHeader>::ID(), "StreamRAW", token);
    // Create data header itself
    DataHeader* Dh = new DataHeader();
    // To accomodate for skipEvents option in EventSelector
    // While skipping BS event Selector does not return SUCCESS code,
    // just advances silently through events. So SG content is not refreshed
    // Lets do refresh of the event header here
    if (m_sgSvc->contains<DataHeader>("ByteStreamDataHeader")) {
      // Temporary event header pointer for retrieval of the old one , if exists
      DataHeader* Dh_temp;
      Dh_temp = m_sgSvc->retrieve<DataHeader>("ByteStreamDataHeader");
      StatusCode sc = m_sgSvc->remove<DataHeader>(Dh_temp);
      if (!sc.isSuccess()) {
        ATH_MSG_ERROR("Failed to remove DataHeader ByteStreamDataHeader");
      }
    }
    // Declare header primary
    Dh->setStatus(DataHeader::Primary);
    //add the Dhe self reference to the object vector
    Dh->insert(Dhe);

    if (m_sgSvc->contains<EventInfo>("ByteStreamEventInfo")) {
      // Temporary event header pointer for retrieval of the old one , if exists
      const DataHandle<EventInfo> Ei_temp;
      //Ei_temp = m_sgSvc->retrieve<EventInfo>("ByteStreamEventInfo");
      if (m_sgSvc->retrieve(Ei_temp,"ByteStreamEventInfo").isSuccess()) {
        StatusCode sc = m_sgSvc->remove(Ei_temp.cptr());
        if (!sc.isSuccess()) {
          ATH_MSG_ERROR("Failed to remove ByteStreamEventInfo");
        }
      }
    }
    // Now add ref to EventInfo objects
    IOpaqueAddress* iop = new ByteStreamAddress(ClassID_traits<EventInfo>::ID(), "ByteStreamEventInfo", "");
    StatusCode ioc = m_sgSvc->recordAddress("ByteStreamEventInfo",iop);
    if (ioc.isSuccess()) {
      const SG::DataProxy* ptmp = m_sgSvc->transientProxy(ClassID_traits<EventInfo>::ID(), "ByteStreamEventInfo");
      if (ptmp !=0) {
        DataHeaderElement DheEI(ptmp->transientAddress(),"ByteStreamEventInfo");
        Dh->insert(DheEI);
      }
      //else ATH_MSG_ERROR("Failed to create EventInfo proxy " << ptmp);
    }
    // Record new data header.Boolean flags will allow it's deletionin case
    // of skipped events.
    return m_sgSvc->record<DataHeader>(Dh, "ByteStreamDataHeader", true, false, true);
  }
  return StatusCode::FAILURE;
}

//__________________________________________________________________________
void ByteStreamEventStorageInputSvc::releaseCurrentEvent()
{
   // cleanup parts of previous event and re-init them
   if (m_re) {
      OFFLINE_FRAGMENTS_NAMESPACE::PointerType fragment = 0;
      m_re->start(fragment);
      delete [] fragment; fragment = 0;
      delete m_re; m_re = 0;
   }
}

//__________________________________________________________________________
void ByteStreamEventStorageInputSvc::closeBlockIterator(bool clearMetadata)
{
    if (clearMetadata) {
       ATH_MSG_WARNING("Clearing input metadata store");
       StatusCode status = m_mdSvc->clearStore();
       if (!status.isSuccess()) {
          ATH_MSG_WARNING("Unable to clear Input MetaData Proxies");
       }
    }
    if (!readerReady()) {
       ATH_MSG_INFO("No more events in this run, high water mark for this file = " << m_evtOffsets.size()-1);
    }
    delete m_reader; m_reader = 0;
}

//__________________________________________________________________________
bool ByteStreamEventStorageInputSvc::setSequentialRead()
{
    // enable SequenceReading
    m_reader->enableSequenceReading();
    return true;
}

bool ByteStreamEventStorageInputSvc::ready()
{
   return readerReady();
}

//__________________________________________________________________________
long ByteStreamEventStorageInputSvc::getBlockIterator(const std::string fileName)
{
   m_fullFile = fileName;

   // open the file
   if (m_reader != 0) closeBlockIterator();
   m_reader = pickDataReader(fileName);
   if (!m_reader) {
      ATH_MSG_ERROR("Failed to open file " << fileName);
      closeBlockIterator();
      return -1; 
   }
   // Initilaize offset vector
   m_evtOffsets.resize(m_reader->eventsInFile(), -1);
   m_evtOffsets.clear();
   // Get ByteStream Metadata into Input MetaData Store (include begin Input File Incident)
   loadMetadata();

   m_evtInFile = 0;
   ++m_fileCount;

   // enable sequentialReading if multiple files
   if (m_sequential) {
      bool test = setSequentialRead();
      if (!test) return -1;
   }
   ATH_MSG_INFO("Picked valid file: " << m_reader->fileName());
   // initialize offsets and counters
   m_evtOffsets.push_back((long long)m_reader->getPosition());
   return m_reader->eventsInFile();
}

//__________________________________________________________________________
bool ByteStreamEventStorageInputSvc::readerReady()
{
   bool eofFlag(false);
   if (m_reader!=0) eofFlag = m_reader->endOfFile();
   else {
      ATH_MSG_INFO("eformat reader object not initialized");
      return false;
   }
   bool moreEvent = m_reader->good();

   return (!eofFlag)&&moreEvent;
}
//__________________________________________________________________________
bool ByteStreamEventStorageInputSvc::ROBFragmentCheck()
{
   bool allOK = true;
   uint32_t total = m_re->nchildren(), lastId = 0;
   std::vector<eformat::FragmentProblem> p;
   for (size_t i = 0; i<total; ++i) {
     OFFLINE_FRAGMENTS_NAMESPACE::PointerType fp;
     m_re->child(fp, i);
     OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment f(fp);
     lastId = f.source_id();
     p.clear();
     f.problems(p);
     if (!p.empty()) {
       allOK = false;
       for (std::vector<eformat::FragmentProblem>::const_iterator i = p.begin(), iEnd = p.end();
	          i != iEnd; i++) {
         ATH_MSG_WARNING("Failed to create ROBFragment id = " << lastId << ", " << eformat::helper::SourceIdentifier(lastId).human()  << " : " << eformat::helper::FragmentProblemDictionary.string(*i));
       }
     }
   }
   return allOK;
}
//__________________________________________________________________________
void ByteStreamEventStorageInputSvc::setEvent(void* data, unsigned int eventStatus)
{
   releaseCurrentEvent();
   OFFLINE_FRAGMENTS_NAMESPACE::DataType* fragment = reinterpret_cast<OFFLINE_FRAGMENTS_NAMESPACE::DataType*>(data);
   m_re = new RawEvent(fragment);
   m_eventStatus = eventStatus;
   // Set it for the data provider
   m_robProvider->setNextEvent(m_re);
   m_robProvider->setEventStatus(m_eventStatus);
   // Build a DH for use by other components
   StatusCode rec_sg = generateDataHeader();
   if (rec_sg != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Fail to record BS DataHeader in StoreGate. Skipping events?! " << rec_sg);
   }
}
//__________________________________________________________________________
const RawEvent* ByteStreamEventStorageInputSvc::currentEvent() const {
   return(m_re);
}
//__________________________________________________________________________
unsigned int ByteStreamEventStorageInputSvc::currentEventStatus() const {
   return(m_eventStatus);
}
//________________________________________________________________________________
StatusCode ByteStreamEventStorageInputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (ByteStreamInputSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<ByteStreamInputSvc*>(this);
   } else {
     // Interface is not directly available: try out a base class
     return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
