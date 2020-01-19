/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0ClusterScaler.cxx
// package:     Reconstruction/tauRec
// authors:     Stephanie Yuen, Benedict Winter, Will Davey
// date:        2014-08-04
//
//-----------------------------------------------------------------------------

#include <vector>

#include "tauRecTools/TauPi0ClusterScaler.h"
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"
#include "tauRecTools/ITauToolBase.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

using std::vector;
using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0ClusterScaler::TauPi0ClusterScaler( const string& name ) :
    TauRecToolBase(name)
    , m_chargedPFOContainer(0)
    , m_chargedPFOContainerName("TauChargedParticleFlowObjects")
    , m_chargedPFOAuxStore(0)
{
    declareProperty("ChargedPFOContainerName", m_chargedPFOContainerName); 
    declareProperty("runOnAOD", m_AODmode=false);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0ClusterScaler::~TauPi0ClusterScaler()
{
}


StatusCode TauPi0ClusterScaler::initialize()
{
    return StatusCode::SUCCESS;
}

StatusCode TauPi0ClusterScaler::eventInitialize() {

    //---------------------------------------------------------------------
    // Create charged PFO container
    //---------------------------------------------------------------------
    if(not m_AODmode){
        m_chargedPFOContainer = new xAOD::PFOContainer();
        m_chargedPFOAuxStore = new xAOD::PFOAuxContainer();
        m_chargedPFOContainer->setStore(m_chargedPFOAuxStore);
        ASG_CHECK( evtStore()->record(m_chargedPFOContainer, m_chargedPFOContainerName ) );
        ASG_CHECK( evtStore()->record( m_chargedPFOAuxStore, m_chargedPFOContainerName + "Aux." ) );
    }
    else{
        ATH_MSG_DEBUG("TauPi0ClusterScaler running in AOD mode");
        ASG_CHECK( evtStore()->retrieve(m_chargedPFOContainer, m_chargedPFOContainerName) );
        ASG_CHECK( evtStore()->retrieve( m_chargedPFOAuxStore, m_chargedPFOContainerName + "Aux." ) );
        m_chargedPFOContainer->clear();
    }
    return StatusCode::SUCCESS;

}

StatusCode TauPi0ClusterScaler::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode TauPi0ClusterScaler::execute(xAOD::TauJet& pTau)
{
    // Clear vector of cell-based charged PFO Links. 
    // Required when rerunning on xAOD level.
    pTau.clearProtoChargedPFOLinks();

    // Only run on 1-5 prong taus 
    if (pTau.nTracks() == 0 or pTau.nTracks() >5 ) 
        return StatusCode::SUCCESS;
    
    ATH_MSG_DEBUG("new tau pt = " << pTau.pt() 
                  << ", eta = " << pTau.eta() 
                  << ", phi = " << pTau.phi() 
                  << ", nprongs = " << pTau.nTracks());

    // reset neutral PFO kinematics (incase re-run on AOD)
    resetNeutralPFOs(pTau);
    // create new proto charged PFOs, extrapolate tracks, add to tau 
    createChargedPFOs(pTau);
    // associate hadronic PFOs to charged PFOs using extrapolated positions in HCal
    associateHadronicToChargedPFOs(pTau);
    // associate charged PFOs to neutral PFOs using extrapolated positions in ECal
    associateChargedToNeutralPFOs(pTau);
    // estimate charged PFO EM energy and subtract from neutral PFOs
    subtractChargedEnergyFromNeutralPFOs(pTau);

    ATH_MSG_DEBUG("End of TauPi0ClusterScaler::execute");

    return StatusCode::SUCCESS;
}

void TauPi0ClusterScaler::resetNeutralPFOs(xAOD::TauJet& pTau)
{
    // Set neutral PFO kinematics to vertex corrected cluster
    ATH_MSG_DEBUG("Resetting neutral PFO kinematics");
    for( auto neutralPFOLink : pTau.protoNeutralPFOLinks() )
    {
        if( not neutralPFOLink.isValid() ){
            ATH_MSG_WARNING("Invalid protoNeutralPFOLink");
            continue;
        }
        xAOD::PFO* pfo = const_cast<xAOD::PFO*>(*neutralPFOLink);
        const xAOD::CaloCluster* cl = pfo->cluster(0);

        // apply cluster vertex correction 
        if(pTau.vertexLink()){
            auto clcorr = xAOD::CaloVertexedTopoCluster(*cl, (*pTau.vertexLink())->position());
            pfo->setP4(clcorr.pt(), clcorr.eta(), clcorr.phi(), 0.0);
        }
        else{
            pfo->setP4(cl->pt(), cl->eta(), cl->phi(), 0.0);
        }

        ATH_MSG_DEBUG("Neutral PFO, ptr: " <<  cl
                        << ", e: " << pfo->e() 
                        << ", pt: " << pfo->pt()
                        << ", eta: " << pfo->eta()
                        << ", eta(unorr): " << cl->eta());
    }
}



void TauPi0ClusterScaler::createChargedPFOs(xAOD::TauJet& pTau)
{
    ATH_MSG_DEBUG("Creating charged PFOs");
    for(auto tauTrackLink : pTau.tauTrackLinks(xAOD::TauJetParameters::classifiedCharged)){
        if( not tauTrackLink.isValid() ){
            ATH_MSG_WARNING("Invalid tauTrackLink");
            continue;
        }
        const xAOD::TauTrack* tauTrack = (*tauTrackLink);
        // create pfo
        xAOD::PFO* chargedPFO = new xAOD::PFO();
        m_chargedPFOContainer->push_back(chargedPFO);
        // set properties
        chargedPFO->setCharge(tauTrack->track()->charge());
        chargedPFO->setP4(tauTrack->p4());
        // link to track
        if(not chargedPFO->setTrackLink((*tauTrackLink)->trackLinks().at(0))) 
            ATH_MSG_WARNING("Could not add Track to PFO");
        // link to tau track
        ElementLink< xAOD::IParticleContainer > newTauTrackLink;
        newTauTrackLink.toPersistent();
        newTauTrackLink.resetWithKeyAndIndex( tauTrackLink.persKey(), tauTrackLink.persIndex() );
        if(not chargedPFO->setAssociatedParticleLink(xAOD::PFODetails::CaloCluster,newTauTrackLink)) 
            ATH_MSG_WARNING("Could not add TauTrack to PFO");
        // link from tau
        pTau.addProtoChargedPFOLink(ElementLink< xAOD::PFOContainer >
                                    (chargedPFO, *m_chargedPFOContainer));
    }
}

void TauPi0ClusterScaler::associateHadronicToChargedPFOs(xAOD::TauJet& pTau)
{
    ATH_MSG_DEBUG("Associating hadronic PFOs to charged PFOs");

    // Will: I'm ashamed of this link-map, but its necessary until the 
    // PFO EDM is improved to allow sequential addition of particle links
    std::map< xAOD::PFO*,std::vector< ElementLink< xAOD::IParticleContainer > > > linkMap;
    ATH_MSG_DEBUG("nHadPFOs: " << pTau.nHadronicPFOs() );
    for( auto hadPFOLink : pTau.hadronicPFOLinks() ){
        if( not hadPFOLink.isValid() ){
            ATH_MSG_WARNING("Invalid hadPFOLink");
            continue;
        }
        ATH_MSG_DEBUG("hadPFO " << hadPFOLink.index() 
                      << ", eta: " << (*hadPFOLink)->eta() 
                      << ", phi: " << (*hadPFOLink)->phi() );
        xAOD::PFO* chargedPFOMatch = 0;
        // assign hadPFO to closest extrapolated chargedPFO track within dR<0.4
        float dRmin = 0.4; 
        for( auto chargedPFOLink : pTau.protoChargedPFOLinks() ){
            if( not chargedPFOLink.isValid() ){
                ATH_MSG_WARNING("Invalid protoChargedPFOLink");
                continue;
            }
            xAOD::PFO* chargedPFO = const_cast<xAOD::PFO*>(*chargedPFOLink);

            // get extrapolated positions from tau-track
            std::vector<const xAOD::IParticle*> tauTrackPcleVec;
            chargedPFO->associatedParticles(xAOD::PFODetails::CaloCluster, tauTrackPcleVec);
            if( tauTrackPcleVec.empty() ){
                ATH_MSG_WARNING("ChargedPFO has no associated TauTrack");
                continue;
            }
            auto tauTrack = dynamic_cast<const xAOD::TauTrack*>(tauTrackPcleVec.at(0));
            if( not tauTrack ){
                ATH_MSG_WARNING("Failed to retrieve TauTrack from ChargedPFO");
                continue;
            } 
            float etaCalo = -10.0;
            float phiCalo = -10.0;
            if( not tauTrack->detail(xAOD::TauJetParameters::CaloSamplingEtaHad, etaCalo))
                ATH_MSG_WARNING("Failed to retrieve extrapolated chargedPFO eta");
            if( not tauTrack->detail(xAOD::TauJetParameters::CaloSamplingPhiHad, phiCalo))
                ATH_MSG_WARNING("Failed to retrieve extrapolated chargedPFO phi");
            // calculate dR (false means use eta instead of rapidity)
            float dR = xAOD::P4Helpers::deltaR((**hadPFOLink), etaCalo, phiCalo, false);
            ATH_MSG_DEBUG("chargedPFO, pt: " << chargedPFO->pt()
                            << ", type: " << tauTrack->flagSet()
                            << ", eta: " << etaCalo 
                            << ", phi: " << phiCalo
                            << ", dR: " << dR );
            if (dR < dRmin){
                dRmin = dR;
                chargedPFOMatch = chargedPFO;
            }
        }
        if( not chargedPFOMatch ){
            ATH_MSG_DEBUG("Unassigned Hadronic PFO");
            continue; 
        }

        // create link to had PFO (add to chargedPFO later)
        ElementLink< xAOD::IParticleContainer > newHadLink;
        newHadLink.toPersistent();
        newHadLink.resetWithKeyAndIndex( hadPFOLink.persKey(), hadPFOLink.persIndex() );
        if (not newHadLink.isValid()){
            ATH_MSG_WARNING("Created an invalid element link to xAOD::PFO");
            continue;
        }

        // temporarily store in linkMap since we can't sequentially add to chargedPFOMatch
        if( not linkMap.count(chargedPFOMatch) )
            linkMap[chargedPFOMatch] = std::vector< ElementLink< xAOD::IParticleContainer > >();
        linkMap[chargedPFOMatch].push_back(newHadLink);
    }

    // finally set hadronic PFO links (note: we use existing TauShot enum)
    for( auto kv : linkMap ){
        if(not kv.first->setAssociatedParticleLinks(xAOD::PFODetails::TauShot,kv.second))
            ATH_MSG_WARNING("Couldn't add hadronic PFO links to charged PFO!");
    }
}

void TauPi0ClusterScaler::associateChargedToNeutralPFOs(xAOD::TauJet& pTau)
{
    ATH_MSG_DEBUG("Associating charged PFOs to neutral PFOs");
    // Will: I'm ashamed of this link-map, but its necessary until the 
    // PFO EDM is improved to allow sequential addition of particle links
    std::map< xAOD::PFO*,std::vector< ElementLink< xAOD::IParticleContainer > > > linkMap;
    ATH_MSG_DEBUG("nChargedPFOs: " << pTau.nProtoChargedPFOs() );
    for( auto chargedPFOLink : pTau.protoChargedPFOLinks() ){
        if( not chargedPFOLink.isValid() ){
            ATH_MSG_WARNING("Invalid protoChargedPFOLink");
            continue;
        }
        const xAOD::PFO* chargedPFO = (*chargedPFOLink);
        
        // get extrapolated positions from tau-track
        std::vector<const xAOD::IParticle*> tauTrackPcleVec;
        chargedPFO->associatedParticles(xAOD::PFODetails::CaloCluster, tauTrackPcleVec);
        if( tauTrackPcleVec.empty() ){
            ATH_MSG_WARNING("ChargedPFO has no associated TauTrack");
            continue;
        }
        auto tauTrack = dynamic_cast<const xAOD::TauTrack*>(tauTrackPcleVec.at(0));
        if( not tauTrack ){
            ATH_MSG_WARNING("Failed to retrieve TauTrack from ChargedPFO");
            continue;
        } 
        float etaCalo = -10.0;
        float phiCalo = -10.0;
        if( not tauTrack->detail(xAOD::TauJetParameters::CaloSamplingEtaEM, etaCalo))
            ATH_MSG_WARNING("Failed to retrieve extrapolated chargedPFO eta");
        if( not tauTrack->detail(xAOD::TauJetParameters::CaloSamplingPhiEM, phiCalo))
            ATH_MSG_WARNING("Failed to retrieve extrapolated chargedPFO phi");
        ATH_MSG_DEBUG("chargedPFO " << chargedPFOLink.index() 
                      << ", eta: " << etaCalo 
                      << ", phi: " << phiCalo );
        
        // assign extrapolated chargedPFO to closest neutralPFO within dR<0.04
        xAOD::PFO* neutralPFOMatch = 0;
        float dRmin = 0.04; 
        for( auto neutralPFOLink : pTau.protoNeutralPFOLinks() ){
            if( not neutralPFOLink.isValid() ){
                ATH_MSG_WARNING("Invalid protoNeutralPFOLink");
                continue;
            }
            xAOD::PFO* neutralPFO = const_cast<xAOD::PFO*>(*neutralPFOLink);
            // calculate dR (false means use eta instead of rapidity)
            float dR = xAOD::P4Helpers::deltaR((*neutralPFO->cluster(0)), etaCalo, phiCalo, false);
            ATH_MSG_DEBUG("neutralPFO, eta: " << neutralPFO->cluster(0)->eta()
                            << ", phi: " << neutralPFO->cluster(0)->phi()
                            << ", dR: " << dR );
            if (dR < dRmin){
                dRmin = dR;
                neutralPFOMatch = neutralPFO;
            }
        }
        if( not neutralPFOMatch ){
            ATH_MSG_DEBUG("Unassigned Charged PFO");
            continue; 
        }
        else ATH_MSG_DEBUG("Assigned Charged PFO");

        // create link to charged PFO 
        ElementLink< xAOD::IParticleContainer > newChargedLink;
        newChargedLink.toPersistent();
        newChargedLink.resetWithKeyAndIndex( chargedPFOLink.persKey(), chargedPFOLink.persIndex() );
        if (not newChargedLink.isValid()){
            ATH_MSG_WARNING("Created an invalid element link to xAOD::PFO");
            continue;
        }

        // temporarily store in linkMap since we can't sequentially add to neutralPFOMatch
        if( not linkMap.count(neutralPFOMatch) )
            linkMap[neutralPFOMatch] = std::vector< ElementLink< xAOD::IParticleContainer > >();
        linkMap[neutralPFOMatch].push_back(newChargedLink);
    }

    // finally set charged PFO links
    for( auto kv : linkMap ){
        if(not kv.first->setAssociatedParticleLinks(xAOD::PFODetails::Track,kv.second))
            ATH_MSG_WARNING("Couldn't add charged PFO links to neutral PFO!");
    }
}


void TauPi0ClusterScaler::subtractChargedEnergyFromNeutralPFOs(xAOD::TauJet& pTau)
{
    ATH_MSG_DEBUG("Subtracting charged energy from neutral PFOs");
    for( auto neutralPFOLink : pTau.protoNeutralPFOLinks() )
    {
        if( not neutralPFOLink.isValid() ){
            ATH_MSG_WARNING("Invalid protoNeutralPFOLink");
            continue;
        }
        xAOD::PFO* neutralPFO = const_cast<xAOD::PFO*>(*neutralPFOLink);

        // get associated charged PFOs
        std::vector<const xAOD::IParticle*> chargedPFOs;
        neutralPFO->associatedParticles(xAOD::PFODetails::Track, chargedPFOs);
        if( chargedPFOs.empty() ){
            ATH_MSG_DEBUG("No associated charged to subtract"); 
            continue;
        }
        ATH_MSG_DEBUG("Associated charged PFOs: " << chargedPFOs.size() );

        // estimate charged EM energy and subtract
        float neutralEnergy = neutralPFO->e();
        for( auto chargedPcle : chargedPFOs )
        {
            // since PFO stores element links as IParticle, need to cast back
            const xAOD::PFO* chargedPFO = dynamic_cast<const xAOD::PFO*>(chargedPcle);
            if( not chargedPFO ){
                ATH_MSG_WARNING("Failed to downcast IParticle ptr: " << chargedPcle << ", to ChargedPFO! " );
                continue;
            }
            float chargedEMEnergy = chargedPFO->e();
            std::vector<const xAOD::IParticle*> hadPFOs;
            chargedPFO->associatedParticles(xAOD::PFODetails::TauShot, hadPFOs);
            for( auto hadPFO : hadPFOs )
                chargedEMEnergy -= hadPFO->e();
            
            if( chargedEMEnergy < 0.0 ) chargedEMEnergy = 0.0;
            neutralEnergy -= chargedEMEnergy;
            ATH_MSG_DEBUG("Subtracting charged energy: " << chargedEMEnergy );
        } 
        float neutralPt = neutralEnergy / cosh(neutralPFO->eta());
        if(neutralPt <= 100.) neutralPt = 100.0;
        
        ATH_MSG_DEBUG("Neutral PFO pt, orig: " << neutralPFO->pt() << "  new: " << neutralPt); 
        neutralPFO->setP4(neutralPt , neutralPFO->eta(), neutralPFO->phi(), neutralPFO->m());
    }
}
