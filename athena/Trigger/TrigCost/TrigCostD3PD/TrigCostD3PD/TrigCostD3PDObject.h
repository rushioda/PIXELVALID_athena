// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//             Code produced by D3PDMakerReader
//
//  author: Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
// -------------------------------------------------------------
#ifndef D3PDREADER_TrigCostD3PDObject_H
#define D3PDREADER_TrigCostD3PDObject_H

// STL include(s):
#include <map>
#include <vector>
using std::vector;

// ROOT include(s):
#include <TObject.h>
#include <TString.h>

#include "D3PDObjectBase.h"
//#include "D3PDReadStats.h"
#include "VarHandle.h"
#include "UserD3PDObject.h"

class TTree;

namespace D3PDReader {

   /**
    * Code generated by CodeGenerator_v2 on:
    *  host = lxplus0164.cern.ch
    *  OS   = Linux / 2.6.32-431.11.2.el6.x86_64
    *  user = tamartin
    *  time = Fri Jun 27 16:52:19 2014
    */
   class TrigCostD3PDObject : public UserD3PDObject {

   public:
      /// Constructor used when reading from a TTree
      TrigCostD3PDObject( const ::Long64_t& master, const char* prefix = "TrigCostHLT_" );
      /// Constructor when the object is only used for writing data out
      TrigCostD3PDObject( const char* prefix = "TrigCostHLT_" );

      /// Get the currently configured prefix value
      virtual const char* GetPrefix() const;
      /// Set the prefix for the variables
      virtual void SetPrefix( const char* prefix );

      /// Connect the object to an input TTree
      virtual void ReadFrom( ::TTree* tree );
      /// Connect the object to an output TTree
      virtual void WriteTo( ::TTree* tree );

      /// Turn (selected) branches either on or off
      virtual void SetActive( ::Bool_t active = kTRUE,
                              const ::TString& pattern = ".*" );
      /// Read in all the variables that we need to write out as well
      virtual void ReadAllActive();

      /// Get the D3PD reading statistics
      //virtual D3PDReadStats GetStatistics() const;

      /// Set the contents of the object according to another object
      TrigCostD3PDObject& Set( const TrigCostD3PDObject& parent );

      /// Hash of AppId of the XPU node processing this event.
      VarHandle< unsigned int > appId;
      /// Enhanced bias weight of the event (only if specificaly calculated)
      VarHandle< float > ebWeight;
      VarHandle< unsigned int > ebWeightBG;
      VarHandle< unsigned char > ebUnbiased;
      /// If the event was a monitoring event
      VarHandle< unsigned char > ranScaleTools;
      /// BCID for this event
      VarHandle< unsigned int > bunchCrossingId;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > chain_counter;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_isExpressStream;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_isPassed;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_isPassedRaw;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_isPassthrough;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_isPrescaled;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_isResurrected;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > chain_level;
      VarHandle< unsigned short > chain_n;
      VarHandle< std::vector<float,std::allocator<float> >* > chain_timer;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_wasL1AfterPrescale;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_wasL1AfterVeto;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > chain_wasL1BeforePrescale;
      /// Sequential number of cost events processed
      VarHandle< float > costEvent;
      /// Nanosecond the data were saved by CostMon
      VarHandle< unsigned int > costRunNsec;
      /// Second the data were saved by CostMon
      VarHandle< unsigned int > costRunSec;
      /// Event number
      VarHandle< unsigned int > eventNumber;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > l1_ctpId;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > l1_isPrescaled;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > l1_isVetoed;
      VarHandle< unsigned short > l1_n;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > l1_passed;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > l1_passedAfterPrescale;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > l1_passedAfterVeto;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > l1_passedBeforePrescale;
      /// Lumi block number for this event
      VarHandle< unsigned int > lumi;
      VarHandle< float > lumiLength;
      /// Start of the event nanoseconds
      VarHandle< unsigned int > nanoSeconds;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > rob_data_id;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_data_isCached;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_data_isDisabled;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_data_isIgnored;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_data_isRetrieved;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_data_isStatusOk;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_data_isStatusPrefetched;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_data_isUnclassified;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > rob_data_n;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > rob_data_size;
      VarHandle< unsigned short > rob_n;
      VarHandle< std::vector<unsigned int,std::allocator<unsigned int> >* > rob_requestorId;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_sum_isCached;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_sum_isDisabled;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_sum_isIgnored;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_sum_isRetrieved;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > rob_sum_isUnclassified;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > rob_sum_n;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > rob_sum_nRob;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > rob_sum_size;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > rob_sum_subDet;
      VarHandle< std::vector<unsigned int,std::allocator<unsigned int> >* > rob_timeStartMicroSec;
      VarHandle< std::vector<unsigned int,std::allocator<unsigned int> >* > rob_timeStartSec;
      VarHandle< std::vector<unsigned int,std::allocator<unsigned int> >* > rob_timeStopMicroSec;
      VarHandle< std::vector<unsigned int,std::allocator<unsigned int> >* > rob_timeStopSec;
      VarHandle< std::vector<double,std::allocator<double> >* > rob_timer;
      VarHandle< std::vector<float,std::allocator<float> >* > roi_area;
      VarHandle< std::vector<float,std::allocator<float> >* > roi_eta;
      VarHandle< std::vector<unsigned int,std::allocator<unsigned int> >* > roi_id;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_isTypeEmTau;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_isTau; // new
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_isTypeEnergy;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_isTypeJet;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_isTypeJetEt;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_isTypeMuon;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_isTypeNone;
      VarHandle< unsigned short > roi_n;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_nL1Thresholds;
      VarHandle< std::vector<float,std::allocator<float> >* > roi_phi;
      // New
      VarHandle< std::vector<float,std::allocator<float> >* > roi_et;
      VarHandle< std::vector<float,std::allocator<float> >* > roi_etLarge;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_muCharge;
      VarHandle< std::vector<unsigned int,std::allocator<unsigned int> >* > roi_isoBits;
      VarHandle< std::vector<float,std::allocator<float> >* >  roi_vectorEX;
      VarHandle< std::vector<float,std::allocator<float> >* > roi_vectorEY;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_overflowEX;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_overflowEY;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > roi_overflowET;
      /// Run number for this event
      VarHandle< unsigned int > runNumber;
      /// Start of the event in seconds
      VarHandle< unsigned int > seconds;
      VarHandle< std::vector<double,std::allocator<double> >* > seq_algTotalTime;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > seq_alg_isCached;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > seq_alg_isCalled;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > seq_alg_n;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > seq_alg_position;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > seq_alg_roi_index;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > seq_alg_roi_n;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > seq_alg_timeStartMicroSec;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > seq_alg_timeStartSec;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > seq_alg_timeStopMicroSec;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > seq_alg_timeStopSec;
      VarHandle< std::vector<std::vector<double,std::allocator<double> >,std::allocator<std::vector<double,std::allocator<double> > > >* > seq_alg_timer;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > seq_channelCounter;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > seq_index;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > seq_isAlreadyExecuted;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > seq_isExecuted;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > seq_isInitial;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > seq_isPrevious;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > seq_level;
      VarHandle< unsigned short > seq_n;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > seq_roi;
      VarHandle< std::vector<float,std::allocator<float> >* > seq_timer;
      VarHandle< std::vector<std::vector<unsigned short,std::allocator<unsigned short> >,std::allocator<std::vector<unsigned short,std::allocator<unsigned short> > > >* > te_childIndex;
      VarHandle< std::vector<std::vector<unsigned int,std::allocator<unsigned int> >,std::allocator<std::vector<unsigned int,std::allocator<unsigned int> > > >* > te_clid;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > te_id;
      VarHandle< std::vector<unsigned short,std::allocator<unsigned short> >* > te_index;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isActiveState;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isErrorState;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isInitialTe;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isL1ThresholdTe;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isOutputEFNode;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isOutputL2Node;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isRegularTe;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isRoITe;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isTerminalNode;
      VarHandle< std::vector<unsigned char,std::allocator<unsigned char> >* > te_isTopologicalTe;
      VarHandle< unsigned short > te_n;
      VarHandle< std::vector<std::vector<unsigned short,std::allocator<unsigned short> >,std::allocator<std::vector<unsigned short,std::allocator<unsigned short> > > >* > te_parentIndex;
      VarHandle< std::vector<std::vector<unsigned char,std::allocator<unsigned char> >,std::allocator<std::vector<unsigned char,std::allocator<unsigned char> > > >* > te_roiId;
      /// Nominal time for this event
      VarHandle< float > timer;
      /// Detailed: Time of chain execution
      VarHandle< float > timerChainProcess;
      /// Detailed: Time to end of steering
      VarHandle< float > timerEndSteer;
      /// Detailed: Total time of monitoring tool(s) execution
      VarHandle< float > timerMon;
      /// Detailed: Time of ResultBuilder tool execution
      VarHandle< float > timerResultBuilder;
      /// Detailed: Time to run the OPI trig cost tools
      VarHandle< float > timerTrigCost;

   private:
      /// Function used internally to access the variables
      VarHandleBase* GetVarHandle( const char* name );
      /// Function setting up all the VarHandle members
      void SetVarHandles( const ::Long64_t* master );

      std::map< ::TString, VarHandleBase* > fHandles; ///< Book-keeper of the VarHandle members
      const ::Bool_t fFromInput; ///< Flag specifying if object is used for D3PD reading
      ::TString fPrefix; ///< Prefix to the branch names

      ClassDef( TrigCostD3PDObject, 0 )

   }; // class TrigCostD3PDObject

} // namespace D3PDReader

#endif // D3PDREADER_TrigCostD3PDObject_H