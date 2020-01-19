/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILECISDEFAULTCALIBTOOL_H
#define TILECALIBALG_TILECISDEFAULTCALIBTOOL_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/HWIdentifier.h"

// Tile includes
#include "TileCalibAlgs/ITileCalibTool.h"
#include "TileConditions/TileCablingService.h"


#include "TString.h"
#include <stdint.h>
#include <string> 
#include <map>

// Forward declaration
class TileBeamInfoProvider;
class TileHWID;
class TileCablingSvc;
class TFile;
class TileRawChannelContainer;
class TMap;
class ITileStuckBitsProbsTool;

class TileCisDefaultCalibTool: public AthAlgTool
                             , virtual public ITileCalibTool {

  public:
    TileCisDefaultCalibTool(const std::string& type, const std::string& name,
        const IInterface* pParent);
    virtual ~TileCisDefaultCalibTool();

    virtual StatusCode initialize();
    virtual StatusCode initNtuple(int runNumber, int runType, TFile * rootfile);
    virtual StatusCode execute();
    virtual StatusCode finalizeCalculations();
    virtual StatusCode writeNtuple(int runNumber, int runType, TFile * rootfile);
    virtual StatusCode finalize();

  private:

    // Bit definitions for calibration quality flag
    enum QualityType {
      includedBit = 0,
      calibratedBit = 1,
      rangeBit = 2,
      probBit = 3,
      noiseBit = 4,
      injRMSBit = 5,
      digiErrorBit = 6,
      probChi2Bit = 7,
      edgeSamp = 8,
      nextToEdgeSamp = 9,
      stuckbitBit = 10
    };
    // typedefs for maps
    // dac map: uses dac value as key
    typedef std::map<uint32_t, int> TDACIntMap;
    typedef std::map<uint32_t, double> TDACDoubleMap;
    typedef std::map<uint32_t, double>::iterator TDACDoubleMapIter;
    // adc map: uses hwid as key
    // values of these maps are dac maps
    typedef std::map<HWIdentifier, TDACIntMap*> TAdcIntMap;
    typedef std::map<HWIdentifier, TDACDoubleMap*> TAdcDoubleMap;
    typedef std::map<HWIdentifier, TDACDoubleMap*>::iterator TAdcDoubleMapIter;

    // Maps for number of events, mean, and rms:
    // Fill these with "running" values, i.e. values are updated event-by-event,
    TAdcIntMap m_NEvtMap;
    TAdcIntMap m_NDigitalErrorsMap;
    TAdcDoubleMap m_MeanMap;
    TAdcDoubleMap m_MeanSqMap;

    // Tools / storegate info
    const TileHWID* m_tileHWID;
    const TileCablingService* m_cabling;
    ServiceHandle<TileCablingSvc> m_cablingSvc;
    ToolHandle<TileBeamInfoProvider> m_beamPrv;
    ToolHandle<ITileStuckBitsProbsTool> m_stuckBitsProbs;

    // jobOptions
    std::string m_rawChannelContainerName;
    std::string m_ntupleID;
    std::string m_DigitsContainerName;
    // whether or not to use small capacitor, by default no
    bool m_useSmallCap;

    // remove pedestal events: should only be necessary for runs where
    // pedestal triggers are mixed with CIS triggers
    bool m_removePed;

    // phase range for calibration, by default use full range
    double m_phaseMin;
    double m_phaseMax;

    // Maximum charge (gain-dependent) for filling dac map: not equivalent to charge range used in linear fit
    // by default, fill low gain up to 700 pC, high gain up to 12.5 pC
    double m_chargeMaxHi;
    double m_chargeMaxLo;

    // Range for linear fits: by default do high gain fit from 3-12 pC, and low gain from 300 - 700 pC
    double m_linfitMaxHi;
    double m_linfitMinHi;
    double m_linfitMaxLo;
    double m_linfitMinLo;

    bool m_doSampleChecking;

    // Results
    float m_calib[5][64][48][2];
    int m_qflag[5][64][48][2];
    int m_nDAC[5][64][48][2]; // This is now deprecated since you can get this form the TGraph
    int m_nDigitalErrors[5][64][48][2];
    float m_chi2[5][64][48][2];

    // These will not be saved to the ntuple
    // They are for the sample check that gets written to qflag
    // Mike Miller - 4 June 2009
    int m_edgeSample[5][64][48][2];
    int m_nextToEdgeSample[5][64][48][2];

    // these arrays are used to contain information about stuck bits in adc's
    // this array pertains to the "stuck bit" quality flag; it is not written to
    // the ntuple
    int m_SampleBit[5][64][48][2][10];
    // This array contains information about each bit in the adc
    // it IS written into the ntuple
    unsigned short m_BitStatus[5][64][48][2][4];
    // used to count the number of injection samples analyzed for odd bit behavior
    int m_NumSamp[5][64][48][2];

    //  TList *scanList;  // This is now deprecated and replaced by the map for speed -CT March 09
    TMap* m_scanMap;
    TMap* m_scanMapRMS;

    // Functions
    inline int chanIsConnected(int ros, int chan) {
      if (m_cabling->channel2hole(ros, chan) < 0) return 0; //negative means not connected
      return 1;
    }

    inline void setBit(QualityType qb, int& bitflag) {
      bitflag |= (1 << qb);
    }

    inline void unsetBit(QualityType qb, int& bitflag) {
      bitflag &= ~(1 << qb);
    }

    inline TString arrayString(int ros, int drawer, int chan, int gain) {
      TString str = "";
      str += ros;
      str += "_";
      str += drawer;
      str += "_";
      str += chan;
      str += "_";
      str += gain;
      return str;
    }

};

#endif // #ifndef TILECALIBALG_TILECISDEFAULTCALIBTOOL_H
