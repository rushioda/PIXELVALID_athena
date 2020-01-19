/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSegmentTruthAssociationAlg.cxx
//   Implementation file for class MuonSegmentTruthAssociationAlg
///////////////////////////////////////////////////////////////////

#include "MuonSegmentTruthAssociationAlg.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "MuonSegment/MuonSegment.h"


namespace Muon {

  // Constructor with parameters:
  MuonSegmentTruthAssociationAlg::MuonSegmentTruthAssociationAlg(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_muonTrackTruthTool("Muon::MuonTrackTruthTool/MuonTrackTruthTool")
  {  
    declareProperty("MuonTruthSegmentName",m_muonTruthSegmentContainerName = "MuonTruthSegments" );
    declareProperty("MuonSegmentLocation", m_muonSegmentCollectionName = "MuonSegments" );
    declareProperty("BarcodeOffset",       m_barcodeOffset = 1000000 );
  }

  // Initialize method:
  StatusCode MuonSegmentTruthAssociationAlg::initialize()
  {
    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_muonTrackTruthTool.retrieve());
    return StatusCode::SUCCESS;
  }
  // Finalize method:
  StatusCode MuonSegmentTruthAssociationAlg::finalize() 
  {
    return StatusCode::SUCCESS;
  }

  // Execute method:
  StatusCode MuonSegmentTruthAssociationAlg::execute() 
  {
    
    // skip if no input data found
    if ( !evtStore()->contains<xAOD::MuonSegmentContainer>(m_muonTruthSegmentContainerName) ) return StatusCode::SUCCESS;
    if ( !evtStore()->contains<xAOD::MuonSegmentContainer>( m_muonSegmentCollectionName ) )   return StatusCode::SUCCESS;

    const xAOD::MuonSegmentContainer* muonTruthSegments = 0;
    ATH_CHECK(evtStore()->retrieve(muonTruthSegments,m_muonTruthSegmentContainerName ));
    ATH_MSG_DEBUG("Retrieved SegmentContainer " << m_muonTruthSegmentContainerName << " size " << muonTruthSegments->size());

    const xAOD::MuonSegmentContainer* segments = 0;
    ATH_CHECK(evtStore()->retrieve(segments,m_muonSegmentCollectionName));
    ATH_MSG_DEBUG("Retrieved SegmentContainer " << m_muonSegmentCollectionName << " size " << segments->size() );

    std::vector<const Muon::MuonSegment*> muonSegments;
    typedef std::map<const Muon::MuonSegment*, ElementLink< xAOD::MuonSegmentContainer > > MuonSegmentLinkMap;
    MuonSegmentLinkMap muonSegmentLinkMap;
    unsigned int segIndex = 0;
    muonSegments.reserve(segments->size());
    for( auto seg : *segments ){
      seg->auxdata< ElementLink< xAOD::MuonSegmentContainer > >("truthSegmentLink") = ElementLink< xAOD::MuonSegmentContainer >();
      if( seg->muonSegment().isValid() ) {
        const Muon::MuonSegment* mseg = dynamic_cast<const Muon::MuonSegment*>(*seg->muonSegment());
        if( mseg ) {
          ATH_MSG_DEBUG(" Reco segment " << m_printer->print(*mseg));
          muonSegments.push_back(mseg);
          muonSegmentLinkMap[mseg] = ElementLink< xAOD::MuonSegmentContainer >(m_muonSegmentCollectionName,segIndex);
        }
      }
      ++segIndex;
    }     
    m_muonTrackTruthTool->createTruthTree();
    ATH_MSG_DEBUG("Matching reconstructed segments " << muonSegments.size() );
    IMuonTrackTruthTool::SegmentResultVec segmentMatchResult = m_muonTrackTruthTool->match(muonSegments);

    // create a map of chamber index onto the truth segments
    std::map<Muon::MuonStationIndex::ChIndex, std::vector<ElementLink< xAOD::MuonSegmentContainer> > > chamberTruthSegmentLinks;
    segIndex = 0;
    for( auto& truthSegment : *muonTruthSegments ){
      truthSegment->auxdata< ElementLink< xAOD::MuonSegmentContainer > >("recoSegmentLink") = ElementLink< xAOD::MuonSegmentContainer >();
      std::vector<ElementLink< xAOD::MuonSegmentContainer> >& linkVec = chamberTruthSegmentLinks[truthSegment->chamberIndex()];
      linkVec.push_back(ElementLink< xAOD::MuonSegmentContainer >(m_muonTruthSegmentContainerName,segIndex));
      ATH_MSG_DEBUG("New truth segment: index " << segIndex << " " << Muon::MuonStationIndex::chName(truthSegment->chamberIndex()) 
                    << " nlinks " << linkVec.size() << " link " << *linkVec.back() );
      ++segIndex;
    }

    // extract segment belonging to this muon 
    IMuonTrackTruthTool::SegmentResultVec segmentResultVec;
    std::set<Muon::MuonStationIndex::ChIndex> chambers; 
    std::vector<std::pair< Muon::MuonStationIndex::ChIndex, const TrackRecord* > > chamberTruthVec;  
    for( const auto& result : segmentMatchResult ){
      // only use segment that are matched
      ATH_MSG_DEBUG("Match reco segment " << m_printer->print(*(result.first)) << " truth " << result.second.truthTrack);
      if( !result.second.truthTrack ) continue;
      int barcode =  result.second.truthTrack->GetBarCode();

      // get chamber Identifier
      Identifier id;
      if( !result.second.mdts.matchedHits.empty() ) id = *result.second.mdts.matchedHits.begin();
      if( !result.second.cscs.matchedHits.empty() ) id = *result.second.cscs.matchedHits.begin();
      if( !id.is_valid() ) continue;

      Muon::MuonStationIndex::ChIndex chIndex = m_idHelper->chamberIndex(id);
      auto pos = chambers.insert(chIndex);

      if( !pos.second ) {
        bool skip = false;
      // select the first segment per chamber
        for( const auto& index : chamberTruthVec){
      // check that it is associated to same truthTrack and skip in that case
          if(index.first==chIndex&&index.second==result.second.truthTrack) {
            ATH_MSG_DEBUG("Skip this reco segment ");
            skip = true;
            break; 
          }
        } 
        if (skip) continue;
      }

      std::pair< Muon::MuonStationIndex::ChIndex, const TrackRecord* > chPair;
      chPair.first = chIndex;
      chPair.second = result.second.truthTrack;
      chamberTruthVec.push_back(chPair);     
 
      // look-up segment element link
      auto segPos = muonSegmentLinkMap.find(result.first);
      if( segPos == muonSegmentLinkMap.end() ){
        ATH_MSG_WARNING("Could not find segment in container");
        continue;
      }
      ElementLink< xAOD::MuonSegmentContainer > recoLink = segPos->second;
      const xAOD::MuonSegment* recoSegment = *recoLink;
      if( !recoLink ){
        ATH_MSG_WARNING("Reco segment link invalid");
        continue;
      }
      ATH_MSG_DEBUG("recoLink " << recoLink << " chamber index: " << Muon::MuonStationIndex::chName(chIndex)) ;
      // look-up truth segments in the chamber layer
      auto truthPos = chamberTruthSegmentLinks.find(chIndex);
      if( truthPos ==  chamberTruthSegmentLinks.end() ) continue;
      ATH_MSG_DEBUG("Found matching chamber index: " <<Muon::MuonStationIndex::chName(chIndex) << " links " << truthPos->second.size() );

      for( auto& truthSegLink : truthPos->second ){
        
        // get truth particle for the segment
        truthSegLink.toPersistent();
        const xAOD::MuonSegment* truthSegment = *truthSegLink;
        if( !truthSegment ) {
          ATH_MSG_WARNING("Invalid truth segment link " << truthSegLink);
          continue;
        }
        ATH_MSG_DEBUG("truthSegLink " << truthSegLink);
        if( !truthSegment->isAvailable< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") ) {
          ATH_MSG_WARNING("truthSegment without truthParticleLink ");
          continue;
        } 
        ElementLink< xAOD::TruthParticleContainer > truthLink = truthSegment->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        const xAOD::TruthParticle* truthParticle = *truthLink; 
        if( !truthParticle ){
          ATH_MSG_WARNING("Invalid truth link " << truthLink);
          continue;
        }
        // match barcodes
        if( barcode%m_barcodeOffset == truthParticle->barcode() ){
          ATH_MSG_DEBUG("Matched reconstructed segment: barcode " << barcode << " layer " << Muon::MuonStationIndex::chName(chIndex) );
          recoLink.toPersistent();
          truthLink.toPersistent();
          const_cast<xAOD::MuonSegment*>(truthSegment)->auxdata< ElementLink< xAOD::MuonSegmentContainer > >("recoSegmentLink") = recoLink;
          const_cast<xAOD::MuonSegment*>(recoSegment)->auxdata< ElementLink< xAOD::MuonSegmentContainer > >("truthSegmentLink") = truthSegLink;
        } else { 
          ATH_MSG_DEBUG("barcode mismatch " << barcode << " truthParticle->barcode " << truthParticle->barcode());
        }
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace Muon

