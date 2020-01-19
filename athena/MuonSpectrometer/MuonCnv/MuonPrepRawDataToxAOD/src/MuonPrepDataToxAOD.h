/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPrepDataToxAOD.h
//   Header file for class MuonPrepDataToxAOD
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPDATATOXAOD_H
#define MUONPREPDATATOXAOD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>

#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "HepMC/GenParticle.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

template <class PRDCOL, class SDOCOL>
class MuonPrepDataToxAOD : public AthAlgorithm  {
public:
  static const int invalid_value = -99999;

  // Constructor with parameters:
  MuonPrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator, std::string inputName, std::string sdoName ) :
    AthAlgorithm(name,pSvcLocator),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
  {  
    declareProperty("MuonTruthParticleContainerName", m_muonTruthParticleContainerName = "MuonTruthParticle"); 
    declareProperty("InputContainerName", m_inputContainerName = inputName); 
    declareProperty("SdoContainerName",   m_sdoContainerName = sdoName); 
  }

  StatusCode initialize() { ATH_CHECK(m_idHelper.retrieve()); return StatusCode::SUCCESS; }

  StatusCode finalize()  { return StatusCode::SUCCESS; }

  // overload this function to add detector specific information 
  virtual void addPRD_TechnologyInformation( xAOD::TrackMeasurementValidation&, const typename PRDCOL::IDENTIFIABLE::base_value_type& ) const {
  }

  // overload this function to add detector specific information 
  virtual void addSDO_TechnologyInformation( xAOD::TrackMeasurementValidation&, const typename PRDCOL::IDENTIFIABLE::base_value_type&, 
                                             const typename SDOCOL::mapped_type* ) const {
    
  }

  // get the work done
  bool buildCollections() {
    const PRDCOL* prds = 0;     
    if( evtStore()->retrieve(prds,m_inputContainerName).isFailure() ) {
      ATH_MSG_ERROR("Cannot retrieve RpcPrepDataContainer " << m_inputContainerName);
      return false;
    }

    // Create the xAOD container and its auxiliary store:
    xAOD::TrackMeasurementValidationContainer* xaod = new xAOD::TrackMeasurementValidationContainer();
    CHECK( evtStore()->record( xaod, m_inputContainerName ) );
    xAOD::TrackMeasurementValidationAuxContainer* aux = new xAOD::TrackMeasurementValidationAuxContainer();
    CHECK( evtStore()->record( aux, m_inputContainerName + "Aux." ) );
    xaod->setStore( aux );
	
    const SDOCOL* sdoCollection = 0;
    //if(evtStore()->contains<SdoCollectionType>(m_sdoContainerName)) // for some reason this does not compile as it is not picking up the template type correctly..
    if(evtStore()->template contains<SDOCOL>(m_sdoContainerName)) // MK : this should compile instead of lign above
    ATH_CHECK(evtStore()->retrieve(sdoCollection,m_sdoContainerName));

    typename PRDCOL::const_iterator it = prds->begin();
    typename PRDCOL::const_iterator it_end = prds->end();
    for( ; it!=it_end; ++it ) {
      // skip empty collections
      if( (*it)->empty() ) continue;
    
      // loop over collection and convert to xAOD
      for( const auto& prd : **it ){
      
        // create and add xAOD object
        xAOD::TrackMeasurementValidation* xprd = new xAOD::TrackMeasurementValidation();
        xaod->push_back(xprd);
      
        xprd->setIdentifier(prd->identify().get_compact());
        xprd->setLocalPosition(prd->localPosition().x(),prd->localPosition().y());
        float errx  = sqrt(prd->localCovariance()(0,0));
        float errxy = invalid_value; 
        float erry  = invalid_value; 
        if( prd->localCovariance().rows() > 1 ) {
          errxy = sqrt(prd->localCovariance()(0,1));
          errxy = sqrt(prd->localCovariance()(1,1));
        }
        xprd->setLocalPositionError(errx,errxy,erry);
        Amg::Vector3D gpos = prd->globalPosition();
        xprd->setGlobalPosition(gpos.x(),gpos.y(),gpos.z());
        ATH_MSG_DEBUG( " PRD:  " << m_idHelper->toString(prd->identify()) << " global position: r " << gpos.perp() << " z " << gpos.z() );
      
        const typename SDOCOL::mapped_type* sdo = 0;
        if( sdoCollection ){
        
          // find hit
          auto pos = sdoCollection->find(prd->identify());
        
          // extract local position
          Amg::Vector3D tlpos(invalid_value,invalid_value,invalid_value);
          Amg::Vector3D tgpos(invalid_value,invalid_value,invalid_value);
          float firstEntry  = invalid_value;
          float secondEntry = invalid_value;
          int sdo_word = invalid_value;
          int pdgId = invalid_value;
          if( pos != sdoCollection->end() ) {
            // transform into local coordinate system
            tgpos = pos->second.globalPosition();
            tlpos = prd->detectorElement()->transform(prd->identify()).inverse()*tgpos;
            sdo_word = pos->second.word();
            /// loop over hits, check for muons and extract additional info
            for( const auto& deposit : pos->second.getdeposits() ){

              // set first and second entry
              firstEntry = deposit.second.firstEntry();
              secondEntry = deposit.second.secondEntry();

              // set pdg id and stop loop once we found a muon entry
              if( !deposit.first.cptr() ) continue;
              pdgId = deposit.first.cptr()->pdg_id();
              if( abs(deposit.first.cptr()->pdg_id()) == 13 ) break;
            }
            ATH_MSG_DEBUG( " found SDO: global pos: r " << tgpos.perp() << " z " << tgpos.z() << " pdgId " << pdgId
                           << " local position (" <<tlpos.x() << "," << tlpos.y() << "," << tlpos.z() << ")" );
            sdo = &pos->second;
          }

          xprd->auxdata<int>("pdgId")         = pdgId;
          xprd->auxdata<int>("sdo_word")      = sdo_word;
          xprd->auxdata<float>("firstEntry")  = firstEntry;
          xprd->auxdata<float>("secondEntry") = secondEntry;
          xprd->auxdata<float>("truth_gx")    = tgpos.x();
          xprd->auxdata<float>("truth_gy")    = tgpos.y();
          xprd->auxdata<float>("truth_gz")    = tgpos.z();
          xprd->auxdata<float>("truth_lx")    = tlpos.x();
          xprd->auxdata<float>("truth_ly")    = tlpos.y();
          xprd->auxdata<float>("truth_lz")    = tlpos.z();
          
          addSDO_TechnologyInformation(*xprd,*prd,sdo);
        }
        
        addPRD_TechnologyInformation(*xprd,*prd);
      }
    }
    ATH_MSG_DEBUG( " recorded xAOD::TrackMeasurementValidation obejcts: size " << xaod->size() << " at " << m_inputContainerName );

    return true;
  }


protected:
  std::string m_muonTruthParticleContainerName;
  std::string m_inputContainerName;
  std::string m_sdoContainerName;
  ToolHandle<Muon::MuonIdHelperTool>    m_idHelper;
};


#endif 
