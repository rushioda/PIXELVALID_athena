/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef TILERECUTILS_TILERAWCHANNELOF1CORRECTOR_H
#define TILERECUTILS_TILERAWCHANNELOF1CORRECTOR_H

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// Tile includes
#include "TileRecUtils/ITileRawChannelTool.h"
#include "TileConditions/ITileCondToolDspThreshold.h"

// forward declarations
class TileRawChannel;
class TileRawChannelContainer;
class TileRawChannelCollection;
class TileHWID;
class TileCondToolNoiseSample;
class ITileCondToolOfc;
class TileCondToolTiming;
class TileCondToolEmscale;
//class ITileCondToolDspThresholds;

/**
 @class TileRawChannelOF1Corrector
 @brief This tool correct TileRawChannels amplitudes which came from OF1 DSP if pedestal changed
 */
class TileRawChannelOF1Corrector: public AthAlgTool, virtual public ITileRawChannelTool {
  public:

    /** AlgTool like constructor */
    TileRawChannelOF1Corrector(const std::string& type, const std::string& name, const IInterface* parent);

    /** Virtual destructor */
    virtual ~TileRawChannelOF1Corrector() {};

    /** AlgTool InterfaceID */
    static const InterfaceID& interfaceID();

    /** AlgTool initialize method.*/
    virtual StatusCode initialize() override;
    /** AlgTool finalize method */
    virtual StatusCode finalize() override;

    /** Callback to handle Data-driven GeoModel initialisation */
    virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

    /** Correct TileRawChannel amplitudes if pedestal changed */
    virtual StatusCode process(const TileRawChannelContainer* rawChannelContainer) override;


  private:

    const TileHWID* m_tileHWID; //!< Pointer to TileHWID

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provided noise values
    ToolHandle<ITileCondToolOfc> m_tileCondToolOfc;    
    ToolHandle<TileCondToolTiming> m_tileToolTiming;
    ToolHandle<TileCondToolEmscale> m_tileToolEms;
    ToolHandle<ITileCondToolDspThreshold> m_tileDspThreshold;

    std::string m_digitsContainerName;
    bool m_zeroAmplitudeWithoutDigits;
    bool m_correctPedestalDifference;
};

#endif // TILERECUTILS_TILERAWCHANNELOF1CORRECTOR_H
