/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/TRTRetriever.h"
#include "StoreGate/DataHandle.h"
#include "JiveXML/DataType.h"

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkEventPrimitives/ParamDefs.h"

namespace JiveXML {

 /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  TRTRetriever::TRTRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("TRT"),
    geo("JiveXML::InDetGeoModelTool/InDetGeoModelTool",this){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    //And the properties
    declareProperty("TRTClusters"  , m_TRTDriftCircleCollName = "TRT_DriftCircles" );
    declareProperty("TRT_TruthMap" , m_TRTTruthMapName = "PRD_MultiTruthTRT" );

  }

  StatusCode TRTRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() <<endreq; 
  
    //First try to retrieve the DriftCircleContainer
    const InDet::TRT_DriftCircleContainer* DriftCircleContainer;
    if ( evtStore()->retrieve(DriftCircleContainer, m_TRTDriftCircleCollName).isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to retrive TRT_DriftCircleContainer with name " << m_TRTDriftCircleCollName << endreq;
      return StatusCode::RECOVERABLE;
    }

    //Also try to obtain the truth container
    const PRD_MultiTruthCollection* TRTMultiTruthMap = NULL;
    if ( evtStore()->contains<PRD_MultiTruthCollection>(m_TRTTruthMapName) ){
      if ( evtStore()->retrieve(TRTMultiTruthMap, m_TRTTruthMapName).isFailure() ){
        //Only warn if this container is not available
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to retrieve PRD_MultiTruthCollection with name " << m_TRTTruthMapName << endreq;
      }
    }

    //Get total size of all all drift circles in all collections
    unsigned long NDriftCircleTotal = 0;
    //Loop over collections in container
    InDet::TRT_DriftCircleContainer::const_iterator DriftCircleContItr = DriftCircleContainer->begin();
    for ( ; DriftCircleContItr != DriftCircleContainer->end(); ++DriftCircleContItr)
      NDriftCircleTotal += (*DriftCircleContItr)->size();

    //be verbose
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Reserving space for " << NDriftCircleTotal << " entries" << endreq;
    
    //Rerserve space in the map
    DataVect rhoz; rhoz.reserve(NDriftCircleTotal);
    DataVect phi; phi.reserve(NDriftCircleTotal);
    DataVect driftR; driftR.reserve(NDriftCircleTotal);
    DataVect threshold; threshold.reserve(NDriftCircleTotal);
    DataVect ident; ident.reserve(NDriftCircleTotal);
    DataVect sub; sub.reserve(NDriftCircleTotal);
    DataVect noise; noise.reserve(NDriftCircleTotal);
    DataVect timeOverThreshold; timeOverThreshold.reserve(NDriftCircleTotal);
    DataVect bitPattern; bitPattern.reserve(NDriftCircleTotal); 
    DataVect numBarcodes; numBarcodes.reserve(NDriftCircleTotal);
    DataVect barcodes; barcodes.reserve(NDriftCircleTotal); //< on average less than one, so this should be enough

    //Now loop over container to retrieve the data
    DriftCircleContItr = DriftCircleContainer->begin();
    for (; DriftCircleContItr != DriftCircleContainer->end(); ++DriftCircleContItr) {
      
      //Get the DriftCircle collection
      const InDet::TRT_DriftCircleCollection* DriftCircleColl = (*DriftCircleContItr);  

      //Now loop over the collection in the container
      InDet::TRT_DriftCircleCollection::const_iterator DriftCircleCollItr = DriftCircleColl->begin();
      for ( ; DriftCircleCollItr != DriftCircleColl->end(); ++DriftCircleCollItr){

        //Get the drift cirlce itself and its unique identifier
        InDet::TRT_DriftCircle* driftcircle = (*DriftCircleCollItr);

        //In verbose mode, print out drift circle information
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Retrieving information from " << (*driftcircle) << endreq;

        Identifier id = driftcircle->identify();

        //Check if it is valid
        if (! id.is_valid()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring TRT_DriftCircle with invalid identifier " << id << endreq;
          continue;
        }

        //Store the identifier
        ident.push_back(DataType(id.get_compact()));

        /**
         * NOTE: to get rhoz and phi, obtain DetectorElement
         * and then change to hashid - faster
         */
        const InDetDD::TRT_BaseElement* element = geo->TRTGeoManager()->getElement(geo->TRTIDHelper()->layer_id(id));

        //get global coord of straw
	Amg::Vector3D global  = element->strawTransform(geo->TRTIDHelper()->straw(id))*Amg::Vector3D(0.,0.,0.);
        
        //store the phi value
        phi.push_back(DataType( (global.phi()<0) ? global.phi() + 2*M_PI : global.phi()));
        
        //store rho for barrel and z for endcap
        if (element->type()==InDetDD::TRT_BaseElement::BARREL)
          rhoz.push_back(DataType(global.perp()*CLHEP::mm/CLHEP::cm));
        else if (element->type()==InDetDD::TRT_BaseElement::ENDCAP)
          rhoz.push_back(DataType(global.z()*CLHEP::mm/CLHEP::cm)); 
        else 
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Unknown TRT base element of type " << element->type();

        //Store local position parameters
	//Amg::Vector2D localPosition = driftcircle->localPosition();
        driftR.push_back(DataType((driftcircle->localPosition())[Trk::driftRadius]*CLHEP::mm/CLHEP::cm));

        //Get subdetector number
        switch ( geo->TRTIDHelper()->barrel_ec(id) ) {
          case -2 : sub.push_back(DataType( 0 )); break;
          case -1 : sub.push_back(DataType( 1 )); break;
          case  1 : sub.push_back(DataType( 2 )); break;
          case  2 : sub.push_back(DataType( 3 )); break;
          default : sub.push_back(DataType(-1 ));
        }
        
        //Store threshold related parameters
        threshold.push_back(DataType(driftcircle->highLevel()));
        timeOverThreshold.push_back(DataType(driftcircle->timeOverThreshold())); 
        noise.push_back(DataType(driftcircle->isNoise()));
        bitPattern.push_back(DataType(driftcircle->getWord()));

        /**
         * Finally, get the associated truth particles, if there are
         */
        if ( TRTMultiTruthMap == NULL ) continue ;

        //Count number of barcodes we get
        int NBarcodes = 0;
        //Loop over associated truth tracks with same id
        typedef PRD_MultiTruthCollection::const_iterator iter;
        std::pair<iter,iter> equalIDRange = TRTMultiTruthMap->equal_range(id);
        for ( iter TRTMultiTruthMapItr = equalIDRange.first; TRTMultiTruthMapItr != equalIDRange.second; TRTMultiTruthMapItr++){
          NBarcodes++;
          barcodes.push_back(DataType(TRTMultiTruthMapItr->second.barcode()));
        }
        numBarcodes.push_back(DataType(NBarcodes));
      } //loop over collection
    }//loop over container
        
    //Add everything to our data map
    DataMap dataMap;
    dataMap["rhoz"]=rhoz;
    dataMap["phi"]=phi;
    dataMap["driftR"]=driftR;
    dataMap["threshold"]=threshold;
    dataMap["id"]=ident;
    dataMap["sub"]=sub;
    dataMap["noise"]=noise;
    dataMap["timeOverThreshold"]=timeOverThreshold;
    dataMap["bitPattern"]=bitPattern;

    //Only write truth association tags if we processed truth
    if ( numBarcodes.size() > 0 ){
      //Add barcodes counter
      dataMap["numBarcodes"]=numBarcodes;
      // Compute the "multiple" for barcodes and put the vector in the map.
      std::string bctag = "barcodes multiple=\""+DataType(barcodes.size()/double(numBarcodes.size())).toString()+"\"";
      dataMap[bctag]=barcodes;
    }
      
    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": " << rhoz.size() << endreq;  

    //forward data to formating tool and return
    return FormatTool->AddToEvent(dataTypeName(), "", &dataMap);  
  }
}

