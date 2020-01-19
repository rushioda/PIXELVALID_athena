/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTBMonTool
// PACKAGE:  TileMonitoring
//
//
// AUTHOR:   
//
// TODO
//
// ********************************************************************

#include "xAODEventInfo/EventInfo.h"

#include "CaloEvent/CaloCellContainer.h"

#include "TileMonitoring/TileTBMonTool.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileEvent/TileCell.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

/*---------------------------------------------------------*/
TileTBMonTool::TileTBMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileTotalEventEnergy(0)
  , m_tileTBHitMapLBC01(0)
  , m_tileTBHitMapEBC02(0)
  , m_isFirstEvent(true)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("CellContainer", m_cellContainer = "AllCalo"); //SG Cell Container

  m_path = "/Tile/TestBeam"; //ROOT File relative directory
}

/*---------------------------------------------------------*/
TileTBMonTool::~TileTBMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileTBMonTool:: initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in initialize()" );

  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileTBMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTBMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHistograms()" );


  if (m_isFirstEvent) {
    // fill event info like L1 trigger type, run number, etc...
    fillEvtInfo();
    initFirstEvent();
  }

  // Pointer to a Tile cell container
  const CaloCellContainer* cell_container;

  //Retrieve Cell collection from SG
  CHECK( evtStore()->retrieve(cell_container, m_cellContainer) );

  if (cell_container->empty()) return StatusCode::SUCCESS;

  double total_energy(0.0);

  for (const CaloCell* cell : *cell_container) {
    //    Identifier id = cell->ID();
    const TileCell* tile_cell = dynamic_cast<const TileCell*>(cell);
    if (tile_cell == 0) continue;

    int side = m_tileID->side(cell->ID());
    int section = m_tileID->section(cell->ID());
    int module = m_tileID->module(cell->ID());
    int tower = m_tileID->tower(cell->ID());
    int sample = m_tileID->sample(cell->ID());


    double energy = cell->energy() * 0.001; // keep energy in pC
    total_energy += energy;
    
    fillHitMap(side, section, module, tower, sample, energy);

  }
  
  m_tileTotalEventEnergy->Fill(total_energy);

  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode TileTBMonTool::procHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in procHistograms()" );

  m_isFirstEvent = true;

  return StatusCode::SUCCESS;
}


void TileTBMonTool::fillHitMap(int side, int section, int module, int tower, int sample, double energy) {


  static int cellHitMapEB[3][17] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 9, 12, 15, 19} // A12, A13, A14, A15, A16
                                    , {0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 7, 10, 13, 16, 19, 0} // C10, B11, B12, B13, B14, B15
                                    , {0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 12, 12, 19, 0, 0, 0}}; // D4, D5, D6
  
  static int cellHitMapLB[4][11] = {{1, 8, 15, 22, 30, 36, 43, 51, 60, 69, 77} // A1, A2, A3, A4, A5, A6, A7, A8, A9, A10
                                    , {1, 9, 17, 24, 32, 40, 49, 58, 68, 77, 0} // B1, B2, B3, B4, B5, B6, B7, B8, B9,
                                    , {1, 10, 19, 28, 37, 47, 57, 67, 77, 0, 0} // C1, C2, C3, C4, C5, C6, C7, C8
                                    , {1, 11, 11, 31, 31, 53, 53, 77, 0, 0, 0}}; // D0, D1, D2, D3

  static double yLB[] = {5.0, 15.0, 30.0, 40.0};
  static double yEB[] = {0.5, 1.5, 2.5, 3.5, 4.5};

  int index(sample);

  if (section == TileID::BARREL && tower == 0 && sample == TileID::SAMP_D) side = TileID::NEGATIVE; // D0
  
  if (side == TileID::NEGATIVE) {

    if (section == TileID::BARREL && module == 0) { // LBC01

      if (sample > TileID::SAMP_B) ++index;
      double y = yLB[index];

      for (int x = cellHitMapLB[index][tower]; x < cellHitMapLB[index][tower + 1]; ++x) {
        m_tileTBHitMapLBC01->Fill(x, y, energy);
      }

      if (sample == TileID::SAMP_C) {
        ++index;
        double y = yLB[index];
        for (int x = cellHitMapLB[index][tower]; x < cellHitMapLB[index][tower + 1]; ++x) {
          m_tileTBHitMapLBC01->Fill(x, y, energy);
        }
      }

    } else if ((section == TileID::EXTBAR || section == TileID::GAPDET) && (module == 1 && sample < TileID::SAMP_E))  { // EBC02
     
      double y = yEB[index * 2];
      for (int x = cellHitMapEB[index][tower]; x < cellHitMapEB[index][tower + 1]; ++x) {
        m_tileTBHitMapEBC02->Fill(x, y, energy);
      }

      if (sample != TileID::SAMP_A && tower > 8) { // A & D4
        
        if (tower != 9) y = yEB[index * 2 - 1]; // C10
        else y = yEB[index * 2 + 1]; // D & B

        for (int x = cellHitMapEB[index][tower]; x < cellHitMapEB[index][tower + 1]; ++x) {
          m_tileTBHitMapEBC02->Fill(x, y, energy);
          
        }
      }

    }
  }
  
}


// Operations to be done only once at the first event
/*---------------------------------------------------------*/
void TileTBMonTool::initFirstEvent() {

  m_isFirstEvent = false; //Set the flag

  std::string runNumber = getRunNumStr();

  m_tileTotalEventEnergy = book1F("", "TileTBTotalEventEnergy", "Run " + runNumber + ": Total TileCal Event Energy"
                                  ,1000, -2., 200., run, ATTRIB_MANAGED, "", "mergeRebinned");

  m_tileTotalEventEnergy->GetXaxis()->SetTitle("Event Energy [pC]");


  double* xBinsEB = new double [19] {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
  double* yBinsEB = new double [6] {0,1,2,3,4,5};

  m_tileTBHitMapEBC02 = bookProfile2D("", "TileTBCellMapEBC02", "Run " + runNumber + ": Test beam cell map: EBC02", 18, 1, 19, 5, 0, 5, -2.e6, 2.e6 );
  m_tileTBHitMapEBC02->SetBins(18, xBinsEB, 5, yBinsEB);
  m_tileTBHitMapEBC02->SetOption("COLZ");
  m_tileTBHitMapEBC02->SetStats(0);
  m_tileTBHitMapEBC02->SetZTitle("Energy[pC]");

  double* xBinsLB = new double [77] {1,2,3,4,5,6,7,8,9,10
                                     ,11,12,13,14,15,15,17,18,19,20
                                     ,21,22,23,24,25,26,27,28,29,30
                                     ,31,32,33,34,35,36,37,38,39,40
                                     ,41,42,43,44,45,46,47,48,49,50
                                     ,51,52,53,54,55,56,57,58,59,60
                                     ,61,62,63,64,65,66,67,68,69,70
                                     ,71,72,73,74,75,76,77};

  double* yBinsLB = new double [5] {0, 9, 20, 34, 45};


  m_tileTBHitMapLBC01 = bookProfile2D("", "TileTBCellMapLBC01", "Run " + runNumber + ": Test beam cell map: LBC01", 76, 1, 77, 44, 1, 45, -2.e6, 2.e6 );
  m_tileTBHitMapLBC01->SetBins(76, xBinsLB, 4, yBinsLB);
  m_tileTBHitMapLBC01->SetOption("COLZ");
  m_tileTBHitMapLBC01->SetStats(0);
  m_tileTBHitMapLBC01->SetZTitle("Energy[pC]");

}
