/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//------------------------------------------------------------------------------
/*
 *  Author:     Gorm Galster <gorm.galster@cern.ch>
 *  
 *  Purpose:    The purpose of the Merging event loop manager is to read a BS
 *              file and a RDO file simultanously to provide an interface for
 *              merging trigger information (from BS) with detector information
 *              (from RDO).
 *
 *  Credit:     Most of the code here is inspired by the original work done by
 *              Attila Krasznahorkay Jr. <Attila.Krasznahorkay@cern.ch> on the
 *              TwoInputEventLoopMgr for used for event-by-event BS comparison.
 *
 */
//------------------------------------------------------------------------------
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventIncident.h"

#include "SGTools/DataProxy.h"

#include "TrigSteeringEvent/HLTResult.h"

#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"





#include "MergingEventLoopMgr.h"




//------------------------------------------------------------------------------
/*
 * PLEASE READ: 
 *
 * There is little black magic and a lot of jazz to this. So read carefully.
 *
 * The objective here is to get access to the DataStore of a StoreGateSvc.
 * This is (seemingly) needed to manipulate the proxies directly.
 *
 * Unfortunately both the pointer, m_pStore, and the getter function, 
 * store(), is declared as private.
 *
 * The "clever" trick here is to use a suitable templated function and get write
 * a specialization of this function for some dummy class.
 *
 * Lets get started...
 *
 */
/*
 * Include the necessary headers
 */
#include "StoreGate/StoreGateSvc.h" 
#include "SGTools/DataStore.h"
/*
 * Define some dummy class in some protected/controled namespace to avoid
 * clashing with the intended behaviour of the function.
 */
// namespace TrigSim {
//     namespace Backdoor {
//         class DSfromSG; // DetectoreStore from StoreGate
//     }
// }
/*
 * I found the following templated function suitable:
 * 
 *  template <typename T>
 *  T* StoreGateSvc::retrieve ()
 *
 * Now the only thing left is to write the specialization of the function.
 * In the function definition one can of cause access private data members, but
 * we will still have to respect the function prototype:
 */
// template <>
// TrigSim::Backdoor::DSfromSG* StoreGateSvc::retrieve<TrigSim::Backdoor::DSfromSG>() {
//   return (TrigSim::Backdoor::DSfromSG *) m_pStore;
// }
/*
 * Usage will be:
 *
 *  DataStorage *pDS = (DataStorage *) pSG->retrieve<TrigSim::BackDoor::DSfromSG>();
 *
 */
//------------------------------------------------------------------------------












//------------------------------------------------------------------------------
namespace TrigSim {
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
    /*
     * Take the vector with proxy pointers and populate the name-to-proxy map
     * as well as the set of proxy names
     */
    inline
    void proxyContainerPopulate(std::vector<const SG::DataProxy *> vector,
                std::map<std::string, SG::DataProxy *> *map,
                std::set<std::string>                  *names
            ) {

        for(std::vector<const SG::DataProxy *>::iterator it = vector.begin();
                it != vector.end(); ++it) {

            (*it)->isValid();

            names->insert((*it)->name());

            // Const cast as we will not respect the constness
            (*map)[(*it)->name()] = const_cast<SG::DataProxy *> (*it);
        }
    }
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
    MergingEventLoopMgr::MergingEventLoopMgr(const std::string& name,
                                             ISvcLocator* svcLoc)
            : MinimalEventLoopMgr(name, svcLoc),
              m_incidentSvc             ("IncidentSvc",  name),
              m_primEvtStore            ("StoreGateSvc", name),
              m_secEvtStore             ("StoreGateSvc", name),
              m_primEvtSelector         (0),
              m_primEvtContext          (0),
              m_primEvtSelName          ("EventSelector"),
              m_secEvtSelector          (0),
              m_secEvtContext           (0),
              m_secEvtSelName           ("EventSelector"),
              m_currentRun              (0),
              m_firstRun                (true),
              m_nEvt                    (0),
              m_log(msgSvc(), name) {

        // Body of constructor
        declareProperty("IncidentSvc", m_incidentSvc);

        declareProperty("PrimaryEventStore",   m_primEvtStore);
        declareProperty("SecondaryEventStore", m_secEvtStore);

        declareProperty("PrimaryEventSelector",   m_primEvtSelName);
        declareProperty("SecondaryEventSelector", m_secEvtSelName);


        /*
         * Steering options
         */
        declareProperty("OverwritePrimaryTriggerInfo",
                        m_overwritePrimTriggerInfo = false,
                        "Overwrite the TriggerInfo object in the primary stream with that from the secondary stream"
                );
        declareProperty("OverwriteSecondaryTriggerInfo",
                        m_overwriteSecTriggerInfo = false,
                        "Overwrite the TriggerInfo object in the secondary stream with that from the primary stream"
                );

        declareProperty("SharePrimaryProxies",
                        m_sharePrimProxies = false,
                        "Provide the proxies of the primary event store to the secondary event store"
                );
        
        declareProperty("ShareSecondaryProxies",
                        m_shareSecProxies = false,
                        "Provide the proxies of the primary event store to the secondary event store"
                );
        
        declareProperty("SafeProxySharing",
                        m_shareProxiesSafe = true,
                        "Try to avoid clashes when sharing proxies."
                );

    }
//------------------------------------------------------------------------------
    MergingEventLoopMgr::~MergingEventLoopMgr() {
    } 
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::initialize() {
        /*
         * Print some welcome message:
         */
        m_log.setLevel( m_outputLevel.value() );

        m_log << MSG::INFO << "Initializing TrigSim::MergingEventLoopMgr/" << name()
              << " - package version " << PACKAGE_VERSION << endreq;
   


        /*
         * Does the steering make sense
         */
        if(m_overwritePrimTriggerInfo && m_overwriteSecTriggerInfo) {
            m_log << MSG::FATAL << "Can not overwrite both primary and secondary TriggerInfo"
                  << endreq;

            return StatusCode::FAILURE;
        }




        /*
         * Initialise the base class:
         */
        StatusCode sc = MinimalEventLoopMgr::initialize();
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to initialize MinimalEventLoopMgr base class"
                  << endreq;
            return sc;
        }

        /*
         * Retrieve the incident service:
         */
        sc = m_incidentSvc.retrieve();
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to retrieve handle to the incident service"
                  << endreq;
            return sc;
        }

        /*
         * Retrieve the primary SG service:
         */
        sc = m_primEvtStore.retrieve();
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to retrieve handle to the primary StoreGateSvc"
                  << endreq;
            return sc;
        }
        
        /*
         * Retrieve the secondary SG service:
         */

        sc = m_secEvtStore.retrieve();
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to retrieve handle to the BS StoreGateSvc"
                  << endreq;
            return sc;
        }


        /*
         * Set up the primary event selector service:
         */
        sc = serviceLocator()->service(m_primEvtSelName.value(),
                                       m_primEvtSelector);
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to access primary event selector called: "
                  << m_primEvtSelName.value() << endreq;
            throw GaudiException("No valid primary event selector", name(),
                                 StatusCode::FAILURE);
        }

        sc = m_primEvtSelector->createContext(m_primEvtContext);
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to setup the primary event selector context"
                  << endreq;
            throw GaudiException("Failed to setup the primary event selector context",
                                 name(), StatusCode::FAILURE);
        }

        Service* theSvc = dynamic_cast< Service* >(m_primEvtSelector);
        if(!theSvc) {
            m_log << MSG::FATAL
                  << "Failed to cast the primary event selector into a Service"
                  << endreq;
            return StatusCode::FAILURE;
        }
        m_log << MSG::INFO << "Setup primary EventSelector service: " << theSvc->name()
              << endreq;


        
        /*
         * Set up the secondary event selector service:
         */

        sc = serviceLocator()->service(m_secEvtSelName.value(),
                                       m_secEvtSelector);
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to access BS event selector called: "
                  << m_secEvtSelName.value() << endreq;
            throw GaudiException("No valid BS event selector", name(),
                                 StatusCode::FAILURE);
        }

        sc = m_secEvtSelector->createContext(m_secEvtContext);
        if(sc.isFailure()) {
            m_log << MSG::FATAL << "Failed to setup the BS event selector context"
                  << endreq;
            throw GaudiException("Failed to setup the BS event selector context",
                                 name(), StatusCode::FAILURE);
        }

        theSvc = dynamic_cast< Service* >(m_secEvtSelector);
        if(!theSvc) {
            m_log << MSG::FATAL
                  << "Failed to cast the BS event selector into a Service"
                  << endreq;
            return StatusCode::FAILURE;
        }
        m_log << MSG::INFO << "Setup BS EventSelector service: " << theSvc->name()
              << endreq;

        return StatusCode::SUCCESS;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::finalize() {
        
        /*
         * Finalize base class
         */

        StatusCode sc = MinimalEventLoopMgr::finalize();
        if(sc.isFailure()) {
            m_log << MSG::ERROR << "Failed to finalize MinimalEventLoopMgr base class"
                  << endreq;
            return StatusCode::FAILURE;
        }


        /*
         * Release remaining services and jazz..
         */

        m_incidentSvc.release().ignore();

        m_primEvtSelector = releaseInterface(m_primEvtSelector);
        delete m_primEvtContext; m_primEvtContext = 0;

        m_secEvtSelector  = releaseInterface(m_secEvtSelector);
        delete m_secEvtContext; m_secEvtContext = 0;
        
        return StatusCode::SUCCESS;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::nextEvent(int maxEvt) {
        m_log << MSG::INFO << "Entering nextEvent(...)" << endreq;
        
        StatusCode sc;

        /*
         * Loop over "all" events...
         */
        while( maxEvt == -1 || m_nEvt < maxEvt ) {

            
            /*
             * Get primary event
             */
            
            const EventInfo* pPrimEvt = 0;

            if(m_primEvtContext) {
                IOpaqueAddress* primAddr = 0;
           
                sc = m_primEvtSelector->next(*m_primEvtContext);

                if(sc.isFailure()) {
                    m_log << MSG::INFO << "No more events in primary event selection"
                          << endreq;
                    sc = StatusCode::SUCCESS;
                    break;
                }

                if (m_primEvtSelector->createAddress(*m_primEvtContext, primAddr).isFailure()) {
                    m_log << MSG::ERROR  << "Could not create a primary IOpaqueAddress"
                          << endreq;
                    break;
                } 
            
                if (primAddr != 0) {
                    sc = m_primEvtStore->recordAddress(primAddr);
                    if(!sc.isSuccess()) {
                        m_log << MSG::WARNING << "Error declaring primary Event object"
                              << endreq;
                        break;
                    }
                }

                if ((sc=m_primEvtStore->loadEventProxies()).isFailure()) {
                    m_log << MSG::ERROR  << "Error loading primary event proxies"
                          << endreq;
                    break;
                }

                sc = m_primEvtStore->retrieve(pPrimEvt);
                if( !sc.isSuccess() ) {
                    m_log << MSG::ERROR << "Unable to retrieve primary Event root object"
                          << endreq;
                    break;
                }
            } // primary context


            /*
             * Get secondary event
             */
            
            const EventInfo* pSecEvt = 0;
            
            if(m_secEvtContext) {
                IOpaqueAddress* secAddr = 0;
           
                sc = m_secEvtSelector->next(*m_secEvtContext);
                
                if(sc.isFailure()) {
                    m_log << MSG::INFO << "No more events in secondary event selection"
                          << endreq;
                    sc = StatusCode::SUCCESS;
                    break;
                }

                if (m_secEvtSelector->createAddress(*m_secEvtContext, secAddr).isFailure()) {
                    m_log << MSG::ERROR  << "Could not create a secondary IOpaqueAddress"
                          << endreq;
                    break;
                } 
            
                if (secAddr != 0) {
                    sc = m_secEvtStore->createProxy(secAddr);
                    if(!sc.isSuccess()) {
                        m_log << MSG::WARNING << "Error declaring secondary Event object"
                              << endreq;
                        break;
                    }
                }

                if ((sc=m_secEvtStore->loadEventProxies()).isFailure()) {
                    m_log << MSG::ERROR  << "Error loading secondary event proxies"
                          << endreq;
                    break;
                }

                sc = m_secEvtStore->retrieve(pSecEvt);
                if( !sc.isSuccess() ) {
                    m_log << MSG::ERROR << "Unable to retrieve secondary Event root object"
                          << endreq;
                    break;
                }
            } // secondary context

        
            /*
             * Check if primary event matches secondary event.
             * NOTE: A sneakier way could have been found to do this,
             *       but for current purposes, events lines up nicely
             */

            if( pPrimEvt == 0 || pSecEvt == 0
                || pPrimEvt->event_ID()->event_number() != pSecEvt->event_ID()->event_number()
                || pPrimEvt->event_ID()->run_number() != pSecEvt->event_ID()->run_number()) {

                m_log << MSG::ERROR << "EventInfo for primary and secondary differs or could not be retreived. This scenario has NOT been counted for (yet)."
                      << endreq;
                sc = StatusCode::FAILURE;
                break;
            }



            /*
             * Start new/first run if necessary
             */

            if(m_currentRun != pPrimEvt->event_ID()->run_number() || m_firstRun) {

                if(!m_firstRun) {
                    m_incidentSvc->fireIncident(Incident( name(), "EndRun"));

                    sc = endRunAlgorithms();
                    if(sc.isFailure()) {
                        break;
                    }
                }

                m_currentRun = pPrimEvt->event_ID()->run_number();
                m_firstRun = false;

                m_incidentSvc->fireIncident(EventIncident(*pPrimEvt, name(),"BeginRun"));

                sc = beginRunAlgorithms();
                if(sc.isFailure()) {
                        break;
                }
            }








            /*
             *  Fire suitable incident
             */

            m_incidentSvc->fireIncident(EventIncident(*pPrimEvt, name(),"BeginEvent"));



            
            /*
             * Owerwrite the TriggerInfo in EventInfo if asked.
             */
            
            if(m_overwritePrimTriggerInfo || m_overwriteSecTriggerInfo) {
                
                EventInfo *from = (EventInfo *)(m_overwritePrimTriggerInfo ? pSecEvt  : pPrimEvt);
                EventInfo *to =   (EventInfo *)(m_overwritePrimTriggerInfo ? pPrimEvt : pSecEvt);

                sc = overwriteTriggerInfo(from, to);

                if(sc.isFailure()) {
                    m_log << MSG::ERROR << "Failed to overwrite the primary TriggerInfo."
                          << endreq;
                    break;
                }
            }


            /*
             * Share proxies
             */
            if(m_sharePrimProxies || m_shareSecProxies) {
                m_log << MSG::INFO << "Setting up proxy sharing. Trouble may lie ahead..." << endreq;
                
                /*
                 * Get and order primary proxies
                 */
                std::map<std::string, SG::DataProxy *> primProxyMap;
                std::set<std::string> *                primProxyNames = new std::set<std::string>;
                proxyContainerPopulate(m_primEvtStore->proxies(), &primProxyMap, primProxyNames);

                /*
                m_log << MSG::DEBUG << "Got " << primProxyNames->size()<< " primary proxies."
                      << endreq;
                for(std::set<std::string>::iterator it = primProxyNames->begin();
                        it != primProxyNames->end(); ++it) {
                    m_log << MSG::DEBUG << "Primary proxy:   " << (*it)
//                          << " @ " << primProxyMap[(*it)]
                          << endreq;
                }
                */


                /*
                 * Get and order secondary proxies
                 */
                std::map<std::string, SG::DataProxy *> secProxyMap;
                std::set<std::string> *                secProxyNames = new std::set<std::string>;
                proxyContainerPopulate(m_secEvtStore->proxies(), &secProxyMap, secProxyNames);


                /*
                m_log << MSG::DEBUG << "Got " << secProxyNames->size()<< " secondary proxies."
                      << endreq;
                for(std::set<std::string>::iterator it = secProxyNames->begin();
                        it != secProxyNames->end(); ++it) {
                    m_log << MSG::DEBUG << "Secondary proxy: " << (*it)
//                          << " @ " << secProxyMap[(*it)]
                          << endreq;
                }
                */


                /*
                 * Make a blacklist and start by filling it with the intersecting proxies
                 * as intersecting names/proxies can not be "shared" in this sense
                 */
                std::set<std::string> blacklist;
                std::set_intersection(primProxyNames->begin(),      primProxyNames->end(),
                                      secProxyNames->begin(),       secProxyNames->end(),
                                      std::inserter(blacklist,      blacklist.begin()));


                
                /*
                 *  Don't allow the EventInfo to be mirrored.
                 */
                blacklist.insert(m_primEvtStore->proxy(pPrimEvt)->name());
                blacklist.insert(m_secEvtStore->proxy(pSecEvt)->name());


                if(m_shareProxiesSafe) {
                    /* BS proxy/proxies known to cause problems */
                    blacklist.insert("ByteStreamDataHeader");

                    
                    /* RDO proxy/proxies known to cause problems */
                    blacklist.insert("Input");
                }


                /*
                 * Remove affected proxies
                 */
                for(std::set<std::string>::iterator it = blacklist.begin();
                        it != blacklist.end(); ++it) {
                    primProxyMap.erase((*it));
                    secProxyMap.erase((*it));
                }

                
                
                
                
                /*
                 * From here on, don't rely on the list of proxy names
                 */
                delete primProxyNames;  primProxyNames = 0;
                delete secProxyNames;   secProxyNames = 0;





                /*
                 *
                 *
                 *      Future potential for other hooks here.
                 *
                 *
                 */






                /*
                 * Get DataStore handles
                 */
                // SG::DataStore *primDataStore = (SG::DataStore *)m_primEvtStore->retrieve<Backdoor::DSfromSG>();
                // SG::DataStore *secDataStore  = (SG::DataStore *)m_secEvtStore->retrieve<Backdoor::DSfromSG>();


                /*
                 * Setup sharing of the primary proxies
                 */
                if(m_sharePrimProxies) {
                    m_log << MSG::INFO << "Setting up sharing of primary proxies..."
                        << endreq;

                    for(std::map<std::string, SG::DataProxy *>::iterator it = primProxyMap.begin();
                            it != primProxyMap.end(); ++it) {
            
                        if (! it->second->address()) continue;

                        m_log << MSG::INFO << "Adding primary proxy '" << it->first << "' to secondary data store."
                              //<< " Proxy is " << (it->second->isValid() ? "" : "NOT ") << "valid"
                              << endreq;

			// sc = secDataStore->addToStore(it->second->address()->clID(), it->second);
                        sc = m_secEvtStore->addToStore(it->second->address()->clID(), it->second);

                        if(sc.isFailure()) break;
                    }
                }




                /*
                 * Setup sharing of the secondary proxies
                 */
                if(m_shareSecProxies) {
                    m_log << MSG::INFO << "Setting up sharing of secondary proxies..."
                        << endreq;

                    for(std::map<std::string, SG::DataProxy *>::iterator it = secProxyMap.begin();
                            it != secProxyMap.end(); ++it) {
            
                        if (! it->second->address()) continue;

                        m_log << MSG::INFO << "Adding secondary proxy '" << it->first << "' to primary data store."
                              //<< " Proxy is " << (it->second->isValid() ? "" : "NOT ") << "valid"
                              << endreq;

			// sc = primDataStore->addToStore(it->second->address()->clID(), it->second);
                        sc = m_primEvtStore->addToStore(it->second->address()->clID(), it->second);

                        if(sc.isFailure()) break;
                    }
                }

                if(sc.isFailure()) {
                    m_log << MSG::ERROR << "Failed to set up proxy sharing of secondary proxies"
                          << endreq;
                    sc = StatusCode::FAILURE;
                    break;
                }
            } // proxy sharing





            sc = loadObjects();
            if(sc.isFailure()) {
                m_log << MSG::ERROR << "Failure while preloading data."
                      << endreq;
                break;
            }

            /*
             * Print some info about the process of execution
             */

            m_log << MSG::INFO << "  ===>>>  start processing of event #"
                  << pPrimEvt->event_ID()->event_number() << ", run #" << pPrimEvt->event_ID()->run_number()
                  << ". " << m_nEvt << " events processed so far  <<<==="
                  << endreq;

            m_log << MSG::DEBUG << " ===>>> dumping trigger info for primary event: <<<==="
                  << (*pPrimEvt->trigger_info())
                  << endreq;

            m_log << MSG::DEBUG << " ===>>> dumping trigger info for primary event: <<<==="
                  << (*pSecEvt->trigger_info())
                  << endreq;



            /*
             * Execute algorithms
             */

            sc = executeAlgorithms();
            if(sc.isFailure()) {
                m_log << MSG::ERROR << "Failure during algorithm execute."
                      << endreq;
                break;
            }




            /*

            SG::DataStore *dataStoreRDO = (SG::DataStore *)m_primEvtStore->retrieve<Backdoor::DSfromSG>();
            SG::DataStore *dataStoreBS  = (SG::DataStore *)m_secEvtStore->retrieve<Backdoor::DSfromSG>();

            std::vector<const SG::DataProxy*> proxiesBS = m_secEvntStore->proxies();
            m_log << MSG::DEBUG << "Found " << proxiesBS.size() << " proxies. Starting mirroring." << endreq;
           



            std::vector<const SG::DataProxy*>::iterator it;
            for(it = proxiesBS.begin(); it != proxiesBS.end(); it++) {
                SG::DataProxy *proxy = (SG::DataProxy *)(*it);

                m_log << MSG::DEBUG << "Found proxy " << proxy->name()
                      << endreq;
                if(!proxy->isValid() ||
                    proxy->name() == "Input" ||
                    proxy->name() == "ByteStreamDataHeader" ||
                    proxy->name() == "ByteStreamEventInfo") {
                    continue;
                }

                sc = dataStoreRDO->addToStore(proxy->address()->clID(), proxy);
                if(sc.isFailure()) {
                    m_log << MSG::ERROR << "Failed to mirror proxy " << proxy->name()
                          << endreq;
                }
            }
            
            
            const HLT::HLTResult* l2ResultRDO;
            m_primaryEventStore->retrieve(l2ResultRDO, "HLTResult_L2" ).ignore();
            */

            //const HLT::HLTResult* l2ResultRDO;
            //m_primEvtStore->retrieve(l2ResultRDO, "HLTResult_L2").ignore();







            m_log << MSG::DEBUG << " ===>>> dumping primary StoreGate: <<<==="
                  << m_primEvtStore->dump()
                  << endreq;
            m_log << MSG::DEBUG << " ===>>> dumping secondary StoreGate: <<<==="
                  << m_secEvtStore->dump()
                  << endreq;

            





            /*
             *  Fire suitable incident
             */
            m_incidentSvc->fireIncident(EventIncident(*pPrimEvt, name(),"EndEvent"));



            /*
             *  Increment the counter
             */
            ++m_nEvt;

            
            /*
             * Dump store gates for debug
             */
/*
            m_log << MSG::DEBUG << " ===>>> dumping primary StoreGate: <<<==="
                  << m_primEvtStore->dump()
                  << endreq;
            m_log << MSG::DEBUG << " ===>>> dumping secondary StoreGate: <<<==="
                  << m_secEvtStore->dump()
                  << endreq;
*/
            

            /*
             * Clear store gate(s)
             */

            sc = m_primEvtStore->clearStore();
            if(sc.isFailure()) {
                m_log << MSG::ERROR << "Failed to clear primary event store"
                      << endreq;
                break;
            }
           
            sc = m_secEvtStore->clearStore();
            if(sc.isFailure()) {
                m_log << MSG::ERROR << "Failed to clear secondary event store"
                      << endreq;
                break;
            }
            

        } // the big while loop


        m_incidentSvc->fireIncident(Incident(name(), "EndRun"));
        endRunAlgorithms().ignore();
        
        return sc;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::executeEvent(void *par) {
        par = par;
        m_log << MSG::ERROR
              << "executeEvent(...) is not implemented for MergingEventLoopMgr"
              << endreq;
        return StatusCode::FAILURE;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::executeRun(int maxEvt) {
        m_log << MSG::INFO << "Entering executeRun(...)" << endreq;
        
        if(initializeAlgorithms().isFailure()) {
            m_log << MSG::ERROR  << "One or more algorithms failed to initialize"
                  << endreq;
            return StatusCode::FAILURE;
        }

        m_incidentSvc->fireIncident(Incident(name(), "BeginEvtLoop"));

        StatusCode sc = nextEvent(maxEvt);
        
        m_incidentSvc->fireIncident(Incident(name(), "EndEvtLoop"));

        return sc;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::seek(int evt) {
        evt = evt;
        m_log << MSG::ERROR
              << "seek() is not implemented for MergingEventLoopMgr"
              << endreq;
        return StatusCode::FAILURE;
    }
//------------------------------------------------------------------------------
    int MergingEventLoopMgr::curEvent() {
        return m_nEvt; 
    } 
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::initializeAlgorithms() {
        bool failed = false;

        ListAlg::iterator it;
        for(it = m_topAlgList.begin(); it != m_topAlgList.end(); ++it) {
            m_log << MSG::DEBUG << "Initializong algorithm: "
                  << (*it)->name() << endreq;

            if((*it)->sysInitialize().isFailure()) {
                m_log << MSG::ERROR << "Unable to initialize algorithm: "
                      << (*it)->name() << endreq;
                failed = true;
            } 
        }

        for(it = m_outStreamList.begin(); it != m_outStreamList.end(); ++it) {
            m_log << MSG::INFO << "Initialize Output Stream: "
                  << (*it)->name() << endreq;
            if((*it)->sysInitialize().isFailure()) {
                m_log << MSG::ERROR << "Unable to initialize Output Stream: "
                      << (*it)->name() << endreq;
                failed = true;
            }
        }


        return failed ? StatusCode::FAILURE : StatusCode::SUCCESS;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::beginRunAlgorithms() {
        bool failed = false;

        for(ListAlg::iterator it = m_topAlgList.begin(); it != m_topAlgList.end(); ++it) {
            m_log << MSG::DEBUG << "Calling sysBeginRun() for algorithm: "
                  << (*it)->name() << endreq;
            
            if((*it)->sysBeginRun().isFailure()) {
                m_log << MSG::ERROR << "Algorithm failed in sysBeginRun(): "
                      << (*it)->name() << endreq;
                failed = true;
            } 
        }

        return failed ? StatusCode::FAILURE : StatusCode::SUCCESS;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::endRunAlgorithms() {
        bool failed = false;

        for(ListAlg::iterator it = m_topAlgList.begin(); it != m_topAlgList.end(); ++it) {
            m_log << MSG::DEBUG << "Calling sysEndRun() for algorithm: "
                  << (*it)->name() << endreq;
            
            if((*it)->sysEndRun().isFailure()) {
                m_log << MSG::ERROR << "Algorithm failed in sysEndRun(): "
                      << (*it)->name() << endreq;
                failed = true;
            } 
        }

        return failed ? StatusCode::FAILURE : StatusCode::SUCCESS;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::executeAlgorithms() {
        StatusCode sc;

        for(ListAlg::iterator it = m_topAlgList.begin(); it != m_topAlgList.end(); ++it) {

            (*it)->resetExecuted();   

            sc = (*it)->sysExecute();
            if(sc.isFailure()) {
                m_log << MSG::ERROR << "Algorithm failed in sysExecute(): "
                      << (*it)->name() << endreq;
                return sc;
            } 
        }
        return sc;
    }
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::overwriteTriggerInfo(EventInfo *pFrom, EventInfo *pTo) {
        m_log << MSG::INFO << "Overwriting TriggerInfo..." << endreq;
        
        pTo->setTriggerInfo(new TriggerInfo(*pFrom->trigger_info()));
       
        return StatusCode::SUCCESS;
    } 
//------------------------------------------------------------------------------
    StatusCode MergingEventLoopMgr::loadObjects() {

        SG::DataProxy *proxy;
        std::vector<const SG::DataProxy *> proxies;
        std::vector<const SG::DataProxy *>::iterator it;
        
        proxies = m_primEvtStore->proxies();
        for(it = proxies.begin(); it != proxies.end(); ++it) {
            proxy = const_cast<SG::DataProxy *>(*it);
            proxy->accessData();
        }
        
        proxies = m_secEvtStore->proxies();
        for(it = proxies.begin(); it != proxies.end(); ++it) {
            proxy = const_cast<SG::DataProxy *>(*it);
            proxy->accessData();
        }
        

        return StatusCode::SUCCESS;
    }
    
    

    









//------------------------------------------------------------------------------
} // Namespace TrigSim
//------------------------------------------------------------------------------

