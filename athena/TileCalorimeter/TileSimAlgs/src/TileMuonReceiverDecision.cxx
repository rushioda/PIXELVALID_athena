/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuonReceiverDecision.cxx
// Author   : Joao Gentil Mendes Saraiva (jmendes@cern.ch)
// Created  : December 2013
//
// DESCRIPTION
// 
// Tile Muon Receiver decision
//
// Properties (JobOption Parameters):
//
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Calo includes

#include "CaloIdentifier/TileID.h"
#include "TileConditions/TileInfo.h"
// Tile includes

#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCondToolEmscale.h"

#include "TileSimAlgs/TileMuonReceiverDecision.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileMuonReceiverObj.h"

TileMuonReceiverDecision::TileMuonReceiverDecision(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_MuRcvRawChContainer("MuRcvRawChCnt"),
    m_TileMuRcvContainer("TileMuRcvCnt"),
    m_tileID(0),
    m_tileHWID(0),
    m_cablingService(0),
    m_tileInfo(0),
    m_tileToolEmscale("TileCondToolEmscale")
{
  // declare properties...
  declareProperty( "MuonReceiverEneThreshCellD6Low"       , m_threshold_d6_lo   = 500. , "Setting the lowest trigger threshold for cell D6 in MeV (Def=500 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6andD5Low"  , m_threshold_d5d6_lo = 500. , "Setting the lowest trigger threshold for cell D5+D6 in MeV (Def=500 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6High"      , m_threshold_d6_hi   = 600. , "Setting the highest trigger threshold for cell D6 in MeV (Def=600 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6andD5High" , m_threshold_d5d6_hi = 600. , "Setting the highest trigger threshold for cell D5+D6 in MeV (Def=600 MeV)");
  declareProperty( "SelectionCutForMatchedFilterQf"       , m_selCutQf=0.              , "Selection cut for the quality factor of the matched filters");
  declareProperty( "TileMuonReceiverContainer"            , m_TileMuRcvContainer       , "Tile Calorimeter decision to TGC Sector Logic");
  declareProperty( "TileCondToolEmscale"                  , m_tileToolEmscale          , "Service to calibrate all channels");
  declareProperty( "TileInfoName"                         , m_infoName = "TileInfo" );
}
 
TileMuonReceiverDecision::~TileMuonReceiverDecision()
{
}

StatusCode TileMuonReceiverDecision::initialize() {

  m_cablingService = TileCablingService::getInstance();

  if (! m_cablingService->isRun2Cabling() ) {
     ATH_MSG_INFO("TileMuonReceiverDecision should not be used for RUN1 simulations");
     return StatusCode::SUCCESS;
  } else {
     ATH_MSG_INFO("Initializing TileMuonReceiverDecision");
  }

  //=== retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );
  CHECK(detStore()->retrieve(m_tileInfo, m_infoName));

  CHECK(m_tileToolEmscale.retrieve());

  ATH_MSG_INFO("TileMuonReceiverDecision initialization completed" );
  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverDecision::execute() {
  
  if (! m_cablingService->isRun2Cabling() ) {
     ATH_MSG_VERBOSE( "ATT: RUN1 settings TileMuonReceiverDecision will end now" );
     return StatusCode::SUCCESS;
  } else {
     ATH_MSG_VERBOSE( "ATT: RUN2 settings TileMuonReceiverDecsionTool will run now" );
     ATH_MSG_DEBUG( "Executing TileMuonReceiverDecision" );
  }

  // Get the container with the matched filter reconstructed raw channels in MeV
  //
  const TileRawChannelContainer * MuonReceiverRawChannelContainer;
  CHECK( evtStore()->retrieve(MuonReceiverRawChannelContainer, m_MuRcvRawChContainer) ); 
  
  // The collection of raw channel containers are the drawers of extended barrel
  //
  TileRawChannelContainer::const_iterator collItr  = MuonReceiverRawChannelContainer->begin();
  TileRawChannelContainer::const_iterator lastColl = MuonReceiverRawChannelContainer->end();

  // Vectors for managemnt for TMDB 2015 configuration with inclusion in trigger in 1.1<eta<1.3
  //
  std::vector<bool>  tile2SL(4,false);
  std::vector<float> ene(2,0.0);
  std::vector<float> time(2,0.0);
  std::vector<float> thresholds;

  thresholds.push_back(m_threshold_d5d6_hi);
  thresholds.push_back(m_threshold_d5d6_lo);
  thresholds.push_back(m_threshold_d6_hi);
  thresholds.push_back(m_threshold_d6_lo);

  // Create the container to store the decision from the algorithm
  //  
  TileMuonReceiverContainer * decisionContainer = new TileMuonReceiverContainer();  

  TileMuonReceiverObj * TileMuRcvObj = new TileMuonReceiverObj(0,thresholds); // Special object with thresholds
  decisionContainer->push_back(TileMuRcvObj);

  // Conversion from TMDB channel number the index to channel number in a drawer
  //
#define nEBchan 6
#define nLBchan 9
#define maxCell 5
  // EB: D5(L,R),D6(L,R),D4(L,R)
  int EBchan[nEBchan]={17,16,37,38,3,2};
  // LB: D0,D1(L,R),D2(L,R),D3(L,R),BC8(L,R)
  int LBchan[nLBchan]={0,13,14,25,24,41,44,39,40};
  // Auxiliary array for cell index
  int EBIdCell[nEBchan]={0,0,1,1,2,2};
  int LBIdCell[nLBchan]={0,1,1,2,2,3,3,4,4};
  // Used for validation only not including in container at the moment   
  float energy_HLX[maxCell]={0.,0.,0.,0.,0.};
  float time_HLX[maxCell]={0.,0.,0.,0.,0.};

  for ( ; collItr != lastColl; ++collItr ) {

    TileRawChannelCollection::const_iterator chanItr  = (*collItr)->begin();
    TileRawChannelCollection::const_iterator lastChan = (*collItr)->end();

    if (chanItr==lastChan) continue;
    int frag_id   = (*collItr)->identify();
    int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(frag_id);
    int ros       = frag_id>>8;
    bool eb_ros   = ((ros == TileHWID::EXTBAR_POS) || (ros == TileHWID::EXTBAR_NEG));
    int upperLim  = (eb_ros) ? nEBchan : nLBchan;

    if (msgLvl(MSG::VERBOSE)) {
      int drawer = (frag_id&0xFF);
      memset(energy_HLX,0,sizeof(energy_HLX));
      memset(time_HLX,0,sizeof(time_HLX));
      msg(MSG::VERBOSE) << "(E.0.0) Frag_id: 0x"<< MSG::hex << frag_id << MSG::dec <<" ros: "<< ros <<" drawer: "<< drawer << endreq;
    }

    float energy_d6   = 0.0;
    float energy_d5d6 = 0.0;
    float time_d6     = 0.0;
    float time_d5d6   = 0.0;
  
    int ich   = 0;
    int jch6  = 0;
    int jch56 = 0;

    for ( ; chanItr != lastChan; ++chanItr ) {
       
      ++ich;
      // For TMDB channel numbers are being set differently (17,16,37,38)->(D5L,D5r,D6L,D6R)->(0,1,2,3)
      HWIdentifier adc_id = (*chanItr)->adc_HWID() ;
      // TMDB channel is used in COOL and goes from 0..n with n=5 for EB and n=8 in LB
      int TMDBchan = m_tileHWID->channel(adc_id) ;
      if ( TMDBchan >= upperLim ) {
        msg(MSG::WARNING) << "(E.1."<< ich <<") hwid: "<< m_tileHWID->to_string(adc_id,-1) <<" ch: "<< TMDBchan <<" --> Tile ch: UNKNOWN" << endreq ;
        continue;
      }
      // TILE channel is the Tile HW channel
      int TILEchan = (eb_ros) ? EBchan[TMDBchan] : LBchan[TMDBchan];
      
      float ADC2MeV_factor = m_tileToolEmscale->channelCalib(drawerIdx, TILEchan, TileID::LOWGAIN, 1.
                                                                      , TileRawChannelUnit::PicoCoulombs
                                                                      , TileRawChannelUnit::MegaElectronVolts) 
                           / m_tileInfo->MuRcvCalib(adc_id);

      float energy = (*chanItr)->amplitude()*ADC2MeV_factor;
      float time   = (*chanItr)->time();

      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "(E.1."<< ich <<") hwid: "<< m_tileHWID->to_string(adc_id,-1) <<" ch: "<< TMDBchan <<" --> Tile ch: "<< TILEchan << endreq ;
        msg(MSG::DEBUG) << "        E[ADC]: "<<(*chanItr)->amplitude()<<" E[MeV]: "<<energy<<" t[ns]: "<<time<<" QF: "<<(*chanItr)->quality() << endreq;
      }

      if ( eb_ros ) {
        if ( TMDBchan<4 ) {
          energy_d5d6 += energy;
          time_d5d6   += time;
          ++jch56;

          if ( TMDBchan==2 || TMDBchan==3 ) { /* choose d6 cell */
            energy_d6 += energy;
            time_d6   += time;
            ++jch6;
          }
        }
        if (msgLvl(MSG::VERBOSE)) {
          energy_HLX[EBIdCell[TMDBchan]] += energy;
          time_HLX[EBIdCell[TMDBchan]]   += time;
        }
      } else {
        if (msgLvl(MSG::VERBOSE)) {
          energy_HLX[LBIdCell[TMDBchan]] += energy;
          time_HLX[LBIdCell[TMDBchan]]   += time;
        }
      }
    }

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "(X.0.0)   Summary of the exteded results for HL-LHC: " << endreq;
      if ( eb_ros ) {
        msg(MSG::VERBOSE) << "(X.1.0)     Energy  D-5 "<<energy_HLX[0]<<" D-6 "<<energy_HLX[1]<<" D-4 "<<energy_HLX[2]<< endreq;
        msg(MSG::VERBOSE) << "(X.2.0)     Time    D-5 "<<time_HLX[0]/2.<<" D-6 "<<time_HLX[1]/2.<<" D-4 "<<time_HLX[2]/2.<< endreq;
      } else {
        msg(MSG::VERBOSE) << "(X.1.0)     Energy  D-0 "<<energy_HLX[0]<<" D-1 "<<energy_HLX[1]<<" D-2 "<<energy_HLX[2]<<" D-3 "<<energy_HLX[3]<<" BC-8 "<<energy_HLX[4]<< endreq;
        msg(MSG::VERBOSE) << "(X.2.0)     Time    D-0 "<<time_HLX[0]<<" D-1 "<<time_HLX[1]/2.<<" D-2 "<<time_HLX[2]/2.<<" D-3 "<<time_HLX[3]/2.<<" BC-8 "<<time_HLX[4]/2.<< endreq;
      }
    }
    
    if (jch56 == 0) { // neigher D5 nor D6 found - nothing to do
      ATH_MSG_VERBOSE( "== NO trigger for this drawer " );
      continue; 
    }
    
    if (jch56>1) {
      time_d5d6 /= jch56;
      if (jch6>1) time_d6 /= jch6;
    }

    // A. Above threshold (hi and lo) d5+d6 
    //
    bool pass_d5d6_hi = (energy_d5d6>m_threshold_d5d6_hi);
    bool pass_d5d6_lo = (energy_d5d6>m_threshold_d5d6_lo);

    //                    (hi and lo) d6
    //
    bool pass_d6_hi = (energy_d6>m_threshold_d6_hi);
    bool pass_d6_lo = (energy_d6>m_threshold_d6_lo);

    // B. Fill a vector with size 4 reproducing the 4-bit word with the structure HLHL:[d5+d6][d6]
    //    vector at 0 (d56hi) is the most significant digit in the 4 bits output
    //    vector at 3 (d6lo) is the least significant digit in the 4 bits output
    //
    tile2SL[0] = pass_d5d6_hi;
    tile2SL[1] = (!pass_d5d6_hi && pass_d5d6_lo);
    tile2SL[2] = pass_d6_hi;
    tile2SL[3] = (!pass_d6_hi && pass_d6_lo);

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "(E.2.0)   Summary: e(d5+d6)= " << energy_d5d6 << " e(d6)= " << energy_d6 << endreq;
      msg(MSG::VERBOSE) << "(E.3.0)   Thresholds: " << m_threshold_d5d6_lo << " " << m_threshold_d5d6_hi << " " << m_threshold_d6_lo << " " << m_threshold_d6_hi << endreq;
      msg(MSG::VERBOSE) << "(E.4.0)   Checking which tresholds have been passed: d56 high " << pass_d5d6_hi << " d56 low " << pass_d5d6_lo << " d6 high " << pass_d6_hi << " d6 low " << pass_d6_lo << endreq;
      msg(MSG::VERBOSE) << "(E.5.0)   Output to SL: " << tile2SL[0] << tile2SL[1] << tile2SL[2] << tile2SL[3] << endreq;
    }
    
    if (tile2SL[0] || tile2SL[1] || tile2SL[2] || tile2SL[3]) {

      ene[0]  = energy_d5d6;
      ene[1]  = energy_d6;
      time[0] = time_d5d6;
      time[1] = time_d6;

      TileMuonReceiverObj * TileMuRcvObj = new TileMuonReceiverObj(frag_id,ene,time,tile2SL);
      decisionContainer->push_back(TileMuRcvObj);
      
    } else {
      ATH_MSG_VERBOSE( "== NULL trigger not include in container " );
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "== Print TileD decision container output to SL" << endreq ; 
    decisionContainer->print();
  }
  CHECK(evtStore()->record( decisionContainer, m_TileMuRcvContainer, false));

  ATH_MSG_DEBUG("TileMuonReceiverDecision execution completed" );
  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverDecision::finalize() {
  ATH_MSG_INFO( "TileMuonReceiverDecision finalized successfully");
  return StatusCode::SUCCESS;
}
