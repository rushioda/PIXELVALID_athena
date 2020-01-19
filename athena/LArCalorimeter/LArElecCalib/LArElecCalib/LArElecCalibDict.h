/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArElecCalibDict.h
 *
 * @brief This file includes the class for dictionary definitions
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArElecCalibDict.h,v 1.6 2009-01-13 19:02:21 wlampl Exp $
 */
#ifndef LARELECCALIB_LARELECCALIBDICT_H
# define LARELECCALIB_LARELECCALIBDICT_H 


#include "LArElecCalib/LArVectorProxy.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArAdc2GeV.h"
#include "LArElecCalib/ILArAutoCorr.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArAutoCorrTotalTool.h"
//#include "LArElecCalib/ILArAutoCorrDecoderTool.h"
#include "LArElecCalib/ILArCableAttenuation.h"
#include "LArElecCalib/ILArCableLength.h"
#include "LArElecCalib/ILArCaliPulseParams.h"
#include "LArElecCalib/ILArDAC2uA.h"
#include "LArElecCalib/ILArDetCellParams.h"
#include "LArElecCalib/ILArDigitOscillationCorrTool.h"
#include "LArElecCalib/ILArEMEC_Cphi.h"
#include "LArElecCalib/ILArEMEC_HValpha.h"
#include "LArElecCalib/ILArEMEC_HVbeta.h"
#include "LArElecCalib/ILArFEBTimeOffset.h"
#include "LArElecCalib/ILArGlobalTimeOffset.h"
#include "LArElecCalib/ILArH6Oscillation.h"
#include "LArElecCalib/ILArMinBias.h"
#include "LArElecCalib/ILArMphysOverMcal.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArOFC_Shape.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArPhysCaliTdiff.h"
#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILArRinj.h"
#include "LArElecCalib/ILArRterm.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArTdrift.h"
#include "LArElecCalib/ILArTshaper.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILAruA2MeV.h"
#include "LArElecCalib/ILArTiming.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArElecCalib/ILArFEBTempTool.h"

#endif // LARELECCALIB_LARELECCALIBDICT_H
