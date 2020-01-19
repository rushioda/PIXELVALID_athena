/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_RawDataProviderTool.h"
#include "MuonRPC_CnvTools/IRpcROD_Decoder.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "MuonRDO/RpcPadContainer.h"
#include "RPCcablingInterface/RpcPadIdHash.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/CablingRPCBase.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// #include "valgrind/callgrind.h"

#define padMaxIndex 600
#define slMaxIndex  200


using eformat::helper::SourceIdentifier;
using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

Muon::RPC_RawDataProviderTool::RPC_RawDataProviderTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p) :
    AthAlgTool(t, n, p),
    m_decoder("Muon__RpcROD_Decoder"),
    m_rdoContainerKey("RPCPAD"),
    m_robDataProvider ("ROBDataProviderSvc",n)
{
    declareInterface<IMuonRawDataProviderTool>(this);
    declareProperty("Decoder",     m_decoder);
    declareProperty("RdoLocation", m_rdoContainerKey);
}


Muon::RPC_RawDataProviderTool::~RPC_RawDataProviderTool()
{}


StatusCode Muon::RPC_RawDataProviderTool::initialize()
{

    StatusCode sc = AlgTool::initialize();
    if (sc.isFailure()) return sc;

   sc = service("ActiveStoreSvc", m_activeStore);
   if ( !sc.isSuccess() ) {
      ATH_MSG_FATAL(  "Could not get active store service" );
      return sc;
   }

    if (m_decoder.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_decoder );
        return StatusCode::FAILURE;
    }
    else
        ATH_MSG_INFO( "Retrieved tool " << m_decoder );


    //m_hashfunc = new RpcPadIdHash();

    // get cabling svc
    const IRPCcablingServerSvc* RpcCabGet = 0;
    sc = service("RPCcablingServerSvc", RpcCabGet);
    if (sc.isFailure()) {
      ATH_MSG_FATAL( "Could not get RPCcablingServerSvc !" );
      return StatusCode::FAILURE;
    }
    else ATH_MSG_VERBOSE( " RPCcablingServerSvc retrieved" );
  
    sc = RpcCabGet->giveCabling(m_rpcCabling);
    if (sc.isFailure()) {
      ATH_MSG_FATAL( "Could not get RPCcablingSvc from the Server !" );
      m_rpcCabling = 0;
      return StatusCode::FAILURE;
    } 
    else {
      ATH_MSG_VERBOSE( " RPCcablingSvc obtained " );
    }    

    // Get ROBDataProviderSvc
    if (m_robDataProvider.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve serive " << m_robDataProvider);
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved service " << m_robDataProvider);
    
    // Check if EventSelector has the ByteStreamCnvSvc
    bool has_bytestream = false;
    IJobOptionsSvc* jobOptionsSvc;
    sc = service("JobOptionsSvc", jobOptionsSvc, false);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "Could not find JobOptionsSvc");
      jobOptionsSvc = 0;
    } else {
      IService* svc = dynamic_cast<IService*>(jobOptionsSvc);
      if(svc != 0 ) {
        ATH_MSG_INFO( " Tool = " << name() 
           << " is connected to JobOptionsSvc Service = "
           << svc->name() );
      } else return StatusCode::FAILURE;
    }
    
    IJobOptionsSvc* TrigConfSvc;
    sc = service("TrigConf::HLTJobOptionsSvc", TrigConfSvc, false);
    if (sc.isFailure()) {
      msg(MSG::DEBUG) << "Could not find TrigConf::HLTJobOptionsSvc" << endmsg;
      TrigConfSvc = 0;
    } else {
      IService* svc = dynamic_cast<IService*>(TrigConfSvc);
      if(svc != 0 ) {
        ATH_MSG_INFO( " Tool = " << name() 
           << " is connected to JobOptionsSvc Service = "
           << svc->name() );
      } else return StatusCode::FAILURE;
    }
    
    if(jobOptionsSvc==0 && TrigConfSvc==0)
    {
      ATH_MSG_FATAL( "Bad job configuration" );
      return StatusCode::FAILURE;  
    }
    
    const std::vector<const Property*>* byteStreamNavProps
        = (jobOptionsSvc)?  jobOptionsSvc->getProperties("ByteStreamNavigationProviderSvc") : 0;

    
    const std::vector<const Property*>* dataFlowProps 
        = (jobOptionsSvc)?  jobOptionsSvc->getProperties("DataFlowConfig") : 0;

    const std::vector<const Property*>* eventSelProps 
        = (jobOptionsSvc)? jobOptionsSvc->getProperties("EventSelector") :
                     TrigConfSvc->getProperties("EventSelector"); 
    
    if     ( dataFlowProps != 0 ) has_bytestream = true;
        if( byteStreamNavProps != 0 ) has_bytestream = true;
    else if( eventSelProps != 0 )
    {
        for (std::vector<const Property*>::const_iterator 
            cur  = eventSelProps->begin();
      cur != eventSelProps->end(); cur++) {
      
          if( (*cur)->name() == "ByteStreamInputSvc" ) has_bytestream = true;
        }
    } 
    else has_bytestream = true;
    //{
    //    msg(MSG::FATAL) << "Cannot get the job configuration" << endmsg;
  //return StatusCode::FAILURE;
    //}
    
    
    // register the container only when the imput from ByteStream is set up     
    m_activeStore->setStore( &*evtStore() ); 
    if( has_bytestream || m_rdoContainerKey != "RPCPAD" )
    {
        RpcPadContainer* container = 
      Muon::MuonRdoContainerAccess::retrieveRpcPad(m_rdoContainerKey);
      
  // create and register the container only once
        if(container==0)
        {
            try 
            {
                container = new RpcPadContainer(padMaxIndex);
            } 
            catch(std::bad_alloc) 
            {
              ATH_MSG_FATAL( "Could not create a new RPC PAD container!");
                return StatusCode::FAILURE;
            }

            // record the container for being used by the convert method

            if( Muon::MuonRdoContainerAccess::record(container,
                                                     m_rdoContainerKey,
                                                     serviceLocator(), msg(), 
                                                     (&*evtStore())).isFailure() )
            {
              ATH_MSG_FATAL("Recording of container " 
                  << m_rdoContainerKey
                  << " into MuonRdoContainerManager has failed");
              return StatusCode::FAILURE;
            }
  }
  
  RpcSectorLogicContainer* sec = 
      Muon::MuonRdoContainerAccess::retrieveRpcSec("RPC_SECTORLOGIC");
  
  if(sec==0)
        {
            try 
            {
                sec = new RpcSectorLogicContainer();
            } 
            catch(std::bad_alloc) 
            {
              ATH_MSG_WARNING( "Could not create a new RpcSectorLogic container!");
                //return StatusCode::FAILURE;
            }

            // record the container for being used by the convert method

            if( Muon::MuonRdoContainerAccess::record(sec,
                                                     "RPC_SECTORLOGIC",
                                                     serviceLocator(),
                                                     msg()).isFailure() )
            {
              ATH_MSG_WARNING("Recording of container " 
              << "RPC_SECTORLOGIC"
              << " into MuonRdoContainerManager has failed");
              //return StatusCode::FAILURE;
            }
  }
    }
    else
    {
      ATH_MSG_DEBUG( "ByteStream conversion service not found.\n"<< "RPC PAD container not registered.");
    }
    
    ATH_MSG_INFO( "initialize() successful in " << name());
    
    return StatusCode::SUCCESS;
}


StatusCode Muon::RPC_RawDataProviderTool::finalize()
{
    StatusCode sc = AlgTool::finalize();
    //if(m_hashfunc) delete m_hashfunc;
    return sc;
}

// the new one 
StatusCode Muon::RPC_RawDataProviderTool::convert()
{
//CALLGRIND_START_INSTRUMENTATION
  /// 
  m_decoder->setSLdecodingRequest();
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  std::vector<uint32_t> robIds = m_rpcCabling->giveFullListOfRobIds();
  m_robDataProvider->getROBData( robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
  return convert(vecOfRobf); // using the old one
}
// the old one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs)
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecRobs,collections); 
}

// the new one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds)
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(robIds, vecOfRobf);
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf,collections); 
}

// the new one
StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect)
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    std::vector<uint32_t> robIds;
    CHECK( m_rpcCabling->giveROB_fromRDO(rdoIdhVect, robIds) );
    m_robDataProvider->getROBData(robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf, rdoIdhVect); // using the old one 
}
// the old one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
                                  const std::vector<IdentifierHash>& collections)
{
 //CALLGRIND_START_INSTRUMENTATION
    // retrieve the container through the MuonRdoContainerManager becasue
    // if the MuonByteStream CNV has to be used, the container must have been
    // registered there!
    m_activeStore->setStore( &*evtStore() );
    RpcPadContainer* pad = Muon::MuonRdoContainerAccess::retrieveRpcPad(m_rdoContainerKey);
      
    if(pad==0)
    {
      ATH_MSG_DEBUG( "Container " << m_rdoContainerKey 
          << " for bytestream conversion not available.\n" << "Try retrieving it from the Store");
        
        return StatusCode::SUCCESS; // Maybe it should be false to stop the job
                                    // because the convert method should not
                                    // have been called .... but this depends
                                    // on the user experience
    }
    
    
    for (ROBFragmentList::const_iterator itFrag = vecRobs.begin(); itFrag != vecRobs.end(); itFrag++)
    {
        //convert only if data payload is delivered
        if ( (**itFrag).rod_ndata()!=0 )
        {
            std::vector<IdentifierHash> coll =
                                      to_be_converted(**itFrag,collections);
      
            if (m_decoder->fillCollections(**itFrag, *pad, coll).isFailure())
                {
                    // store the error conditions into the StatusCode and continue
                }
        }
        else
        {
	  if(msgLvl(MSG::DEBUG))
            {
                uint32_t sourceId= (**itFrag).source_id();
                msg(MSG::DEBUG) << " ROB " << MSG::hex << sourceId
        << " is delivered with an empty payload" << MSG::dec 
        << endmsg;
            }
            // store the error condition into the StatusCode and continue
        }
    }
    //in presence of errors return FAILURE
//CALLGRIND_STOP_INSTRUMENTATION
    return StatusCode::SUCCESS;
}


std::vector<IdentifierHash> 
Muon::RPC_RawDataProviderTool::to_be_converted(const ROBFragment& robFrag,
                            const std::vector<IdentifierHash>& coll) const
{
    std::vector<IdentifierHash> to_return;
    if(coll.size()==0)
    {
    // get SubdetectorId and ModuleId
        uint32_t source_id = robFrag.rod_source_id();
        SourceIdentifier sid(source_id);
        to_return =
            (m_rpcCabling->padHashFunction())->rod2hash(sid.subdetector_id(), sid.module_id() ); 
    }
    else
    {
        uint32_t source_id = robFrag.rod_source_id();
        std::vector<IdentifierHash>::const_iterator it = coll.begin();
        for ( ; it!= coll.end() ; ++it)
        {
            if( source_id == (m_rpcCabling->padHashFunction())->hash2source(*it) ) 
                to_return.push_back(IdentifierHash(*it));
        }
    }

    return to_return;
}
