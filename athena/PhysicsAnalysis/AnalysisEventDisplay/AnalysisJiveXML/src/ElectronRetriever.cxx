/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/ElectronRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "egammaEvent/ElectronContainer.h"

// for associations:
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  ElectronRetriever::ElectronRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("Electron"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "ElectronAODCollection";
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
  }
   
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode ElectronRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<ElectronContainer> iterator, end;
    const ElectronContainer* electrons;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(electrons, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(electrons);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Electron retrieved" << endreq;
      }
    }

    //obtain all other collections from StoreGate
    if (( evtStore()->retrieve(iterator, end)).isFailure()){
       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endreq;
//        return StatusCode::WARNING;
    }
      
    for (; iterator!=end; iterator++) {
       if (iterator.key()!=m_sgKey) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
            DataMap data = getData(iterator);
            if ( FormatTool->AddToEvent(dataTypeName(), iterator.key(), &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") Electron retrieved" << endreq;
            }
	  }
    }	  
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap ElectronRetriever::getData(const ElectronContainer* elCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect pt; pt.reserve(elCont->size());
    DataVect phi; phi.reserve(elCont->size());
    DataVect eta; eta.reserve(elCont->size());
    DataVect mass; mass.reserve(elCont->size());
    DataVect energy; energy.reserve(elCont->size());
    DataVect px; px.reserve(elCont->size());
    DataVect py; py.reserve(elCont->size());
    DataVect pz; pz.reserve(elCont->size());

    DataVect eOverp; eOverp.reserve(elCont->size());
    DataVect isEM; isEM.reserve(elCont->size());
    DataVect isEMString; isEMString.reserve(elCont->size());
    DataVect hasTrack; hasTrack.reserve(elCont->size());
    DataVect author; author.reserve(elCont->size());
    DataVect label; label.reserve(elCont->size());
    
    DataVect pdgId; pdgId.reserve(elCont->size());
    DataVect dataType; dataType.reserve(elCont->size());
    DataVect hitsBLayer; hitsBLayer.reserve(elCont->size());
    DataVect hitsPixel; hitsPixel.reserve(elCont->size());
    DataVect hitsSCT; hitsSCT.reserve(elCont->size());
    DataVect hitsTRT; hitsTRT.reserve(elCont->size());

    // for associations:
    DataVect clusterKeyVec; clusterKeyVec.reserve(elCont->size());
    DataVect clusterIndexVec; clusterIndexVec.reserve(elCont->size());
    DataVect trackKeyVec; trackKeyVec.reserve(elCont->size());
    DataVect trackIndexVec; trackIndexVec.reserve(elCont->size());

    ElectronContainer::const_iterator elItr  = elCont->begin();
    ElectronContainer::const_iterator elItrE = elCont->end();

    int m_MCdataType = 1;

// isEM from
//   https://uimon.cern.ch/twiki/bin/view/Atlas/RemoveOverlap

    double ep = 0., p=0., e=0.; // for eOverP calculation from track
    int counter = 0;
    std::string electronAuthor = "";
    std::string electronIsEMString = "none";
    std::string electronLabel = "";
        
    for (; elItr != elItrE; ++elItr) {
      electronIsEMString = "none";
      phi.push_back(DataType((*elItr)->phi()));
      eta.push_back(DataType((*elItr)->eta()));
      pt.push_back(DataType((*elItr)->pt()/CLHEP::GeV));

      mass.push_back(DataType((*elItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*elItr)->e()/CLHEP::GeV ) );
      px.push_back( DataType((*elItr)->px()/CLHEP::GeV ) );
      py.push_back( DataType((*elItr)->py()/CLHEP::GeV ) );
      pz.push_back( DataType((*elItr)->pz()/CLHEP::GeV ) );

      pdgId.push_back( DataType((*elItr)->pdgId() ) );

      electronAuthor = "author"+DataType( (*elItr)->author() ).toString(); // for odd ones eg FWD
      electronLabel = electronAuthor;
      if (( (*elItr)->author()) == 0){ electronAuthor = "unknown"; electronLabel += "_unknown"; }
      if (( (*elItr)->author()) == 8){ electronAuthor = "forward"; electronLabel += "_forward"; }
      if (( (*elItr)->author()) == 2){ electronAuthor = "softe"; electronLabel += "_softe"; }
      if (( (*elItr)->author()) == 1){ electronAuthor = "egamma"; electronLabel += "_egamma"; }

      // Tight but without the use of isolation criteria:   
      // code from https://twiki.cern.ch/twiki/bin/view/Atlas/ElectronReconstruction#isEM_flag
      // New PP Ids: PhysicsAnalysis/D3PDMaker/TrigEgammaD3PDMaker/python/EFElectronD3PDObject.py

      if ( (*elItr)->isem(egammaPID::ElectronLoose)==0){ 
            electronLabel += "_Loose"; 
            electronIsEMString = "Loose"; // assume that hierarchy is obeyed !
      } 
      if ( (*elItr)->isem(egammaPID::ElectronMedium)==0){ 
            electronLabel += "_Medium"; 
            electronIsEMString = "Medium"; // assume that hierarchy is obeyed !
      }   
      if ( (*elItr)->isem(egammaPID::ElectronTight)==0){ 
            electronLabel += "_Tight"; 
            electronIsEMString = "Tight"; // assume that hierarchy is obeyed !
      }     
      if ( (*elItr)->isem(egammaPID::ElectronTightPP)==0){ 
            electronLabel += "_TightPP"; 
            electronIsEMString = "TightPP"; // last entry overwrites all others ! 
      }     
      if ( (*elItr)->isem(egammaPID::ElectronMediumPP)==0){ 
            electronLabel += "_MediumPP"; 
//            electronIsEMString = "MediumNoIso"; // would need AtlantisJava changes
      }     

      if ( (*elItr)->isem(egammaPID::ElectronMediumNoIso)==0){ 
            electronLabel += "_MediumNoIso"; 
//            electronIsEMString = "MediumNoIso"; // would need AtlantisJava changes
      }     
      if ( (*elItr)->isem(egammaPID::ElectronTightTRTNoIso)==0){ 
            electronLabel += "_TightTRTNoIso"; 
//            electronIsEMString = "TightTRTNoIso"; // would need AtlantisJava changes
      }     
      // Tight but without the use of isolation criteria:   
      if ( (*elItr)->isem(egammaPID::ElectronTightNoIsolation)==0){ 
            electronLabel += "_TightNoIsolation"; 
//            electronIsEMString = "_TightNoIsolation"; // would need AtlantisJava changes
      }     
      m_MCdataType = (*elItr)->dataType();
      dataType.push_back( DataType(  m_MCdataType ) );

// check: full simulation input file (1) or fast (0) 
// code from:
// PhysicsAnalysis/AnalysisCommon/AnalysisExamples/src/MiscellaneousExamples.cxx

      if (m_MCdataType != 3){ // full simulation

          isEM.push_back( DataType((**elItr).isem() ) );

	  const Trk::TrackSummary *summary;
          bool elecTrack = (*elItr)->trackParticle();
            if ( elecTrack ){

// eOperp calculation from: Reconstruction/egamma/egammaRec/egammaAODBuilder.cxx, 
// advised by Frederic Derue 16Mar09
// Old version with eOverP from EMTrackMatch is unreliable.

	      ep = 0.;
              p = (*elItr)->trackParticle()->p();
              e = (*elItr)->e();
              ep = p>0. ? e/p : 0.;
              eOverp.push_back( DataType( ep ));

              summary = (*elItr)->trackParticle()->trackSummary();
              hasTrack.push_back( DataType( 1 ) );
	      hitsBLayer.push_back( DataType( summary->get(Trk::numberOfBLayerHits) ));
              hitsPixel.push_back( DataType(  summary->get(Trk::numberOfPixelHits) ));
              hitsSCT.push_back( DataType( summary->get(Trk::numberOfSCTHits) ));
              hitsTRT.push_back( DataType( summary->get(Trk::numberOfTRTHits) ));
            } else {
  	      eOverp.push_back( DataType( "0." ));
              hasTrack.push_back( DataType( 0 ) );
	      hitsBLayer.push_back( DataType( -1 ));
              hitsPixel.push_back( DataType( -1 ));
              hitsSCT.push_back( DataType( -1 ));
              hitsTRT.push_back( DataType( -1 ));
            }

// code from:
// PhysicsAnalysis/AnalysisCommon/AnalysisExamples/src/MiscellaneousExamples.cxx

// parameters for associations: Storegate key and Index

       const ElementLink<CaloClusterContainer> clusterLink = (*elItr)->clusterElementLink();
       if (clusterLink.isValid()) {
         std::string clusterKey = clusterLink.dataID(); // Storegate key of 
         int clusterIndex = clusterLink.index(); // index into the contianer
//         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << counter << " CaloCluster " << clusterKey << " ," 
//                        << " Index " << clusterIndex << " " << endreq; 

	  clusterKeyVec.push_back(DataType( clusterKey ));
	  clusterIndexVec.push_back(DataType( clusterIndex ));
       } else { // no clusterLink
	  clusterKeyVec.push_back(DataType( "none" ));
	  clusterIndexVec.push_back(DataType( -1 ));
       }
       
       const ElementLink<Rec::TrackParticleContainer> trackLink = (*elItr)->trackParticleElementLink();
       if (trackLink.isValid()) {
         std::string trackKey = trackLink.dataID(); // Storegate key of 
         int trackIndex = trackLink.index(); // index into the contianer
 //         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << counter << " TrackParticle " << trackKey << " ," 
 //                  << " Index " << trackIndex << " " << endreq; 

	  trackKeyVec.push_back(DataType( trackKey ));
	  trackIndexVec.push_back(DataType( trackIndex));
        } else { // no trackLink
	  trackKeyVec.push_back(DataType( "none" ));
	  trackIndexVec.push_back(DataType( -1 ));
        }

       counter++;

// end of associations data    

     } else {  // fast simulation: placeholders
          hasTrack.push_back( DataType( 1 ) );
          eOverp.push_back( DataType( 1. ) );
          isEM.push_back( DataType( 0 ) );
	  electronIsEMString += "fastSim";
	  electronLabel += "fastSim" ;
          hitsBLayer.push_back( DataType( 0 ) );
          hitsPixel.push_back( DataType( 0 ) );
          hitsSCT.push_back( DataType( 0 ) );
          hitsTRT.push_back( DataType( 0 ) );

	  clusterKeyVec.push_back(DataType( "none" ));
	  clusterIndexVec.push_back(DataType( -1 ));
	  trackKeyVec.push_back(DataType( "none" ));
	  trackIndexVec.push_back(DataType( -1 ));
      } // end datatype case

      author.push_back( DataType( electronAuthor ) );
      label.push_back( DataType( electronLabel ) );
      isEMString.push_back( DataType( electronIsEMString ) );
    } // end ElectronIterator 

    // four-vectors
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["pt"] = pt;
    m_DataMap["energy"] = energy;
    m_DataMap["mass"] = mass;
    m_DataMap["px"] = px;
    m_DataMap["py"] = py;
    m_DataMap["pz"] = pz;

    // special Electron parameters
    m_DataMap["eOverp"] = eOverp;
    m_DataMap["isEM"] = isEM;
    m_DataMap["isEMString"] = isEMString;
    m_DataMap["label"] = label;
    m_DataMap["hasTrack"] = hasTrack;
    m_DataMap["author"] = author;
    m_DataMap["pdgId"] = pdgId;
    m_DataMap["dataType"] = dataType;
    m_DataMap["hitsBLayer"] = hitsBLayer;
    m_DataMap["hitsPixel"] = hitsPixel;
    m_DataMap["hitsSCT"] = hitsSCT;
    m_DataMap["hitsTRT"] = hitsTRT;

    // associations
    m_DataMap["clusterKey"] = clusterKeyVec;
    m_DataMap["clusterIndex"] = clusterIndexVec;
    m_DataMap["trackKey"] = trackKeyVec;
    m_DataMap["trackIndex"] = trackIndexVec;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
