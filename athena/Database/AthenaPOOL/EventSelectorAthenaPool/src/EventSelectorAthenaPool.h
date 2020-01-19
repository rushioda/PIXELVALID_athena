/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSELECTORATHENAPOOL_H
#define EVENTSELECTORATHENAPOOL_H

/** @file EventSelectorAthenaPool.h
 *  @brief This file contains the class definition for the EventSelectorAthenaPool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PersistentDataModel/Guid.h"

#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "AthenaKernel/ICollectionSize.h"
#include "AthenaBaseComps/AthService.h"
#ifdef ATHENAHIVE
 #include "tbb/recursive_mutex.h"
#endif

#include <map>

// Forward declarations
namespace pool {
   class ICollectionCursor;
}
class IChronoStatSvc;
class IClassIDSvc;
class IIncidentSvc;
class IOpaqueAddress;
class ISvcLocator;
class EventContextAthenaPool;
class PoolCollectionConverter;
class IAthenaIPCTool;
class ActiveStoreSvc;
class StoreGateSvc;
class IAthenaPoolCnvSvc;

/** @class EventSelectorAthenaPool
 *  @brief This class is the EventSelector for event data.
 **/
class EventSelectorAthenaPool :
                public ::AthService,
	virtual public IEvtSelector,
	virtual public IEventSeek,
	virtual public IEventShare,
	virtual public ICollectionSize,
	virtual public IIoComponent {

public: // Constructor and Destructor
   /// Standard Service Constructor
   EventSelectorAthenaPool(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~EventSelectorAthenaPool();

   /// Required of all Gaudi Services
   virtual StatusCode initialize();
   virtual StatusCode start();
   virtual StatusCode stop();
   virtual StatusCode finalize();
   /// Does this object satisfy a given interface?  See Gaudi documentation for details.
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   virtual StatusCode createContext(IEvtSelector::Context*& ctxt) const;

   /// @param ctxt [IN/OUT] current event context is interated to next event.
   virtual StatusCode next(IEvtSelector::Context& ctxt) const;
   /// @param ctxt [IN/OUT] current event context is interated to next event.
   /// @param jump [IN] number of events to jump (currently not supported).
   virtual StatusCode next(IEvtSelector::Context& ctxt, int jump) const;

   /// @param ctxt [IN/OUT] current event context is interated to previous event.
   virtual StatusCode previous(IEvtSelector::Context& ctxt) const;
   /// @param ctxt [IN/OUT] current event context is interated to previous event.
   /// @param jump [IN] number of events to jump (currently not supported).
   virtual StatusCode previous(IEvtSelector::Context& ctxt, int jump) const;

   /// @param ctxt [IN/OUT] current event context is interated to last event.
   virtual StatusCode last(IEvtSelector::Context& ctxt) const;

   /// @param ctxt [IN/OUT] current event context is rewound to first event.
   virtual StatusCode rewind(IEvtSelector::Context& ctxt) const;

   /// @param ctxt [IN] current event context.
   /// @param iop [OUT] pointer to IOpaqueAddress reference of selection context.
   virtual StatusCode createAddress(const IEvtSelector::Context& ctxt, IOpaqueAddress*& iop) const;

   /// @param ctxt [IN] current event context is released.
   virtual StatusCode releaseContext(IEvtSelector::Context*& ctxt) const;

   /// Set a selection criteria.
   /// @param criteria filter predicate (SQL-style WHERE clause)
   /// @param ctxt [IN] current event context.
   virtual StatusCode resetCriteria(const std::string& criteria, IEvtSelector::Context& ctxt) const;

   /// Seek to a given event number.
   /// @param evtnum [IN]  The event number to which to seek.
   virtual StatusCode seek(int evtnum);

   /// Return the current event number.
   virtual int curEvent() const;

   /// Make this a server.
   virtual StatusCode makeServer(int num);

   /// Make this a client.
   virtual StatusCode makeClient(int num);

   /// Request to share a given event number.
   /// @param evtnum [IN]  The event number to share.
   virtual StatusCode share(int evtnum);

   /// Read the next maxevt events.
   /// @param evtnum [IN]  The number of events to read.
   virtual StatusCode readEvent(int maxevt);

   /// Return the size of the collection.
   virtual int size();

   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit();
   /// Callback method to finalize the internal state of the component for I/O purposes (e.g. before @c fork(2))
   virtual StatusCode io_finalize();

private: // internal member functions
   /// Return pointer to active event SG
   StoreGateSvc* eventStore() const;
   /// Reinitialize the service when a @c fork() occured/was-issued
   StatusCode reinit();
   /// Return pointer to new PoolCollectionConverter
   PoolCollectionConverter* getCollectionCnv(bool throwIncidents = false) const;
   /// Record AttributeList in StoreGate
   StatusCode recordAttributeList() const;
   /// Search for event number evtNum.
   int findEvent(int evtNum);

   /// Fires the EndInputFile incident (if there is an open file), EndTagFile incident, and LastInputFile incidents at end of selector
   void fireEndFileIncidents(bool isLastFile, bool fireEndTagIncident) const;

private: // data
   EventContextAthenaPool*      m_beginIter;
   EventContextAthenaPool*      m_endIter;

   ServiceHandle<ActiveStoreSvc> m_activeStoreSvc;
   ServiceHandle<StoreGateSvc> m_tagDataStore;

   mutable PoolCollectionConverter* m_poolCollectionConverter;
   mutable pool::ICollectionCursor* m_headerIterator;
   mutable Guid m_guid;
   mutable long long int m_satelliteOid1;

   ServiceHandle<IAthenaPoolCnvSvc> m_athenaPoolCnvSvc;
   ServiceHandle<IChronoStatSvc> m_chronoStatSvc;
   ServiceHandle<IClassIDSvc> m_clidSvc;
   ServiceHandle<IIncidentSvc> m_incidentSvc;

private: // properties
   /// BackNavigation, switch on back navigation to find objects in input streams: default = false.
   BooleanProperty m_backNavigationFlag;
   /// ProcessMetadata, switch on firing of FileIncidents which will trigger processing of metadata: default = true.
   BooleanProperty m_processMetadata;
   /// ShowSizeStat, show size statistics from POOL for all persistified objects: default = false.
   BooleanProperty m_showSizeStat;
   /// CollectionType, type of the collection: default = "ImplicitROOT".
   StringProperty m_collectionType;
   /// CollectionTree, prefix of the collection TTree: default = "POOLContainer_".
   StringProperty m_collectionTree;
   /// Connection, connection string.
   StringProperty m_connection;
   /// RefName, attribute name.
   StringProperty m_refName;
   StringProperty m_derRefName;
   /// AttributeList SG key
   StringProperty m_attrListKey;
   /// InputCollections, vector with names of the input collections.
   StringArrayProperty m_inputCollectionsProp;
   mutable std::vector<std::string>::const_iterator m_inputCollectionsIterator;
   void inputCollectionsHandler(Property&);
   /// Query, query string.
   StringProperty m_query;

   /// SkipBadFiles, boolean flag to allow skipping of non-existing or corrupted files.
   BooleanProperty m_skipBadFiles;

   /// KeepInputFilesOpen, boolean flag to keep files open after PoolCollection reaches end: default = false.
   /// Needed for PilUp to run without PoolFileCatalog. Relies on POOL to close files when reaching DB_AGE_LIMIT.
   BooleanProperty m_keepInputFilesOpen;

   /// HelperTools, vector of names of AlgTools that are executed by the EventSelector
   ToolHandleArray<IAthenaSelectorTool> m_helperTools;
   ToolHandle<IAthenaSelectorTool> m_counterTool;
   ToolHandle<IAthenaIPCTool> m_eventStreamingTool;

   /// The following are included for compatibility with McEventSelector and are not really used.
   /// However runNo, oldRunNo and overrideRunNumberFromInput are used to reset run number for
   /// simulated events, needed to use conditions
   IntegerProperty m_runNo;
   IntegerProperty m_oldRunNo;
   BooleanProperty m_overrideRunNumberFromInput;
   IntegerProperty m_firstEventNo;
   IntegerProperty m_eventsPerRun;
   IntegerProperty m_firstLBNo;
   IntegerProperty m_eventsPerLB;
   IntegerProperty m_initTimeStamp;
   IntegerProperty m_timeStampInterval;

   /// Flags to indicate override of run/event/time
   /// These are almost always false.
   BooleanProperty m_overrideRunNumber;
   BooleanProperty m_overrideEventNumber;
   BooleanProperty m_overrideTimeStamp;

   mutable long m_curCollection;
   mutable std::vector<int> m_numEvt;
   mutable std::vector<int> m_firstEvt;

   /// SkipEvents, numbers of events to skip: default = 0.
   IntegerProperty m_skipEvents;
   LongArrayProperty m_skipEventSequenceProp;
   mutable std::vector<long> m_skipEventSequence;

   mutable int m_evtCount; // internal count of events
   mutable bool m_firedIncident;

#ifdef ATHENAHIVE
   typedef tbb::recursive_mutex CallMutex;
   mutable CallMutex m_callLock;
#endif

};

#endif
