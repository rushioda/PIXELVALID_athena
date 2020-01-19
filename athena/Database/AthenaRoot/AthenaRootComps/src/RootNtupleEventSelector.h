///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootNtupleEventSelector.h 
// Header file for class RootNtupleEventSelector
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ATHENA_ROOTNTUPLEEVENTSELECTOR_H 
#define ATHENAROOTCOMPS_ATHENA_ROOTNTUPLEEVENTSELECTOR_H 1

// STL includes
#include <unordered_map>

// framework includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/ICollectionSize.h"

#include "TFile.h"
#include "TObjString.h"

// Forward declaration
class ISvcLocator;
class StoreGateSvc;
class TTree;
class IClassIDSvc;
class IDictLoaderSvc;
namespace Athena { class RootNtupleEventContext; }

namespace Athena {

/** @brief Class implementing the GAUDI @c IEvtSelector interface using 
 *         ROOT @c TTree as a backend
 */
class RootNtupleEventSelector : 
    virtual public IEvtSelector, virtual public ICollectionSize,
  virtual public IEventSeek,
  virtual public IAddressProvider,
  virtual public IIoComponent, virtual public IIncidentListener,
          public ::AthService
{ 
  friend class Athena::RootNtupleEventContext;



  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  RootNtupleEventSelector( const std::string& name, ISvcLocator* svcLoc );

  /// Destructor: 
  virtual ~RootNtupleEventSelector(); 

  // Athena hooks
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  
   void handle(const Incident& incident);

  ///@{
  /// @c IEvtSelector interface
  virtual StatusCode createContext( Context*& refpCtxt ) const;

  virtual StatusCode last( Context& refContext ) const;
  virtual StatusCode next( Context& refCtxt ) const;
  virtual StatusCode next( Context& refCtxt, int jump ) const;
  virtual StatusCode previous( Context& refCtxt ) const;
  virtual StatusCode previous( Context& refCtxt, int jump ) const;
  virtual StatusCode rewind( Context& refCtxt ) const;

  virtual StatusCode createAddress( const Context& refCtxt, 
                                    IOpaqueAddress*& ) const;
  virtual StatusCode releaseContext( Context*& refCtxt ) const;
  virtual StatusCode resetCriteria( const std::string& cr, 
                                    Context& ctx )const;
  ///@}

  ///@{
  /// @c IEventSeek interface
  /**
   * @brief Seek to a given event number.
   * @param evtnum  The event number to which to seek.
   */
  virtual StatusCode seek (int evtnum);

  /**
   * @brief return the current event number.
   * @return The current event number.
   */
  virtual int curEvent () const;
  ///@}
  
  /// Callback method to reinitialize the internal state of the component 
  /// for I/O purposes (e.g. upon @c fork(2))
  virtual StatusCode io_reinit();

  ///@{
  /// @c IAddressProvider interface
  ///get all addresses from Provider : Called before Begin Event
  virtual 
  StatusCode preLoadAddresses(StoreID::type storeID, tadList& list);
 
   /// get all new addresses from Provider for this Event.
  virtual 
  StatusCode loadAddresses(StoreID::type storeID, tadList& list);
 
  /// update a transient Address
  virtual 
  StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad);
  ///@}

  ///@c ICollectionSize interface
  virtual int size();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// callback to synchronize the list of input files
  void setupInputCollection( Property& inputCollectionsName );

  /// helper method to create proxies
  StatusCode createRootBranchAddresses(StoreID::type storeID,
                                       tadList &tads);


  /// helper method to retrieve the correct tuple
  TTree* fetchNtuple(const std::string& fname) const;

  void addMetadataFromDirectoryName(const std::string &metadirname, 
                                    TFile *fileObj, 
                                    const std::string &prefix = "") const;
  void addMetadataFromDirectory(TDirectoryFile *metadir, 
                                const std::string &prefix = "") const;
  void addMetadata(TTree *metatree, const std::string &path = "") const;
  void addMetadata(TObjString *metastring, const std::string &path = "") const;
  
  /// helper method to create proxies for the metadata store
  StatusCode createMetaDataRootBranchAddresses(StoreGateSvc *store,
   					       TTree *tree,
   					       const std::string& prefix) const;

  /// helper method to init the i/o components
  StatusCode do_init_io();

  /// helper method to get the collection index (into `m_inputCollectionsName`)
  /// for a given event index `evtidx`.
  /// returns -1 if not found.
  int find_coll_idx(int evtidx);

  /// non-const access to self (b/c ::next() is const)
  RootNtupleEventSelector *self() const
  { return const_cast<RootNtupleEventSelector*>(this); }

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to the @c StoreGateSvc event store
  StoreGateSvc_t m_dataStore;

  /// Pointer to the @c StoreGateSvc input metadata store
  StoreGateSvc_t m_imetaStore;

  /// Pointer to the @c StoreGateSvc output metadata store
  StoreGateSvc_t m_ometaStore;

  typedef ServiceHandle<IClassIDSvc> ICLIDSvc_t;
  /// Pointer to the @c IClassIDSvc
  ICLIDSvc_t m_clidsvc;

  typedef ServiceHandle<IDictLoaderSvc> IDictSvc_t;
  /// Pointer to the @c IDictLoaderSvc
  IDictSvc_t m_dictsvc;

  typedef ServiceHandle<IIncidentSvc> IIncSvc_t;
  /// Handle to the incident service
  IIncSvc_t m_incsvc;

  /// List of input files containing @c TTree 
  StringArrayProperty m_inputCollectionsName;

  /// Name of @c TTree to load from collection of input files
  StringProperty m_tupleName;

  /// List of branches to activate in the @c TTree 
  StringArrayProperty m_activeBranchNames;

  /// Number of events to skip at the beginning 
  long m_skipEvts;

  /// Number of Events read so far.
  mutable long m_nbrEvts;
  
  /// current event index
  mutable long m_curEvt;

  /// current collection index (into `m_inputCollectionsName`)
  mutable long m_collIdx;

  struct CollMetaData {
    /// number of entries up to this collection
    long min_entries;
    /// number of entries after this collection
    long max_entries;
  };
  /// cache of the number of entries for each collection
  mutable std::vector<CollMetaData> m_collEvts;

  /// current tree being read
  mutable TTree *m_tuple;

  /** The (python) selection function to apply on the @c TChain we are reading
   */
  //PyObject* m_pySelectionFct;

  // flag to trigger reloading of root branch addresses
  mutable bool m_needReload;
  // flag to trigger firing BeginInputFile incidents once the root branch 
  // addresses have been reloaded. 
  // Reloading addresses (we assume?) means a new TTree
  // has been loaded from a new file in the list of input files.
  //
  // FIXME: use some kind of state-machine to couple 
  //   m_needReload and m_fireBIF ?
  mutable bool m_fireBIF;

  typedef std::unordered_map<SG::TransientAddress*, bool> Addrs_t;
  // the list of transient addresses we "manage" or know about
  // these addresses are the actual TTree's branch names
  // for the event data
  Addrs_t m_rootAddresses;

  // the list of transient addresses we "manage" or know about
  // these addresses are the actual TTree's branch names
  // for the metadata tree(s)
  //Addrs_t m_rootAddressesMetaData;

  // List of files that we've created, so that we can delete them
  // during finalize.  FIXME: get rid of the mutable.
  mutable std::vector<TFile*> m_files;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> namespace Athena

#endif //> ATHENAROOTCOMPS_ATHENA_ROOTNTUPLEEVENTSELECTOR_H
