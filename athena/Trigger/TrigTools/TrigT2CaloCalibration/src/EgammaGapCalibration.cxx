/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     EgammaGapCalibration.cxx
// PACKAGE:  Trigger/TrigTools/EgammaGapCalibration.cxx
//
// AUTHOR:   Denis Oliveira Damazio
//
// REFERENCES: Tool to provide Longitudinal Weight calibration
// Based on Scott's code for the offline
//
// ********************************************************************

#include "TrigT2CaloCalibration/EgammaGapCalibration.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloGeoHelpers/CaloSampling.h"

using CaloClusterCorr::interpolate;

StatusCode EgammaGapCalibration::initialize(){

	CHECK (AthAlgTool::initialize());
	CHECK (CaloRec::ToolWithConstantsMixin::initialize() );
	m_log = new MsgStream(AthAlgTool::msgSvc(), name() );

	(*m_log) << MSG::DEBUG << "Initialize Tool : " << name() << endreq;

	return StatusCode::SUCCESS;

}


StatusCode EgammaGapCalibration::finalize(){

	(*m_log) << MSG::DEBUG << "Finalize Tool : " << name() << endreq;
	delete m_log;

	return StatusCode::SUCCESS;

}

void EgammaGapCalibration::makeCorrection(xAOD::TrigEMCluster* clus,
					  const void * /*param*/){
	
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "makeCorrection for tool : "
		<< name() << endreq;
	(*m_log) << MSG::DEBUG << "Cluster E input : " <<
		clus->energy() << endreq;
#endif
	float the_aeta=(clus->eta());
	if (the_aeta<0) the_aeta=-the_aeta;
	// If far from the crack, nothing to do
	if (the_aeta < m_eta_start_crack || the_aeta > m_eta_end_crack) return;
 
	float a      = interpolate(m_correction, the_aeta, m_degree,1);
	float alpha  = interpolate(m_correction, the_aeta, m_degree,2);
	float offset = interpolate(m_correction, the_aeta, m_degree,3);
	float eh_scint = clus->energy(CaloSampling::TileGap3);
	float ec = clus->energy();
	clus->setEnergy(a*(ec+alpha*eh_scint + offset));
	clus->setEt(clus->energy()/cosh(clus->eta()));
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "Cluster E output : " <<
		clus->energy() << endreq;
#endif

}

StatusCode
EgammaGapCalibration::setProperty (const std::string& propname,
                                    const std::string& value)
{
  CHECK( AthAlgTool::setProperty(propname,value) );
  CHECK( CaloRec::ToolWithConstantsMixin::setProperty (propname, value) );
  return StatusCode::SUCCESS;
}

StatusCode
EgammaGapCalibration::setProperty (const Property& p)
{
  CHECK( AthAlgTool::setProperty(p) );
  CHECK( CaloRec::ToolWithConstantsMixin::setProperty (p) );
  return StatusCode::SUCCESS;
}
