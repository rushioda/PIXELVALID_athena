/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALCPED_H
#define CSCCALCPED_H
/**CscCalcPed - algorithm that finds the Cathode Strip Chamber pedestals from 
an RDO
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/DataHandle.h"

#include "GaudiKernel/ToolHandle.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "DataModel/DataVector.h"

#include <vector>
#include <bitset>

#include "CscCalibData/CscCalibResultCollection.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2F.h"
#include "MuonCondInterface/CscICoolStrSvc.h"

class cscIdHelper;
class TFile;
class IdentifierHash;
//class ICscCalibTool;

namespace Muon {
  class ICSC_RDO_Decoder;
}

namespace MuonCalib{
  /** 
    @class CscCalcPed

    @brief does calibration of the CSC chambers

    @author lampen@physics.arizona.edu

    @section Description
    CscCalcPed is an algorithm that cycles through calibration events and generates
    the pedestals. A root file is also generated where the user can	view the validity of the constants.
   */

  class CscCalcPed: public AthAlgorithm
  {
    public:
      CscCalcPed(const std::string& name, ISvcLocator* pSvcLocator);
      ~CscCalcPed(void);

      /**basic required functions*/
      StatusCode initialize(void);
      StatusCode execute(void);
      StatusCode finalize(void); 

    private:

      /***********Private member functions*/
      /**event loop functions*/
      StatusCode collectEventInfo();
      /**Finalize functions*/
      StatusCode calculateParameters();
      StatusCode writeCalibrationFile();
      StatusCode storeGateRecord();
      DataVector<TH2F> * makeBitCorrelation();

      StatusCode calOutput0();
      StatusCode calOutput1();
      StatusCode calOutput3();
      void outputParameter3(const CscCalibResultCollection & results, std::ofstream & out);

    /**Utility functions*/
      StatusCode hashToChamberName(IdentifierHash,std::string);		
      StatusCode fillBitHist(TH1I * bitHist, const uint16_t & val, TH2F* bitProds);
      template <typename dataType> dataType GetMinMax(std::vector<dataType> & vec) {

        typename std::vector<dataType>::const_iterator itr = vec.begin();
        typename std::vector<dataType>::const_iterator end = vec.end();
        if(itr == end)
          return 0;
        dataType max =*itr;
        dataType min =*itr;
        itr++;
        for(; itr != end ; ++itr) {
          if(*itr < min)
            min = *itr;
          if(*itr > max)
            max = *itr;
        }
        return max - min;
      }
  
      StatusCode onlineToOfflineHashId(const unsigned int & onlineId, unsigned int &hashId) const;

      /*********Private member variables*/
      /**Services and tools*/
      StoreGateSvc * m_storeGate;
      StoreGateSvc* m_detStore;

      // ITHistSvc * m_thistSvc;
    //      ICscCalibTool * m_cscCalibTool;
      const CscIdHelper *m_cscId;
      const MuonGM::MuonDetectorManager * m_muon_mgr;
      IChronoStatSvc* m_chronoSvc;
      ServiceHandle<CscICoolStrSvc> m_cscCoolStrSvc;
      ToolHandle<Muon::ICSC_RDO_Decoder> m_cscRdoDecoderTool;
    

    /**Parameters input through joboptions*/
      std::string m_outputFileName;
      std::string m_titlePrefix, m_titlePostfix;
      std::string m_calOutputVersion;
      bool m_doCorrelation;


      //bool m_makeHists, m_dumpAllHists;
      float m_thresholdMultiplier;
      int m_expectedChamberLayer;

      std::string m_onlineDbFile; //!< filename for file with online database information

      /**Internally global variables*/
      unsigned int m_maxStripHash;

      unsigned int m_ampHistLowBound, m_ampHistHighBound, m_ampHistNumBins; 

      const unsigned int m_numBits;

      bool m_doBitHists;    
      bool m_doSampleHists;

      int m_numSamplesExpected;

      bool m_doF001;

      DataVector<TH1I> * m_ampHists;
      DataVector< DataVector<TH1I> > * m_sampHists;
      DataVector<TH1I> * m_bitHists;
      DataVector<TH2F> * m_bitProds;
      DataVector<TH1F> * m_bitCorrelation;
      std::vector<int> m_onlineThresholds;
      std::vector<int> m_onlineThresholdFailureCount;
      CscCalibResultCollection * m_peds;
      CscCalibResultCollection * m_noises;
      CscCalibResultCollection * m_rmses;
      CscCalibResultCollection * m_f001s;
      CscCalibResultCollection * m_onlineTHoldBreaches;
      int m_eventCnt;	
      double * m_crossTalkFix;

      //String for interface to patch changes
      std::string m_cmt_parameter;

      //debug level
      bool m_debug, m_verbose;

      bool m_doOnlineDbFile;

  };//end class CscCalcPed


  inline StatusCode CscCalcPed::fillBitHist(TH1I * bitHist, const uint16_t & val, TH2F* bitProds)
  {
    if(!bitHist)
      return StatusCode::RECOVERABLE;

    //Num bits should always be m_numBits
    std::bitset<12> bitVal(val);

    for(unsigned int bitIndex = 0; bitIndex < m_numBits; bitIndex++){
      if(bitVal[bitIndex]){
        bitHist->Fill(bitIndex);
        if(bitProds){
          for(unsigned int bitIndex2 = 0 ; bitIndex2 <= bitIndex ; bitIndex2++) {
            if(bitVal[bitIndex2]){
              bitProds->Fill(bitIndex,bitIndex2);
              if(bitIndex != bitIndex2)
                bitProds->Fill(bitIndex2,bitIndex);
            }

          }
        }
      }

    }


    return StatusCode::SUCCESS;
  }

}//end namespace:

#endif
