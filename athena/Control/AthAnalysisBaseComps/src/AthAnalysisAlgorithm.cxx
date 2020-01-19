///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisAlgorithm.cxx 
// Implementation file for class AthAnalysisAlgorithm
// Exactly like an AthAlgorithm except also has Metadata accessors
// and beginInputFile method
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "TROOT.h"

AthAnalysisAlgorithm::AthAnalysisAlgorithm( const std::string& name, 
			    ISvcLocator* pSvcLocator,
			    const std::string& ) : 
        AthHistogramAlgorithm(name,pSvcLocator),
        m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name ),
        m_outputMetaStore( "StoreGateSvc/MetaDataStore", name )
{
  
}

AthAnalysisAlgorithm::~AthAnalysisAlgorithm() { }


ServiceHandle<StoreGateSvc>& AthAnalysisAlgorithm::inputMetaStore() const {
      return m_inputMetaStore;
}

ServiceHandle<StoreGateSvc>& AthAnalysisAlgorithm::outputMetaStore() const {
      return m_outputMetaStore;
}


StatusCode AthAnalysisAlgorithm::sysInitialize() {

      // Connect to the IncidentSvc:
      ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
      ATH_CHECK( incSvc.retrieve() );

      // Set up the right callbacks: //but ensure we don't double-register if sysInitialize called twice (appears to be the case)
      incSvc->removeListener( this, IncidentType::BeginInputFile );
      incSvc->addListener( this, IncidentType::BeginInputFile, 0, true );

      // Let the base class do its thing:
      ATH_CHECK( AthHistogramAlgorithm::sysInitialize() );

      // Return gracefully:
      return StatusCode::SUCCESS;
}

StatusCode AthAnalysisAlgorithm::sysExecute() {
  if(!m_doneFirstEvent) {
    m_doneFirstEvent=true;
    if( firstExecute().isFailure() ) {
      ATH_MSG_FATAL("Failure in firstEvent method");
      return StatusCode::FAILURE;
    }
  }
  return AthHistogramAlgorithm::sysExecute();
}

void AthAnalysisAlgorithm::handle( const Incident& inc ) {

   // Tell the user what's happening:
   ATH_MSG_VERBOSE( "Callback received with incident: " << inc.type() );

   // Call the appropriate member function:
   if( inc.type() == IncidentType::BeginInputFile ) {
      m_currentFile=0;
      if( beginInputFile().isFailure() ) {
         ATH_MSG_FATAL( "Failed to call beginInputFile()" );
         throw std::runtime_error( "Couldn't call beginInputFile()" );
      }
   } else {
      ATH_MSG_WARNING( "Unknown incident type received: " << inc.type() );
   }

   return;
}

/// Dummy implementation that can be overridden by the derived tool.
///
StatusCode AthAnalysisAlgorithm::beginInputFile() {

   // Return gracefully:
   return StatusCode::SUCCESS;
}


/// Dummy implementation that can be overridden by the derived tool.
///
StatusCode AthAnalysisAlgorithm::firstExecute() {

   // Return gracefully:
   return StatusCode::SUCCESS;
}



TFile* AthAnalysisAlgorithm::currentFile(const char* evtSelName) {
   if(m_currentFile) return m_currentFile;

   //get the EventSelector so we can get it's list of input files
   ServiceHandle<IProperty> evtSelector(evtSelName,name());

   if(evtSelector.retrieve().isFailure()) {
      ATH_MSG_ERROR("Couldn't find the service: " << evtSelName);return 0;
   }

   //get the list of input files - use this to determine which open file is the current input file
   const StringArrayProperty& inputCollectionsName = dynamic_cast<const StringArrayProperty&>(evtSelector->getProperty("InputCollections"));
   ATH_MSG_VERBOSE("nOpenFile=" << gROOT->GetListOfFiles()->GetSize() << ". nFilesInInputCollection=" << inputCollectionsName.value().size());
   if(msgLvl(MSG::VERBOSE)) {
      for(int i=0;i<gROOT->GetListOfFiles()->GetSize();i++) {
         ATH_MSG_VERBOSE("Open file: " << gROOT->GetListOfFiles()->At(i)->GetName());
      }
   }

   //look through list of files and find the one from the input collection that is currently open

   for(int i=0;i<gROOT->GetListOfFiles()->GetSize();i++) {
         TFile *g = (TFile*)gROOT->GetListOfFiles()->At(i);
         //see if this file is in the input file list
         //strip everything except stuff either side of last /
         TString s(g->GetName());
         TObjArray* tokens = s.Tokenize("/");
         TObjString* lastToken = dynamic_cast<TObjString*>(tokens->Last());
         TString sToCompare("");
         bool shortComparison(false);
         if(tokens->GetEntries()>1) {
            TString beforeSlash((dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-2)))->GetString());
            if(beforeSlash.Length()>0) sToCompare += beforeSlash;
            sToCompare += "/";
         } else {
            shortComparison=true;
         }
         sToCompare += lastToken->GetString();
         TString sToCompare_short(lastToken->GetString()); //short versions search
         delete tokens;
//         ATH_MSG_VERBOSE("Look at " << sToCompare);
         for(unsigned int j=0;j<inputCollectionsName.value().size();j++) {
            TString t(inputCollectionsName.value()[j].c_str());
            //try perfect match first
            if(s.EqualTo(t)) {
               ATH_MSG_VERBOSE("Current File is: " << inputCollectionsName.value()[j]);
               m_currentFile = g;
               return g;
            }
            TObjArray* tokens = t.Tokenize("/");
            TObjString* lastToken = dynamic_cast<TObjString*>(tokens->Last());
            TString tToCompare = "";
            bool shortComparison2(false);
            if(tokens->GetEntries()>1) {
               TString beforeSlash((dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-2)))->GetString());
               if(beforeSlash.Length()>0) tToCompare += beforeSlash;
               tToCompare += "/";
            } else {
               shortComparison2=true;
            }
            tToCompare += lastToken->GetString();
            TString tToCompare_short(lastToken->GetString());
            delete tokens;
            //ATH_MSG_VERBOSE("cf with : " << inputCollectionsName.value()[j]);
            if(shortComparison || shortComparison2) { //doing short version search, no directories to distinguish files!
               if(sToCompare_short.EqualTo(tToCompare_short)) {
                  ATH_MSG_VERBOSE("Current File is: " << inputCollectionsName.value()[j]);
                  m_currentFile = g;
                  return g;
               }
            } else
            if(sToCompare.EqualTo(tToCompare)) {
               ATH_MSG_VERBOSE("Current File is: " << inputCollectionsName.value()[j]);
               m_currentFile=g;
               return g;
            }
         }
   } 
   ATH_MSG_ERROR("Could not find the current file!");
   return 0; //something went wrong :-(

}
