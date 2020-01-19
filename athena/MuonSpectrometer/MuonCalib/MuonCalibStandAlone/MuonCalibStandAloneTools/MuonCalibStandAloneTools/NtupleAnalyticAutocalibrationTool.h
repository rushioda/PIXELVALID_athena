/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NtupleAnalyticAutocalibrationTool_H
#define NtupleAnalyticAutocalibrationTool_H

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
class MdtCalibInputSvc;

namespace MuonCalib {
	
class RtCalibrationAnalytic;

/** @class NtupleAnalyticAutocalibrationTool
	Interface tool for the Integration Method for finding an initial r-t 
	relation.
	@author rauscher@cern.ch
*/


class NtupleAnalyticAutocalibrationTool : public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor */
		NtupleAnalyticAutocalibrationTool(const std::string& t, const std::string& n, const IInterface* p);
	/** Tool initialization */
		StatusCode initialize();
	/** Tool finalization */
		inline StatusCode finalize()
			{
			return StatusCode::SUCCESS;
			}
	/** analyze segments */
		StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & segemnts);
	/** set calibratino region */
		void setRegion();
	/** get calibration results */
		const IMdtCalibrationOutput * getResults() const;
//==============================================================================
	private:
	//! pointer to algoritm
		RtCalibrationAnalytic *m_autocalibration;
	//! estimateted initial rt acuracy - job option
		double m_rt_accuracy;
	//! the type of the correction function (LEGENDRE|CHEBYSHEV|POLYGON - job option
		std::string m_func_type;
	//! the function type as a numeric value
	//! = 1: Legendre polynomial,
	//! = 2: Chebyshev polynomial
	//! = 3: polygon equidistant in r
		int m_func_type_num;
	//! order of the correctio nfunctino
		int m_order;
	//! restrict segments to mutilayers - job option
		bool m_split;
	//! lets the algorithm use the full matrix - job option
		bool m_full_matrix;
	//! fix_min, fix_max=true: fix r(t_min), r(t_max) - job option
		bool m_fix_min, m_fix_max;
	//! maximum number of iterations. - job otion
		int max_it;
	//! force monotonuous rt relation - job option
		bool m_force_mono;
	//! control histogram output - job option
		bool m_control_histograms;
	//! calibratin input service
		ServiceHandle<MdtCalibInputSvc> m_calib_input_svc;
	//! is set to true if the analysis failed
		bool m_failed;
	//! is set to true if r(t) should be smoothened using the conventional
	//! autocalibration
		bool m_smoothening;
	//! is set to true if parabolic extrapolation should be performed for small
    //! and large radii
		bool m_parabolic_extrapolation;
	};

#endif
	
}
