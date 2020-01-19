/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT2CALOCALIBRATION_IEGAMMACALIBRATION
#define TRIGT2CALOCALIBRATION_IEGAMMACALIBRATION

#include "GaudiKernel/IAlgTool.h"
#include "xAODTrigCalo/TrigEMCluster.h"

// Just for the pointer definition
//class TrigEMCluster;

static const InterfaceID IID_IEgammaCalibration("IEgammaCalibration",1,0);

/** General Interface for calibrations at the LVL2 Egamma Calo Fex algo */
class IEgammaCalibration : virtual public IAlgTool{
	public:
		static const InterfaceID& interfaceID();

	/** method to perform the correction. The correction
		type is defined by the tool which also uses
		this interface. In some cases, the tool needs
		more than the cluster to perform the calibration.
		This can be passed via the void pointer */
	virtual void makeCorrection(xAOD::TrigEMCluster*, const void* par=NULL) = 0;
		
};

inline const InterfaceID& IEgammaCalibration::interfaceID()
{
	return IID_IEgammaCalibration;
}

#endif
