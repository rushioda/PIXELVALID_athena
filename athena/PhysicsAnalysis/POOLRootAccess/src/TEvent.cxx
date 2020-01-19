/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLRootAccess/TEvent.h"

#include "AthenaKernel/ICollectionSize.h"

#include "GaudiKernel/IEventProcessor.h"

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TChain.h"

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

namespace Gaudi {
  IAppMgrUI* Init() {
    return AAH::initGaudi("POOLRootAccess/basic.opts");
  }
}

namespace POOL {

IAppMgrUI* Init( const char* options ) {
  return AAH::initGaudi(options); //see AthAnalysisHelper
}

TEvent::~TEvent() {
  //need to destroy my storegate, selector, and loop
  //take refcounts down to 1 before handle release, to ensure services are destroyed
  if(m_evtLoop.isSet()) {
    //while(m_evtLoop->refCount()>1) m_evtLoop->release(); //CRASH
    m_evtLoop.release();
  }
  if(m_evtSelect.isSet()) {
    //while(m_evtSelect->refCount()>1) m_evtSelect->release();
    m_evtSelect.release();
  }
  if(m_evtStore.isSet()) {
    //while(m_evtStore->refCount()>1) m_evtStore->release();
    m_evtStore.release();
  }
}

TEvent::TEvent(const std::string& name ) : TEvent( kPOOLAccess , name ) { }

TEvent::TEvent(EReadMode mode, const std::string& name) : 
   m_joSvc("JobOptionsSvc","TEvent"+name),
   m_evtLoop("AthenaEventLoopMgr/"+name+"_EventLoopMgr","TEvent"+name),
   m_evtSelect("EventSelectorAthenaPool/"+name+"_EventSelector","TEvent"+name),
   m_evtStore("StoreGateSvc/"+name,"TEvent"+name),
   m_activeStoreSvc("ActiveStoreSvc","TEvent"+name),
   m_inputMetaStore("InputMetaDataStore","TEvent"+name) /*fixme, when reading multiple files at once?*/ {

   Gaudi::Init();

   //FIXME: Should protect against attempt to mix POOL with nonPOOL
   if(mode==kPOOLAccess) {
    //add the AthenaPoolAddressProviderSvc to ProxyProviderSvc
     ServiceHandle<IService> ppSvc("ProxyProviderSvc","TEvent"+name);
     AAH::setProperty( ppSvc , "ProviderNames", "['MetaDataSvc', 'AthenaPoolAddressProviderSvc']" );
   } else {
     //switch selector type to xAODEventSelector:
     m_evtSelect.setTypeAndName("Athena::xAODEventSelector/"+name+"_EventSelector");
   }

   //check if a SelectorType has been specified in the joSvc 
   //should retire this code at some point (hangover from basicxAOD.opts)
   auto properties = m_joSvc->getProperties("TEvent");
   if(properties) {
      for(auto prop : *properties) {
         if(prop->name()=="EventSelectorType") m_evtSelect.setTypeAndName(prop->toString() + "/" + name+"_EventSelector");
      }
   }

   AAH::setProperty( m_evtLoop , "ClearStorePolicy", "BeginEvent" );    //for interactive use of storegate
   AAH::setProperty( m_evtLoop , "EvtSel", m_evtSelect.typeAndName() ); //connect loop to selector
   AAH::setProperty( m_evtLoop , "EvtStore", m_evtStore.typeAndName() );//connect loop to store
   AAH::setProperty( m_evtLoop , "EventPrintoutInterval", 999999999 ); //disable printout (speeds up loop)

   if(m_evtSelect.type()=="Athena::xAODEventSelector") {
     AAH::setProperty( m_evtSelect , "ReadMetaDataWithPool" , true); //uses hybrid xAOD reading by default
     AAH::setProperty( m_evtSelect , "AccessMode" , int(mode) ); //sets the mode
   }

   //set outputlevels to WARNING 
   AAH::setProperty( m_evtLoop, "OutputLevel", 4 );
   AAH::setProperty( m_evtSelect, "OutputLevel", 4 );
   AAH::setProperty( m_evtStore, "OutputLevel", 4 );
   AAH::setProperty( m_activeStoreSvc, "OutputLevel", 4 );

   //suppress messages below WARNING too
   //do this here to stop some pre initialize INFO messages from showing
   ServiceHandle<IProperty> messageSvc("MessageSvc","");
   std::vector<std::string> p;
   Gaudi::Parsers::parse(p,messageSvc->getProperty("setWarning").toString());
   p.push_back( m_evtLoop.name() );
   p.push_back( m_evtSelect.name() );
   p.push_back( m_evtStore.name() );
   p.push_back( m_activeStoreSvc.name() );
   messageSvc->setProperty(  StringArrayProperty( "setWarning" , p ) );
   
   //also push this into the joboptionsvc so that if a reinit happens then these settings are remembered 
   //m_joSvc->addPropertyToCatalogue( "MessageSvc", StringArrayProperty( "setWarning", p ) );
   
}


void TEvent::setActive() {
  m_activeStoreSvc->setStore(&*m_evtStore);
}

long TEvent::getEntries() {
   if(m_size>=0) return m_size;
   if(!m_evtLoop.isSet()) { if(m_evtLoop.retrieve().isFailure()) return 0; }
   m_size = dynamic_cast<ICollectionSize*>(&*m_evtLoop)->size();
   return m_size;
}

StatusCode TEvent::readFrom( TFile* file ) {
   if(file==0 || file->IsZombie()) return StatusCode::FAILURE;
   return readFrom( file->GetName() );
}

StatusCode TEvent::readFrom( const char* file ) {
   if(m_curEntry >= 0) {
      std::cout << "Unable to change file after already reading" << std::endl;
      return StatusCode::FAILURE;
   }
   std::vector<std::string> myFiles;
   //see if contains wildcard
   //separate by comma
   TString sFileIn(file);
   std::unique_ptr<TObjArray> sFiles(sFileIn.Tokenize(","));
   std::unique_ptr<TIterator> itr(sFiles->MakeIterator());
   TObject* obj;
   while( (obj = itr->Next()) ) {
     TString sFile(static_cast<TObjString*>(obj)->String());
     if(sFile.Contains('*')) {
      std::unique_ptr<TObjArray> theFiles(gSystem->GetFromPipe(("ls " + std::string(file)).c_str()).Tokenize("\n"));
      for(int i=0;i<theFiles->GetEntries();i++) {
         //std::cout << "Adding " << dynamic_cast<TObjString*>(theFiles->At(i))->String().Data() << std::endl;
	myFiles.push_back(gSystem->ExpandPathName(dynamic_cast<TObjString*>(theFiles->At(i))->String().Data()));
      }
     } else {
       myFiles.push_back( gSystem->ExpandPathName(sFile.Data()) );
     }
   }

   return AAH::setProperty( m_evtSelect , "InputCollections" , myFiles );
}

StatusCode TEvent::readFrom(TChain* files) {

  //add all files to the property list by giving a comma separated list of files
  std::string sfiles;
  std::unique_ptr<TIterator> itr(files->GetListOfFiles()->MakeIterator());
  TObject* obj;
  bool hasFile(false);
  while( (obj = itr->Next()) ) {
    if(hasFile) sfiles += ",";
    sfiles += obj->GetTitle();
    hasFile=true;
  }
  //std::cout << sfiles << std::endl;
  return readFrom(sfiles.c_str());

}

int TEvent::getEntry( long entry ) {
   if(!m_evtLoop.isSet()) { if(m_evtLoop.retrieve().isFailure()) return -1; }
   if(entry==0 || entry != m_curEntry+1) m_evtLoop->seek(entry); //need to seek on first read or if not reading next event
   //ensure our storegate is the active store
   setActive();
   StatusCode out = dynamic_cast<IEventProcessor*>(&*m_evtLoop)->nextEvent(entry+1);
   m_curEntry = entry;
   return (out.isSuccess()) ? 0 : -1;
}


  //python bindings

  PyObject* TEvent::retrieve( PyObject* tp, PyObject* pykey )
 {
   return AthenaInternal::retrieveObjectFromStore( &*evtStore() , tp, pykey );
 }

  PyObject* TEvent::get_item( PyObject* pykey ){
    return AthenaInternal::py_sg_getitem( &*evtStore() , pykey );
  }
  PyObject* TEvent::contains( PyObject* tp, PyObject* pykey ) {
    return AthenaInternal::py_sg_contains( &*evtStore() , tp,  pykey );
  }
  PyObject* TEvent::retrieveMetaInput(PyObject* tp, PyObject* pykey) {
    return AthenaInternal::retrieveObjectFromStore( &*inputMetaStore() , tp, pykey );
  }
  PyObject* TEvent::record  (PyObject* obj, PyObject* pykey,bool allowMods,bool resetOnly,bool noHist) {
    return AthenaInternal::recordObjectToStore( &*evtStore() , obj, pykey,allowMods,resetOnly,noHist );
  }



} //end of POOL namespace
