/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// IDScanHitFilter
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "GaudiKernel/MsgStream.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "IRegionSelector/IRoiDescriptor.h"

#include "IDScanHitFilter/IDScanHitFilter.h"
#include "IDScanHitFilter/IdScanSpPoint.h"
#include "IDScanHitFilter/Group.h"


IDScanHitFilter::IDScanHitFilter(const std::string& t, 
		     const std::string& n,
		     const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_numberingTool("TrigL2LayerNumberTool")
{
  declareInterface< ITrigHitFilter >( this );

  declareProperty( "PrintDiagnosticMessages", m_printDiagnosticMessages = false );
  declareProperty( "phiHalfWidth",            m_ROIphiHalfWidth = 0.1     );
  declareProperty( "etaHalfWidth",            m_ROIetaHalfWidth = 0.1     );

  declareProperty( "PhiBinSize",              m_phiBinSize = 2.4          );
  declareProperty( "EtaBinSize",              m_etaBinSize = 0.005        );
  declareProperty( "LayerThreshold",          m_layerThreshold = 3.5      );
  declareProperty( "EnhanceLayer0",           m_enhanceLayer0 = true      );
  declareProperty( "RemoveDuplicates",        m_cloneRemoval = true       );
  declareProperty( "CloneRemovalCut",         m_cloneRemovalCut = 3       );
  declareProperty( "UsePtSign",               m_usePtSign = true          );

  declareProperty( "LooseTightBoundary",      m_looseTight = 20.5         );

  declareProperty( "deltadzdrCut_loose",      m_loose_deltadzdrCut = 0.04 );
  declareProperty( "deltadzdrCut_tight",      m_tight_deltadzdrCut = 0.01 );
  declareProperty( "etaCutInner_loose",       m_loose_etaCutInner = 0.008 );
  declareProperty( "etaCutInner_tight",       m_tight_etaCutInner = 0.004 );
  declareProperty( "etaCutOuter_loose",       m_loose_etaCutOuter = 0.02  );
  declareProperty( "etaCutOuter_tight",       m_tight_etaCutOuter = 0.01  );
  declareProperty( "d0Cut_loose",             m_loose_d0Cut = 4.0         );
  declareProperty( "d0Cut_tight",             m_tight_d0Cut = 1.0         );
  declareProperty( "dPhidRBinSize_loose",     m_loose_dPhidRBinSize = 0.0000030 );
  declareProperty( "dPhidRBinSize_tight",     m_tight_dPhidRBinSize = 0.0000024 );
  declareProperty( "pTcutInMeV",              m_pTcutInMeV = 1000.0       );
  declareProperty( "phi0Bins_loose",          m_loose_phi0Bins = 16000    );
  declareProperty( "phi0Bins_tight",          m_tight_phi0Bins = 20000    );

  declareProperty( "maxBeamSpotShift",        m_maxBeamSpotShift = 4.0    );
  declareProperty( "fullScan",                m_fullScan = false    );

}

StatusCode IDScanHitFilter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  m_dPhidRCut = 0.3/m_pTcutInMeV;

  athenaLog << MSG::INFO << "IDScanHitFilter constructed "                       << endreq;
  athenaLog << MSG::INFO << "phiBinSize       set to " << m_phiBinSize     << endreq;
  athenaLog << MSG::INFO << "etaBinSize       set to " << m_etaBinSize     << endreq;
  athenaLog << MSG::INFO << "layerThreshold   set to " << m_layerThreshold << endreq;
  athenaLog << MSG::INFO << "enhanceZeroLayer set to " << m_enhanceLayer0  << endreq;
  athenaLog << MSG::INFO << "Clone removal    set to " << m_cloneRemoval   << endreq;
  athenaLog << MSG::INFO << "dphidrcut    set to " << m_dPhidRCut   << endreq;

  if (m_numberingTool.retrieve().isFailure()){
    athenaLog << MSG::FATAL << "Tool " << m_numberingTool
	      << " not found " << endreq;
    return StatusCode::FAILURE;
  } 

  /// get first endcap layer, so we know how
  /// barrel layers there are 
  int offsetEndcapPixels = m_numberingTool->offsetEndcapPixels();
  int maxSiliconLayerNum = m_numberingTool->maxSiliconLayerNum(); 
  //  int offsetBarrelSCT    = m_numberingTool->offsetBarrelSCT(); 
 
  /// Set the layer variables for the FilterBin class
  /// NB: decrement the endcap pixels, as we want the layer number 
  ///     of the last barrel layer, not the number of the first 
  ///     endcap pixel layer
  FilterBin::setMaxLayers( maxSiliconLayerNum );
  FilterBin::setMaxBarrelLayer( offsetEndcapPixels-1 );

  /// initialisise the HitFilter with the correct number of layers
  /// NB: DO NOT pass in the max eta here, but remember that it can be set here!!
  ///     the parameter initialisation is very messy for this stuff 
  iHitFilter::initialise( maxSiliconLayerNum ); /// , 3.0 ); 

  // NB: Now set the other way - ie, set m_fullScan=true, 
  //     then this will enforce m_ROIphiHalfWidth=M_PI, but in 
  //     iHitFilterinitialise() where it should have been in any 
  //     case 
  //  
  //  m_fullScan = (2*m_ROIphiHalfWidth > 6.28);

  return sc;
}


StatusCode IDScanHitFilter::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


  //Luke: edit this to take recoTracks as an input pointer, passed by reference 
void IDScanHitFilter::findTracks( std::vector<const TrigSiSpacePoint* > spVec, TrigInDetTrackCollection& recoTracks, 
	                          const IRoiDescriptor* RoI, double zVertex,
				  double shiftx=0, double shifty=0,
				  const int missing_layers=0 )
{
  MsgStream athenaLog(msgSvc(), name());

  setLayerThresholdTmp(getLayerThreshold()-double(missing_layers));
  athenaLog << MSG::DEBUG
	    << "m_layerThreshold="      << m_layerThreshold
	    << "\tm_layerThresholdTmp=" << m_layerThresholdTmp << endreq;

  // 0. check if RoI is in [0,2pi] boundary (boundary for the space point definition!)
  //    and calculate phi offset <- now done in spacepoints themselves!!!

  // 1. make internal IDScan SPs
  if (RoI!=nullptr) {
		makeIDScanSPs( spVec, RoI->eta(), 0.5*fabs(RoI->etaPlus()-RoI->etaMinus()),
				RoI->phi(), 0.5*fabs(RoI->phiPlus()-RoI->phiMinus()), 
				zVertex );
	}
	else {
		makeIDScanSPs( spVec, 0, 3,
				0, M_PI, 
				zVertex );
	}

  if (this->outputLevel() <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Made " << m_internalSPs->size() 
						   << " IdScanSpPoints. Proceeding to HitFilter." << endreq;
  if (m_printDiagnosticMessages) std::cout << "IdScanMain DIAGNOSTIC " <<  " Made " << m_internalSPs->size() 
					   << " IdScanSpPoints. Proceeding to HitFilter." << std::endl;

  // 2. run HitFilter & GroupCleaner

  GroupList cleanGroups = execute();

  // 3. turn groups into TrigInDetTracks
  
  //Luke: delete this line
  //TrigInDetTrackCollection* recoTracks=NULL;
  if ( !cleanGroups.empty() ) {
    makeTracks( cleanGroups, recoTracks, zVertex, shiftx, shifty);
  }

  cleanInternalSPs();
  
  //return recoTracks;
}


//Luke: need to pass recoTracks by reference to this or have it as a member variable or something?
void IDScanHitFilter::makeTracks( GroupList& idScanGroups, TrigInDetTrackCollection& recoTracks,  double zPosition, 
				  double shiftx=0, double shifty=0)
{

  MsgStream athenaLog(msgSvc(), name());

    //  recoTracks->reserve( idScanGroups.size() );

  // Since the FilterMap is a hashmap and hash tables are not guaranteed
  // to be sorted, running the code twice sometimes ends up producing
  // differently ordered REGTEST output files. So sort them is debugging:

  if ( this->outputLevel() <= MSG::DEBUG ) idScanGroups.sort();

  for ( GroupList::iterator gItr( idScanGroups.begin() ) ; gItr != idScanGroups.end(); ++gItr ) {
    if(gItr->groupHits().size()==0) continue;
    gItr->groupHits().sort(orderInRho());
    double phi0 = gItr->getPhi0();
    if(phi0<-M_PI) 
      { 
	phi0+=2.0*M_PI;
      }
    else if(phi0>M_PI) 
      {
	phi0-=2.0*M_PI;
      }
    if (this->outputLevel() <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / group phi0/ptInv/eta: " 
					       << phi0 << " / " 
					       << gItr->getPtInv() << " / " 
					       << gItr->getEta() << endreq;

    if (m_printDiagnosticMessages) std::cout << "IdScanMain DIAGNOSTIC " << " group phi0/ptInv/eta: " 
					     << phi0 << " / " 
					     << gItr->getPtInv() << " / " 
					     << gItr->getEta() << std::endl;

    double ptInv=gItr->getPtInv();   
    if(fabs(ptInv)<1e-9) ptInv=1e-9; 

    double input_d0 = 0;
    if (shiftx || shifty){
      input_d0 = -shiftx*sin(phi0)+shifty*cos(phi0);
    }

    TrigInDetTrackFitPar* perigee = new TrigInDetTrackFitPar( input_d0, 
							      phi0, 
							      zPosition, 
							      gItr->getEta(), 
							      1.0/ptInv );

    std::vector<const TrigSiSpacePoint*> spacePointsOnTrack;
    for(std::list<IdScanSpPoint *>::iterator hitItr=gItr->groupHits().begin();
	hitItr!=gItr->groupHits().end();hitItr++) 
      {
	spacePointsOnTrack.push_back( (**hitItr).commonSP() );
      }
    std::vector<const TrigSiSpacePoint*>* spv = new std::vector<const TrigSiSpacePoint*>;
    spv->assign(spacePointsOnTrack.begin(),spacePointsOnTrack.end());
    TrigInDetTrack* newTrack = new TrigInDetTrack(spv, perigee, NULL, gItr->getChi2());
    newTrack->algorithmId(TrigInDetTrack::IDSCANID);
    recoTracks.push_back( newTrack );
  }

  //return recoTracks;

}
