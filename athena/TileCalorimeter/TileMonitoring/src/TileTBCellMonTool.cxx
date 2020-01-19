/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// ********************************************************************
//
// NAME:     TileTBCellMonTool.cxx
// PACKAGE:  TileMonitoring
//
// AUTHORS:   Alexander Solodkov
//            Luca Fiorini (Luca.Fiorini@cern.ch)
//
// ********************************************************************

#include "TileMonitoring/TileTBCellMonTool.h"
#include "TileMonitoring/PairBuilder.h"

#include "xAODEventInfo/EventInfo.h"

#include "CaloEvent/CaloCellContainer.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileCell.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TString.h"
#include "TMath.h"
#include "TROOT.h"
#include "TF1.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <sstream>
#include <iomanip>
#include <map>

//#include "langaufun.C"

using CLHEP::GeV;
using CLHEP::ns;

/*---------------------------------------------------------*/
TileTBCellMonTool::TileTBCellMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileChannelTimeLBC01{}
  , m_tileChannelTimeEBC02{}
  , m_tileChannelEnergyLBC01{}
  , m_tileChannelEnergyEBC02{}
  , m_tileChannelEnergyVsTimeLBC01{}
  , m_tileChannelEnergyVsTimeEBC02{}
  , m_tileTBTotalEnergyEBA(0)
  , m_tileTBHitMapEBA(0)
  , m_tileTBCellEneSumEBA{}
  , m_tileTBCellEneDiffEBA{}
  , m_tileTBCellTimeSumEBA{}
  , m_tileTBCellTimeDiffEBA{}
  , m_tileTBTotalEnergyEBC(0)
  , m_tileTBHitMapEBC(0)
  , m_tileTBCellEneSumEBC{}
  , m_tileTBCellEneDiffEBC{}
  , m_tileTBCellTimeSumEBC{}
  , m_tileTBCellTimeDiffEBC{}
  , m_tileTBTotalEnergyLBA(0)
  , m_tileTBHitMapLBA(0)
  , m_tileTBCellEneSumLBAD0(0)
  , m_tileTBCellEneSumLBA{}
  , m_tileTBCellEneDiffLBA{}
  , m_tileTBCellTimeSumLBAD0(0)
  , m_tileTBCellTimeSumLBA{}
  , m_tileTBCellTimeDiffLBA{}
  , m_tileTBTotalEnergyLBC(0)
  , m_tileTBHitMapLBC(0)
  , m_tileTBCellEneSumLBCD0(0)
  , m_tileTBCellEneSumLBC{}
  , m_tileTBCellEneDiffLBC{}
  , m_tileTBCellTimeSumLBCD0(0)
  , m_tileTBCellTimeSumLBC{}
  , m_tileTBCellTimeDiffLBC{}
  , m_cellHitMapEB{}
  , m_cellHitMapLB{}
  , m_cellHitMapLB_C{}

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("energyThreshold", m_energyThreshold = 300.); //Threshold in MeV
  declareProperty("energyThresholdForTime", m_energyThresholdForTime = 0.1); //Threshold in MeV
  declareProperty("negEnergyThreshold", m_negThreshold = -2000.); //Threshold in MeV
  declareProperty("cellsContainerName", m_cellsContName = "AllCalo"); //SG Cell Container
  declareProperty("energyBalanceThreshold", m_eneBalThreshold = 3.); //Threshold is in the energy ratio
  declareProperty("TimeBalanceThreshold", m_timBalThreshold = 25.); //Threshold in ns
  declareProperty("doOnline", m_doOnline = false); //online mode
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("FillTimeHistograms", m_fillTimeHistograms = false);

  //declareProperty("FillTimeHistograms"     , m_fillTimeHistograms = false);



}

/*---------------------------------------------------------*/
TileTBCellMonTool::~TileTBCellMonTool() {
  /*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileTBCellMonTool::initialize() {
  /*---------------------------------------------------------*/

  ATH_MSG_INFO("in initialize()");
  //=== get TileBadChanTool
  //E3
  m_cellHitMapEB[0][0] = -1;
  m_cellHitMapEB[0][1] = -1;
  m_cellHitMapEB[0][2] = -1;
  m_cellHitMapEB[0][3] = -1;
  m_cellHitMapEB[0][4] = -1;
  //E4
  m_cellHitMapEB[1][0] = -1;
  m_cellHitMapEB[1][1] = -1;
  m_cellHitMapEB[1][2] = -1;
  m_cellHitMapEB[1][3] = -1;
  m_cellHitMapEB[1][4] = -1;
  //D4
  m_cellHitMapEB[2][0] = 1;
  m_cellHitMapEB[2][1] = 5;
  m_cellHitMapEB[2][2] = 3;
  m_cellHitMapEB[2][3] = 5;
  m_cellHitMapEB[2][4] = 0;
  m_cellHitMapNameEB[0] = "D4";
  //D4
  m_cellHitMapEB[3][0] = 1;
  m_cellHitMapEB[3][1] = 5;
  m_cellHitMapEB[3][2] = 3;
  m_cellHitMapEB[3][3] = 5;
  m_cellHitMapEB[3][4] = 0;
  //C10
  m_cellHitMapEB[4][0] = 3;
  m_cellHitMapEB[4][1] = 3;
  m_cellHitMapEB[4][2] = 3;
  m_cellHitMapEB[4][3] = 4;
  m_cellHitMapEB[4][4] = 1;
  m_cellHitMapNameEB[1] = "C10";
  //C10
  m_cellHitMapEB[5][0] = 3;
  m_cellHitMapEB[5][1] = 3;
  m_cellHitMapEB[5][2] = 3;
  m_cellHitMapEB[5][3] = 4;
  m_cellHitMapEB[5][4] = 1;
  //A12
  m_cellHitMapEB[6][0] = 4;
  m_cellHitMapEB[6][1] = 1;
  m_cellHitMapEB[6][2] = 5;
  m_cellHitMapEB[6][3] = 1;
  m_cellHitMapEB[6][4] = 2;
  m_cellHitMapNameEB[2] = "A12";
  //A12
  m_cellHitMapEB[7][0] = 4;
  m_cellHitMapEB[7][1] = 1;
  m_cellHitMapEB[7][2] = 5;
  m_cellHitMapEB[7][3] = 1;
  m_cellHitMapEB[7][4] = 2;
  //B11
  m_cellHitMapEB[8][0] = 4;
  m_cellHitMapEB[8][1] = 2;
  m_cellHitMapEB[8][2] = 6;
  m_cellHitMapEB[8][3] = 3;
  m_cellHitMapEB[8][4] = 3;
  m_cellHitMapNameEB[3] = "B11";
  //B11
  m_cellHitMapEB[9][0] = 4;
  m_cellHitMapEB[9][1] = 2;
  m_cellHitMapEB[9][2] = 6;
  m_cellHitMapEB[9][3] = 3;
  m_cellHitMapEB[9][4] = 3;
  //A13
  m_cellHitMapEB[10][0] = 6;
  m_cellHitMapEB[10][1] = 1;
  m_cellHitMapEB[10][2] = 8;
  m_cellHitMapEB[10][3] = 1;
  m_cellHitMapEB[10][4] = 4;
  m_cellHitMapNameEB[4] = "A13";
  //A13
  m_cellHitMapEB[11][0] = 6;
  m_cellHitMapEB[11][1] = 1;
  m_cellHitMapEB[11][2] = 8;
  m_cellHitMapEB[11][3] = 1;
  m_cellHitMapEB[11][4] = 4;
  //E1
  m_cellHitMapEB[12][0] = -1;
  m_cellHitMapEB[12][1] = -1;
  m_cellHitMapEB[12][2] = -1;
  m_cellHitMapEB[12][3] = -1;
  m_cellHitMapEB[12][4] = -1;
  //E2
  m_cellHitMapEB[13][0] = -1;
  m_cellHitMapEB[13][1] = -1;
  m_cellHitMapEB[13][2] = -1;
  m_cellHitMapEB[13][3] = -1;
  m_cellHitMapEB[13][4] = -1;
  //B12
  m_cellHitMapEB[14][0] = 7;
  m_cellHitMapEB[14][1] = 2;
  m_cellHitMapEB[14][2] = 9;
  m_cellHitMapEB[14][3] = 3;
  m_cellHitMapEB[14][4] = 5;
  m_cellHitMapNameEB[5] = "B12";
  //B12
  m_cellHitMapEB[15][0] = 7;
  m_cellHitMapEB[15][1] = 2;
  m_cellHitMapEB[15][2] = 9;
  m_cellHitMapEB[15][3] = 3;
  m_cellHitMapEB[15][4] = 5;
  //D5
  m_cellHitMapEB[16][0] = 4;
  m_cellHitMapEB[16][1] = 4;
  m_cellHitMapEB[16][2] = 11;
  m_cellHitMapEB[16][3] = 5;
  m_cellHitMapEB[16][4] = 6;
  m_cellHitMapNameEB[6] = "D5";
  //D5
  m_cellHitMapEB[17][0] = 4;
  m_cellHitMapEB[17][1] = 4;
  m_cellHitMapEB[17][2] = 11;
  m_cellHitMapEB[17][3] = 5;
  m_cellHitMapEB[17][4] = 6;
  //E3*
  m_cellHitMapEB[18][0] = -1;
  m_cellHitMapEB[18][1] = -1;
  m_cellHitMapEB[18][2] = -1;
  m_cellHitMapEB[18][3] = -1;
  m_cellHitMapEB[18][4] = -1;
  //E4*
  m_cellHitMapEB[19][0] = -1;
  m_cellHitMapEB[19][1] = -1;
  m_cellHitMapEB[19][2] = -1;
  m_cellHitMapEB[19][3] = -1;
  m_cellHitMapEB[19][4] = -1;
  //A14
  m_cellHitMapEB[20][0] = 9;
  m_cellHitMapEB[20][1] = 1;
  m_cellHitMapEB[20][2] = 11;
  m_cellHitMapEB[20][3] = 1;
  m_cellHitMapEB[20][4] = 7;
  m_cellHitMapNameEB[7] = "A14";
  //A14
  m_cellHitMapEB[21][0] = 9;
  m_cellHitMapEB[21][1] = 1;
  m_cellHitMapEB[21][2] = 11;
  m_cellHitMapEB[21][3] = 1;
  m_cellHitMapEB[21][4] = 7;
  //B13
  m_cellHitMapEB[22][0] = 10;
  m_cellHitMapEB[22][1] = 2;
  m_cellHitMapEB[22][2] = 12;
  m_cellHitMapEB[22][3] = 3;
  m_cellHitMapEB[22][4] = 8;
  m_cellHitMapNameEB[8] = "B13";
  //B13
  m_cellHitMapEB[23][0] = 10;
  m_cellHitMapEB[23][1] = 2;
  m_cellHitMapEB[23][2] = 12;
  m_cellHitMapEB[23][3] = 3;
  m_cellHitMapEB[23][4] = 8;
  //NOTHINg
  m_cellHitMapEB[24][0] = -1;
  m_cellHitMapEB[24][1] = -1;
  m_cellHitMapEB[24][2] = -1;
  m_cellHitMapEB[24][3] = -1;
  m_cellHitMapEB[24][4] = -1;
  //NOTHINg
  m_cellHitMapEB[25][0] = -1;
  m_cellHitMapEB[25][1] = -1;
  m_cellHitMapEB[25][2] = -1;
  m_cellHitMapEB[25][3] = -1;
  m_cellHitMapEB[25][4] = -1;
  //NOTHINg
  m_cellHitMapEB[26][0] = -1;
  m_cellHitMapEB[26][1] = -1;
  m_cellHitMapEB[26][2] = -1;
  m_cellHitMapEB[26][3] = -1;
  m_cellHitMapEB[26][4] = -1;
  //NOTHINg
  m_cellHitMapEB[27][0] = -1;
  m_cellHitMapEB[27][1] = -1;
  m_cellHitMapEB[27][2] = -1;
  m_cellHitMapEB[27][3] = -1;
  m_cellHitMapEB[27][4] = -1;
  //NOTHINg
  m_cellHitMapEB[28][0] = -1;
  m_cellHitMapEB[28][1] = -1;
  m_cellHitMapEB[28][2] = -1;
  m_cellHitMapEB[28][3] = -1;
  m_cellHitMapEB[28][4] = -1;
  //NOTHINg
  m_cellHitMapEB[29][0] = -1;
  m_cellHitMapEB[29][1] = -1;
  m_cellHitMapEB[29][2] = -1;
  m_cellHitMapEB[29][3] = -1;
  m_cellHitMapEB[29][4] = -1;
  //B14
  m_cellHitMapEB[30][0] = 13;
  m_cellHitMapEB[30][1] = 2;
  m_cellHitMapEB[30][2] = 15;
  m_cellHitMapEB[30][3] = 3;
  m_cellHitMapEB[30][4] = 9;
  m_cellHitMapNameEB[9] = "B14";
  //A15
  m_cellHitMapEB[31][0] = 12;
  m_cellHitMapEB[31][1] = 1;
  m_cellHitMapEB[31][2] = 14;
  m_cellHitMapEB[31][3] = 1;
  m_cellHitMapEB[31][4] = 10;
  m_cellHitMapNameEB[10] = "A15";
  //A15
  m_cellHitMapEB[32][0] = 12;
  m_cellHitMapEB[32][1] = 1;
  m_cellHitMapEB[32][2] = 14;
  m_cellHitMapEB[32][3] = 1;
  m_cellHitMapEB[32][4] = 10;
  //NOTHINg
  m_cellHitMapEB[33][0] = -1;
  m_cellHitMapEB[33][1] = -1;
  m_cellHitMapEB[33][2] = -1;
  m_cellHitMapEB[33][3] = -1;
  m_cellHitMapEB[33][4] = -1;
  //NOTHINg
  m_cellHitMapEB[34][0] = -1;
  m_cellHitMapEB[34][1] = -1;
  m_cellHitMapEB[34][2] = -1;
  m_cellHitMapEB[34][3] = -1;
  m_cellHitMapEB[34][4] = -1;
  //B14
  m_cellHitMapEB[35][0] = 13;
  m_cellHitMapEB[35][1] = 2;
  m_cellHitMapEB[35][2] = 15;
  m_cellHitMapEB[35][3] = 3;
  m_cellHitMapEB[35][4] = 9;
  //B15
  m_cellHitMapEB[36][0] = 16;
  m_cellHitMapEB[36][1] = 2;
  m_cellHitMapEB[36][2] = 18;
  m_cellHitMapEB[36][3] = 3;
  m_cellHitMapEB[36][4] = 11;
  m_cellHitMapNameEB[11] = "B15";
  //D6
  m_cellHitMapEB[37][0] = 12;
  m_cellHitMapEB[37][1] = 4;
  m_cellHitMapEB[37][2] = 18;
  m_cellHitMapEB[37][3] = 5;
  m_cellHitMapEB[37][4] = 12;
  m_cellHitMapNameEB[12] = "D6";
  //D6
  m_cellHitMapEB[38][0] = 12;
  m_cellHitMapEB[38][1] = 4;
  m_cellHitMapEB[38][2] = 18;
  m_cellHitMapEB[38][3] = 5;
  m_cellHitMapEB[38][4] = 12;
  //B15
  m_cellHitMapEB[39][0] = 16;
  m_cellHitMapEB[39][1] = 2;
  m_cellHitMapEB[39][2] = 18;
  m_cellHitMapEB[39][3] = 3;
  m_cellHitMapEB[39][4] = 11;
  //A16
  m_cellHitMapEB[40][0] = 15;
  m_cellHitMapEB[40][1] = 1;
  m_cellHitMapEB[40][2] = 18;
  m_cellHitMapEB[40][3] = 1;
  m_cellHitMapEB[40][4] = 13;
  m_cellHitMapNameEB[13] = "A16";
  //A16
  m_cellHitMapEB[41][0] = 15;
  m_cellHitMapEB[41][1] = 1;
  m_cellHitMapEB[41][2] = 18;
  m_cellHitMapEB[41][3] = 1;
  m_cellHitMapEB[41][4] = 13;
  //NOTHINg
  m_cellHitMapEB[42][0] = -1;
  m_cellHitMapEB[42][1] = -1;
  m_cellHitMapEB[42][2] = -1;
  m_cellHitMapEB[42][3] = -1;
  m_cellHitMapEB[42][4] = -1;
  //NOTHINg
  m_cellHitMapEB[43][0] = -1;
  m_cellHitMapEB[43][1] = -1;
  m_cellHitMapEB[43][2] = -1;
  m_cellHitMapEB[43][3] = -1;
  m_cellHitMapEB[43][4] = -1;
  //NOTHINg
  m_cellHitMapEB[44][0] = -1;
  m_cellHitMapEB[44][1] = -1;
  m_cellHitMapEB[44][2] = -1;
  m_cellHitMapEB[44][3] = -1;
  m_cellHitMapEB[44][4] = -1;
  //NOTHINg
  m_cellHitMapEB[45][0] = -1;
  m_cellHitMapEB[45][1] = -1;
  m_cellHitMapEB[45][2] = -1;
  m_cellHitMapEB[45][3] = -1;
  m_cellHitMapEB[45][4] = -1;
  //NOTHINg
  m_cellHitMapEB[46][0] = -1;
  m_cellHitMapEB[46][1] = -1;
  m_cellHitMapEB[46][2] = -1;
  m_cellHitMapEB[46][3] = -1;
  m_cellHitMapEB[46][4] = -1;
  //NOTHINg
  m_cellHitMapEB[47][0] = -1;
  m_cellHitMapEB[47][1] = -1;
  m_cellHitMapEB[47][2] = -1;
  m_cellHitMapEB[47][3] = -1;
  m_cellHitMapEB[47][4] = -1;

  //D0
  m_cellHitMapLB[0][0] = 1;
  m_cellHitMapLB[0][1] = 35;
  m_cellHitMapLB[0][2] = 10;
  m_cellHitMapLB[0][3] = 44;
  m_cellHitMapLB[0][4] = -1;
  m_cellHitMapLB[0][5] = -1;
  //A1
  m_cellHitMapLB[1][0] = 1;
  m_cellHitMapLB[1][1] = 1;
  m_cellHitMapLB[1][2] = 7;
  m_cellHitMapLB[1][3] = 9;
  m_cellHitMapLB[1][4] = 0;
  m_cellHitMapLB[1][5] = -1;
  m_cellHitMapNameLB[0] = "A1";
  //B1
  m_cellHitMapLB[2][0] = 1;
  m_cellHitMapLB[2][1] = 10;
  m_cellHitMapLB[2][2] = 8;
  m_cellHitMapLB[2][3] = 20;
  m_cellHitMapLB[2][4] = 1;
  m_cellHitMapLB[2][5] = 0;
  //C1
  m_cellHitMapLB_C[0][0] = 1;
  m_cellHitMapLB_C[0][1] = 21;
  m_cellHitMapLB_C[0][2] = 9;
  m_cellHitMapLB_C[0][3] = 34;
  m_cellHitMapNameLB[1] = "BC1";
  //B1
  m_cellHitMapLB[3][0] = 1;
  m_cellHitMapLB[3][1] = 10;
  m_cellHitMapLB[3][2] = 8;
  m_cellHitMapLB[3][3] = 20;
  m_cellHitMapLB[3][4] = 1;
  m_cellHitMapLB[3][5] = 0;
  //A1
  m_cellHitMapLB[4][0] = 1;
  m_cellHitMapLB[4][1] = 1;
  m_cellHitMapLB[4][2] = 7;
  m_cellHitMapLB[4][3] = 9;
  m_cellHitMapLB[4][4] = 0;
  m_cellHitMapLB[4][5] = -1;
  //A2
  m_cellHitMapLB[5][0] = 8;
  m_cellHitMapLB[5][1] = 1;
  m_cellHitMapLB[5][2] = 14;
  m_cellHitMapLB[5][3] = 9;
  m_cellHitMapLB[5][4] = 2;
  m_cellHitMapLB[5][5] = -1;
  m_cellHitMapNameLB[2] = "A2";
  //B2
  m_cellHitMapLB[6][0] = 9;
  m_cellHitMapLB[6][1] = 10;
  m_cellHitMapLB[6][2] = 16;
  m_cellHitMapLB[6][3] = 20;
  m_cellHitMapLB[6][4] = 3;
  m_cellHitMapLB[6][5] = 1;
  m_cellHitMapNameLB[3] = "BC2";
  //C2
  m_cellHitMapLB_C[1][0] = 10;
  m_cellHitMapLB_C[1][1] = 21;
  m_cellHitMapLB_C[1][2] = 18;
  m_cellHitMapLB_C[1][3] = 34;

  //B2
  m_cellHitMapLB[7][0] = 9;
  m_cellHitMapLB[7][1] = 10;
  m_cellHitMapLB[7][2] = 16;
  m_cellHitMapLB[7][3] = 20;
  m_cellHitMapLB[7][4] = 3;
  m_cellHitMapLB[7][5] = 1;
  //A2
  m_cellHitMapLB[8][0] = 8;
  m_cellHitMapLB[8][1] = 1;
  m_cellHitMapLB[8][2] = 14;
  m_cellHitMapLB[8][3] = 9;
  m_cellHitMapLB[8][4] = 2;
  m_cellHitMapLB[8][5] = -1;
  //A3
  m_cellHitMapLB[9][0] = 15;
  m_cellHitMapLB[9][1] = 1;
  m_cellHitMapLB[9][2] = 21;
  m_cellHitMapLB[9][3] = 9;
  m_cellHitMapLB[9][4] = 4;
  m_cellHitMapLB[9][5] = -1;
  m_cellHitMapNameLB[4] = "A3";
  //A3
  m_cellHitMapLB[10][0] = 15;
  m_cellHitMapLB[10][1] = 1;
  m_cellHitMapLB[10][2] = 21;
  m_cellHitMapLB[10][3] = 9;
  m_cellHitMapLB[10][4] = 4;
  m_cellHitMapLB[10][5] = -1;
  //B3
  m_cellHitMapLB[11][0] = 17;
  m_cellHitMapLB[11][1] = 10;
  m_cellHitMapLB[11][2] = 23;
  m_cellHitMapLB[11][3] = 20;
  m_cellHitMapLB[11][4] = 5;
  m_cellHitMapLB[11][5] = 2;
  m_cellHitMapNameLB[5] = "BC3";
  //C3
  m_cellHitMapLB_C[2][0] = 19;
  m_cellHitMapLB_C[2][1] = 21;
  m_cellHitMapLB_C[2][2] = 27;
  m_cellHitMapLB_C[2][3] = 34;
  //B3
  m_cellHitMapLB[12][0] = 17;
  m_cellHitMapLB[12][1] = 10;
  m_cellHitMapLB[12][2] = 23;
  m_cellHitMapLB[12][3] = 20;
  m_cellHitMapLB[12][4] = 5;
  m_cellHitMapLB[12][5] = 2;
  //D1
  m_cellHitMapLB[13][0] = 11;
  m_cellHitMapLB[13][1] = 35;
  m_cellHitMapLB[13][2] = 30;
  m_cellHitMapLB[13][3] = 44;
  m_cellHitMapLB[13][4] = 6;
  m_cellHitMapLB[13][5] = -1;
  m_cellHitMapNameLB[6] = "D1";
  //D1
  m_cellHitMapLB[14][0] = 11;
  m_cellHitMapLB[14][1] = 35;
  m_cellHitMapLB[14][2] = 30;
  m_cellHitMapLB[14][3] = 44;
  m_cellHitMapLB[14][4] = 6;
  m_cellHitMapLB[14][5] = -1;
  //A4
  m_cellHitMapLB[15][0] = 22;
  m_cellHitMapLB[15][1] = 1;
  m_cellHitMapLB[15][2] = 28;
  m_cellHitMapLB[15][3] = 9;
  m_cellHitMapLB[15][4] = 7;
  m_cellHitMapLB[15][5] = -1;
  m_cellHitMapNameLB[7] = "A4";
  //B4
  m_cellHitMapLB[16][0] = 24;
  m_cellHitMapLB[16][1] = 10;
  m_cellHitMapLB[16][2] = 31;
  m_cellHitMapLB[16][3] = 20;
  m_cellHitMapLB[16][4] = 8;
  m_cellHitMapLB[16][5] = 3;
  m_cellHitMapNameLB[8] = "BC4";
  //C4
  m_cellHitMapLB_C[3][0] = 28;
  m_cellHitMapLB_C[3][1] = 21;
  m_cellHitMapLB_C[3][2] = 36;
  m_cellHitMapLB_C[3][3] = 34;
  //B4
  m_cellHitMapLB[17][0] = 24;
  m_cellHitMapLB[17][1] = 10;
  m_cellHitMapLB[17][2] = 31;
  m_cellHitMapLB[17][3] = 20;
  m_cellHitMapLB[17][4] = 8;
  m_cellHitMapLB[17][5] = 3;
  //A4
  m_cellHitMapLB[18][0] = 22;
  m_cellHitMapLB[18][1] = 1;
  m_cellHitMapLB[18][2] = 28;
  m_cellHitMapLB[18][3] = 9;
  m_cellHitMapLB[18][4] = 7;
  m_cellHitMapLB[18][5] = -1;
  //A5
  m_cellHitMapLB[19][0] = 29;
  m_cellHitMapLB[19][1] = 1;
  m_cellHitMapLB[19][2] = 35;
  m_cellHitMapLB[19][3] = 9;
  m_cellHitMapLB[19][4] = 9;
  m_cellHitMapLB[19][5] = -1;
  m_cellHitMapNameLB[9] = "A5";
  //A5
  m_cellHitMapLB[20][0] = 29;
  m_cellHitMapLB[20][1] = 1;
  m_cellHitMapLB[20][2] = 35;
  m_cellHitMapLB[20][3] = 9;
  m_cellHitMapLB[20][4] = 9;
  m_cellHitMapLB[20][5] = -1;
  //B5
  m_cellHitMapLB[21][0] = 32;
  m_cellHitMapLB[21][1] = 10;
  m_cellHitMapLB[21][2] = 39;
  m_cellHitMapLB[21][3] = 20;
  m_cellHitMapLB[21][4] = 10;
  m_cellHitMapLB[21][5] = 4;
  m_cellHitMapNameLB[10] = "BC5";
  //C5
  m_cellHitMapLB_C[4][0] = 37;
  m_cellHitMapLB_C[4][1] = 21;
  m_cellHitMapLB_C[4][2] = 46;
  m_cellHitMapLB_C[4][3] = 34;
  //B5
  m_cellHitMapLB[22][0] = 32;
  m_cellHitMapLB[22][1] = 10;
  m_cellHitMapLB[22][2] = 39;
  m_cellHitMapLB[22][3] = 20;
  m_cellHitMapLB[22][4] = 10;
  m_cellHitMapLB[22][5] = 4;
  //A6
  m_cellHitMapLB[23][0] = 36;
  m_cellHitMapLB[23][1] = 1;
  m_cellHitMapLB[23][2] = 42;
  m_cellHitMapLB[23][3] = 9;
  m_cellHitMapLB[23][4] = 11;
  m_cellHitMapLB[23][5] = -1;
  m_cellHitMapNameLB[11] = "A6";
  //D2
  m_cellHitMapLB[24][0] = 31;
  m_cellHitMapLB[24][1] = 35;
  m_cellHitMapLB[24][2] = 52;
  m_cellHitMapLB[24][3] = 44;
  m_cellHitMapLB[24][4] = 12;
  m_cellHitMapLB[24][5] = -1;
  m_cellHitMapNameLB[12] = "D2";
  //D2
  m_cellHitMapLB[25][0] = 31;
  m_cellHitMapLB[25][1] = 35;
  m_cellHitMapLB[25][2] = 52;
  m_cellHitMapLB[25][3] = 44;
  m_cellHitMapLB[25][4] = 12;
  m_cellHitMapLB[25][5] = -1;
  //A6
  m_cellHitMapLB[26][0] = 36;
  m_cellHitMapLB[26][1] = 1;
  m_cellHitMapLB[26][2] = 42;
  m_cellHitMapLB[26][3] = 9;
  m_cellHitMapLB[26][4] = 11;
  m_cellHitMapLB[26][5] = -1;
  //B6
  m_cellHitMapLB[27][0] = 40;
  m_cellHitMapLB[27][1] = 10;
  m_cellHitMapLB[27][2] = 48;
  m_cellHitMapLB[27][3] = 20;
  m_cellHitMapLB[27][4] = 13;
  m_cellHitMapLB[27][5] = 5;
  m_cellHitMapNameLB[13] = "BC6";
  //C6
  m_cellHitMapLB_C[5][0] = 47;
  m_cellHitMapLB_C[5][1] = 21;
  m_cellHitMapLB_C[5][2] = 56;
  m_cellHitMapLB_C[5][3] = 34;
  //B6
  m_cellHitMapLB[28][0] = 40;
  m_cellHitMapLB[28][1] = 10;
  m_cellHitMapLB[28][2] = 48;
  m_cellHitMapLB[28][3] = 20;
  m_cellHitMapLB[28][4] = 13;
  m_cellHitMapLB[28][5] = 5;
  //A7
  m_cellHitMapLB[29][0] = 43;
  m_cellHitMapLB[29][1] = 1;
  m_cellHitMapLB[29][2] = 50;
  m_cellHitMapLB[29][3] = 9;
  m_cellHitMapLB[29][4] = 14;
  m_cellHitMapLB[29][5] = -1;
  m_cellHitMapNameLB[14] = "A7";
  //Nothing
  m_cellHitMapLB[30][0] = -1;
  m_cellHitMapLB[30][1] = -1;
  m_cellHitMapLB[30][2] = -1;
  m_cellHitMapLB[30][3] = -1;
  m_cellHitMapLB[30][4] = -1;
  m_cellHitMapLB[30][5] = -1;
  //Nothing
  m_cellHitMapLB[31][0] = -1;
  m_cellHitMapLB[31][1] = -1;
  m_cellHitMapLB[31][2] = -1;
  m_cellHitMapLB[31][3] = -1;
  m_cellHitMapLB[31][4] = -1;
  m_cellHitMapLB[31][5] = -1;
  //A7
  m_cellHitMapLB[32][0] = 43;
  m_cellHitMapLB[32][1] = 1;
  m_cellHitMapLB[32][2] = 50;
  m_cellHitMapLB[32][3] = 9;
  m_cellHitMapLB[32][4] = 14;
  m_cellHitMapLB[32][5] = -1;
  //B7
  m_cellHitMapLB[33][0] = 49;
  m_cellHitMapLB[33][1] = 10;
  m_cellHitMapLB[33][2] = 57;
  m_cellHitMapLB[33][3] = 20;
  m_cellHitMapLB[33][4] = 15;
  m_cellHitMapLB[33][5] = 6;
  m_cellHitMapNameLB[15] = "BC7";
  //C7
  m_cellHitMapLB_C[6][0] = 57;
  m_cellHitMapLB_C[6][1] = 21;
  m_cellHitMapLB_C[6][2] = 66;
  m_cellHitMapLB_C[6][3] = 34;
  //B7
  m_cellHitMapLB[34][0] = 49;
  m_cellHitMapLB[34][1] = 10;
  m_cellHitMapLB[34][2] = 57;
  m_cellHitMapLB[34][3] = 20;
  m_cellHitMapLB[34][4] = 15;
  m_cellHitMapLB[34][5] = 6;
  //A8
  m_cellHitMapLB[35][0] = 51;
  m_cellHitMapLB[35][1] = 1;
  m_cellHitMapLB[35][2] = 59;
  m_cellHitMapLB[35][3] = 9;
  m_cellHitMapLB[35][4] = 16;
  m_cellHitMapLB[35][5] = -1;
  m_cellHitMapNameLB[16] = "A8";
  //A9
  m_cellHitMapLB[36][0] = 60;
  m_cellHitMapLB[36][1] = 1;
  m_cellHitMapLB[36][2] = 68;
  m_cellHitMapLB[36][3] = 9;
  m_cellHitMapLB[36][4] = 17;
  m_cellHitMapLB[36][5] = -1;
  m_cellHitMapNameLB[17] = "A9";
  //A9
  m_cellHitMapLB[37][0] = 60;
  m_cellHitMapLB[37][1] = 1;
  m_cellHitMapLB[37][2] = 68;
  m_cellHitMapLB[37][3] = 9;
  m_cellHitMapLB[37][4] = 17;
  m_cellHitMapLB[37][5] = -1;
  //A8
  m_cellHitMapLB[38][0] = 51;
  m_cellHitMapLB[38][1] = 1;
  m_cellHitMapLB[38][2] = 59;
  m_cellHitMapLB[38][3] = 9;
  m_cellHitMapLB[38][4] = 16;
  m_cellHitMapLB[38][5] = -1;
  //B8
  m_cellHitMapLB[39][0] = 58;
  m_cellHitMapLB[39][1] = 10;
  m_cellHitMapLB[39][2] = 67;
  m_cellHitMapLB[39][3] = 20;
  m_cellHitMapLB[39][4] = 18;
  m_cellHitMapLB[39][5] = 7;
  m_cellHitMapNameLB[18] = "BC8";
  //C8
  m_cellHitMapLB_C[7][0] = 67;
  m_cellHitMapLB_C[7][1] = 21;
  m_cellHitMapLB_C[7][2] = 76;
  m_cellHitMapLB_C[7][3] = 34;
  //B8
  m_cellHitMapLB[40][0] = 58;
  m_cellHitMapLB[40][1] = 10;
  m_cellHitMapLB[40][2] = 67;
  m_cellHitMapLB[40][3] = 20;
  m_cellHitMapLB[40][4] = 18;
  m_cellHitMapLB[40][5] = 7;
  //D3
  m_cellHitMapLB[41][0] = 53;
  m_cellHitMapLB[41][1] = 35;
  m_cellHitMapLB[41][2] = 76;
  m_cellHitMapLB[41][3] = 44;
  m_cellHitMapLB[41][4] = 19;
  m_cellHitMapLB[41][5] = -1;
  m_cellHitMapNameLB[19] = "D3";
  //B9
  m_cellHitMapLB[42][0] = 68;
  m_cellHitMapLB[42][1] = 10;
  m_cellHitMapLB[42][2] = 76;
  m_cellHitMapLB[42][3] = 20;
  m_cellHitMapLB[42][4] = 20;
  m_cellHitMapLB[42][5] = -1;
  m_cellHitMapNameLB[20] = "B9";
  //NOTHINg
  m_cellHitMapLB[43][0] = -1;
  m_cellHitMapLB[43][1] = -1;
  m_cellHitMapLB[43][2] = -1;
  m_cellHitMapLB[43][3] = -1;
  m_cellHitMapLB[43][4] = -1;
  m_cellHitMapLB[43][5] = -1;
  //D3
  m_cellHitMapLB[44][0] = 53;
  m_cellHitMapLB[44][1] = 35;
  m_cellHitMapLB[44][2] = 76;
  m_cellHitMapLB[44][3] = 44;
  m_cellHitMapLB[44][4] = 19;
  m_cellHitMapLB[44][5] = -1;
  //A10
  m_cellHitMapLB[45][0] = 69;
  m_cellHitMapLB[45][1] = 1;
  m_cellHitMapLB[45][2] = 76;
  m_cellHitMapLB[45][3] = 9;
  m_cellHitMapLB[45][4] = 21;
  m_cellHitMapLB[45][5] = -1;
  m_cellHitMapNameLB[21] = "A10";
  //A10
  m_cellHitMapLB[46][0] = 69;
  m_cellHitMapLB[46][1] = 1;
  m_cellHitMapLB[46][2] = 76;
  m_cellHitMapLB[46][3] = 9;
  m_cellHitMapLB[46][4] = 21;
  m_cellHitMapLB[46][5] = -1;
  //B9
  m_cellHitMapLB[47][0] = 68;
  m_cellHitMapLB[47][1] = 10;
  m_cellHitMapLB[47][2] = 76;
  m_cellHitMapLB[47][3] = 20;
  m_cellHitMapLB[47][4] = 20;
  m_cellHitMapLB[47][5] = -1;

  return TileFatherMonTool::initialize();
}

/// BookHistogram method is called at every event block ,lumi block and run.
/// At the moment we need to book only every run
/// It calls bookHistTrig for the 'AnyTrig' trigger type
/// All concrete trigger type histograms will only be booked when needed
/*---------------------------------------------------------*/
StatusCode TileTBCellMonTool::bookHistograms() {
  /*---------------------------------------------------------*/

  ATH_MSG_INFO("in bookHistograms()");
  ATH_MSG_INFO("Using base path " << m_path);

  if (m_fillTimeHistograms) {

    std::string module_name;
    std::string cell_name;
    std::string channel_name;

    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      module_name = "LBC01";
      cell_name = getCellName(2, channel);

      m_tileChannelEnergyVsTimeLBC01[channel] = book2F("ChannelEnergyVsTime/" + module_name
          , "ChannelEnergyVsTime_" + module_name + "_" + cell_name + "_ch" + std::to_string(channel)
          , "Tile channel energy vs time " + module_name + " " + cell_name + " ch" + std::to_string(channel)
          , 121, -60.5, 60.5, 400, 0, 200);

      m_tileChannelEnergyLBC01[channel] = book1F("ChannelEnergy/" + module_name
          , "ChannelEnergy_" + module_name + "_" + cell_name + "_ch" + std::to_string(channel)
          , "Tile channel energy " + module_name + " " + cell_name + " ch" + std::to_string(channel), 400,0,200);

      m_tileChannelTimeLBC01[channel] = book1F("ChannelTime/" + module_name
          , "ChannelTime_" + module_name + "_" + cell_name + "_ch" + std::to_string(channel)
          , "Tile channel time " + module_name + " " + cell_name + " ch" + std::to_string(channel), 121, -60.5, 60.5);

    }

    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      module_name = "EBC02";
      cell_name = getCellName(4, channel);

      m_tileChannelEnergyVsTimeEBC02[channel] = book2F("ChannelEnergyVsTime/" + module_name
         , "ChannelEnergyVsTime_" + module_name + "_" + cell_name + "_ch" + std::to_string(channel)
         , "Tile channel energy vs time " + module_name + " " + cell_name + " ch" + std::to_string(channel)
         , 121, -60.5, 60.5, 400, 0, 200);

      m_tileChannelEnergyEBC02[channel] = book1F("ChannelEnergy/" + module_name
         , "ChannelEnergy_" + module_name + "_" + cell_name + "_ch" + std::to_string(channel)
         , "Tile channel energy " + module_name + " " + cell_name + " ch" + std::to_string(channel), 400, 0, 200);

      m_tileChannelTimeEBC02[channel] = book1F("ChannelTime/" + module_name
         , "ChannelTime_" + module_name + "_" + cell_name + "_ch" + std::to_string(channel)
         , "Tile channel time " + module_name + " " + cell_name + " ch" + std::to_string(channel), 121, -60.5, 60.5);
    }

  }

  m_tileTBHitMapEBA = book2F("EBA", "CellMap", "CellMap", 18, 1, 19, 5, 1, 6);
  m_tileTBHitMapEBA->SetOption("COLZ");
  m_tileTBHitMapEBA->SetStats(0);
  m_tileTBHitMapEBA->SetZTitle("Charge[pC]");

  m_tileTBTotalEnergyEBA = book1F("EBA", "EnergyTotalEBA", "EnergyTotalEBA", 400, 0, 200);
  m_tileTBTotalEnergyEBA->SetXTitle("Charge[pC]");
  m_tileTBTotalEnergyEBA->SetYTitle("Entries");

  m_tileTBHitMapEBC = book2F("EBC", "CellMap", "CellMap", 18, 1, 19, 5, 1, 6);
  m_tileTBHitMapEBC->SetOption("COLZ");
  m_tileTBHitMapEBC->SetStats(0);
  m_tileTBHitMapEBC->SetZTitle("Charge[pC]");

  m_tileTBTotalEnergyEBC = book1F("EBC", "EnergyTotalEBC", "EnergyTotalEBC", 400, 0, 200);
  m_tileTBTotalEnergyEBC->SetXTitle("Charge[pC]");
  m_tileTBTotalEnergyEBC->SetYTitle("Entries");

  m_tileTBHitMapLBA = book2F("LBA", "CellMap", "CellMap", 76, 1, 77, 44, 1, 45);
  m_tileTBHitMapLBA->SetOption("COLZ");
  m_tileTBHitMapLBA->SetStats(0);
  m_tileTBHitMapLBA->SetZTitle("Charge[pC]");

  m_tileTBTotalEnergyLBA = book1F("LBA", "EnergyTotalLBA", "EnergyTotalLBA", 400, 0, 200);
  m_tileTBTotalEnergyLBA->SetXTitle("Charge[pC]");
  m_tileTBTotalEnergyLBA->SetYTitle("Entries");

  m_tileTBHitMapLBC = book2F("LBC", "CellMap", "CellMap", 76, 1, 77, 44, 1, 45);
  m_tileTBHitMapLBC->SetOption("COLZ");
  m_tileTBHitMapLBC->SetStats(0);
  m_tileTBHitMapLBC->SetZTitle("Charge[pC]");

  m_tileTBTotalEnergyLBC = book1F("LBC", "EnergyTotalLBC", "EnergyTotalLBC", 400, 0, 200);
  m_tileTBTotalEnergyLBC->SetXTitle("Charge[pC]");
  m_tileTBTotalEnergyLBC->SetYTitle("Entries");

  for (int i = 0; i < 14; ++i) {
    m_tileTBCellEneSumEBA[i] = book1F("EBA", "CellEnergy" + m_cellHitMapNameEB[i]
                                      , "CellEnergy" + m_cellHitMapNameEB[i], 400, 0, 200);
    m_tileTBCellEneSumEBA[i]->SetXTitle("Charge[pC]");

    m_tileTBCellEneDiffEBA[i] = book1F("EBA", "CellEnergyDiff" + m_cellHitMapNameEB[i]
                                       , "CellEnergyDiff" + m_cellHitMapNameEB[i], 50, -20, 20);
    m_tileTBCellEneDiffEBA[i]->SetXTitle("Charge[pC]");

    m_tileTBCellTimeSumEBA[i] = book1F("EBA", "CellTime" + m_cellHitMapNameEB[i]
                                       , "CellTime" + m_cellHitMapNameEB[i], 100, -75, 75);
    m_tileTBCellTimeSumEBA[i]->SetXTitle("Time");

    m_tileTBCellTimeDiffEBA[i] = book1F("EBA", "CellTimeDiff" + m_cellHitMapNameEB[i]
                                        , "CellTimeDiff" + m_cellHitMapNameEB[i], 100, -75, 75);
    m_tileTBCellTimeDiffEBA[i]->SetXTitle("Time");

    m_tileTBCellEneSumEBC[i] = book1F("EBC", "CellEnergy" + m_cellHitMapNameEB[i]
                                      , "CellEnergy" + m_cellHitMapNameEB[i], 400, 0, 200);
    m_tileTBCellEneSumEBC[i]->SetXTitle("Charge[pC]");

    m_tileTBCellEneDiffEBC[i] = book1F("EBC", "CellEnergyDiff" + m_cellHitMapNameEB[i]
                                       , "CellEnergyDiff" + m_cellHitMapNameEB[i], 50, -20, 20);
    m_tileTBCellEneDiffEBC[i]->SetXTitle("Charge[pC]");

    m_tileTBCellTimeSumEBC[i] = book1F("EBC", "CellTime" + m_cellHitMapNameEB[i]
                                       , "CellTime" + m_cellHitMapNameEB[i], 100, -75, 75);
    m_tileTBCellTimeSumEBC[i]->SetXTitle("Time");

    m_tileTBCellTimeDiffEBC[i] = book1F("EBC", "CellTimeDiff" + m_cellHitMapNameEB[i]
                                         , "CellTimeDiff" + m_cellHitMapNameEB[i], 100, -75, 75);
    m_tileTBCellTimeDiffEBC[i]->SetXTitle("Time");
  }

  for (int i = 0; i < 22; ++i) {
    m_tileTBCellEneSumLBA[i] = book1F("LBA", "CellEnergy" + m_cellHitMapNameLB[i]
                                      , "CellEnergy" + m_cellHitMapNameLB[i], 400, 0, 200);
    m_tileTBCellEneSumLBA[i]->SetXTitle("Charge[pC]");

    m_tileTBCellEneDiffLBA[i] = book1F("LBA", "CellEnergyDiff" + m_cellHitMapNameLB[i]
                                       , "CellEnergyDiff" + m_cellHitMapNameLB[i], 50, -20, 20);
    m_tileTBCellEneDiffLBA[i]->SetXTitle("Charge[pC]");

    m_tileTBCellTimeSumLBA[i] = book1F("LBA", "CellTime" + m_cellHitMapNameLB[i]
                                       , "CellTime" + m_cellHitMapNameLB[i], 100, -75, 75);
    m_tileTBCellTimeSumLBA[i]->SetXTitle("Time");

    m_tileTBCellTimeDiffLBA[i] = book1F("LBA", "CellTimeDiff" + m_cellHitMapNameLB[i]
                                        , "CellTimeDiff" + m_cellHitMapNameLB[i], 100, -75, 75);
    m_tileTBCellTimeDiffLBA[i]->SetXTitle("Time");

    m_tileTBCellEneSumLBC[i] = book1F("LBC", "CellEnergy" + m_cellHitMapNameLB[i]
                                      , "CellEnergy" + m_cellHitMapNameLB[i], 400, 0, 200);
    m_tileTBCellEneSumLBC[i]->SetXTitle("Charge[pC]");

    m_tileTBCellEneDiffLBC[i] = book1F("LBC", "CellEnergyDiff" + m_cellHitMapNameLB[i]
                                       , "CellEnergyDiff" + m_cellHitMapNameLB[i], 50, -20, 20);
    m_tileTBCellEneDiffLBC[i]->SetXTitle("Charge[pC]");

    m_tileTBCellTimeSumLBC[i] = book1F("LBC", "CellTime" + m_cellHitMapNameLB[i]
                                       , "CellTime" + m_cellHitMapNameLB[i], 100, -75, 75);
    m_tileTBCellTimeSumLBC[i]->SetXTitle("Time");

    m_tileTBCellTimeDiffLBC[i] = book1F("LBC", "CellTimeDiff" + m_cellHitMapNameLB[i]
                                        , "CellTimeDiff" + m_cellHitMapNameLB[i], 100, -75, 75);
    m_tileTBCellTimeDiffLBC[i]->SetXTitle("Time");
  }
  //cell D0 for long barrel.
  m_tileTBCellEneSumLBAD0 = book1F("LBA", "CellEnergyD0", "CellEnergyD0", 400, 0, 200);
  m_tileTBCellEneSumLBCD0 = book1F("LBC", "CellEnergyD0", "CellEnergyD0", 400, 0, 200);
  m_tileTBCellTimeSumLBAD0 = book1F("LBA", "CellTimeD0", "CellTimeD0", 100, -75, 75);
  m_tileTBCellTimeSumLBCD0 = book1F("LBC", "CellTimeD0", "CellTimeD0", 100, -75, 75);
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTBCellMonTool::fillHistograms() {
  /*---------------------------------------------------------*/

  // Pointer to a Tile cell container
  const CaloCellContainer* cell_container;

  //Retrieve Cell collection from SG
  CHECK(evtStore()->retrieve(cell_container, m_cellsContName));

  //CaloCellContainer::const_iterator iCell = cell_container->begin();
  //CaloCellContainer::const_iterator lastCell = cell_container->end();

  double totalEnergyEBA = 0.0;
  double totalEnergyEBC = 0.0;
  double totalEnergyLBA = 0.0;
  double totalEnergyLBC = 0.0;

  for (const CaloCell* cell : *cell_container) {

    Identifier id = cell->ID();

    if (m_tileID->is_tile(id)) {

      const TileCell* tile_cell = dynamic_cast<const TileCell*>(cell);
      if (tile_cell == 0)
        continue;

      // int drw = 0; // drawer number, range 0-63, the same for both channels
      short ch1 = -1, ch2 = -1; // channel number, range 0-47 or -1 for unknown
      int ros1(-1);
      int ros2(-1);

      const CaloDetDescrElement * caloDDE = tile_cell->caloDDE();

      IdentifierHash hash1 = caloDDE->onl1();
      if (hash1 != TileHWID::NOT_VALID_HASH) {
        HWIdentifier hw1 = m_tileHWID->channel_id(hash1);
        ch1 = m_tileHWID->channel(hw1);
        // drw = m_tileHWID->drawer(hw1);
        ros1 = m_tileHWID->ros(hw1);
      }

      IdentifierHash hash2 = caloDDE->onl2();
      if (hash2 != TileHWID::NOT_VALID_HASH) {
        HWIdentifier hw2 = m_tileHWID->channel_id(hash2);
        ch2 = m_tileHWID->channel(hw2);
        // drw = m_tileHWID->drawer(hw2);
        ros2 = m_tileHWID->ros(hw2);
      }
      //ATH_MSG_INFO( "id="<<id <<" ros1="<<ros1);

      // note that drawer from HWID and module from ID are different for E3 cells near MBTS
      // double drawer = (double)drw + 1.0; // range from 1-64
      // double module = (double)m_tileID->module(id) + 1.0; // range from 1-64

      int samp = m_tileID->sample(id);

      bool single_PMT_scin = (samp == TileID::SAMP_E);
      bool single_PMT_C10 = (m_tileID->section(id) == TileID::GAPDET && samp == TileID::SAMP_C
          && (!m_cabling->C10_connected(m_tileID->module(id))));

      // distinguish cells with one or two PMTs
      bool single_PMT = single_PMT_C10 || single_PMT_scin;

      // distinguish normal cells and fantoms (e.g. non-existing D4 in EBA15, EBC18
      // or non-existing E3/E4 - they might appear in CaloCellContainer)
      bool real_cell = single_PMT_C10 || m_cabling->TileGap_connected(id);

      // int gn1 = tile_cell->gain1(); // gain of first PMT
      // int gn2 = tile_cell->gain2(); // gain of second PMT

      // get the cell energy, time and position info
      double energy = cell->energy();
      double time = cell->time();
      // double quality = cell->quality();
      // double eta = cell->eta();
      // double phi = cell->phi();
      double ene1 = tile_cell->ene1();
      double ene2 = tile_cell->ene2();
      double ediff = (single_PMT) ? 0.0 : tile_cell->eneDiff();
      // double eratio = (energy!=0.0) ? ediff/energy : 0.0;
      double t1 = tile_cell->time1();
      double t2 = tile_cell->time2();
      double tdiff = (single_PMT) ? 0.0 : 2. * tile_cell->timeDiff(); // attention! factor of 2 is needed here

      energy = energy / 1000.; //units of pC
      ediff = ediff / 1000.;

      if (real_cell) {

        if (m_fillTimeHistograms) {

          if (ros2 == 2 && ch2 == 0 && ene2 > m_energyThresholdForTime) {
            m_tileChannelTimeLBC01[ch2]->Fill(t2, 1.0);
          } else if (ros1 == 2) {
            if (ch1 > 0 && ene1 > m_energyThresholdForTime)
              m_tileChannelTimeLBC01[ch1]->Fill(t1, 1.0);
            if (ch2 > 0 && ene2 > m_energyThresholdForTime)
              m_tileChannelTimeLBC01[ch2]->Fill(t2, 1.0);
            if (ch1 > 0) {
              m_tileChannelEnergyLBC01[ch1]->Fill(ene1, 1.0);
              m_tileChannelEnergyVsTimeLBC01[ch1]->Fill(t1, ene1);
            }
            if (ch2 > 0) {
              m_tileChannelEnergyLBC01[ch2]->Fill(ene2, 1.0);
              m_tileChannelEnergyVsTimeLBC01[ch2]->Fill(t2, ene2);
            }
          }
          if (ros1 == 4) {
            if (ch1 > 0 && ene1 > m_energyThresholdForTime)
              m_tileChannelTimeEBC02[ch1]->Fill(t1, 1.0);
            if (ch2 > 0 && ene2 > m_energyThresholdForTime)
              m_tileChannelTimeEBC02[ch2]->Fill(t2, 1.0);
            if (ch1 > 0) {
              m_tileChannelEnergyEBC02[ch1]->Fill(ene1, 1.0);
              m_tileChannelEnergyVsTimeEBC02[ch1]->Fill(t1, ene1);
            }
            if (ch2 > 0) {
              m_tileChannelEnergyEBC02[ch2]->Fill(ene2, 1.0);
              m_tileChannelEnergyVsTimeEBC02[ch2]->Fill(t2, ene2);
            }
          }
        }

        // masking on the fly: check if channel is bad in event, while it is good in DB

        // when only one channel is bad, it might be that gain of masked channel is not correct
        // if two qualities are not identical, then gains are not the same (new feature introduced in rel 17.2)
        //int qual1 = (int)tile_cell->qual1();
        //int qual2 = (int)tile_cell->qual2();

        //ATH_MSG_INFO( "ch1="<<ch1 );
        // check that both PMTs are good and that it is cell with 2 PMTs and it is an end barrel

        if (ros2 == 2 && ch2 == 0) {

        }
        if (ros1 == 1) {
          totalEnergyLBA += energy;
          fillHitMap(m_tileTBHitMapLBA, m_cellHitMapLB[ch1], energy);
          //count channel D0 as energy in LBC
          if (ch1 == 0) {
            totalEnergyLBC += energy;
            fillHitMap(m_tileTBHitMapLBC, m_cellHitMapLB[ch1], energy);
          }

          //check if it is a B Cell with a correspond C cell;
          if (m_cellHitMapLB[ch1][5] != -1) {
            fillHitMap(m_tileTBHitMapLBA, m_cellHitMapLB_C[m_cellHitMapLB[ch1][5]], energy);
          }

        }
        if (!single_PMT && ch1 > -1 && ch2 > -1 && ros1 == 1) {
          //fillHitMap(m_TileTBHitMapEBC,cellHitMapEB[ch1],energy);
          if (m_cellHitMapLB[ch1][4] != -1) {
            m_tileTBCellEneSumLBA[m_cellHitMapLB[ch1][4]]->Fill(energy);
            m_tileTBCellEneDiffLBA[m_cellHitMapLB[ch1][4]]->Fill(ediff);
            if (energy > m_energyThresholdForTime) {
              m_tileTBCellTimeSumLBA[m_cellHitMapLB[ch1][4]]->Fill(time);
              m_tileTBCellTimeDiffLBA[m_cellHitMapLB[ch1][4]]->Fill(tdiff);
            }
          }
          //If it is cell D0;
          /*if (ch1==0) {
           m_TileTBCellEneSumLBAD0->Fill(energy);
           if (energy>m_energyThresholdForTime) {
           m_TileTBCellTimeSumLBAD0->Fill(time);
           }
           }*/
        }
        if (ros1 == 2) {
          totalEnergyLBC += energy;
          fillHitMap(m_tileTBHitMapLBC, m_cellHitMapLB[ch1], energy);
          //ATH_MSG_INFO( "cellHitMapLB["<<ch1<<"]="<<cellHitMapLB[ch1] );

          //check if it is a B Cell with a correspond C cell;
          if (m_cellHitMapLB[ch1][5] != -1) {
            //ATH_MSG_INFO( "cellHitMapLB["<<ch1<<"][5]="<<cellHitMapLB[ch1][5]<<"  "<<cellHitMapLB_C[cellHitMapLB[ch1][5]] );
            fillHitMap(m_tileTBHitMapLBC, m_cellHitMapLB_C[m_cellHitMapLB[ch1][5]], energy);
          }
        }
        if (!single_PMT && ch1 > -1 && ch2 > -1 && ros1 == 2) {
          //fillHitMap(m_TileTBHitMapEBC,cellHitMapEB[ch1],energy);
          if (m_cellHitMapLB[ch1][4] != -1) {
            m_tileTBCellEneSumLBC[m_cellHitMapLB[ch1][4]]->Fill(energy);
            m_tileTBCellEneDiffLBC[m_cellHitMapLB[ch1][4]]->Fill(ediff);
            if (energy > m_energyThresholdForTime) {
              m_tileTBCellTimeSumLBC[m_cellHitMapLB[ch1][4]]->Fill(time);
              m_tileTBCellTimeDiffLBC[m_cellHitMapLB[ch1][4]]->Fill(tdiff);
            }
          }
          //If it is cell D0;
          /*if (ch2==0) {
           m_TileTBCellEneSumLBCD0->Fill(energy);
           if (energy>m_energyThresholdForTime) {
           m_TileTBCellTimeSumLBCD0->Fill(time);
           }
           }*/
        }
        if (ros1 == 3) {
          totalEnergyEBA += energy;
        }
        if (!single_PMT && ch1 > -1 && ch2 > -1 && ros1 == 3) {
          if (m_cellHitMapEB[ch1][4] != -1) {
            fillHitMap(m_tileTBHitMapEBA, m_cellHitMapEB[ch1], energy);
            m_tileTBCellEneSumEBA[m_cellHitMapEB[ch1][4]]->Fill(energy);
            m_tileTBCellEneDiffEBA[m_cellHitMapEB[ch1][4]]->Fill(ediff);
            if (energy > m_energyThresholdForTime) {
              m_tileTBCellTimeSumEBA[m_cellHitMapEB[ch1][4]]->Fill(time);
              m_tileTBCellTimeDiffEBA[m_cellHitMapEB[ch1][4]]->Fill(tdiff);
            }
          }
        }
        if (ros1 == 4) {
          totalEnergyEBC += energy;
        }
        if (!single_PMT && ch1 > -1 && ch2 > -1 && ros1 == 4) {
          if (m_cellHitMapEB[ch1][4] != -1) {
            fillHitMap(m_tileTBHitMapEBC, m_cellHitMapEB[ch1], energy);
            m_tileTBCellEneSumEBC[m_cellHitMapEB[ch1][4]]->Fill(energy);
            m_tileTBCellEneDiffEBC[m_cellHitMapEB[ch1][4]]->Fill(ediff);
            if (energy > m_energyThresholdForTime) {
              m_tileTBCellTimeSumEBC[m_cellHitMapEB[ch1][4]]->Fill(time);
              m_tileTBCellTimeDiffEBC[m_cellHitMapEB[ch1][4]]->Fill(tdiff);
            }
          }
        }

      } //end if real cell

    } // end if tile_cell
  } // end of loop over the Cells
  m_tileTBTotalEnergyEBA->Fill(totalEnergyEBA);
  m_tileTBTotalEnergyEBC->Fill(totalEnergyEBC);
  m_tileTBTotalEnergyLBA->Fill(totalEnergyLBA);
  m_tileTBTotalEnergyLBC->Fill(totalEnergyLBC);

  return StatusCode::SUCCESS;
}

void TileTBCellMonTool::fillHitMap(TH2F *hHitMap, int cellHitMap2[], double energy) {
  for (int x = cellHitMap2[0]; x <= cellHitMap2[2]; x++) {
    for (int y = cellHitMap2[1]; y <= cellHitMap2[3]; y++) {
      //ATH_MSG_INFO( "x="<<x<<" y="<<y );
      hHitMap->Fill(x, y, energy);
    }
  }
}

/*---------------------------------------------------------*/
StatusCode TileTBCellMonTool::procHistograms() {
  /*---------------------------------------------------------*/

  if (endOfRunFlag()) {
    ATH_MSG_INFO("in procHistograms()");
    ATH_MSG_INFO("Doing fits");
    Double_t fr[2];
    Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4];
    fr[0] = .5;
    fr[1] = 800;

    pllo[0] = 0.5;
    pllo[1] = 40;
    pllo[2] = 1.0;
    pllo[3] = 0.4;
    plhi[0] = 100;
    plhi[1] = 120.0;
    plhi[2] = 1000000000.0;
    plhi[3] = 100;
    sv[0] = 1.8;
    sv[1] = 80.0;
    sv[2] = m_tileTBTotalEnergyLBC->GetEntries();
    sv[3] = 3.0;

    Double_t chisqr;
    Int_t ndf;
    langaufit(m_tileTBTotalEnergyLBC, fr, sv, pllo, plhi, fp, fpe, &chisqr, &ndf);

    pllo[0] = 0.5;
    pllo[1] = 40;
    pllo[2] = 1.0;
    pllo[3] = 0.4;
    plhi[0] = 100;
    plhi[1] = 120.0;
    plhi[2] = 1000000000.0;
    plhi[3] = 100;
    sv[0] = 1.8;
    sv[1] = 80.0;
    sv[2] = m_tileTBTotalEnergyLBA->GetEntries();
    sv[3] = 3.0;

    langaufit(m_tileTBTotalEnergyLBA, fr, sv, pllo, plhi, fp, fpe, &chisqr, &ndf);

    pllo[0] = 0.5;
    pllo[1] = 40;
    pllo[2] = 1.0;
    pllo[3] = 0.4;
    plhi[0] = 100;
    plhi[1] = 120.0;
    plhi[2] = 1000000000.0;
    plhi[3] = 100;
    sv[0] = 1.8;
    sv[1] = 80.0;
    sv[2] = m_tileTBTotalEnergyEBA->GetEntries();
    sv[3] = 3.0;

    langaufit(m_tileTBTotalEnergyEBA, fr, sv, pllo, plhi, fp, fpe, &chisqr, &ndf);

    pllo[0] = 0.5;
    pllo[1] = 40;
    pllo[2] = 1.0;
    pllo[3] = 0.4;
    plhi[0] = 100;
    plhi[1] = 120.0;
    plhi[2] = 1000000000.0;
    plhi[3] = 100;
    sv[0] = 1.8;
    sv[1] = 80.0;
    sv[2] = m_tileTBTotalEnergyEBC->GetEntries();
    sv[3] = 3.0;

    langaufit(m_tileTBTotalEnergyEBC, fr, sv, pllo, plhi, fp, fpe, &chisqr, &ndf);
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTBCellMonTool::checkHists(bool /* fromFinalize */) {
  /*---------------------------------------------------------*/

  ATH_MSG_INFO("in checkHists()");

  return StatusCode::SUCCESS;
}

// Operations to be done only once at the first event
/*---------------------------------------------------------*/
void TileTBCellMonTool::initFirstEvent() {
  /*---------------------------------------------------------*/

}

Double_t langaufun(Double_t *x, Double_t *par) {

  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc = 5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow, xupp;
  Double_t step;
  Double_t i;

  // MP shift correction
  mpc = par[1] - mpshift * par[0];

  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];

  step = (xupp - xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for (i = 1.0; i <= np / 2; i++) {
    xx = xlow + (i - .5) * step;
    fland = TMath::Landau(xx, mpc, par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0], xx, par[3]);

    xx = xupp - (i - .5) * step;
    fland = TMath::Landau(xx, mpc, par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0], xx, par[3]);
  }

  return (par[2] * step * sum * invsq2pi / par[3]);
}

TF1 *TileTBCellMonTool::langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo,
    Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF) {
  // Once again, here are the Landau * Gaussian parameters:
  //   par[0]=Width (scale) parameter of Landau density
  //   par[1]=Most Probable (MP, location) parameter of Landau density
  //   par[2]=Total area (integral -inf to inf, normalization constant)
  //   par[3]=Width (sigma) of convoluted Gaussian function
  //
  // Variables for langaufit call:
  //   his             histogram to fit
  //   fitrange[2]     lo and hi boundaries of fit range
  //   startvalues[4]  reasonable start values for the fit
  //   parlimitslo[4]  lower parameter limits
  //   parlimitshi[4]  upper parameter limits
  //   fitparams[4]    returns the final fit parameters
  //   fiterrors[4]    returns the final fit errors
  //   ChiSqr          returns the chi square
  //   NDF             returns ndf

  Int_t i;
  Char_t FunName[100];

  sprintf(FunName, "Fitfcn_%s", his->GetName());

  TF1 *ffit = new TF1(FunName, langaufun, fitrange[0], fitrange[1], 4);
  ffit->SetParameters(startvalues);
  ffit->SetParNames("Width", "MP", "Area", "GSigma");

  for (i = 0; i < 4; i++) {
    ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
  }

  his->Fit(FunName, "RB");   // fit within specified range, use ParLimits, do not plot

  ffit->GetParameters(fitparams);    // obtain fit parameters
  for (i = 0; i < 4; i++) {
    fiterrors[i] = ffit->GetParError(i);     // obtain fit parameter errors
  }
  ChiSqr[0] = ffit->GetChisquare();  // obtain chi^2
  NDF[0] = ffit->GetNDF();           // obtain ndf

  return (ffit);              // return fit function

}

Int_t TileTBCellMonTool::langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM) {

  // Seaches for the location (x value) at the maximum of the
  // Landau-Gaussian convolute and its full width at half-maximum.
  //
  // The search is probably not very efficient, but it's a first try.

  Double_t p, x, fy, fxr, fxl;
  Double_t step;
  Double_t l, lold;
  Int_t i = 0;
  Int_t MAXCALLS = 10000;

  // Search for maximum

  p = params[1] - 0.1 * params[0];
  step = 0.05 * params[0];
  lold = -2.0;
  l = -1.0;

  while ((l != lold) && (i < MAXCALLS)) {
    i++;

    lold = l;
    x = p + step;
    l = langaufun(&x, params);

    if (l < lold)
      step = -step / 10;

    p += step;
  }

  if (i == MAXCALLS)
    return (-1);

  maxx = x;

  fy = l / 2;

  // Search for right x location of fy

  p = maxx + params[0];
  step = params[0];
  lold = -2.0;
  l = -1e300;
  i = 0;

  while ((l != lold) && (i < MAXCALLS)) {
    i++;

    lold = l;
    x = p + step;
    l = TMath::Abs(langaufun(&x, params) - fy);

    if (l > lold)
      step = -step / 10;

    p += step;
  }

  if (i == MAXCALLS)
    return (-2);

  fxr = x;

  // Search for left x location of fy

  p = maxx - 0.5 * params[0];
  step = -params[0];
  lold = -2.0;
  l = -1e300;
  i = 0;

  while ((l != lold) && (i < MAXCALLS)) {
    i++;

    lold = l;
    x = p + step;
    l = TMath::Abs(langaufun(&x, params) - fy);

    if (l > lold)
      step = -step / 10;

    p += step;
  }

  if (i == MAXCALLS)
    return (-3);

  fxl = x;

  FWHM = fxr - fxl;
  return (0);
}
