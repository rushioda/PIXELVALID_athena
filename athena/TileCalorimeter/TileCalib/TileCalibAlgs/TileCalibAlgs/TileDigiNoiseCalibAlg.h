/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEDIGINOISECALIBALG_H
#define TILEDIGINOISECALIBALG_H

// ********************************************************************
//
// NAME:     TileDigiNoiseCalib.h
// PACKAGE:  TileCalib
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
// 
// February 2008	
// ********************************************************************

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Tile includes
#include "TileConditions/TileCablingService.h"

#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <stdint.h>

// Forward declaration
class TileHWID;
class TileBeamInfoProvider;
class TileBeamElemContByteStreamCnv;
class TileRawChannelBuilderFlatFilter;
class TileOFCorrelation;

class TileDigiNoiseCalibAlg: public AthAlgorithm {
  public:
    TileDigiNoiseCalibAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TileDigiNoiseCalibAlg();

    // Functions
    StatusCode initialize(); //only array initialization is done here
    StatusCode execute();
    StatusCode finalize();

  private:

    enum trigtype {
      Unknown = 0
      , Phys = 1
      , Las = 2
      , Ped = 4
      , Cis = 8
    };

    StatusCode FirstEvt_initialize(); // real initialization is done in this method

    StatusCode fillDigits(); //accumulate statistics for pedestal, lfn, hfn, noise_cov calculation
    void finalDigits(); // pedestal, lfn, hfn, noise_cov calculation is done here
    void StoreRunInfo(); // called only at the first event. General variables

    inline int digiChannel2PMT(int ros, int chan) {
      return (abs(m_cabling->channel2hole(ros, chan)) - 1);
    }

    // If data should be put in calib mode
    bool m_calibMode;

    // by default we use channels
    bool m_usePMT;

    // Toggle usage of average correlation calcualtion or rigorous
    // Default: Rigorous
    bool m_doAvgCorr;
    
    // Toggle usage of robust method to calculate covariance matrix or standard method    
    // Default: use standard                                                              
    bool m_doRobustCov;

    // Tools / storegate info
    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandle<TileRawChannelBuilderFlatFilter> m_adderFilterAlgTool;
    TileBeamElemContByteStreamCnv* m_beamCnv;
    const TileCablingService* m_cabling;
    TileOFCorrelation* m_tileOFCorrelation;

    // Identifiers
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;

    const uint32_t* m_cispar;

    // Name of ntuple and file
    std::string m_ntupleID;
    std::string m_file;
    int64_t m_treeSize;
    int m_nSamples;
//    unsigned int m_nDrawers;

    // Container Parameters
    std::string m_digitsContainer;
    std::string m_beamElemContainer;
    /*  std::string m_flatRawChannelContainer;
     std::string m_fitRawChannelContainer;
     std::string m_optRawChannelContainer;*/
    std::string m_dspRawChannelContainer;

    double m_SumPed2[5][64][48][2];
    double m_SumRms2[5][64][48][2];
    double m_MeanAmp[5][64][48][2];
    double m_MeanAmp_ij[5][64][48][48][2];

    // event number
    int m_evtNr;
    int m_evt[5][64][48][2];

    // Trigger items
    int m_time;
    int m_year;
    int m_month;
    int m_day;
    int m_yday;
    int m_hour;
    int m_min;
    int m_run;
    int m_trigType;
    uint8_t m_ros[5][64][48][2];
    uint8_t m_drawer[5][64][48][2];
    uint8_t m_channel[5][64][48][2];
    bool m_gain[5][64][48][2];
    float m_ped[5][64][48][2];
    float m_lfn[5][64][48][2];
    float m_hfn[5][64][48][2];
    float m_noise_cov[5][64][2];
    float m_auto_corr[5][64][48][2][36];
};

#endif // TILENOISECALIBALG_H
