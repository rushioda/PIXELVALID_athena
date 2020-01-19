/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv_v1.cxx 751655 2016-06-02 03:55:32Z griffith $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/TauJetContainer.h"

// Local include(s):
#include "xAODTauJetAuxContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODTauJetAuxContainerCnv_v1::xAODTauJetAuxContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::TauJetAuxContainer,
                           xAOD::TauJetAuxContainer_v1 >() {

}

void xAODTauJetAuxContainerCnv_v1::
persToTrans( const xAOD::TauJetAuxContainer_v1* oldObj,
             xAOD::TauJetAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::TauJetAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Set up interface containers on top of them:
   xAOD::TauJetContainer_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
     oldInt.push_back( new xAOD::TauJet_v1() );
   }
   oldInt.setStore( oldObj );
   xAOD::TauJetContainer newInt;
   newInt.setStore( newObj );

   // Loop over the interface objects, and do the conversion with their help:
   for( const xAOD::TauJet_v1* oldTau : oldInt ) {

      // Add an object to the output container:
      xAOD::TauJet* newTau = new xAOD::TauJet;
      newInt.push_back( newTau );
      
      //
      // Copy the 4-momentum variables:
      //
      newTau->setP4( oldTau->pt(), oldTau->eta(), oldTau->phi(), oldTau->m() );
      newTau->setP4( xAOD::TauJetParameters::JetSeed, oldTau->ptJetSeed(), oldTau->etaJetSeed(), oldTau->phiJetSeed(), oldTau->mJetSeed() );
      newTau->setP4( xAOD::TauJetParameters::DetectorAxis, oldTau->ptDetectorAxis(), oldTau->etaDetectorAxis(), oldTau->phiDetectorAxis(), oldTau->mDetectorAxis() );
      newTau->setP4( xAOD::TauJetParameters::IntermediateAxis, oldTau->ptIntermediateAxis(), oldTau->etaIntermediateAxis(), oldTau->phiIntermediateAxis(), oldTau->mIntermediateAxis() );
      newTau->setP4( xAOD::TauJetParameters::TauEnergyScale, oldTau->ptTauEnergyScale(), oldTau->etaTauEnergyScale(), oldTau->phiTauEnergyScale(), oldTau->mTauEnergyScale() );
      newTau->setP4( xAOD::TauJetParameters::TauEtaCalib, oldTau->ptTauEtaCalib(), oldTau->etaTauEtaCalib(), oldTau->phiTauEtaCalib(), oldTau->mTauEtaCalib() );
      /// don't need to convert this anymore as v2 taujet doesn't have these 4-vectors anymore
      // newTau->setP4( xAOD::TauJetParameters::PanTauEFlowRec, oldTau->ptPanTauEFlowRec(), oldTau->etaPanTauEFlowRec(), oldTau->phiPanTauEFlowRec(), oldTau->mPanTauEFlowRec() );
      // newTau->setP4( xAOD::TauJetParameters::PanTauEFlowRecProto, oldTau->ptPanTauEFlowRecProto(), oldTau->etaPanTauEFlowRecProto(), oldTau->phiPanTauEFlowRecProto(), oldTau->mPanTauEFlowRecProto() );
      newTau->setP4( xAOD::TauJetParameters::PanTauCellBased, oldTau->ptPanTauCellBased(), oldTau->etaPanTauCellBased(), oldTau->phiPanTauCellBased(), oldTau->mPanTauCellBased() );
      newTau->setP4( xAOD::TauJetParameters::PanTauCellBasedProto, oldTau->ptPanTauCellBasedProto(), oldTau->etaPanTauCellBasedProto(), oldTau->phiPanTauCellBasedProto(), oldTau->mPanTauCellBasedProto() );

      // ROI and charge
      newTau->setROIWord( oldTau->ROIWord() );
      newTau->setCharge( oldTau->charge() );
      newTau->setTrackFilterProngs( oldTau->trackFilterProngs() );
      newTau->setTrackFilterQuality( oldTau->trackFilterQuality() );
      newTau->setPi0ConeDR( oldTau->pi0ConeDR() );

      //
      //copy PID variables
      //
      newTau->setDiscriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans    , oldTau->discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans) );
      //      newTau->setDiscriminant(xAOD::TauJetParameters::BDTJetScoreBkgTrans    , oldTau->discriminant(xAOD::TauJetParameters::BDTJetScoreBkgTrans) );
      newTau->setDiscriminant(xAOD::TauJetParameters::BDTJetScore    , oldTau->discriminant(xAOD::TauJetParameters::BDTJetScore) );
      newTau->setDiscriminant(xAOD::TauJetParameters::BDTEleScore    , oldTau->discriminant(xAOD::TauJetParameters::BDTEleScore) );
      // newTau->setDiscriminant(xAOD::TauJetParameters::Likelihood     , oldTau->discriminant(xAOD::TauJetParameters::Likelihood) );
      // newTau->setDiscriminant(xAOD::TauJetParameters::SafeLikelihood , oldTau->discriminant(xAOD::TauJetParameters::SafeLikelihood) );
      // newTau->setIsTau(xAOD::TauJetParameters::ElectronVetoLoose  ,   oldTau->isTau(xAOD::TauJetParameters::ElectronVetoLoose) );
      // newTau->setIsTau(xAOD::TauJetParameters::ElectronVetoLoose  ,   oldTau->isTau(xAOD::TauJetParameters::ElectronVetoLoose) );
      // newTau->setIsTau(xAOD::TauJetParameters::ElectronVetoMedium ,   oldTau->isTau(xAOD::TauJetParameters::ElectronVetoMedium) );
      // newTau->setIsTau(xAOD::TauJetParameters::ElectronVetoTight  ,   oldTau->isTau(xAOD::TauJetParameters::ElectronVetoTight) );
      newTau->setIsTau(xAOD::TauJetParameters::MuonVeto           ,   oldTau->isTau(xAOD::TauJetParameters::MuonVeto) );
      // newTau->setIsTau(xAOD::TauJetParameters::TauCutLoose        ,   oldTau->isTau(xAOD::TauJetParameters::TauCutLoose) );
      // newTau->setIsTau(xAOD::TauJetParameters::TauCutMedium       ,   oldTau->isTau(xAOD::TauJetParameters::TauCutMedium) );
      // newTau->setIsTau(xAOD::TauJetParameters::TauCutTight        ,   oldTau->isTau(xAOD::TauJetParameters::TauCutTight) );
      // newTau->setIsTau(xAOD::TauJetParameters::TauLlhLoose        ,   oldTau->isTau(xAOD::TauJetParameters::TauLlhLoose) );
      // newTau->setIsTau(xAOD::TauJetParameters::TauLlhMedium       ,   oldTau->isTau(xAOD::TauJetParameters::TauLlhMedium) );
      // newTau->setIsTau(xAOD::TauJetParameters::TauLlhTight        ,   oldTau->isTau(xAOD::TauJetParameters::TauLlhTight) );
      newTau->setIsTau(xAOD::TauJetParameters::JetBDTSigLoose     ,   oldTau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) );
      newTau->setIsTau(xAOD::TauJetParameters::JetBDTSigMedium    ,   oldTau->isTau(xAOD::TauJetParameters::JetBDTSigMedium) );
      newTau->setIsTau(xAOD::TauJetParameters::JetBDTSigTight     ,   oldTau->isTau(xAOD::TauJetParameters::JetBDTSigTight) );
      newTau->setIsTau(xAOD::TauJetParameters::EleBDTLoose        ,   oldTau->isTau(xAOD::TauJetParameters::EleBDTLoose) );
      newTau->setIsTau(xAOD::TauJetParameters::EleBDTMedium       ,   oldTau->isTau(xAOD::TauJetParameters::EleBDTMedium) );
      newTau->setIsTau(xAOD::TauJetParameters::EleBDTTight        ,   oldTau->isTau(xAOD::TauJetParameters::EleBDTTight) );         

      //
      //set individual int type details variables
      //
      //r21 cleanup
      // newTau->setDetail(xAOD::TauJetParameters::nPi0                   , oldTau->detail<int>(xAOD::TauJetParameters::nPi0) );                     
      // newTau->setDetail(xAOD::TauJetParameters::nPi0Topo                   , oldTau->detail<int>(xAOD::TauJetParameters::nPi0Topo) );                     
      newTau->setDetail(xAOD::TauJetParameters::nCharged                   , oldTau->detail<int>(xAOD::TauJetParameters::nCharged) );                     
      newTau->setDetail(xAOD::TauJetParameters::numCells               ,      oldTau->detail<int>(xAOD::TauJetParameters::numCells) );				  
      newTau->setDetail(xAOD::TauJetParameters::numTopoClusters        ,      oldTau->detail<int>(xAOD::TauJetParameters::numTopoClusters) );			  
      newTau->setDetail(xAOD::TauJetParameters::nStrip                 ,      oldTau->detail<int>(xAOD::TauJetParameters::nStrip) );				  

      //
      //set individual float type details variables
      //
      newTau->setDetail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk ,       oldTau->detail<float>(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk) );			  
      newTau->setDetail(xAOD::TauJetParameters::etOverPtLeadTrk        ,       oldTau->detail<float>(xAOD::TauJetParameters::etOverPtLeadTrk) );				  
      newTau->setDetail(xAOD::TauJetParameters::leadTrkPt              ,       oldTau->detail<float>(xAOD::TauJetParameters::leadTrkPt) );				  
      newTau->setDetail(xAOD::TauJetParameters::ipSigLeadTrk           ,       oldTau->detail<float>(xAOD::TauJetParameters::ipSigLeadTrk) );				  
      newTau->setDetail(xAOD::TauJetParameters::massTrkSys             ,       oldTau->detail<float>(xAOD::TauJetParameters::massTrkSys) );				  
      newTau->setDetail(xAOD::TauJetParameters::trkWidth2              ,       oldTau->detail<float>(xAOD::TauJetParameters::trkWidth2) );				  
      newTau->setDetail(xAOD::TauJetParameters::trFlightPathSig        ,       oldTau->detail<float>(xAOD::TauJetParameters::trFlightPathSig) );				  
      //r21 cleanup
      // newTau->setDetail(xAOD::TauJetParameters::etEflow                ,       oldTau->detail<float>(xAOD::TauJetParameters::etEflow) );					  
      // newTau->setDetail(xAOD::TauJetParameters::mEflow		      ,        oldTau->detail<float>(xAOD::TauJetParameters::mEflow) );					  
      // newTau->setDetail(xAOD::TauJetParameters::ele_E237E277           ,       oldTau->detail<float>(xAOD::TauJetParameters::ele_E237E277) );				  
      // newTau->setDetail(xAOD::TauJetParameters::ele_PresamplerFraction ,       oldTau->detail<float>(xAOD::TauJetParameters::ele_PresamplerFraction) );			  
      // newTau->setDetail(xAOD::TauJetParameters::ele_ECALFirstFraction  ,       oldTau->detail<float>(xAOD::TauJetParameters::ele_ECALFirstFraction) );			  
      newTau->setDetail(xAOD::TauJetParameters::numEffTopoClusters     ,       oldTau->detail<float>(xAOD::TauJetParameters::numEffTopoClusters) );			  
      newTau->setDetail(xAOD::TauJetParameters::topoInvMass            ,       oldTau->detail<float>(xAOD::TauJetParameters::topoInvMass) );				  
      newTau->setDetail(xAOD::TauJetParameters::effTopoInvMass         ,       oldTau->detail<float>(xAOD::TauJetParameters::effTopoInvMass) );				  
      newTau->setDetail(xAOD::TauJetParameters::topoMeanDeltaR         ,       oldTau->detail<float>(xAOD::TauJetParameters::topoMeanDeltaR) );				  
      newTau->setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR      ,       oldTau->detail<float>(xAOD::TauJetParameters::effTopoMeanDeltaR) );			  
      newTau->setDetail(xAOD::TauJetParameters::EMRadius               ,       oldTau->detail<float>(xAOD::TauJetParameters::EMRadius) );			  
      newTau->setDetail(xAOD::TauJetParameters::hadRadius              ,       oldTau->detail<float>(xAOD::TauJetParameters::hadRadius) );			  
      newTau->setDetail(xAOD::TauJetParameters::etEMAtEMScale          ,       oldTau->detail<float>(xAOD::TauJetParameters::etEMAtEMScale) );			  
      newTau->setDetail(xAOD::TauJetParameters::etHadAtEMScale         ,       oldTau->detail<float>(xAOD::TauJetParameters::etHadAtEMScale) );			  
      newTau->setDetail(xAOD::TauJetParameters::isolFrac               ,       oldTau->detail<float>(xAOD::TauJetParameters::isolFrac) );			  
      newTau->setDetail(xAOD::TauJetParameters::centFrac               ,       oldTau->detail<float>(xAOD::TauJetParameters::centFrac) );			  
      newTau->setDetail(xAOD::TauJetParameters::stripWidth2            ,       oldTau->detail<float>(xAOD::TauJetParameters::stripWidth2) );			  
      //r21 cleanup
      // newTau->setDetail(xAOD::TauJetParameters::etEMCalib              ,       oldTau->detail<float>(xAOD::TauJetParameters::etEMCalib) );			  
      // newTau->setDetail(xAOD::TauJetParameters::etHadCalib             ,       oldTau->detail<float>(xAOD::TauJetParameters::etHadCalib) );			  
      // newTau->setDetail(xAOD::TauJetParameters::seedCalo_eta           ,       oldTau->detail<float>(xAOD::TauJetParameters::seedCalo_eta) );				  
      // newTau->setDetail(xAOD::TauJetParameters::seedCalo_phi           ,       oldTau->detail<float>(xAOD::TauJetParameters::seedCalo_phi) );				  
      newTau->setDetail(xAOD::TauJetParameters::trkAvgDist             ,       oldTau->detail<float>(xAOD::TauJetParameters::trkAvgDist) );			  
      newTau->setDetail(xAOD::TauJetParameters::trkRmsDist             ,       oldTau->detail<float>(xAOD::TauJetParameters::trkRmsDist) );			  
      newTau->setDetail(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE , oldTau->detail<float>(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE) );	  
      newTau->setDetail(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE , oldTau->detail<float>(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE) );	  
      newTau->setDetail(xAOD::TauJetParameters::caloIso ,	               oldTau->detail<float>(xAOD::TauJetParameters::caloIso) );			  
      newTau->setDetail(xAOD::TauJetParameters::caloIsoCorrected ,             oldTau->detail<float>(xAOD::TauJetParameters::caloIsoCorrected) );		  
      newTau->setDetail(xAOD::TauJetParameters::dRmax ,	                       oldTau->detail<float>(xAOD::TauJetParameters::dRmax) );				  
      newTau->setDetail(xAOD::TauJetParameters::secMaxStripEt  ,	       oldTau->detail<float>(xAOD::TauJetParameters::secMaxStripEt) );				  
      newTau->setDetail(xAOD::TauJetParameters::sumEMCellEtOverLeadTrkPt  ,    oldTau->detail<float>(xAOD::TauJetParameters::sumEMCellEtOverLeadTrkPt) );				  
      newTau->setDetail(xAOD::TauJetParameters::hadLeakEt  ,	               oldTau->detail<float>(xAOD::TauJetParameters::hadLeakEt) );				  
      //r21 cleanup
      // newTau->setDetail(xAOD::TauJetParameters::EM_TES_scale ,		       oldTau->detail<float>(xAOD::TauJetParameters::EM_TES_scale) );				  
      //      newTau->setDetail(xAOD::TauJetParameters::LC_TES_precalib ,	       oldTau->detail<float>(xAOD::TauJetParameters::LC_TES_precalib) );				  
      newTau->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing1 ,	       oldTau->detail<float>(xAOD::TauJetParameters::cellBasedEnergyRing1) );			  
      newTau->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing2 ,	       oldTau->detail<float>(xAOD::TauJetParameters::cellBasedEnergyRing2) );			  
      newTau->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing3 ,	       oldTau->detail<float>(xAOD::TauJetParameters::cellBasedEnergyRing3) );			  
      newTau->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing4 ,	       oldTau->detail<float>(xAOD::TauJetParameters::cellBasedEnergyRing4) );			  
      newTau->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing5 ,	       oldTau->detail<float>(xAOD::TauJetParameters::cellBasedEnergyRing5) );			  
      newTau->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing6 ,	       oldTau->detail<float>(xAOD::TauJetParameters::cellBasedEnergyRing6) );			  
      newTau->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing7 ,	       oldTau->detail<float>(xAOD::TauJetParameters::cellBasedEnergyRing7) );			  
      newTau->setDetail(xAOD::TauJetParameters::TRT_NHT_OVER_NLT ,	       oldTau->detail<float>(xAOD::TauJetParameters::TRT_NHT_OVER_NLT) );			  
      newTau->setDetail(xAOD::TauJetParameters::TauJetVtxFraction ,	       oldTau->detail<float>(xAOD::TauJetParameters::TauJetVtxFraction) );			  
      //r21 cleanup
      // newTau->setDetail(xAOD::TauJetParameters::ptRatioEflow ,	               oldTau->detail<float>(xAOD::TauJetParameters::ptRatioEflow) );			  
      // newTau->setDetail(xAOD::TauJetParameters::etEflowTopo ,	               oldTau->detail<float>(xAOD::TauJetParameters::etEflowTopo) );			  
      newTau->setDetail(xAOD::TauJetParameters::TauJetVtxFraction ,	       oldTau->detail<float>(xAOD::TauJetParameters::TauJetVtxFraction) );			  
      //r21 cleanup
      // newTau->setDetail(xAOD::TauJetParameters::mEflowTopo ,	               oldTau->detail<float>(xAOD::TauJetParameters::mEflowTopo) );			  
      // newTau->setDetail(xAOD::TauJetParameters::ptRatioEflowTopo ,	       oldTau->detail<float>(xAOD::TauJetParameters::ptRatioEflowTopo) );			  
      newTau->setDetail(xAOD::TauJetParameters::PSSFraction ,	               oldTau->detail<float>(xAOD::TauJetParameters::PSSFraction) );			  
      newTau->setDetail(xAOD::TauJetParameters::ChPiEMEOverCaloEME ,	       oldTau->detail<float>(xAOD::TauJetParameters::ChPiEMEOverCaloEME) );			  
      newTau->setDetail(xAOD::TauJetParameters::EMPOverTrkSysP ,	       oldTau->detail<float>(xAOD::TauJetParameters::EMPOverTrkSysP) );			  
      newTau->setDetail(xAOD::TauJetParameters::TESOffset ,	               oldTau->detail<float>(xAOD::TauJetParameters::TESOffset) );			  
      newTau->setDetail(xAOD::TauJetParameters::TESCalibConstant ,	       oldTau->detail<float>(xAOD::TauJetParameters::TESCalibConstant) );			  
      newTau->setDetail(xAOD::TauJetParameters::centFracCorrected ,	       oldTau->detail<float>(xAOD::TauJetParameters::centFracCorrected) );			  
      newTau->setDetail(xAOD::TauJetParameters::etOverPtLeadTrkCorrected ,     oldTau->detail<float>(xAOD::TauJetParameters::etOverPtLeadTrkCorrected) );			  
      newTau->setDetail(xAOD::TauJetParameters::innerTrkAvgDist ,	       oldTau->detail<float>(xAOD::TauJetParameters::innerTrkAvgDist) );			  
      newTau->setDetail(xAOD::TauJetParameters::innerTrkAvgDistCorrected ,     oldTau->detail<float>(xAOD::TauJetParameters::innerTrkAvgDistCorrected) );			  
      newTau->setDetail(xAOD::TauJetParameters::SumPtTrkFrac ,	       oldTau->detail<float>(xAOD::TauJetParameters::SumPtTrkFrac) );			  
      newTau->setDetail(xAOD::TauJetParameters::SumPtTrkFracCorrected ,	       oldTau->detail<float>(xAOD::TauJetParameters::SumPtTrkFracCorrected) );			  
 

      //
      //set pantau details
      //
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_isPanTauCandidate,                           oldTau->panTauDetail<int>(xAOD::TauJetParameters::PanTau_isPanTauCandidate ));
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto,                              oldTau->panTauDetail<int>(xAOD::TauJetParameters::PanTau_DecayModeProto ));	
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayMode, 	                                oldTau->panTauDetail<int>(xAOD::TauJetParameters::PanTau_DecayMode ));		 
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTValue_1p0n_vs_1p1n,                       oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTValue_1p0n_vs_1p1n ));		  
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTValue_1p1n_vs_1pXn, 			oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTValue_1p1n_vs_1pXn ));	  
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTValue_3p0n_vs_3pXn, 			oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTValue_3p0n_vs_3pXn ));		  
      // newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Basic_NNeutralConsts, 		oldTau->panTauDetail<int>(xAOD::TauJetParameters::PanTau_BDTVar_Basic_NNeutralConsts ));		  
      // newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt, 	oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt )); 
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts, 	oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts )); 
      // newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_HLV_SumM, 			oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_HLV_SumM 		     ));  
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1, 	oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1 ));  
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2, 	oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2 ));  
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts,oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts )); 
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts, 	oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts ));  
      // newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed, 	oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed ));	  
      newTau->setPanTauDetail(xAOD::TauJetParameters::PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged,oldTau->panTauDetail<float>(xAOD::TauJetParameters::PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged ));
      
      //copy element links
      newTau->setJetLink( oldTau->jetLink() );
      newTau->setVertexLink( oldTau->vertexLink() );
      newTau->setSecondaryVertexLink( oldTau->secondaryVertexLink() );

      // newTau->setTrackLinks( oldTau->trackLinks() );
      // newTau->setOtherTrackLinks( oldTau->otherTrackLinks() );
      // newTau->setWideTrackLinks( oldTau->wideTrackLinks() );

      newTau->setNeutralPFOLinks( oldTau->neutral_PFOLinks() );
      newTau->setChargedPFOLinks( oldTau->charged_PFOLinks() );
      newTau->setPi0PFOLinks( oldTau->pi0_PFOLinks() );
      newTau->setShotPFOLinks( oldTau->shot_PFOLinks() );
      /// can't set hadronic pfo links because v1 taujet doesn't have them
      // newTau->setHadronicPFOLinks( oldTau->hadronic_PFOLinks() );

      //v2 doesn't have pfo element link with specific type name, so copy cellbased ones into proto
      newTau->setProtoNeutralPFOLinks( oldTau->cellBased_Neutral_PFOLinks() );
      newTau->setProtoChargedPFOLinks( oldTau->cellBased_Charged_PFOLinks() );
      newTau->setProtoPi0PFOLinks( oldTau->cellBased_Pi0_PFOLinks() );
     

      // //
      // //set per track track variables
      // //
      // for (unsigned int i = 0; i < oldTau->nTracks(); ++i) 
      // 	{
      // 	  //set track filter info
      // 	  newTau->setTrackFlag(oldTau->track(i), xAOD::TauJetParameters::failTrackFilter, oldTau->trackFilterPass(i)  );
      // 	  //set extrapolated track position
      // 	  newTau->setTrackEtaStrip( i ,  oldTau->trackEtaStrip(i) );
      // 	  newTau->setTrackPhiStrip( i ,  oldTau->trackPhiStrip(i) );
      // 	}

      // for (unsigned int i = 0; i < oldTau->nConversionTracks(); ++i) 
      // 	{
      // 	  //set conversion track flags
      // 	  newTau->setTrackFlag(oldTau->conversionTrack(i), xAOD::TauJetParameters::isConversion, true  );
      // 	}


   }

   // Print what happened:
   ATH_MSG( "Converting xAOD::TauJetAuxContainer_v1 to current version "
	    "[OK]" );
   
   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTauJetAuxContainerCnv_v1::transToPers( const xAOD::TauJetAuxContainer*,
                                                xAOD::TauJetAuxContainer_v1*,
                                                MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODTauJetAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODTauJetAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

