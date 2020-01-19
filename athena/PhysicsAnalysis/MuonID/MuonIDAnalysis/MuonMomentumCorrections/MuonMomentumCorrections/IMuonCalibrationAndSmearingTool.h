/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H
#define MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODMuon/Muon.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicCode.h"     
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"

namespace CP {

class IMuonCalibrationAndSmearingTool : public virtual asg::IAsgTool, virtual public CP::ISystematicsTool {

  //::: Declare the interface that the class provides
  ASG_TOOL_INTERFACE( CP::IMuonCalibrationAndSmearingTool )

public:
  //::: Apply the correction on a modifyable object
  virtual CorrectionCode applyCorrection( xAOD::Muon& mu ) = 0;
  //::: Create a corrected copy from a constant muon
  virtual CorrectionCode correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) = 0;
  //::: Is the tool affected by a specific systematic?
  virtual bool isAffectedBySystematic( const SystematicVariation& systematic ) const = 0;
  //::: Which systematics have an effect on the tool's behaviour?
  virtual SystematicSet affectingSystematics() const = 0;
  //::: Systematics to be used for physics analysis
  virtual SystematicSet recommendedSystematics() const = 0;
  //::: Use specific systematic
  virtual SystematicCode applySystematicVariation ( const SystematicSet& systConfig ) = 0;
  //::: External setting of random seed
  virtual void setRandomSeed( unsigned seed ) = 0;

}; // class IMuonCalibrationAndSmearingTool

} // namespace CP

#endif // MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H
