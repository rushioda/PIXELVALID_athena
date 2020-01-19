/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/TgcPrepDataRetriever.h"

#include "MuonJiveXML/MuonFullIDHelper.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  TgcPrepDataRetriever::TgcPrepDataRetriever(const std::string& type,const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("TGC")
  {

    declareInterface<IDataRetriever>(this);
    
    declareProperty("StoreGateKey", m_sgKey = "TGC_Measurements", "Storegate key for TGC PredData container");
  }
 
  //--------------------------------------------------------------------------

  StatusCode TgcPrepDataRetriever::initialize(){
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initializing retriever for " << dataTypeName() << endmsg; 

    StatusCode sc=detStore()->retrieve(m_tgcIdHelper);
    if (sc.isFailure())
      {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << MSG::ERROR << "Could not retrieve TgcIdHelper!" << endmsg;
        return StatusCode::FAILURE;
      }

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

  StatusCode TgcPrepDataRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() << endmsg; 

    const Muon::TgcPrepDataContainer *tgcContainer;
    if ( evtStore()->retrieve(tgcContainer, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Muon::TgcPrepDataContainer '" << m_sgKey << "' was not retrieved." << endmsg;
      return StatusCode::SUCCESS;
    }

    int ndata = 0;
    Muon::TgcPrepDataContainer::const_iterator containerIt;
    for (containerIt=tgcContainer->begin(); containerIt!=tgcContainer->end(); ++containerIt) {
               ndata += (*containerIt)->size();
    }
    //Make the vectors to contain the information and reserve space accordingly
    DataVect x; x.reserve(ndata);
    DataVect y; y.reserve(ndata);
    DataVect z; z.reserve(ndata);
    DataVect lengthVec; lengthVec.reserve(ndata);
    DataVect swidthVec; swidthVec.reserve(ndata);
    DataVect lwidthVec; lwidthVec.reserve(ndata);
    DataVect idVec; idVec.reserve(ndata);
    DataVect identifierVec; identifierVec.reserve(ndata);
    DataVect barcode; barcode.reserve(ndata);

    for (containerIt=tgcContainer->begin(); containerIt!=tgcContainer->end(); ++containerIt) {
      const Muon::TgcPrepDataCollection *tgcCollection = *containerIt;

      Muon::TgcPrepDataCollection::const_iterator collectionIt;
      for (collectionIt=tgcCollection->begin(); collectionIt!=tgcCollection->end(); ++collectionIt) {

        const Muon::TgcPrepData *data = *collectionIt;
        const MuonGM::TgcReadoutElement *element = data->detectorElement();
        Identifier id = data->identify();

        if (!element) {
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No MuonGM::TgcReadoutElement for hit " << id << endmsg;
          continue;
        }

	Amg::Vector3D globalPos; 
        double shortWidth, longWidth, length;
        if (m_tgcIdHelper->isStrip(id)) {
          int gasGap = m_tgcIdHelper->gasGap(id);
          int strip = m_tgcIdHelper->channel(id);
          shortWidth = element->stripShortWidth(gasGap, strip);
          longWidth = element->stripLongWidth(gasGap, strip);
          length = element->stripLength(gasGap, strip);
          globalPos = element->stripPos(gasGap, strip);
        } else {
          int gasGap = m_tgcIdHelper->gasGap(id);
          int gang = m_tgcIdHelper->channel(id);
          shortWidth = element->gangShortWidth(gasGap, gang);
          longWidth = element->gangLongWidth(gasGap, gang);
          length = element->gangLength(gasGap, gang);
          globalPos = element->gangPos(gasGap, gang);
        }

        x.push_back(DataType(globalPos.x()/CLHEP::cm));
        y.push_back(DataType(globalPos.y()/CLHEP::cm));
        z.push_back(DataType(globalPos.z()/CLHEP::cm));
        swidthVec.push_back(DataType(shortWidth/CLHEP::cm));
        lwidthVec.push_back(DataType(longWidth/CLHEP::cm));
        lengthVec.push_back(DataType(length/CLHEP::cm));
        identifierVec.push_back(DataType(MuonFullIDHelper::getFullID(id, m_tgcIdHelper)));
        idVec.push_back(DataType( id.get_compact() ));
        barcode.push_back(DataType(0));

      }
    }      

    DataMap myDataMap;
    myDataMap["x"] = x;
    myDataMap["y"] = y;
    myDataMap["z"] = z;
    myDataMap["length"] = lengthVec;
    myDataMap["swidth"] = swidthVec;
    myDataMap["lwidth"] = lwidthVec;
    myDataMap["identifier"] = identifierVec;
    myDataMap["id"] = idVec;
    myDataMap["barcode"] = barcode;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< x.size() << endmsg;

    ////forward data to formating tool
    //return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
    //// Atlantis can't deal with SGkey in xml output in CSCD (freezes)
    //// So not output SGKey for now. jpt 20Aug09
    std::string emptyStr="";
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &myDataMap);
  }
}
