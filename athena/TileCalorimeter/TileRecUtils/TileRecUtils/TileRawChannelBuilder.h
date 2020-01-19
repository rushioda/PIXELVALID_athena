/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TILERECUTILS_ITILERAWCHANNELBUILDER_H
#define TILERECUTILS_ITILERAWCHANNELBUILDER_H

/********************************************************************
 *
 * NAME:     TileRawChanelBuilder
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  K. Gellerstedt
 * CREATED:  Nov 17 2003
 *
 * PURPOSE:  Interface for builder algorithms
 *
 *  Input: TileDigitsContainer
 *  Output: Container with TileRawChannels
 *  Parameters:
 *    TileRawChannelContainer - Name of output container
 *   
 ********************************************************************/

//Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"

#include <utility>
#include <vector>

class TileDigits;
class TileRawChannel;
class TileID;
class TileHWID;
class TileInfo;
class TileBeamInfoProvider;
class ITileRawChannelTool;
class StoreGateSvc;
class MsgStream;

//class vector;

#include "TileIdentifier/TileFragHash.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsCollection.h"


typedef std::vector<std::pair<TileRawChannel*, const TileDigits*> > Overflows_t;

class TileRawChannelBuilder: public AthAlgTool {
  public:
    TileRawChannelBuilder(const std::string& type, const std::string& name,
        const IInterface* parent);

    virtual ~TileRawChannelBuilder();

    // Algorithm tool virtual methods
    virtual StatusCode initialize();
    //virtual StatusCode execute();
    virtual StatusCode finalize();

    /**
     * Create container in SG with name given by
     * parameter (m_TileRawChannelContainerID)
     */
    virtual StatusCode createContainer();

    /**
     * Builder virtual method to be implemented by subclasses
     * @param digits Pointer to TileDigitsContainer
     *
     */
    virtual TileRawChannel* rawChannel(const TileDigits* digits);

    /**
     * Commit RawChannelContiner in SG and make const
     */
    virtual StatusCode commitContainer();

    void initLog();
    void endLog();

    // process one digit and store result in internal container
    void build(const TileDigits* digits) {
      m_rawChannelCnt->push_back(rawChannel(digits));
    }

    // find all bad patterns in a drawer and fill internal static arrays
    void fill_drawer_errors(const TileDigitsCollection* collection);

    // process all digits from one collection and store results in internal container
    void build(const TileDigitsCollection* collection);

    // process digits from a given vector and store results in internal container
    template<class ITERATOR>
    void build(const ITERATOR & begin, const ITERATOR & end) {
      for (ITERATOR rawItr = begin; rawItr != end; ++rawItr)
        m_rawChannelCnt->push_back(rawChannel((*rawItr)));
    }

    // process digits from a given vector and store results in collection
    template<class ITERATOR, class COLLECTION>
    void build(const ITERATOR & begin, const ITERATOR & end, COLLECTION * coll) {
      initLog();
      for (ITERATOR rawItr = begin; rawItr != end; ++rawItr)
        coll->push_back(rawChannel((*rawItr)));
      endLog();
    }

    /**
     * AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

    static double correctAmp(double phase, bool of2 = true); //!< Amplitude correction factor according to the time when using weights for tau=0 without iterations

    static double correctTime( double phase, bool of2 = true); //!< Time correction factor

    static int CorruptedData(int ros, int drawer, int channel, int gain,
        const std::vector<float> & digits, float &dmin, float &dmax);

    static const char* BadPatternName(float ped);

    static void resetDrawer();
    void resetOverflows(void);
    Overflows_t& getOverflowedChannels(void);
    std::string getTileRawChannelContainerID(void);

  protected:
    // FIXME: Get rid of this abomination.
    friend class TileHid2RESrcID;

    // properties
    std::string m_TileRawChannelContainerID;

    // RawChannelContainer
    TileRawChannelContainer* m_rawChannelCnt;

    // parameters for RawChannelContainer
    TileFragHash::TYPE m_rChType;
    TileRawChannelUnit::UNIT m_rChUnit;
    unsigned int m_bsflags;

    // Should energy be calibrated
    bool m_calibrateEnergy;

    // Should time be calibrated (delta added from laser run)
    bool m_correctTime;


    // Use DSP noise correction for incomplete containers
    bool m_useDSP;

    // thresholds for parabolic amplitude correction (used in optimal filter without iterations)
    float m_ampMinThresh;   //!< correct amplitude if it's above amplitude threshold (in ADC counts)
    float m_timeMinThresh;   //!< correct amplitude is time is above time min threshold
    float m_timeMaxThresh;   //!< correct amplitude is time is below time max threshold

    // runType used to override value of trigType in event
    int m_runType;

    // Tile objects
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
    const TileInfo* m_tileInfo;

    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandleArray<ITileRawChannelTool> m_noiseFilterTools;

    int m_trigType;
    bool m_idophys;   // Phys fitting
    bool m_idolas; // Laser fitting
    bool m_idoped; // Ped run, phys fitting
    bool m_idocis; // CIS fitting
    int m_cischan; // which channel is fired by CIS
    double m_capdaq; // Capacitor

    unsigned int m_evtCounter;
    unsigned int m_chCounter;

    int m_nChL;
    int m_nChH;
    double m_RChSumL;
    double m_RChSumH;

    Overflows_t m_overflows;
    int m_dataPoollSize;

    static const int MAX_CHANNELS = 48;
    static const int MAX_DMUS = 16;
    static int s_error[MAX_CHANNELS];
    static int s_dmuerr[MAX_DMUS];
    static int s_lastDrawer;
    static bool s_badDrawer;
};

#endif
