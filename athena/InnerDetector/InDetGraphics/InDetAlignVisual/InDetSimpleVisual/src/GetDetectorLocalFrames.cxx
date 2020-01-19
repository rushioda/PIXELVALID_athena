/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    Authors: John Alison <johnda@hep.upenn.edu>
    
    Date: 22 Aug 2008
    
    Description: This algorithm loops over the Inner Detector elements 
    and prints thier global positions to a text file

*/

#include "InDetSimpleVisual/GetDetectorLocalFrames.h"

#include "GaudiKernel/MsgStream.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "EventPrimitives/EventPrimitives.h"

#include <stdlib.h>
#include <string.h>
#include <vector>


/** Constructor */
GetDetectorLocalFrames::GetDetectorLocalFrames(std::string const&  name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_outputFileName("IDLocalFrames.txt"),  
  
  /** Pixel Variables */

  /** SCT variables */  

  /** TRT variables */    
  m_trt_barrel_ec(0),
  m_trt_layer_or_wheel(0),
  m_trt_phi_module(0),
  m_trt_straw_layer(0),
  m_trt_straw(0),

  /** ID Tools */
  m_PixelHelper(0),
  m_pixelDetectorManager(0),
  m_SCTHelper(0),
  m_SCTDetectorManager(0),
  m_TRTHelper(0),
  m_TRTDetectorManager(0)

{
  declareProperty("OutputTextFile",m_outputFileName);
}

/** initialize */
StatusCode GetDetectorLocalFrames::initialize(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "initialize()" << endreq;
  
  /** Retrive TRT info */
  if (detStore()->retrieve(m_TRTHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the TRT ID" << endreq;

  if ((detStore()->retrieve(m_TRTDetectorManager)).isFailure()) {
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endreq;
    return StatusCode::FAILURE;
  }
  
  /** Retrive SCT info */
  if (detStore()->retrieve(m_SCTHelper, "SCT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the SCT ID" << endreq;

  if ((detStore()->retrieve(m_SCTDetectorManager)).isFailure()) {
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving SCT_DetectorManager" << endreq;
    return StatusCode::FAILURE;
  }
  
  /** Retrive Pixel info */
  if (detStore()->retrieve(m_PixelHelper, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
    return StatusCode::FAILURE;
    }
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "got the Pixel ID" << endreq;
  
  if ((detStore()->retrieve(m_pixelDetectorManager)).isFailure()) {
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problem retrieving PixelDetectorManager" << endreq;
    return StatusCode::FAILURE;
  }
  
  /** Output text File */
  outputFile.open((m_outputFileName).c_str());
  
  return StatusCode::SUCCESS;
}

/** execute */
StatusCode GetDetectorLocalFrames::execute() {
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "execute() check global position" << endreq;
  
    StatusCode sc = StatusCode::SUCCESS;
    static int eventCount(-1); eventCount++;
    
    /** run only for one event */
    if (eventCount!=0) 
      return sc; 
    
    std::cout << "========================================================================================" <<std::endl;
    std::cout << "==================== Getting the Inner Detector Local Frames ===========================" <<std::endl;
    std::cout << "========================================================================================" <<std::endl;

    //Write pixel positions
    writePixelFames();

    //Write SCT positions
    writeSCTFrames();

    //Write SCT positions
    writeTRTFrames();

    return StatusCode::SUCCESS;
}

/** Finalize */
StatusCode GetDetectorLocalFrames::finalize() {
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "finalize()" << endreq;
  
  /** Close the file */
  outputFile.close();

  return StatusCode::SUCCESS;
}

/** Writing the Pixel Positions */
void GetDetectorLocalFrames::writePixelFames(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writePixelFames()" << endreq;

  //Loop over pixel elements
  std::vector<Identifier>::const_iterator pixIt = m_PixelHelper->wafer_begin();
  std::vector<Identifier>::const_iterator pixItE = m_PixelHelper-> wafer_end();
  for(; pixIt != pixItE; pixIt++  ) {
    
    //InDetDD::SiDetectorElement* si_hit = m_pixelDetectorManager->getDetectorElement( *pixIt );
    // Get local Frame
  
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writePixelFames()" << endreq;
  return;
}

/** Writing the SCT Positions */
void GetDetectorLocalFrames::writeSCTFrames(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeSCTFrames()" << endreq;
  
  //Loop over SCT elements
  std::vector<Identifier>::const_iterator sctIt = m_SCTHelper->wafer_begin();
  std::vector<Identifier>::const_iterator sctItE = m_SCTHelper->wafer_end();
  for(; sctIt != sctItE; sctIt++  ) {
    
    //InDetDD::SiDetectorElement* si_hit = m_SCTDetectorManager->getDetectorElement( *sctIt );
    // Get local Frame
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeSCTFrames()" << endreq;
  return;
}

/** Writing the Detailed (all straws) TRT Positions */
void GetDetectorLocalFrames::writeTRTFrames(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTRTFrames()" << endreq;
  
  TRT_ID::const_expanded_id_iterator trtStrawIt = m_TRTHelper->straw_begin();
  TRT_ID::const_expanded_id_iterator trtStrawItE = m_TRTHelper->straw_end();
  
  for (; trtStrawIt != trtStrawItE; ++trtStrawIt) {
    const ExpandedIdentifier& expId = (*trtStrawIt);
    
    Identifier trtId = m_TRTHelper->straw_id(expId[2], 
					     expId[3], 
					     expId[4],
					     expId[5], 
					     expId[6]);
    
    m_trt_barrel_ec = m_TRTHelper->barrel_ec(trtId);
    m_trt_layer_or_wheel = m_TRTHelper->layer_or_wheel(trtId);
    m_trt_phi_module = m_TRTHelper->phi_module(trtId);
    m_trt_straw_layer = m_TRTHelper->straw_layer(trtId);
    m_trt_straw = m_TRTHelper->straw(trtId);
    
    outputFile << 3 << " "
	       << m_trt_barrel_ec << " "
	       << m_trt_layer_or_wheel << " "
	       << m_trt_phi_module << " "
	       << m_trt_straw_layer << " "
	       << m_trt_straw << " ";
    outputFile << std::endl;

    //const HepGeom::Transform3D& localTransform = m_TRTDetectorManager->getElement( trtId )->transform(trtId);
    Amg::Vector3D xaxis(1,0,0);
    Amg::Vector3D yaxis(0,1,0);
    Amg::Vector3D zaxis(0,0,1);
    const Amg::Vector3D strawXAxis = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * xaxis * m_TRTDetectorManager->getElement( trtId )->strawDirection();
    const Amg::Vector3D strawYAxis = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * yaxis * m_TRTDetectorManager->getElement( trtId )->strawDirection();
    const Amg::Vector3D strawZAxis = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * zaxis * m_TRTDetectorManager->getElement( trtId )->strawDirection();
    
    const Amg::Vector3D strawXAxis_NoSign = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * xaxis;
    const Amg::Vector3D strawYAxis_NoSign = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * yaxis;
    const Amg::Vector3D strawZAxis_NoSign = m_TRTDetectorManager->getElement( trtId )->strawTransform(m_trt_straw) * zaxis;
    //const HepGeom::Vector3D<double> theStrawAxis = m_TRTDetectorManager->getElement( trtId )->strawAxis(m_trt_straw);
    
    //writeTransForm(localTransform);
    writeVector("straw x-axis",strawXAxis);
    writeVector("straw y-axis",strawYAxis);
    writeVector("straw z-axis",strawZAxis);
    outputFile << std::endl;
    writeVector("straw x-axis (No sign)",strawXAxis_NoSign);
    writeVector("straw y-axis (No sign)",strawYAxis_NoSign);
    writeVector("straw z-axis (No sign)",strawZAxis_NoSign);
    outputFile << std::endl;
  }
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTRTFrames()" << endreq;
  return;
}

void GetDetectorLocalFrames::writeVector(std::string name, const Amg::Vector3D& vector){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeVector()" << endreq;
  
  outputFile << name << " " << vector.x() << " " << vector.y() << "  " << vector.z() << std::endl;
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeVector()" << endreq;
  return;
}

void GetDetectorLocalFrames::writeTransForm(const HepGeom::Transform3D& transform){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In writeTransForm()" << endreq;
  
  outputFile << " Trans(x,y,z): " << transform.dx() << " " << transform.dy() << "  " << transform.dz() << std::endl;
  outputFile << " Rotation Matrix " << std::endl;
  outputFile << transform.xx() << " " << transform.xy() << "  " << transform.xz() << std::endl;
  outputFile << transform.yx() << " " << transform.yy() << "  " << transform.yz() << std::endl;
  outputFile << transform.zx() << " " << transform.zy() << "  " << transform.zz() << std::endl;

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Leaving writeTransForm()" << endreq;
  return;
}

