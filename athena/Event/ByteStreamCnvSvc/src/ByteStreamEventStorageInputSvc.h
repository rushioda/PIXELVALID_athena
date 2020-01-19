/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMEVENTSTORAGEINPUTSVC_H
#define BYTESTREAMEVENTSTORAGEINPUTSVC_H

/** @file ByteStreamEventStorageInputSvc.h
 *  @brief This file contains the class definition for the ByteStreamEventStorageInputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// Include files.
#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamCnvSvc/IByteStreamFreeMetadataSvc.h"
#include "ByteStreamData/RawEvent.h"

#include "EventStorage/DataReader.h"

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class IIncidentSvc;

/** @class ByteStreamEventStorageInputSvc
 *  @brief This class is the ByteStreamInputSvc for reading events written by EventStorage.
 **/
class ByteStreamEventStorageInputSvc :
	public ByteStreamInputSvc {
public:
   /// Constructors:
   ByteStreamEventStorageInputSvc(const std::string& name, ISvcLocator* svcloc);

   /// Destructor.
   virtual ~ByteStreamEventStorageInputSvc();

   /// Required of all Gaudi Services
   virtual StatusCode initialize();
   virtual StatusCode stop();
   /// Required of all Gaudi Services
   virtual StatusCode finalize();

   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Implementation of the ByteStreamInputSvc interface methods.
   virtual const RawEvent* currentEvent() const;
   virtual const RawEvent* nextEvent();            //!< ++, new
   virtual const RawEvent* previousEvent();        //!< --, old
   virtual       void setEvent(void* data, unsigned int eventStatus);

   /// Return the current event status
   virtual unsigned int currentEventStatus() const;
   virtual void validateEvent();

   virtual long positionInBlock();
   virtual long getBlockIterator(const std::string fileName);
   void         closeBlockIterator(bool clearMetadata=true);
   bool         setSequentialRead();
   bool         ready();
   StatusCode   generateDataHeader();

private: // internal member functions
   bool loadMetadata();

private: // data
   //int                m_totalEventCounter; //!< event Counter
   RawEvent*          m_re;            //!< current event
   unsigned int       m_eventStatus;   //!< check_tree() status of the current event
   DataReader*        m_reader;        //!< DataReader from EventStorage

   mutable std::vector<int>     m_numEvt;    //!< number of events in that file
   mutable std::vector<int>     m_firstEvt;  //!< event number of first event in that file
   mutable std::vector<long long int> m_evtOffsets;  //!< offset for event i in that file
   unsigned int                 m_evtInFile;

   // Event back navigation info
   std::string        m_fileGUID;      //!< current file GUID
   long long int      m_eventOffset;   //!< event offset within a file, can be -1

   /// Pointer to StoreGate
   ServiceHandle<StoreGateSvc> m_sgSvc; //!< StoreGateSvc
   ServiceHandle<StoreGateSvc> m_mdSvc; //!< StoreGateSvc
   ServiceHandle<IIncidentSvc> m_incidentSvc; //!< IncidentSvc
   ServiceHandle<IByteStreamFreeMetadataSvc> m_attlistsvc; 
   ServiceHandle<IROBDataProviderSvc> m_robProvider;

private: // properties
   std::vector<std::string>	m_vExplicitFile;  //!< prefix of the file names

   std::vector<std::string>::const_iterator m_itFullFile; //!< iter  for full filename

   bool               m_sequential;    //!< enable sequential reading.
   int                m_fileCount;     //!< number of files to process.
   std::string        m_fullFile;      //!< current full file name

   BooleanProperty    m_dump;          //!< flag for Dump fragments
   FloatProperty      m_wait;          //!< Number of seconds to wait if the input is in the wait state.
   BooleanProperty    m_valEvent;      //!< switch on check_tree() call when reading events.
   BooleanProperty    m_procBadEvent;  //!< DEFUNCT process bad events, which fail check_tree().
   IntegerProperty    m_maxBadEvts;    //!< DEFUNCT number of bad events allowed before quitting.
   std::vector<std::string> m_keys;

private: // internal helper functions

   void buildFragment(void* data, uint32_t eventSize, bool validate);
   void releaseCurrentEvent();
   bool readerReady();
   bool ROBFragmentCheck();
};

#endif
