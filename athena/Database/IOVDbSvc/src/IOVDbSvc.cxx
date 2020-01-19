/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbSvc.cxx
// Re-implementation of Athena IOVDbSvc
// Richard Hawkijngs, started 23/11/08
// based on earlier code by RD Schaffer, Antoine Perus and RH

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreClearedIncident.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Guards.h"
#include "AthenaKernel/IOVRange.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"
#include "PoolSvc/IPoolSvc.h"
#include "FileCatalog/IFileCatalog.h"

#include "EventInfoMgt/ITagInfoMgr.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TagInfo.h"


#include "IOVDbParser.h"
#include "IOVDbFolder.h"
#include "IOVDbSvc.h"

#include <list>

// helper function for getting jobopt properties
namespace {
  template<typename Prop_t>
  StatusCode fetchProp( const std::vector<const Property*>* properties,
                        Prop_t& p )
  {
    if (0!=properties) {
      for ( std::size_t i=0, iEnd=properties->size(); i!=iEnd; ++i ) {
        const Property* ip=(*properties)[i];
        if ( ip->name()==p.name() ) {
          ip->load(p);
          return StatusCode::SUCCESS;
        }
      }
    }
    return StatusCode::FAILURE;
  }
}


IOVDbSvc::IOVDbSvc( const std::string& name, ISvcLocator* svc )
  : AthService( name, svc ),
    m_par_defaultConnection("sqlite://;schema=cooldummy.db;dbname=OFLP200"),
    m_par_globalTag(""),m_par_dbinst(""),
    m_par_manageConnections(true),
    m_par_managePoolConnections(true),
    m_par_dumpkeys(false),
    m_par_forceRunNumber(0),            // default no global force run number
    m_par_forceLumiblockNumber(0),      // Default no global force LB
    m_par_maxNumPoolFiles(5),           // Default=0 means no limit
    m_par_timeStampSlop(0.0),           // default of 0 seconds
    m_par_cacheRun(0),
    m_par_cacheTime(0),
    m_par_cacheAlign(0),
    m_par_onlineMode(false),
    m_par_checklock(false),
    m_h_IOVSvc     ("IOVSvc", name),
    m_h_sgSvc      ("StoreGateSvc", name),
    m_h_detStore   ("DetectorStore", name),
    m_h_metaDataStore ("StoreGateSvc/MetaDataStore", name), 
    m_h_persSvc    ("EventPersistencySvc", name),
    m_h_clidSvc    ("ClassIDSvc", name),
    m_h_poolSvc    ("PoolSvc", name),
    m_h_metaDataTool("IOVDbMetaDataTool"),
    m_h_tagInfoMgr("TagInfoMgr", name),
    m_log(0),
    m_poolPayloadRequested(false),
    m_poolSvcContext(0),
    m_state (INITIALIZATION),
    m_globalTag(""),
    m_iovslop(),
    m_abort(false)
{
  // declare all properties
  declareProperty("dbConnection",          m_par_defaultConnection);
  declareProperty("GlobalTag",             m_par_globalTag );
  declareProperty("DBInstance",            m_par_dbinst);
  declareProperty("Folders",               m_par_folders );
  declareProperty("overrideTags",          m_par_overrideTags);
  declareProperty("forceRunNumber",        m_par_forceRunNumber );
  declareProperty("forceLumiblockNumber",  m_par_forceLumiblockNumber);
  declareProperty("forceTimestamp",        m_par_forceTimestamp);
  declareProperty("ManageConnections",     m_par_manageConnections );
  declareProperty("ManagePoolConnections", m_par_managePoolConnections );
  declareProperty("DumpKeys",              m_par_dumpkeys);
  declareProperty("FoldersToMetaData",     m_par_foldersToWrite );
  declareProperty("MaxPoolFilesOpen",      m_par_maxNumPoolFiles );      // maximum number of pools files allowed to be open at once
  declareProperty("TimeStampSlop",         m_par_timeStampSlop);
  declareProperty("CacheRun",              m_par_cacheRun);
  declareProperty("CacheTime",             m_par_cacheTime);
  declareProperty("CacheAlign",            m_par_cacheAlign);
  declareProperty("OnlineMode",            m_par_onlineMode);
  declareProperty("CheckLock",             m_par_checklock);
}

IOVDbSvc::~IOVDbSvc() {}

const InterfaceID& IOVDbSvc::type() const { 
  return IIOVDbSvc::interfaceID();
}

/// Identify interfaces to which this service is responsive
StatusCode
IOVDbSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IIOVDbSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IIOVDbSvc*)this;
  } else if  ( IIOVCondDbSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IIOVCondDbSvc*)this;
  } else {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::initialize() {
  if (StatusCode::SUCCESS!=AthService::initialize()) return StatusCode::FAILURE;
  // initialise message stream after service init to get correct print level
  m_log=new MsgStream(msgSvc(),name());
  // subscribe to events
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  if (StatusCode::SUCCESS!=incSvc.retrieve()) {
    *m_log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
    return StatusCode::FAILURE;
  }
  long int pri=100;
  incSvc->addListener( this, "BeginEvent", pri );
  incSvc->addListener( this, "StoreCleared", pri );
  incSvc->addListener( this, "EndOfBeginRun", pri );

  // Get context for POOL conditions files, and created an initial connection
  if (m_par_managePoolConnections) {
    m_poolSvcContext=m_h_poolSvc->getInputContext("Conditions",
                                                  m_par_maxNumPoolFiles);
  } else {
    m_poolSvcContext=m_h_poolSvc->getInputContext("Conditions");
  }
  
  if (StatusCode::SUCCESS==m_h_poolSvc->connect(pool::ITransaction::READ,
                                                m_poolSvcContext)) {
    *m_log << MSG::INFO << "Opened read transaction for POOL PersistencySvc"
           << endreq;
  } else {
    // We only emit info for failure to connect (for the moment? RDS 01/2008)
    *m_log << MSG::INFO << "Cannot connect to POOL PersistencySvc" << endreq;
  }

  // print warnings/info depending on state of job options
  if (!m_par_manageConnections)
    *m_log << MSG::INFO << "COOL connection management disabled - connections kept open throughout job" << endreq;
  if (!m_par_managePoolConnections)
    *m_log << MSG::INFO << "POOL file connection management disabled - files kept open throught job" << endreq;
  if (m_par_maxNumPoolFiles.value() > 0)
    *m_log << MSG::INFO << "Only " << m_par_maxNumPoolFiles.value() <<  
      " POOL conditions files will be open at once" << endreq;
  if (m_par_forceRunNumber.value() > 0 || m_par_forceLumiblockNumber.value() > 0)
    *m_log << MSG::WARNING << "Global run/LB number forced to be [" <<
      m_par_forceRunNumber.value() << "," << m_par_forceLumiblockNumber.value()
           <<  "]" << endreq;
  if (m_par_forceTimestamp.value() > 0) 
    *m_log << MSG::WARNING << "Global timestamp forced to be " <<
      m_par_forceTimestamp.value() << endreq;
  if (m_par_cacheRun.value() > 0) 
    *m_log << MSG::INFO << "Run-LB data will be cached in groups of " << 
      m_par_cacheRun.value() << " runs" << endreq;
  if (m_par_cacheTime.value() > 0)
    *m_log << MSG::INFO << "Timestamp data will be cached in groups of "
           << m_par_cacheTime.value() << " seconds" << endreq;
  if (m_par_cacheAlign > 0) 
    *m_log << MSG::INFO << "Cache alignment will be done in " << 
      m_par_cacheAlign.value() << " slices" << endreq;
  if (m_par_onlineMode) 
    *m_log << MSG::INFO << 
      "Online mode ignoring potential missing channels outside cache" << 
      endreq;
  if (m_par_checklock)
    *m_log << MSG::INFO << "Tags will be required to be locked"
           << endreq;

  // make sure iovTime is undefined
  m_iovTime.reset();

  // extract information from EventSelector for run/LB/time overrides
  if (StatusCode::SUCCESS!=checkEventSel()) return StatusCode::FAILURE;

  // initialise default connection
  if (m_par_defaultConnection!="") {
    // default connection is readonly if no : in name (i.e. logical conn)
    bool readonly=(m_par_defaultConnection.find(':')==std::string::npos);
    m_connections.push_back(new IOVDbConn(m_par_defaultConnection,readonly,
                                          m_log));
  }

  // set time of timestampslop in nanoseconds
  m_iovslop=static_cast<cool::ValidityKey>(m_par_timeStampSlop*1.E9);

  // check for global tag in jobopt, which will override anything in input file
  if (m_par_globalTag!="") {
    m_globalTag=m_par_globalTag;
    *m_log << MSG::INFO << "Global tag: " << m_par_globalTag << 
      " set from joboptions" << endreq;
  }

  // setup folders and process tag overrides
  if (StatusCode::SUCCESS!=setupFolders()) return StatusCode::FAILURE;

  // Set state to initialize
  m_state=IOVDbSvc::INITIALIZATION;
  *m_log << MSG::INFO << "Initialised with " << m_connections.size() << 
    " connections and " << m_foldermap.size() << " folders" << endreq;
  *m_log << MSG::INFO << "Service IOVDbSvc initialised successfully" <<
    endreq;
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::finalize() {
  // summarise and delete folders, adding total read from COOL
  unsigned long long nread=0;
  float readtime=0.;
  // accumulate a map of readtime by connection
  typedef std::map<IOVDbConn*,float> CTMap;
  CTMap ctmap;
  for (FolderMap::iterator itr=m_foldermap.begin();itr!=m_foldermap.end();
       ++itr) {
    IOVDbFolder* folder=itr->second;
    folder->summary();
    nread+=folder->bytesRead();
    const float& fread=folder->readTime();
    readtime+=fread;
    IOVDbConn* cptr=folder->conn();
    CTMap::iterator citr=ctmap.find(cptr);
    if (citr!=ctmap.end()) {
      (citr->second)+=fread;
    } else {
      ctmap.insert(CTMap::value_type(cptr,fread));
    }
    delete folder;
  }
  *m_log << MSG::INFO << "Total payload read from COOL: " << nread << 
    " bytes in (( " << std::fixed << std::setw(9) << std::setprecision(2) <<
    readtime << " ))s" << endreq;

  // close and delete connections, printing time in each one
  for (ConnVec::iterator itr=m_connections.begin(); itr!=m_connections.end();
       ++itr) {
    float fread=0;
    CTMap::iterator citr=ctmap.find(*itr);
    if (citr!=ctmap.end()) fread=citr->second;
    (*itr)->setInactive();
    (*itr)->summary(fread);
    delete *itr;
  }
  // finally remove the msg svc
  delete m_log;
  return AthService::finalize();
}

cool::IDatabasePtr IOVDbSvc::getDatabase(bool readOnly) {
  // get default database connection
  cool::IDatabasePtr dbconn;
  if (m_par_defaultConnection=="" || m_connections.size()==0) {
    *m_log << MSG::ERROR << "No default COOL database connection is available"
           << endreq;
    dbconn.reset();
  } else {
    if (m_connections[0]->isReadOnly()!=readOnly) {
      *m_log << MSG::WARNING << 
        "Changing state of default connection to readonly=" << readOnly 
             << endreq;
      m_connections[0]->setReadOnly(readOnly);
    }
    dbconn=m_connections[0]->getCoolDb();
  }
  return dbconn;
}

StatusCode IOVDbSvc::preLoadAddresses(StoreID::type storeID,tadList& tlist) {
  // Read information for folders and setup TADs
  if (storeID!=StoreID::DETECTOR_STORE) return StatusCode::SUCCESS;
  // Preloading of addresses should be done ONLY for detector store
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "preLoadAddress: storeID -> " << storeID << endreq;
  // check FLMD of input, see if any requested folders are available there
  const DataHandle<IOVMetaDataContainer> cont;
  const DataHandle<IOVMetaDataContainer> contEnd;
  if (StatusCode::SUCCESS==m_h_metaDataStore->retrieve(cont,contEnd)) {
    unsigned int ncontainers=0;
    unsigned int nused=0;
    for (;cont!=contEnd; ++cont) {
      ++ncontainers;
      const std::string& fname=cont->folderName();
      // check if this folder is in list requested by IOVDbSvc
      for (FolderMap::const_iterator fitr=m_foldermap.begin();
           fitr!=m_foldermap.end();++fitr) {
        // take data from FLMD only if tag override is NOT set
        if (fitr->second->folderName()==fname && 
            !(fitr->second->tagOverride())) {
          *m_log << MSG::INFO << "Folder " << fname << 
            " will be taken from file metadata" << endreq;
          fitr->second->setMetaCon(cont.cptr());
          // print metadata if in debug mode
          if (m_log->level()<=MSG::DEBUG) printMetaDataContainer(cont.cptr());
          ++nused;
          break;
        }
      }
    }
    *m_log << MSG::INFO << "Found " << ncontainers << 
      " metadata containers in input file, " << nused << " will be used" << 
      endreq;
  } else {
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
                                      "Could not retrieve IOVMetaDataContainer objects from MetaDataStore" << 
                                      endreq;
  }

  // Remove folders which should only be read from file meta data, but
  // were not found in the MetaDataStore

  // Note: we cannot iterate and perform erase within the iteration
  // because the iterator becomes invalid. So first collect the keys
  // to erase in a first pass and then erase them.
  std::vector<std::string> keysToDelete;
  for (FolderMap::iterator fitr = m_foldermap.begin(); fitr != m_foldermap.end(); ++fitr) {
    if (fitr->second->fromMetaDataOnly() && !fitr->second->readMeta()) {
      *m_log << MSG::INFO << "preLoadAddresses: Removing folder " << 
        fitr->second->folderName() << 
        ". It should only be in the file meta data and was not found." 
             << endreq;
      keysToDelete.push_back(fitr->first);
    }
  }
  std::vector<std::string>::const_iterator sit = keysToDelete.begin();
  for (; sit != keysToDelete.end(); ++sit) {
    FolderMap::iterator fitr=m_foldermap.find(*sit);
    if (fitr != m_foldermap.end()) {
      fitr->second->conn()->decUsage();
      delete (fitr->second);
      m_foldermap.erase(fitr);
    }
    else {
      *m_log << MSG::ERROR << "preLoadAddresses: Could not find folder " << *sit 
             << " for removal" << endreq;
    }
  }
  

  // loop over all folders, grouped by connection
  // do metadata folders on first connection (default connection)
  bool doMeta=true;
  // do not close COOL connection until next one has been opened, this enables
  // connection sharing in CORAL, so all COOL connections will use the same
  // CORAL one (althugh they will each be given a separate session)
  IOVDbConn* oldconn=0;
  for (ConnVec::const_iterator citr=m_connections.begin();
       citr!=m_connections.end(); ++citr) {
    if ((*citr)->nFolders()>0 || doMeta) {
      // loop over all folders using this connection
      for (FolderMap::iterator fitr=m_foldermap.begin();
           fitr!=m_foldermap.end();++fitr) {
        IOVDbFolder* folder=fitr->second;
        if (folder->conn()==*citr || (folder->conn()==0 && doMeta)) {
          std::unique_ptr<SG::TransientAddress> tad =
            folder->preLoadFolder(&(*m_h_detStore),m_par_cacheRun.value(),
                                  m_par_cacheTime.value());
          if (oldconn!=*citr) {
            // close old connection if appropriate
            if (m_par_manageConnections && oldconn!=0) oldconn->setInactive();
            oldconn=(*citr);
          }
          if (tad==0) {
            *m_log << MSG::ERROR << "preLoadFolder failed for folder " << folder->folderName() << endreq;
            return StatusCode::FAILURE;
          }
          // for write-metadata folder, request data preload
          if (folder->writeMeta()) {
            if (StatusCode::SUCCESS!=m_h_IOVSvc->preLoadDataTAD(tad.get(),
                                                                folder->eventStore())) {
              *m_log << MSG::ERROR << 
                "Could not request IOVSvc to preload metadata for " << 
                folder->folderName() << endreq;
              return StatusCode::FAILURE;
            }
          } else {
            // for other folders, just preload TAD (not data)
            if (StatusCode::SUCCESS!=m_h_IOVSvc->preLoadTAD(tad.get(),
                                                            folder->eventStore())) {
              *m_log << MSG::ERROR << 
                "Could not request IOVSvc to preload metadata for " << 
                folder->folderName() << endreq;
              return StatusCode::FAILURE;
            }
          }
          // Add TAD to Storegate
          tlist.push_back(tad.release());
          // check for IOV override
          folder->setIOVOverride(m_par_forceRunNumber.value(),
                                 m_par_forceLumiblockNumber.value(),m_par_forceTimestamp.value());
        }
      }
    }
    doMeta=false;
  }
  // close last connection
  if (oldconn!=0 and m_par_manageConnections) oldconn->setInactive();

  // some folder keys may have changed during preloadFolder due to use of
  // <key> specification in folder description string
  // build a new foldermap with the updated keys
  FolderMap newmap;
  for (FolderMap::const_iterator itr=m_foldermap.begin();
       itr!=m_foldermap.end();++itr) {
    newmap[itr->second->key()]=itr->second;
  }
  m_foldermap=newmap;
  // fill global and explicit folder tags into TagInfo
  if (StatusCode::SUCCESS!=fillTagInfo()) 
    *m_log << MSG::ERROR << 
      "Could not fill TagInfo object from preLoadAddresses" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::loadAddresses(StoreID::type /*storeID*/, tadList& /*list*/ ) {
  // this method does nothing
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode IOVDbSvc::updateAddress(StoreID::type storeID, SG::TransientAddress* tad) {
  // Provide TAD and associated range, actually reading the conditions data

  // Read information for folders and setup TADs
  if (storeID!=StoreID::DETECTOR_STORE) return StatusCode::FAILURE;
  Gaudi::Guards::AuditorGuard auditor(std::string("UpdateAddr::")+(tad->name().empty() ? "anonymous" : tad->name()),
                                      auditorSvc(), "preLoadProxy");

  // first check if this key is managed by IOVDbSvc
  // return FAILURE if not - this allows other AddressProviders to be 
  // asked for the TAD
  const std::string& key=tad->name();
  FolderMap::const_iterator fitr=m_foldermap.find(key);
  if (fitr==m_foldermap.end()) {
    if (m_log->level()<=MSG::VERBOSE) 
      *m_log << MSG::VERBOSE << 
        "updateAddress cannot find description for TAD " << key << endreq;
    return StatusCode::FAILURE;
  }
  IOVDbFolder* folder=fitr->second;
  if (folder->clid()!=tad->clID()) {
    if (m_log->level()<=MSG::VERBOSE)
      *m_log << MSG::VERBOSE << "CLID for TAD " << key << " is " << tad->clID()
             << " but expecting " << folder->clid() << endreq;
    
    return StatusCode::FAILURE;
  }

  // IOVDbSvc will satisfy the request, using already found folder
  // now determine the current IOVTime
  if (m_state==IOVDbSvc::INITIALIZATION && !m_iovTime.isValid()) {
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
                                      "updateAddress: in initialisation phase and no iovTime defined" << endreq;
    return::StatusCode::SUCCESS;
  }
  if (m_state==IOVDbSvc::EVENT_LOOP) {
    // determine iovTime from event
    const DataHandle<EventInfo> evt;
    const DataHandle<EventInfo> evtEnd;
    if (StatusCode::SUCCESS==m_h_sgSvc->retrieve(evt,evtEnd)) {
      m_iovTime.setRunEvent(evt->event_ID()->run_number(),
                            evt->event_ID()->lumi_block());
      // save both seconds and ns offset for timestamp
      uint64_t nsTime=evt->event_ID()->time_stamp()*1000000000LL;
      nsTime += evt->event_ID()->time_stamp_ns_offset();
      m_iovTime.setTimestamp(nsTime);
      if (m_log->level()<=MSG::DEBUG)
        *m_log << MSG::DEBUG <<
          "updateAddress - using iovTime from EventInfo: " << 
          m_iovTime << endreq;
    } else {
      // failed to get event info - just return success
      if (m_log->level()<=MSG::DEBUG)
        *m_log << MSG::DEBUG << "Could not get event - initialise phase" 
               << endreq;
      return StatusCode::SUCCESS;
    }
  } else {
    if (m_log->level()<=MSG::DEBUG)
      *m_log << MSG::DEBUG << 
        "updateAddress: using iovTime from init/beginRun: " << m_iovTime 
             << endreq;
  }

  // check consistency of global tag and database instance, if set
  // catch most common user misconfigurations
  // this is only done here as need global tag to be set even if read from file
  if (!m_par_dbinst.empty() && !m_globalTag.empty()) {
    const std::string tagstub=m_globalTag.substr(0,7);
    if (m_log->level()<=MSG::DEBUG)
      *m_log << MSG::DEBUG << "Checking " << m_par_dbinst << " against " <<
        tagstub << endreq;
    if (((m_par_dbinst=="COMP200" || m_par_dbinst=="CONDBR2") && 
         (tagstub!="COMCOND" && tagstub!="CONDBR2")) ||
        (m_par_dbinst=="OFLP200" && (tagstub!="OFLCOND" && tagstub!="CMCCOND"))) {
      *m_log << MSG::FATAL << "Likely incorrect conditions DB configuration! " 
             <<  "Attached to database instance " << m_par_dbinst <<
        " but global tag begins " << tagstub << endreq;
      *m_log << MSG::FATAL << "See Atlas/CoolTroubles wiki for details," << 
        " or set IOVDbSvc.DBInstance=\"\" to disable check" << endreq;
      return StatusCode::FAILURE;
    }
  }


  // obtain the validity key for this folder (includes overrides)
  cool::ValidityKey vkey=folder->iovTime(m_iovTime);
  if (!folder->readMeta() && !folder->cacheValid(vkey)) {
    // mark this folder as not-dropped so cache-read will succeed
    folder->setDropped(false);
    // reload cache for this folder (and all others sharing this DB connection)
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Triggering cache load for folder " << 
        folder->folderName() << endreq;
    if (StatusCode::SUCCESS!=loadCaches(folder->conn())) {
      *m_log << MSG::ERROR << "Cache load failed for folder " <<  folder->folderName() << endreq;
      return StatusCode::FAILURE;
    }
  }

  // data should now be in cache
  IOpaqueAddress* address=0;
  IOVRange range;
  // setup address and range
  {
    Gaudi::Guards::AuditorGuard auditor(std::string("FldrSetup:")+(tad->name().empty() ? "anonymous" : tad->name()),
                                        auditorSvc(), "preLoadProxy");
    if (!folder->getAddress(vkey,&(*m_h_persSvc),m_poolSvcContext,address,
                            range,m_poolPayloadRequested)) {
      *m_log << MSG::ERROR << "getAddress failed for folder " << 
        folder->folderName() << endreq;
      return StatusCode::FAILURE;
    }
  }
  // reduce minimum IOV of timestamp folders to avoid 'thrashing' 
  // due to events slightly out of order in HLT
  if (folder->timeStamp()) {
    cool::ValidityKey start=range.start().timestamp();
    if (start>m_iovslop) start-=m_iovslop;
    range=IOVRange(IOVTime(start),range.stop());
  }

  // Pass range onto IOVSvc
  if (StatusCode::SUCCESS!=m_h_IOVSvc->setRange(tad->clID(),tad->name(),
                                                range,folder->eventStore())) {
    *m_log << MSG::ERROR << "setRange failed for folder " << 
      folder->folderName() << endreq;
    return StatusCode::FAILURE;
  }
  tad->setAddress(address);
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode IOVDbSvc::getRange( const CLID&        clid, 
                               const std::string& dbKey,
                               const IOVTime&     time,
                               IOVRange&          range,
                               std::string&       tag,
                               IOpaqueAddress*&   address) {

  *m_log << MSG::DEBUG << "getRange  clid: " << clid << " key: \""
         << dbKey << "\"  t: " << time << endmsg;

  const std::string& key=dbKey;
  FolderMap::const_iterator fitr=m_foldermap.find(key);
  if (fitr==m_foldermap.end()) {
    if (m_log->level()<=MSG::VERBOSE) 
      *m_log << MSG::VERBOSE << 
        "getRange cannot find description for dbKey " << key << endreq;
    return StatusCode::FAILURE;
  }
  IOVDbFolder* folder=fitr->second;
  if (folder->clid()!=clid) {
    if (m_log->level()<=MSG::VERBOSE)
      *m_log << MSG::VERBOSE << "supplied CLID for " << key << " is " 
             << clid
             << " but expecting " << folder->clid() << endreq;
    
    return StatusCode::FAILURE;
  }

  // *m_log << MSG::INFO << "folderName: " << folder->folderName()
  //        << " key: " << folder->key() 
  //        << " resolvedTag: " << folder->resolvedTag()
  //        << endmsg;

  /// FIXME?
  tag = folder->key();


  // check consistency of global tag and database instance, if set
  // catch most common user misconfigurations
  // this is only done here as need global tag to be set even if read from file
  if (!m_par_dbinst.empty() && !m_globalTag.empty()) {
    const std::string tagstub=m_globalTag.substr(0,7);
    if (m_log->level()<=MSG::DEBUG)
      *m_log << MSG::DEBUG << "Checking " << m_par_dbinst << " against " <<
        tagstub << endreq;
    if (((m_par_dbinst=="COMP200" || m_par_dbinst=="CONDBR2") && 
         (tagstub!="COMCOND" && tagstub!="CONDBR2")) ||
        (m_par_dbinst=="OFLP200" && (tagstub!="OFLCOND" && tagstub!="CMCCOND"))) {
      *m_log << MSG::FATAL << "Likely incorrect conditions DB configuration! " 
             <<  "Attached to database instance " << m_par_dbinst <<
        " but global tag begins " << tagstub << endreq;
      *m_log << MSG::FATAL << "See Atlas/CoolTroubles wiki for details," << 
        " or set IOVDbSvc.DBInstance=\"\" to disable check" << endreq;
      return StatusCode::FAILURE;
    }
  }


  // obtain the validity key for this folder (includes overrides)
  cool::ValidityKey vkey=folder->iovTime(time);
  if (!folder->readMeta() && !folder->cacheValid(vkey)) {
    // mark this folder as not-dropped so cache-read will succeed
    folder->setDropped(false);
    // reload cache for this folder (and all others sharing this DB connection)
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Triggering cache load for folder " << 
        folder->folderName() << endreq;
    if (StatusCode::SUCCESS!=loadCaches(folder->conn())) {
      *m_log << MSG::ERROR << "Cache load failed for folder " <<  folder->folderName() << endreq;
      return StatusCode::FAILURE;
    }
  }

  // data should now be in cache
  //  IOpaqueAddress* address=0;
  address=0;
  // setup address and range
  {
    Gaudi::Guards::AuditorGuard auditor(std::string("FldrSetup:")+(key.empty() ? "anonymous" : key),
                                        auditorSvc(), "preLoadProxy");
    if (!folder->getAddress(vkey,&(*m_h_persSvc),m_poolSvcContext,address,
                            range,m_poolPayloadRequested)) {
      *m_log << MSG::ERROR << "getAddress failed for folder " << 
        folder->folderName() << endreq;
      return StatusCode::FAILURE;
    }
  }

  // // reduce minimum IOV of timestamp folders to avoid 'thrashing' 
  // // due to events slightly out of order in HLT
  // if (folder->timeStamp()) {
  //   cool::ValidityKey start=range.start().timestamp();
  //   if (start>m_iovslop) start-=m_iovslop;
  //   range=IOVRange(IOVTime(start),range.stop());
  // }




  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode IOVDbSvc::setRange( const CLID&        /*clid*/,
                               const std::string& /*dbKey*/,
                               const IOVRange&    /*range*/,
                               const std::string& /*storeName*/ ) {
  // this method does nothing
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::signalBeginRun(const IOVTime& beginRunTime) {
  // Begin run - set state and save time for later use
  m_state=IOVDbSvc::BEGIN_RUN;
  m_iovTime=beginRunTime;
  if (m_log->level()<<MSG::DEBUG) 
    *m_log << MSG::DEBUG << "signalBeginRun> begin run time " << m_iovTime 
           << endreq;
  if(!m_par_onlineMode) return StatusCode::SUCCESS;
  static int first=0;
  if (!first) {
    first=1; 
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "first call SKIPPING ... " << endreq;
    return StatusCode::SUCCESS;
  }
  // all other stuff is event based so happens after this. 
  // this is before first event of each run
  if (m_log->level()<=MSG::DEBUG)  {
    *m_log << MSG::DEBUG << "In online mode will recheck ... " << endreq;
    *m_log << MSG::DEBUG << "First reload PoolCataloge ... " << endreq;
  }
  pool::IFileCatalog* catalog=
    const_cast<pool::IFileCatalog*>(m_h_poolSvc->catalog());
  catalog->commit();
  catalog->start(); 

  std::vector<IOVDbConn*>::iterator dbi=m_connections.begin();
  for (;dbi!=m_connections.end();++dbi){
    // only access connections which are actually in use - avoids waking up
    // the default DB connection if it is not being used
    if ((*dbi)->nFolders()>0) {
      //request for database activates connection
      cool::IDatabasePtr dbconn=(*dbi)->getCoolDb();
      if (dbconn.get()==0) {
        *m_log << MSG::FATAL << "Conditions database connection " <<  (*dbi)->name() << " cannot be opened - STOP" << endreq;
        return StatusCode::FAILURE;
      }
      for (FolderMap::iterator fitr=m_foldermap.begin();fitr!=m_foldermap.end();++fitr) { 
        IOVDbFolder* folder=fitr->second;
        if (folder->conn()!=(*dbi)) continue; 
        folder->printCache();
        cool::ValidityKey vkey=folder->iovTime(m_iovTime);
        {
          Gaudi::Guards::AuditorGuard auditor(std::string("FldrCache:")+folder->folderName(), auditorSvc(), "preLoadProxy");
          if (!folder->loadCacheIfDbChanged(vkey, m_globalTag, dbconn, m_h_IOVSvc)) {
            *m_log << MSG::ERROR << "Problem RELOADING: " << folder->folderName()
                   << endreq;
            return StatusCode::FAILURE;
          }
        }
        folder->printCache();
      }
    }
    if (m_par_manageConnections) (*dbi)->setInactive();
  }
  return StatusCode::SUCCESS;
}

void IOVDbSvc::signalEndProxyPreload() {
  // this method does nothing
}

void IOVDbSvc::handle( const Incident& inc) {
  // Handle incidents:
  // BeginEvent to set IOVDbSvc state to EVENT_LOOP
  // StoreCleared or EndOfBeginRun to close any open POOL files
  if (m_log->level() <= MSG::VERBOSE)
    *m_log << MSG::VERBOSE << "entering handle(), incident type " << inc.type()
           << " from " << inc.source() << endreq;
  if (inc.type()=="BeginEvent") {
    m_state=IOVDbSvc::EVENT_LOOP;
  } else {
    const StoreClearedIncident* sinc= 
      dynamic_cast<const StoreClearedIncident*>(&inc);
    if ((inc.type()=="StoreCleared" && sinc!=0 && sinc->store()==&*m_h_sgSvc)
        || inc.type()=="EndOfBeginRun") {
      if (inc.type()=="StoreCleared") {
        m_state=IOVDbSvc::FINALIZE_ALG;
        if (m_par_dumpkeys) {
          dumpKeys();
          m_par_dumpkeys=false;
        }
      }
      if (m_par_managePoolConnections && m_poolPayloadRequested) {
        // reset POOL connection to close all open conditions POOL files
        m_poolPayloadRequested=false;
        if (StatusCode::SUCCESS==m_h_poolSvc->disconnect(m_poolSvcContext)) {
          if (m_log->level()<=MSG::DEBUG) 
            *m_log << MSG::DEBUG << "Successfully closed input POOL connections"
                   << endreq;
        } else {
          *m_log << MSG::WARNING << "Unable to close input POOL connections" << 
            endreq;
        }
        // reopen transaction
        if (StatusCode::SUCCESS==m_h_poolSvc->connect(pool::ITransaction::READ,
                                                      m_poolSvcContext)) {
          if (m_log->level()<=MSG::DEBUG)                   *m_log << MSG::DEBUG << 
                                                              "Reopend read transaction for POOL conditions input files" << endreq;
        } else {
          *m_log << MSG::WARNING << 
            "Cannot reopen read transaction for POOL conditions input files" 
                 << endreq;
        }
      }
    }
  }
}

StatusCode IOVDbSvc::registerTagInfoCallback() {
  // register callback for taginfo handling
  // for the moment, this calls processTagInfo directly, rather than going
  // via a call back (following RDS 08/2006)
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "registerTagInfoCallback called" << endreq;
  std::list<std::string> alist;
  int a=0;
  if (StatusCode::SUCCESS!=processTagInfo(a,alist)) {
    *m_log << MSG::ERROR << "Cannot process TagInfo" << endreq;
    return StatusCode::FAILURE;
  } else {
    return StatusCode::SUCCESS;
  }
}

StatusCode IOVDbSvc::processTagInfo(IOVSVC_CALLBACK_ARGS) {
  // Processing of taginfo callback
  // Set GlobalTag and any folder-specific overrides if given
  const TagInfo* tagInfo=0;
  if (StatusCode::SUCCESS!=m_h_detStore->retrieve(tagInfo)) {
    *m_log << MSG::ERROR << "No TagInfo in DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  // dump out contents of TagInfo
  if (m_log->level()<=MSG::DEBUG) {
    *m_log << MSG::DEBUG << "Tags from input TagInfo:" << endreq;
    tagInfo->printTags(*m_log);
  }
  // check IOVDbSvc GlobalTag, if not already set
  if (m_globalTag=="") {
    tagInfo->findTag("IOVDbGlobalTag",m_globalTag);
    if (m_globalTag!="")
      if (m_log->level()<=MSG::INFO) 
        *m_log << MSG::INFO << "Global tag: " << m_globalTag
               << " set from input file" << endreq;
  }

  // now check for tag overrides for specific folders
  TagInfo::NameTagPairVec tags;
  tagInfo->getInputTags(tags);
  for (TagInfo::NameTagPairVec::const_iterator itr=tags.begin();
       itr!=tags.end();++itr) {
    // assume tags relating to conditions folders start with /
    if ((itr->first).substr(0,1)!="/") continue;
    // check for folder(s) with this name
    for (FolderMap::iterator fitr=m_foldermap.begin();fitr!=m_foldermap.end();
         ++fitr) {
      IOVDbFolder* folder=fitr->second;
      const std::string& ifname=folder->folderName();
      if (ifname!=itr->first) continue; 
      // use an override from TagInfo only if there is not an explicit jo tag,
      // and folder meta-data is not used, and there is no <noover/> spec,
      // and no global tag set in job options
      if (folder->joTag()=="" && !folder->readMeta() && 
          !folder->noOverride() && m_par_globalTag=="") {
        folder->setTagOverride(itr->second,false);
        if (m_log->level()<=MSG::INFO) 
          *m_log << MSG::INFO << "TagInfo override for tag " 
                 << itr->second << " in folder " << ifname << endreq;
      } else if (folder->joTag()!=itr->second) {
        if (m_log->level()<=MSG::INFO) {
          *m_log << MSG::INFO << "Ignoring inputfile TagInfo request for tag "
                 << itr->second << " in folder " << ifname;
          if (folder->joTag()=="") {
            *m_log << " in favour of hierarchical tag" << endreq;
          } else {
            *m_log <<
              " in favour of jobOption tag " << folder->joTag() << endreq;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

std::vector<std::string> IOVDbSvc::getKeyList() {
  // return a list of all the StoreGate keys being managed by IOVDbSvc
  std::vector<std::string> keys;
  keys.reserve(m_foldermap.size());
  for (FolderMap::iterator fitr=m_foldermap.begin();fitr!=m_foldermap.end();
       ++fitr) {
    keys.push_back(fitr->first);
  }
  return keys;
}

bool IOVDbSvc::getKeyInfo(const std::string& key,std::string& foldername,
                          std::string& tag, IOVRange& range, bool& retrieved,
                          unsigned long long& bytesRead, float& readTime) {
  // return information about given SG key
  // first attempt to find the folder object for this key
  FolderMap::const_iterator itr=m_foldermap.find(key);
  if (itr!=m_foldermap.end()) {
    IOVDbFolder* folder=itr->second;
    foldername=folder->folderName();
    tag=folder->resolvedTag();
    range=folder->currentRange();
    retrieved=folder->retrieved();
    bytesRead=folder->bytesRead();
    readTime=folder->readTime();
    return true;
  } else {
    retrieved=false;
    return false;
  }
}

bool IOVDbSvc::dropObject(const std::string& key, const bool resetCache) {
  // find the folder corresponding to this object
  FolderMap::const_iterator itr=m_foldermap.find(key);
  if (itr!=m_foldermap.end()) {
    IOVDbFolder* folder=itr->second;
    CLID clid=folder->clid();
    SG::DataProxy* proxy=m_h_detStore->proxy(clid,key);
    if (proxy!=0) {
      m_h_detStore->clearProxyPayload(proxy);
      if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
                                        "Dropped payload for key " << key << endreq;
      folder->setDropped(true);
      if (resetCache) {
        folder->resetCache();
        if (m_log->level()<=MSG::DEBUG) 
          *m_log << MSG::DEBUG << "Cache reset done for folder " << 
            folder->folderName() << endreq;
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


/************************/
// private methods of IOVDbSvc

StatusCode IOVDbSvc::checkEventSel() {
  // check if EventSelector is being used to override run numbers
  // if so, we can set IOV time already to allow conditons retrieval
  // in the initialise phase, needed for setting up simulation

  // access jobOptionSvc and get properties of EventSelector
  ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc",name());
  if (!joSvc.retrieve().isSuccess()) {
    *m_log << MSG::FATAL << "Could not retrieve [" << joSvc.typeAndName() 
           << "]" << endreq;
    return StatusCode::FAILURE;
  }
  typedef std::vector<const Property*> Properties_t;
  const Properties_t* evtSelProps=joSvc->getProperties("EventSelector");
  // do not return FAILURE if the EventSelector cannot be found, as this
  // happens online when we have no EventSelector
  if (0==evtSelProps) {
    if (m_log->level()<=MSG::DEBUG)     
      *m_log << MSG::DEBUG
             << "Could not retrieve properties of 'EventSelector' from ["
             << joSvc.typeAndName() << "]" << endreq;
    return StatusCode::SUCCESS;
  }
  BooleanProperty bprop("OverrideRunNumber",false);
  if (StatusCode::SUCCESS==::fetchProp<BooleanProperty>(evtSelProps,bprop)) {
    if (bprop.value()) {
      // if flag is set, extract Run,LB and time
      *m_log << MSG::INFO << 
        "Setting run/LB/time from EventSelector override in initialize" << endreq;
      uint32_t run,lumib;
      uint64_t time;
      bool allGood=true;
      if (m_par_forceRunNumber.value()!=0 || 
          m_par_forceLumiblockNumber.value()!=0)
        *m_log << MSG::WARNING << "forceRunNumber property also set" << endreq;
      IntegerProperty iprop1("RunNumber",0);
      if (StatusCode::SUCCESS==
          ::fetchProp<IntegerProperty>(evtSelProps,iprop1)) {
        run=iprop1.value();
      } else {
        *m_log << MSG::ERROR << "Unable to get RunNumber from EventSelector"
               << endreq;
        allGood=false;
      }
      IntegerProperty iprop2("FirstLB",0);
      if (StatusCode::SUCCESS==
          ::fetchProp<IntegerProperty>(evtSelProps,iprop2)) {
        lumib=iprop2.value();
      } else {
        *m_log << MSG::ERROR << "Unable to get FirstLB from EventSelector"
               << endreq;
        allGood=false;
      }
      IntegerProperty iprop3("InitialTimeStamp",0);
      if (StatusCode::SUCCESS==
          ::fetchProp<IntegerProperty>(evtSelProps,iprop3)) {
        time=iprop3.value();
      } else {
        *m_log << MSG::ERROR << 
          "Unable to get InitialTimeStamp from EventSelector" << endreq;
        allGood=false;
      }
      if (allGood) {
        m_iovTime.setRunEvent(run,lumib);
        uint64_t nsTime=time*1000000000LL;
        m_iovTime.setTimestamp(nsTime);
        *m_log << MSG::INFO << "run/LB/time set to [" << run << "," << lumib
               << " : " << nsTime << "]" << endreq;
      } else {
        *m_log << MSG::ERROR << "run/LB/Time NOT changed" << endreq;
      }
    }
  } else {
    // this is not treated as an error if EventSelector has no override prop
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << 
        "Unable to get OverrideRunNumber flag from EventSelector" << endreq;

  }
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::setupFolders() {
  // read the Folders joboptions and setup the folder list
  // no wildcards are allowed

  //1. Loop through folders
  std::list<IOVDbParser> allFolderdata;
  for (std::vector<std::string>::const_iterator itr=m_par_folders.begin();
       itr!=m_par_folders.end();++itr) {
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << "Setup folder " << *itr << endreq;
    IOVDbParser folderdata(*itr,m_log);
    if (!folderdata.isValid()) return StatusCode::FAILURE;
    allFolderdata.push_back(folderdata);
  }

  //2. Loop through overwrites:
  // syntax for entries is <prefix>folderpath</prefix> <tag>value</tag>
  // folderpath matches from left of folderName
  // but if partial match, next character must be / so override for /Fred/Ji
  // matches /Fred/Ji/A and /Fred/Ji but not /Fred/Jim

  for (std::vector<std::string>::const_iterator titr=m_par_overrideTags.begin();
       titr!=m_par_overrideTags.end();++titr) {
    IOVDbParser keys(*titr,m_log);
    std::string prefix;
    if (!keys.getKey("prefix","",prefix)) { // || !keys.getKey("tag","",tag)) {
      *m_log << MSG::ERROR << "Problem in overrideTag specification " << 
        *titr << endreq;
      return StatusCode::FAILURE;
    }

    for (auto& folderdata : allFolderdata) {
      const std::string& ifname=folderdata.folderName();
      if (ifname.substr(0,prefix.size())==prefix && 
          (ifname.size()==prefix.size() || 
           ifname.substr(prefix.size(),1)=="/")) {
        //Match! 
        folderdata.applyOverrides(keys,m_log);
      }// end if
    }// end loop over allFolderdata
  }// end loop over overrides

  //3. Remove any duplicates:
  std::list<IOVDbParser>::iterator it1=allFolderdata.begin(); 
  std::list<IOVDbParser>::iterator it_e=allFolderdata.end(); 
  for (;it1!=it_e;++it1) {
    const IOVDbParser& folder1=*it1;
    std::list<IOVDbParser>::iterator it2=it1;
    ++it2;
    while(it2!=it_e) {
      const IOVDbParser& folder2=*it2;
      if (folder1==folder2) {
        it2=allFolderdata.erase(it2); //FIXME: Smarter distinction/reporting about same folder but different keys.
        *m_log << MSG::DEBUG << "Removing duplicate folder " << folder1.folderName() << endreq;
      } 
      else {
        ++it2;
        //Catch suspicous cases:
        if (folder1.folderName()==folder2.folderName()) {
          *m_log << MSG::WARNING << "Folder name appears twice: " << folder1.folderName() << endreq;
          *m_log << MSG::WARNING << folder1 << " vs " << folder2 << endreq;
        }
      }
    }//end inner loop
  }//end outer loop

  //4.Set up folder map with cleaned folder list

  for (const auto& folderdata : allFolderdata) {
    // find the connection specification first - db or dbConnection
    // default is to use the 'default' connection
    IOVDbConn* conn=0;
    std::string connstr;
    if (folderdata.getKey("db","",connstr) || 
        folderdata.getKey("dbConnection","",connstr)) {
      // an explicit database name is specified
      // check if it is already present in the existing connections
      for (ConnVec::const_iterator citr=m_connections.begin();
           citr!=m_connections.end();++citr) {
        if ((*citr)->name()==connstr) {
          // found existing connection - use that
          conn=*citr;
          break;
        }
      }
      if (conn==0) {
        // create new read-onlyconnection
        conn=new IOVDbConn(connstr,true,m_log);
        m_connections.push_back(conn);
      }
    } else {
      // no connection specified - use default if available
      if (m_par_defaultConnection!="") {
        conn=m_connections[0];
      } else {
        *m_log << MSG::FATAL << "Folder request " << folderdata.folderName() << 
          " gives no DB connection information and no default set" << endreq;
        return StatusCode::FAILURE;
      }
    }
    // create the new folder, but only if a folder for this SG key has not
    // already been requested
    IOVDbFolder* folder=new IOVDbFolder(conn,folderdata,m_log,&(*m_h_clidSvc),
                                        m_par_checklock);
    const std::string& key=folder->key();
    if (m_foldermap.find(key)==m_foldermap.end()) {  //This check is too weak. For POOL-based folders, the SG key is in the folder description (not known at this point).
      m_foldermap[key]=folder;
      conn->incUsage();
    } else {
      *m_log << MSG::ERROR << "Duplicate request for folder " << 
        folder->folderName() << 
        " associated to already requested Storegate key " << key << endreq;
      // clean up this duplicate request
      delete folder;
    }
  }// end loop over folders
  // check for folders to be written to metadata
  for (std::vector<std::string>::const_iterator 
         titr=m_par_foldersToWrite.begin();titr!=m_par_foldersToWrite.end();++titr) {
    // match wildcard * at end of string only (i.e. /A/* matches /A/B, /A/C/D)
    std::string match=*titr;
    std::string::size_type idx=titr->find("*");
    if (idx!=std::string::npos) {
      match=titr->substr(0,idx);
    }
    for (FolderMap::iterator fitr=m_foldermap.begin();fitr!=m_foldermap.end();
         ++fitr) {
      IOVDbFolder* fptr=fitr->second;
      if ((fptr->folderName()).substr(0,match.size())==match) {
        fptr->setWriteMeta(&(*m_h_metaDataTool));
        *m_log << MSG::INFO << "Folder " << fptr->folderName() << 
          " will be written to file metadata" << endreq;
      }
    }//end loop over FolderMap
  }//end loop over  m_par_foldersToWrite
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::fillTagInfo() {
  if (m_par_globalTag!="") {
    if (m_log->level()<=MSG::DEBUG)     
      *m_log << MSG::DEBUG << "Adding GlobalTag " << m_par_globalTag << 
        " into TagInfo" << endreq;
    if (StatusCode::SUCCESS!=m_h_tagInfoMgr->addTag("IOVDbGlobalTag",
                                                    m_par_globalTag))
      return StatusCode::FAILURE;
  }
  // add all explicit tags specified in folders
  // can be from Folders or tagOverrides properties
  for (FolderMap::const_iterator fitr=m_foldermap.begin();
       fitr!=m_foldermap.end();++fitr) {
    const IOVDbFolder* folder=fitr->second;
    if (folder->joTag()!="") {
      if (m_log->level()<=MSG::DEBUG) 
        *m_log << MSG::DEBUG << "Adding folder " << folder->folderName() <<
          " tag " << folder->joTag() << " into TagInfo" << endreq;
      if (StatusCode::SUCCESS!=m_h_tagInfoMgr->addTag(folder->folderName(),
                                                      folder->joTag()))
        return StatusCode::FAILURE;
    }
    // check to see if any input TagInfo folder overrides should be removed
    // this anticipates the decisions which will be made in processTagInfo
    // Here we do not have access to the TagInfo object, but can put remove
    // requests in for all folders if the global tag is set, or if there is
    // an explict joboption tag, nooverride spec, or data comes from metadata
    if (m_par_globalTag!="" || folder->joTag()!="" || folder->noOverride() ||
        folder->readMeta()) {
      if (StatusCode::SUCCESS!=
          m_h_tagInfoMgr->removeTagFromInput(folder->folderName())) {
        *m_log << MSG::WARNING << "Could not add TagInfo remove request for "
               << folder->folderName() << endreq;
      } else {
        *m_log << MSG::INFO << "Added taginfo remove for " << 
          folder->folderName() << endreq;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::loadCaches(IOVDbConn* conn) {
  // load the caches for all folders using the given connection
  // so connection use is optimised

  Gaudi::Guards::AuditorGuard auditor(std::string("loadCachesOverhead:")+conn->name(), auditorSvc(), "preLoadProxy");

  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "loadCaches: Begin for connection " << conn->name() 
           << endreq;
  // if global abort already set, load nothing
  if (m_abort) return StatusCode::FAILURE;
  bool access=false;
  StatusCode sc=StatusCode::SUCCESS;
  for (FolderMap::iterator fitr=m_foldermap.begin();fitr!=m_foldermap.end();
       ++fitr) {
    IOVDbFolder* folder=fitr->second;
    if (folder->conn()!=conn) continue;
    cool::ValidityKey vkey=folder->iovTime(m_iovTime);
    // protect against out of range times (timestamp -1 happened in FDR2)
    if (vkey>cool::ValidityKeyMax) {
      *m_log << MSG::WARNING << "Requested validity key " << vkey << 
        " is out of range, reset to 0" << endreq;
      vkey=0;
    }
    if (!folder->cacheValid(vkey) && !folder->dropped()) {
      access=true;
      {
        Gaudi::Guards::AuditorGuard auditor(std::string("FldrCache:")+folder->folderName(), auditorSvc(), "preLoadProxy");
        if (!folder->loadCache(vkey,m_par_cacheAlign,m_globalTag,m_par_onlineMode)) {
          *m_log << MSG::ERROR << "Cache load (prefetch) failed for folder " << 
            folder->folderName() << endreq;
          // remember the failure, but also load other folders on this connection
          // while it is open
          sc=StatusCode::FAILURE;
        }
      }
    }
  }
  // disconnect from database if we connected
  if (access && m_par_manageConnections) conn->setInactive();
  // if connection aborted, set overall abort so we do not waste time trying
  // to read data from other schema
  if (conn->aborted()) {
    *m_log << MSG::FATAL << "Connection " << conn->name() << 
      " was aborted, set global abort" << endreq;
    m_abort=true;
    *m_log << MSG::FATAL << "loadCache: impossible to load cache!" << endreq; 
    throw std::exception();
  }
  return sc;
}

void IOVDbSvc::printMetaDataContainer(const IOVMetaDataContainer* cont) {
  if (m_log->level()>MSG::DEBUG)
    return;
  // Print out the contents of a meta data container (in debug mode)
  *m_log << MSG::DEBUG << "printMetaDataContainer " << endreq;
  *m_log << MSG::DEBUG << "Folder name " << cont->folderName() << endreq;
  *m_log << MSG::DEBUG << "Description " << cont->folderDescription() << endreq;
  // Print out contents of payload
  const IOVPayloadContainer*  payload=cont->payloadContainer();
  // print out iovs and attribute lists
  *m_log << MSG::DEBUG << "payload size " << payload->size() << endreq;
  *m_log << MSG::DEBUG << "IOVs and attribute lists: " << endreq;
  IOVPayloadContainer::const_iterator itAttList=payload->begin();
  IOVPayloadContainer::const_iterator itAttListEnd=payload->end();
  for (;itAttList!=itAttListEnd;++itAttList) {
    *m_log << MSG::DEBUG << (*itAttList)->minRange() << " iov size " 
           << (*itAttList)->iov_size() << endreq;
    CondAttrListCollection::iov_const_iterator itIOV=(*itAttList)->iov_begin();
    CondAttrListCollection::iov_const_iterator itIOVEnd=(*itAttList)->iov_end();
    for (;itIOV!=itIOVEnd; ++itIOV) 
      *m_log << MSG::DEBUG << (*itIOV).first << " " << (*itIOV).second << 
        endreq;
    CondAttrListCollection::const_iterator itAtt=(*itAttList)->begin();
    CondAttrListCollection::const_iterator itAttEnd=(*itAttList)->end();
    for (;itAtt!=itAttEnd;++itAtt) {
      std::ostringstream attrStr;
      attrStr << "{";
      for (coral::AttributeList::const_iterator itr=(*itAtt).second.begin();
           itr!=(*itAtt).second.end();++itr) {
        if (itr!=(*itAtt).second.begin()) attrStr << ",";
        itr->toOutputStream(attrStr);
      }
      attrStr << "}";
      *m_log << MSG::DEBUG << (*itAtt).first << " " << attrStr.str() << endreq;
    }
  }
}

void IOVDbSvc::dumpKeys() {
  // use the getKeyList and getKeyInfo methods to dump all keys in event
  *m_log << MSG::INFO << "Dump IOVDbSvc-managed SG keys for first event" 
         << endreq;
  std::vector<std::string> keys=getKeyList();
  *m_log << MSG::INFO << "Total of " << keys.size() << " keys to list" << 
    endreq;
  for (std::vector<std::string>::const_iterator kitr=keys.begin();
       kitr!=keys.end();++kitr) {
    std::string foldername,tag;
    IOVRange range;
    bool retrieved;
    unsigned long long nread;
    float rtime;
    if (getKeyInfo(*kitr,foldername,tag,range,retrieved,nread,rtime)) {
      if (retrieved) {
        *m_log << MSG::INFO << "Data for key " << *kitr << " : foldername " <<
          foldername << ", tag" << tag << ", range " << range << 
          " read " << nread << " bytes in " << rtime << " seconds" << endreq;
      } else {
        *m_log << MSG::INFO << "Key " << *kitr << 
          " was not yet retrieved from StoreGate" << endreq;
      }
    } else {
      *m_log << MSG::ERROR << "No data for key " << *kitr << endreq;
    }
  }
}
