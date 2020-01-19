/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/InDetGeoModelTool.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetIdentifier/TRT_ID.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetIdentifier/SCT_ID.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetIdentifier/PixelID.h"

#include "IdDictDetDescr/IdDictManager.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  InDetGeoModelTool::InDetGeoModelTool(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent) {

      //Only declare the interface
      declareInterface<IInDetGeoModelTool>(this);
  }

  /**
   * In initalizie, retrieve all the requested geometry model managers
   */
  StatusCode InDetGeoModelTool::initialize(){

    /**
     * Check for Pixel geo model manager and ID helper
     */

    // Get geo model manager
    if ( detStore()->retrieve(m_PixelGeoManager, "Pixel").isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get Pixel GeoModel Manager!" << endreq;
      return StatusCode::RECOVERABLE;
    }

    // Get identifier helper
    if (detStore()->retrieve(m_PixelIDHelper, "PixelID").isFailure()) {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get Pixel ID helper" << endreq;
        return StatusCode::RECOVERABLE;
    }
  
    /**
     * Check for SCT geo model manager and ID helper
     */

    // Get geo model manager
    if ( detStore()->retrieve(m_SCTGeoManager, "SCT").isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get SCT GeoModel Manager!" << endreq;
      return StatusCode::RECOVERABLE;
    }

    // Get identifier helper
    if (detStore()->retrieve(m_SCTIDHelper, "SCT_ID").isFailure()) {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get SCT ID helper" << endreq;
        return StatusCode::RECOVERABLE;
    }
 
    // check if SLHC geo is used (TRT not implemented)
    // if not SLHC, get the TRT Det Descr manager
    bool isSLHC = false;
    const IdDictManager* idDictMgr;
    if (detStore()->retrieve(idDictMgr, "IdDict").isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get IdDictManager !" << endreq;
      return StatusCode::RECOVERABLE;
    } else {
      const IdDictDictionary* dict = idDictMgr->manager()->find_dictionary("InnerDetector"); 
      if(!dict) {
	if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << " Cannot access InnerDetector dictionary "<< endreq;
	return StatusCode::RECOVERABLE;
      }else{
	//	if (dict->file_name().find("SLHC")!=std::string::npos) isSLHC=true;
	if (dict->m_version.find("SLHC")!=std::string::npos) isSLHC=true;
      }
    }
 
    if(!isSLHC){
      /**
       * Check for TRT geo model manager and ID helper
       */

      // Get geo model manager
      if ( detStore()->retrieve(m_TRTGeoManager, "TRT").isFailure()) {
	if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get TRT GeoModel Manager!" << endreq;
	return StatusCode::RECOVERABLE;
      }

      // Get identifier helper
      if (detStore()->retrieve(m_TRTIDHelper, "TRT_ID").isFailure()) {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get TRT ID helper" << endreq;
        return StatusCode::RECOVERABLE;
      }
    }

    return StatusCode::SUCCESS;
  }
 
}//namespace





