/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileRODMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHORS:  Michael Miller (Michael.Earl.Miller@cern.ch)
//	     Luca Fiorini   (Luca.Fiorini@cern.ch)
//
// ********************************************************************
#ifndef TILERODMONTOOL_H
#define TILERODMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

#include <vector>

class ITileBadChanTool;
class TileCondToolEmscale;
class TileBeamInfoProvider;
class IROBDataProviderSvc;

/** @class TileRODMonTool
 *  @brief Class for TileROD based monitoring
 */

class TileRODMonTool: public TileFatherMonTool {

  public:

    TileRODMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileRODMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    StatusCode checkHists(bool fromFinalize);

    StatusCode bookHistTrig(int trig);

    void cleanHistVec();

  private:

    int m_tileRODTrig; //trigger index
    //int  m_runType;
    int m_compUnit;
    bool m_useFitRef;
    bool m_useOFRef;
    bool m_OFI;
    bool m_OFNI;
    bool m_corrtime;
    bool m_isOnline;

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;
    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;
    ServiceHandle<IROBDataProviderSvc> m_robSvc;

    std::string m_contName;
    std::string m_contNameDSP;
    std::string m_contNameOF;
    std::string m_contNameOFNI;

    std::vector<TH2F *> m_TileAvgDspRefEn[4];        // 4 partitions
    std::vector<TH2F *> m_TileAvgDspRefEnPhase[4];        // 4 partitions
    std::vector<TH1F *> m_TileDspRefEnSummary[4];     // 4 partitions - 8 RODs - 4 DSPs
    std::vector<TH1F *> m_TileDspRefEn[4][8][4];     // 4 partitions - 8 RODs - 4 DSPs
    std::vector<TProfile *> m_TileAvgDspRefTim[4];       // 4 partitions
    std::vector<TProfile *> m_TileAvgDspRefTimPhase[4];       // 4 partitions
    std::vector<TH1F *> m_TileDspRefTim[4][8][4];    // 4 partitions - 8 RODs - 4 DSPs
    std::vector<TH1F *> m_TileDspRefTimSummary[4];    // 4 partitions - 8 RODs - 4 DSPs
    std::vector<TProfile *> m_TileDspRefSummary[2];


    std::vector<TProfile2D*> m_TdspProfile[4]; //4 partitions
    std::vector<TProfile2D*> m_tileRodFragmentSize; //
    std::vector<TProfile*> m_tileRodFragmentSizeLB; 
    std::vector<TH1F*> m_tileRodFragmentSize1D[5][16];    // 4 partitions - 16 ROD fragments

    double m_evEref[4][64][48];    //partitions - 64 modules - 48 channels
    double m_evTref[4][64][48];    //partitions - 64 modules - 48 channels

    std::vector<uint32_t> m_tileRobIds;
    int m_nEventsProcessed[9]; // number of processed events per trigger
    int32_t m_old_lumiblock;
    bool m_details;
    int m_nLumiblocks;
    bool m_fillDetailFragmentSize;
    int m_nEvents4FragmentSize;
    float m_rodFragmentSizeSum[5][16][9]; // accumulator of ROD fragment size per trigger
    float m_lastRodFragmentSize[5][16][9]; // ROD fragment size per trigger in the last event
};

#endif
