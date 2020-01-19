/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.10.2006, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>
#include <typeinfo>
#include "cmath"

// Gaudi //
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGateSvc.h"

//geo model
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

// MuonCalib //
#include "MdtCalibIOSvc/MdtCalibOutputDbSvc.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"
#include "MdtCalibT0/T0CalibrationOutput.h"
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MdtCalibRt/RtCalibrationOutput.h" 
#include "MdtCalibData/IRtRelation.h"
//#include "MdtCalibData/RtChebyshev.h"
//#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtResolutionLookUp.h"
//#include "MuonCalibIdentifier/MuonFixedId.h"

#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
//#include "MuonCalibDbOperations/CalibDBCoral.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/RtResolutionFromPoints.h"

#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
using namespace MuonCalib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS MdtCalibOutputDbSvc ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//////////////////
// SET POINTERS //
//////////////////

//MdtCalibOutputDbSvc *MdtCalibOutputDbSvc::m_MdtCalibOutputDbSvc_pointer=0;

//*****************************************************************************


//::::::::::::::::::::::::
//:: PUBLIC CONSTRUCTOR ::
//::::::::::::::::::::::::

//MdtCalibOutputDbSvc::MdtCalibOutputDbSvc(const std::string & name,
//					 ISvcLocator *svc_locator) : Service(name, svc_locator), m_results(NULL), m_postprocess_calibration(false), m_write_root_database(false), m_db(NULL), m_iov_start(-1), m_iov_end(-1) {
MdtCalibOutputDbSvc::MdtCalibOutputDbSvc(const std::string &name,ISvcLocator *svc_locator) : AthService(name, svc_locator), m_results(NULL), 
											     m_postprocess_calibration(false), 
											     m_calib_output_tool("MuonCalib::CalibrationFileIOTool"), 
											     m_iov_start(-1), m_iov_end(-1), 
											     m_reg_sel_svc("RegionSelectionSvc", name), 
											     m_input_service("MdtCalibInputSvc",name) {

  declareProperty("PostprocessCalibration", m_postprocess_calibration);
  m_flat_default_resolution=-1;
  declareProperty("FlatDefaultResolution", m_flat_default_resolution);
  declareProperty("OutputTool", m_calib_output_tool);
  m_force_default_resolution=false;
  declareProperty("ForceDefaultResolution", m_force_default_resolution);
  
  declareProperty("MdtCalibInputSvc", m_input_service);
  declareProperty("RegionSelectionSvc", m_reg_sel_svc);
	
  //for the sake of coverity
  m_detMgr=NULL;
  m_mdtIdHelper=NULL;
  m_resolution=NULL;
	
  return;
}

//*****************************************************************************

//:::::::::::::::::::::::::::
//:: METHOD queryInterface ::
//:::::::::::::::::::::::::::
StatusCode MdtCalibOutputDbSvc::queryInterface(const InterfaceID &riid, void **ppvUnknown) {

  if (IID_IMdtCalibOutputDbSvc.versionMatch(riid)) { 
    *ppvUnknown = (MdtCalibOutputDbSvc *)this; 
  } else { 
    return AthService::queryInterface(riid, ppvUnknown); 
  }

  return StatusCode::SUCCESS;
}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD initialize ::
//:::::::::::::::::::::::
StatusCode MdtCalibOutputDbSvc::initialize(void) {
  
//to get service via static function 
//  m_MdtCalibOutputDbSvc_pointer = this;
  ATH_MSG_INFO("initialize MdtCalibOutputDbSvc");

//get id helper and detector manager if postprocessing of calibration is selected
  if(m_postprocess_calibration)	{
    //retrieve detector store
    StoreGateSvc *m_detStore;
    ATH_CHECK( serviceLocator()->service("DetectorStore", m_detStore) );
    //retrieve mdt id helper
    ATH_CHECK( m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );
    //retrieve detector manager
    ATH_CHECK( m_detStore->retrieve( m_detMgr ) );
  }

//get region selection service
  ATH_CHECK( m_reg_sel_svc.retrieve() );
  region_ids=m_reg_sel_svc->GetStationsInRegions();
  ATH_MSG_INFO("Regions selected: "<<region_ids.size() );
//retrieve tool
  ATH_CHECK( m_calib_output_tool.retrieve() );
  ATH_CHECK( m_input_service.retrieve() );

  return StatusCode::SUCCESS;
}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::
StatusCode MdtCalibOutputDbSvc::finalize(void) {
  ATH_CHECK( saveCalibrationResults() );
  ATH_MSG_INFO("Results saved!");
  return StatusCode::SUCCESS;
}

//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD AddRunNumber ::
//::::::::::::::::::::::::::::::
void MdtCalibOutputDbSvc:: AddRunNumber(int run_number) {
  if(run_number < m_iov_start || m_iov_start < 0) {
    m_iov_start = run_number;
  }
  if(run_number > m_iov_end || m_iov_end < 0) {
    m_iov_end = run_number;
  }
}

//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD memorize(., ., .) ::
//::::::::::::::::::::::::::::::
bool MdtCalibOutputDbSvc::memorize( const MuonCalib::IMdtCalibrationOutput *result) {
// no overwriting is allowed //
  m_results=result;
  m_resolution=0;
  return true;
}

//*****************************************************************************

//:::::::::::::::::::::::::::::::::
//:: METHOD memorize(., ., ., .) ::
//:::::::::::::::::::::::::::::::::
bool MdtCalibOutputDbSvc::memorize(const MuonCalib::IMdtCalibrationOutput *result, const MuonCalib::IRtResolution *resol) {
  m_results=result;
  m_resolution=resol;
  return true;
}

//*****************************************************************************

//:::::::::::::::::::::::::::::::::::
//:: METHOD saveCalibrationResults ::
//:::::::::::::::::::::::::::::::::::
StatusCode MdtCalibOutputDbSvc::saveCalibrationResults(void) {

  if(m_results==NULL) return StatusCode::SUCCESS;

  StatusCode sc;
  
///////////////
// VARIABLES //
///////////////

/////////////////////////////////////////////////////////////////////////////
// LOOP OVER THE STORED RESULTS AND WRITE THEM OUT ACCORDING TO THEIR TYPE //
/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
  for (std::vector<MuonCalib::NtupleStationId>::const_iterator it=region_ids.begin(); it!=region_ids.end(); it++) {
//-----------------------------------------------------------------------------
    MuonCalib::NtupleStationId the_id(*it);
// get region geometry if required
    if (m_postprocess_calibration) {
      if(!the_id.InitializeGeometry(m_mdtIdHelper, m_detMgr)) {
	ATH_MSG_ERROR( "Faild to get geometry for " << the_id.regionId() );
      }
    }
// t0 calibration //
    const T0CalibrationOutput *t0_output(dynamic_cast<const T0CalibrationOutput*>(m_results));
    if (t0_output!=0) {
      ATH_MSG_INFO( "Writing out t0s." );
      MdtTubeFitContainer *new_t0s = t0_output->t0s();
      if(t0_output->GetMap().size()) {
	std::map<NtupleStationId, MdtTubeFitContainer*>::const_iterator mit(t0_output->GetMap().find(*it));
	if(mit!=t0_output->GetMap().end()) {
	  new_t0s=mit->second;
	}
      }
      if(!new_t0s) continue;
      if(m_postprocess_calibration) new_t0s = postprocess_t0s(new_t0s, the_id);
      //      if(new_t0s == NULL) cerr<<"new_t0s == NULL"<<endl;
	  
      sc=m_calib_output_tool->WriteT0(new_t0s, the_id, m_iov_start, m_iov_end);
      if(sc.isFailure()) return sc;
    }

    // r-t calibration //
    bool real_resolution(true), real_rt(true);
    const RtCalibrationOutput *rt_output(dynamic_cast<const RtCalibrationOutput*>(m_results));
    if (rt_output!=0) {
      if(rt_output->rt()) {
	if(!rt_output->rt()->HasTmaxDiff()) {
	  const IRtRelation *old_rel=m_input_service->GetRtRelation();
	  if(old_rel && old_rel->HasTmaxDiff()) {
	    const_cast<IRtRelation *>(rt_output->rt())->SetTmaxDiff(old_rel->GetTmaxDiff());
	  }
	}
      }

      ATH_MSG_INFO( "Writing out r-t relationships." );						
      if(m_resolution==NULL) {
	real_resolution=false;
	create_default_resolution(rt_output->rt());
      }	else {
	real_rt=false;
      }
      sc = m_calib_output_tool->WriteRt(rt_output, m_resolution, the_id, m_iov_start, m_iov_end, real_rt, real_resolution);				
      if(sc.isFailure()) return sc;
    }

//-----------------------------------------------------------------------------
  }
//-----------------------------------------------------------------------------
  ATH_MSG_INFO( "Finished writing" );
  return StatusCode::SUCCESS;
}  //end MdtCalibOutputDbSvc::saveCalibrationResults

//*****************************************************************************

//::::::::::::::::::
//:: METHOD reset ::
//::::::::::::::::::
void MdtCalibOutputDbSvc::reset() {
  m_results=0;
  return;
}

MuonCalib::MdtTubeFitContainer* MdtCalibOutputDbSvc::postprocess_t0s(MuonCalib::MdtTubeFitContainer *t0, const MuonCalib::NtupleStationId &id) {
  int n_layers(-1), n_tubes(-1);
  if(id.NMultilayers() <0) return t0;
//get the maximum number of layers and tubes per layer for this station
  for(int i=0; i<id.NMultilayers(); i++) {
    if(n_layers<id.NLayers(i)) n_layers=id.NLayers(i);
    if(n_tubes<id.NTubes(i)) n_tubes=id.NTubes(i);
  }
  MdtTubeFitContainer *new_t0=new MdtTubeFitContainer(id.regionId(), id.NMultilayers(), n_layers, n_tubes);
  new_t0->setImplementation(t0->implementation());
  new_t0->setGroupBy(t0->GroupBy());

//loop over tubes
  double n_goods(0.0);
  double mean_t0(0.0);
  bool has_bad_fits(false);
  for(unsigned int ml=0; ml<t0->numMultilayers(); ml++) {
    if(static_cast<int>(ml)==id.NMultilayers()) break;
    for(unsigned int ly=0; ly<t0->numLayers(); ly++) {
      if(static_cast<int>(ly)==n_layers) break;
      for(unsigned int tb=0; tb<t0->numTubes(); tb++) {
	if(static_cast<int>(tb)==n_tubes) break;
	new_t0->setFit(ml, ly, tb, *(t0->getFit(ml, ly, tb)));
	new_t0->setCalib(ml, ly, tb, *(t0->getCalib(ml, ly, tb)));
	//if tube was ok use for mean t0
	if(t0->getCalib(ml, ly, tb)->statusCode == 0) {
	  n_goods++;
	  mean_t0+=t0->getCalib(ml, ly, tb)->t0;
	} else {
	  has_bad_fits=true;
	}
      }
    }
  }

//check for bad fits and insert mean t0;
  if(has_bad_fits) {
    mean_t0/=n_goods;
    for(unsigned int ml=0; ml<new_t0->numMultilayers(); ml++) {
      for(unsigned int ly=0; ly<new_t0->numLayers(); ly++) {
	for(unsigned int tb=0; tb<new_t0->numTubes(); tb++) {
	  if(t0->getCalib(ml, ly, tb)->statusCode != 0)	{
	    MdtTubeCalibContainer::SingleTubeCalib calib(*(new_t0->getCalib(ml, ly, tb)));
	    calib.t0=mean_t0;
	    new_t0->setCalib(ml, ly, tb, calib);
	  }
	}
      }
    }
  }
  return new_t0;
}  //end MdtCalibOutputDbSvc::postprocess_t0s

inline void MdtCalibOutputDbSvc::create_default_resolution(const MuonCalib::IRtRelation *rt) {
//check if resolution is saved in input service 
  const IRtResolution *old_res = m_input_service->GetResolution();
  const IRtRelation   *old_rel = m_input_service->GetRtRelation();
  if(old_res!=NULL && old_rel!=NULL && !m_force_default_resolution) {
    //scale the old resolution to the new rt relation
    ATH_MSG_INFO( "Taken old resolution" );
    std::vector<SamplePoint> res_points(100);
    for (unsigned int i=0; i<100; i++) {
      double di=static_cast<double>(i);
      double t_new=rt->tLower() + (di/100.0) * (rt->tUpper() - rt->tLower());
      double t_old=old_rel->tLower() + (di/100.0) * (old_rel->tUpper() - old_rel->tLower());
      res_points[i].set_x1(t_new);
      res_points[i].set_x2(old_res->resolution(t_old));
      if(res_points[i].x2()>100 && i>0) res_points[i].set_x2(res_points[i-1].x2());
      res_points[i].set_error(1);
    }
    RtResolutionFromPoints respoints;
    m_resolution=new RtResolutionLookUp(respoints.getRtResolutionLookUp(res_points));
    return;
  }
  ATH_MSG_INFO( "Creating default resolution" );	
  //parameters for default resolution curve
  double alpha[9] = {
    0.31476,
    -0.217661,
    0.118337,
    -0.0374466,
    0.00692553,
    -0.000764969,
    4.97305E-05,
    -1.75457E-06,
    2.59E-08
  }; 
  MuonCalib::CalibFunc::ParVec par_vec;
	
  if(m_flat_default_resolution<0.0) {
    double bin_width=(rt->tUpper() - rt->tLower())/100.0;
    par_vec.resize(102);
    par_vec[0]=rt->tLower();
    par_vec[1]=bin_width;
    for(unsigned int i=0; i<100; i++) {
      double t=rt->tLower() + i*bin_width;
      double r=rt->radius(t);
      double resol(0.0);
      for (int l=0; l<9; l++) {
	resol = resol+alpha[l]*pow(r, l);
      }
      par_vec[i+2]=resol;
    }
  } else {
    par_vec.resize(4);
    par_vec[0]=rt->tLower();
    par_vec[1]=(rt->tUpper() - rt->tLower())/2.0;
    par_vec[2]=m_flat_default_resolution;
    par_vec[3]=m_flat_default_resolution;
  }
  m_resolution = new MuonCalib::RtResolutionLookUp(par_vec);
}  //end MdtCalibOutputDbSvc::create_default_resolution
