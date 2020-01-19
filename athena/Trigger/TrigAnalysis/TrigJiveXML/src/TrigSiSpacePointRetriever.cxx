/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/TrigSiSpacePointRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/PixelDetectorManager.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  TrigSiSpacePointRetriever::TrigSiSpacePointRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("TrigS3D")
  {

    declareInterface<IDataRetriever>(this);

    m_pixelSpContainerName = "TrigPixelSpacePoints";
    m_SCT_SpContainerName  = "TrigSCT_SpacePoints";
    declareProperty("PixelSpacePoints", m_pixelSpContainerName);
    declareProperty("SCTSpacePoints"  , m_SCT_SpContainerName);
  }

  //--------------------------------------------------------------------------

  StatusCode TrigSiSpacePointRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    const TrigSiSpacePointContainer* pCont;

    DataVect x;
    DataVect y; 
    DataVect z; 
    DataVect layer; 
    DataVect clustersVec; 
    std::string clustersStr = "clusters multiple=\"2\"";
  
    if ( evtStore()->retrieve(pCont,m_pixelSpContainerName).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trig SP Pixel container not found at "
          << m_pixelSpContainerName << endreq;
//      return StatusCode::SUCCESS; // not exit here (as in original code)
    }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieving TrigSPPixel with size " 
         << pCont->size() << " from " << m_pixelSpContainerName << endreq;
 
    int maxHash = m_pixelHelper->wafer_hash_max();
     for(int id=0;id<maxHash;++id){
	    TrigSiSpacePointContainer::const_iterator spCollIt=pCont->indexFind(id);
	    if(spCollIt==pCont->end()) continue;
	    for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin(); spIt!=(*spCollIt)->end();++spIt){
		x.push_back(DataType((*spIt)->x() /10.));
		y.push_back(DataType((*spIt)->y() /10.));
		z.push_back(DataType((*spIt)->z() /10.));
	        layer.push_back( DataType( calcPixelLayer( (*spIt)->identify() ) ) );
		clustersVec.push_back(DataType(((*spIt)->clusters()).first->identify().get_compact()));
		clustersVec.push_back(DataType(-1));
	    }
      }
    }
    if ( evtStore()->retrieve(pCont,m_SCT_SpContainerName).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trig SP Pixel container not found at "
          << m_SCT_SpContainerName << endreq;
//      return StatusCode::SUCCESS; // not exit here (as in original code)
    }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieving TrigSPPixel with size " 
         << pCont->size() << " from " << m_SCT_SpContainerName << endreq;

    int maxHash = m_sctHelper->wafer_hash_max();
       for(int id=0;id<maxHash;++id){
	    TrigSiSpacePointContainer::const_iterator spCollIt=pCont->indexFind(id);
	    if(spCollIt==pCont->end()) continue;
	    for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin(); spIt!=(*spCollIt)->end();++spIt){
		x.push_back(DataType((*spIt)->x() /10.));
		y.push_back(DataType((*spIt)->y() /10.));
		z.push_back(DataType((*spIt)->z() /10.));
	        layer.push_back( DataType( calcSCTLayer( (*spIt)->identify() ) ) );
		clustersVec.push_back(DataType(((*spIt)->clusters()).first->identify().get_compact()));
		clustersVec.push_back(DataType(((*spIt)->clusters()).second->identify().get_compact()));
	    }
       }
    }
    DataMap myDataMap;
    myDataMap["x"] = x;
    myDataMap["y"] = y;
    myDataMap["z"] = z;
    myDataMap["layer"] = layer;
    myDataMap[clustersStr] = clustersVec;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< x.size() << endreq;

    //forward data to formating tool
    std::string emptyStr="";
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &myDataMap);
  }
  //--------------------------------------------------------------------------
  
  StatusCode TrigSiSpacePointRetriever::initialize(){

    const InDetDD::PixelDetectorManager* m_pixel_geomanager;
    if (detStore()->retrieve(m_pixel_geomanager, "Pixel").isFailure() ){
	if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get Pixel GeoModel Manager!" << endreq;
	return StatusCode::RECOVERABLE;
    }

    const InDetDD::SCT_DetectorManager* m_sct_geomanager;
    if ( detStore()->retrieve(m_sct_geomanager, "SCT").isFailure() ){
	if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get SCT GeoModel Manager!" << endreq;
	return StatusCode::RECOVERABLE;
      } 

//migration: https://twiki.cern.ch/twiki/bin/view/Atlas/InDetPkgFixing 
    if ( detStore()->retrieve(m_pixelHelper, "PixelID").isFailure() ){
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get Pixel ID helper" << endreq;
        return StatusCode::RECOVERABLE;
    }

    if ( detStore()->retrieve(m_sctHelper, "SCT_ID").isFailure() ){
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)  << "Could not get SCT ID helper" << endreq;
        return StatusCode::RECOVERABLE;
    }

    return StatusCode::SUCCESS;
  }
  
  //--------------------------------------------------------------------------

  int TrigSiSpacePointRetriever::calcPixelLayer(const Identifier& id)
  {
    int layer = m_pixelHelper->layer_disk( id );
    if ( m_pixelHelper->barrel_ec( id ) )  layer += 3; 
    // Endcap Pixels 3,4,5
    return layer;
  }
  
  //--------------------------------------------------------------------------

  int TrigSiSpacePointRetriever::calcSCTLayer(const Identifier& id)
  {
    int layer = m_sctHelper->layer_disk( id );
    if ( m_sctHelper->barrel_ec( id ) )  layer += 11;
    // Endcap SCT 11,12,13,14,15,16,17,18,19
    else  layer += 7;                               
    // Barrel SCT 7,8,9,10
    return layer;
  }

  //--------------------------------------------------------------------------
}
