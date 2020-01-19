/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MMSimHitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"

#include "TTree.h"

StatusCode MMSimHitVariables::fillVariables() 
{

  ATH_MSG_INFO("do fillNSWMMHitVariables()");

  CHECK( this->clearVariables() );

  const GenericMuonSimHitCollection *nswContainer = nullptr;
  CHECK( m_evtStore->retrieve(nswContainer, m_ContainerName.c_str()) );
  
  // Get the MicroMegas Id hit helper
  MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper();
  MM_SimIdToOfflineId simToOffline(*m_MmIdHelper);

  if(nswContainer->size()==0) ATH_MSG_WARNING(" GenericMuonSimHit empty ");
  for( auto it : *nswContainer ) {
    const GenericMuonSimHit hit = it;

    if(hit.depositEnergy()==0.) continue; // SimHits without energy loss are not recorded. 
    //    if( hit.kineticEnergy()<m_energyThreshold && abs(hit.particleEncoding())==11) continue ; 
       
    // connect the hit with the MC truth
//    int barcode = hit.particleLink().barcode();
//    m_NSWMM_trackId->push_back(barcode);
//    for (unsigned int tr=0;tr<m_Truth_particleBarcode->size();tr++) {
//        if (barcode==m_Truth_particleBarcode->at(tr)) {
//            m_NSWMM_truthEl->push_back(tr);
//        }
//    }


    m_NSWMM_globalTime->push_back(hit.globalTime());
  
    const Amg::Vector3D globalPosition = hit.globalPosition();
    m_NSWMM_hitGlobalPositionX->push_back(globalPosition.x());
    m_NSWMM_hitGlobalPositionY->push_back(globalPosition.y());
    m_NSWMM_hitGlobalPositionZ->push_back(globalPosition.z());
    m_NSWMM_hitGlobalPositionR->push_back(globalPosition.perp());
    m_NSWMM_hitGlobalPositionP->push_back(globalPosition.phi());
    const Amg::Vector3D globalDirection = hit.globalDirection();
    m_NSWMM_hitGlobalDirectionX->push_back(globalDirection.x());
    m_NSWMM_hitGlobalDirectionY->push_back(globalDirection.y());
    m_NSWMM_hitGlobalDirectionZ->push_back(globalDirection.z());

    const Amg::Vector3D localPosition = hit.localPosition();
    m_NSWMM_hitLocalPositionX->push_back(localPosition.x());
    m_NSWMM_hitLocalPositionY->push_back(localPosition.y());
    m_NSWMM_hitLocalPositionZ->push_back(localPosition.z());

    m_NSWMM_particleEncoding->push_back(hit.particleEncoding());
    m_NSWMM_kineticEnergy->push_back(hit.kineticEnergy());
    m_NSWMM_depositEnergy->push_back(hit.depositEnergy());
    m_NSWMM_StepLength->push_back(hit.StepLength());


    // Read the information about the Micro Megas hit
    int simId = hit.GenericId();
    std::string sim_stationName = hitHelper->GetStationName(simId);
    int sim_stationEta  = hitHelper->GetZSector(simId);
    int sim_stationPhi  = hitHelper->GetPhiSector(simId);
    int sim_multilayer  = hitHelper->GetMultiLayer(simId);
    int sim_layer       = hitHelper->GetLayer(simId);
    int sim_side        = hitHelper->GetSide(simId);

    ATH_MSG_DEBUG(     "MicroMegas SimHit  id:  Station Name [" << sim_stationName << "]"
                    << " Station Eta ["  << sim_stationEta  << "]"
                    << " Station Phi ["  << sim_stationPhi  << "]"
                    << " MultiLayer ["   << sim_multilayer  << "]"
                    << " Layer  ["        << sim_layer       << "]"
                    << " Side ["         << sim_side        << "]" );

    if( sim_stationPhi == 0 ){
      ATH_MSG_ERROR("MicroMegas validation: unexpected phi range " << sim_stationPhi);
      return StatusCode::FAILURE;
    }

    // Fill Ntuple with SimId data
    m_NSWMM_sim_stationName ->push_back(sim_stationName);
    m_NSWMM_sim_stationEta  ->push_back(sim_stationEta);
    m_NSWMM_sim_stationPhi  ->push_back(sim_stationPhi);
    m_NSWMM_sim_multilayer  ->push_back(sim_multilayer);
    m_NSWMM_sim_layer       ->push_back(sim_layer);
    m_NSWMM_sim_side        ->push_back(sim_side);


    //  convert simHit id to offline id; make sanity checks; retrieve the associated detector element.
    Identifier offId = simToOffline.convert(hit.GenericId());
    
    // sanity checks
    if( !m_MmIdHelper->is_mm(offId) ){
      ATH_MSG_WARNING("MM id is not a mm id! " << m_MmIdHelper->print_to_string(offId));
    }
    if( !m_MmIdHelper->is_muon(offId) ){
      ATH_MSG_WARNING("MM id is not a muon id! " << m_MmIdHelper->print_to_string(offId));
    }
    if( m_MmIdHelper->is_mdt(offId)||m_MmIdHelper->is_rpc(offId)||m_MmIdHelper->is_tgc(offId)||m_MmIdHelper->is_csc(offId)||m_MmIdHelper->is_stgc(offId) ){
      ATH_MSG_WARNING("MM id has wrong technology type! " << m_MmIdHelper->is_mdt(offId) << " " << m_MmIdHelper->is_rpc(offId)
                      << " " << m_MmIdHelper->is_tgc(offId) << " " << m_MmIdHelper->is_csc(offId) << " " << m_MmIdHelper->is_stgc(offId) );
    }
    if( m_MmIdHelper->gasGap(offId) != sim_layer ) {
      ATH_MSG_WARNING("MM id has bad layer field! " << m_MmIdHelper->print_to_string(offId)  );
    }


    std::string stName = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(offId));
    int off_stationEta   = m_MmIdHelper->stationEta(offId);
    int off_stationPhi   = m_MmIdHelper->stationPhi(offId);
    int off_multiplet    = m_MmIdHelper->multilayer(offId);
    int off_gas_gap      = m_MmIdHelper->gasGap(offId);
    int off_channel      = m_MmIdHelper->channel(offId);

    // Get MM_READOUT from MMDetectorDescription
    char side = off_stationEta < 0 ? 'C' : 'A';
    char sector_l = stName.substr(2,1)=="L" ? 'L' : 'S';
    MMDetectorHelper aHelper;
    MMDetectorDescription* mm = aHelper.Get_MMDetector(sector_l, abs(off_stationEta), off_stationPhi, off_multiplet, side);
    MMReadoutParameters roParam = mm->GetReadoutParameters();

    ATH_MSG_INFO(     "MicroMegas Offline id:  Station Name [" << stName << " ]"
                    << " Station Eta ["  << off_stationEta      << "]"
                    << " Station Phi ["  << off_stationPhi      << "]"
                    << " Multiplet  ["   << off_multiplet       << "]"
                    << " GasGap ["       << off_gas_gap         << "]"
                    << " ChNr ["         << off_channel         << "]" );


    int isSmall = stName[2] == 'S';
    ATH_MSG_DEBUG("MicroMegas geometry, retrieving detector element for: isSmall " << isSmall << " eta " << m_MmIdHelper->stationEta(offId)
                  << " phi " << m_MmIdHelper->stationPhi(offId) << " ml " << m_MmIdHelper->multilayer(offId) );

    int phiCor = m_MmIdHelper->stationPhi(offId);
    int mlCor  = m_MmIdHelper->multilayer(offId);

    const MuonGM::MMReadoutElement* detEl = m_detManager->getMMRElement_fromIdFields(isSmall, m_MmIdHelper->stationEta(offId), phiCor, mlCor );  
    // const MuonGM::MMReadoutElement* detEl = m_detManager->getMMReadoutElement(offId);

    if( !detEl ){
      /*
	ATH_MSG_WARNING("MicroMegas geometry, failed to retrieve detector element for: isSmall " << isSmall << " eta " << m_MmIdHelper->stationEta(offId)
	<< " phi " << m_MmIdHelper->stationPhi(offId) << " ml " << m_MmIdHelper->multiplet(offId) );
      */		      
      ATH_MSG_WARNING("MicroMegas geometry, failed to retrieve detector element for: " << m_MmIdHelper->print_to_string(offId) );
      continue;
    }

    // surface
    const Trk::PlaneSurface& surf = detEl->surface(offId);
    // compute hit position within the detector element/surfaces
    Amg::Transform3D     gToL = detEl->absTransform().inverse();
    Amg::Vector3D hpos(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
    Amg::Vector3D dSurface_pos = gToL*hpos;

    // compute the hit position on the readout plane (same as in MuonFastDigitization)
    Amg::Vector3D rSurface_pos = surf.transform().inverse()*hpos;
     
    Amg::Vector2D  posOnSurfUnProjected(rSurface_pos.x(),rSurface_pos.y());
//    double gasGapThickness = detEl->getDesign(offId)->gasGapThickness();

    // check where the readout plane is located and compute the local direction accordingly 
    Amg::Vector3D ldir(0., 0., 0.);
    if ((roParam.stereoAngel).at(m_MmIdHelper->gasGap(offId)-1)==1)
      ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
    else
      ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), -hit.globalDirection().z());
  
    double scale, scaletop;
    double gasgap = 5.;
   
    scale = -rSurface_pos.z()/ldir.z();
    scaletop = (gasgap+rSurface_pos.z())/ldir.z();
    // scaletop = (fabs(gasGapThickness) + slpos.z())/locdir.z();
      	
    Amg::Vector3D hitOnSurface = rSurface_pos + scale*ldir;
    Amg::Vector3D hitOnTopSurface = rSurface_pos + scaletop*ldir;
    Amg::Vector2D posOnSurf (hitOnSurface.x(), hitOnSurface.y());
    Amg::Vector2D posOnTopSurf (hitOnTopSurface.x(),hitOnTopSurface.y());
        
  
    int stripNumber = detEl->stripNumber(posOnSurf,offId);
//    int LastStripNumber = detEl->stripNumber(posOnTopSurf, offId);
     
    // perform bound check
    if( !surf.insideBounds(posOnSurf) ) continue;
        
    if( stripNumber == -1 ){
      ATH_MSG_WARNING("MicroMegas validation: failed to obtain strip number " << m_MmIdHelper->print_to_string(offId) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << rSurface_pos.z() );
      stripNumber = 1;
    }
    
    Identifier oldId = offId;
    offId = m_MmIdHelper->channelID(offId, m_MmIdHelper->multilayer(offId), m_MmIdHelper->gasGap(offId),stripNumber);
    if( m_MmIdHelper->gasGap(offId) != sim_layer ) {
      ATH_MSG_WARNING("MicroMegas validation: MM id has bad layer field(2)! " << std::endl << " " << m_MmIdHelper->print_to_string(offId) << std::endl
                      << " " << m_MmIdHelper->print_to_string(oldId) << " stripN " << stripNumber );
    }

    Amg::Vector2D fastDigitPos(0.,0.);
    if( !detEl->stripPosition(offId,fastDigitPos ) ){
      ATH_MSG_WARNING("MicroMegas validation: failed to obtain local position for identifier " << m_MmIdHelper->print_to_string(offId) );
      continue;
    }

    Amg::Vector3D detpos = detEl->globalPosition();
    ATH_MSG_DEBUG("Global hit   : r " << hit.globalPosition().perp() << ", phi " << hit.globalPosition().phi() << ", z " << hit.globalPosition().z()
                    << "; detEl: r " << detpos.perp() << ", phi " << detpos.phi() << ", z " << detpos.z()
                    << "; surf z "   << surf.center().z() << ", ml "  << sim_multilayer << ", l " << sim_layer );
    ATH_MSG_DEBUG("Local hit    : x " << hit.localPosition().x() << " y " << hit.localPosition().y() << " z " << hit.localPosition().z()
                    << " detEl: x " << dSurface_pos.x() << " y " << dSurface_pos.y() << " z " << dSurface_pos.z());
    ATH_MSG_DEBUG("MM Fast digit: x " << fastDigitPos.x() << " y " << fastDigitPos.y()
                    << ", gToL: x " << rSurface_pos.x() << " y " << rSurface_pos.y() << " z " << rSurface_pos.z() );



    // Fill Ntuple with offline ID data
    m_NSWMM_off_stationName   ->push_back(stName);
    m_NSWMM_off_stationEta    ->push_back(off_stationEta);
    m_NSWMM_off_stationPhi    ->push_back(off_stationPhi);
    m_NSWMM_off_multiplet     ->push_back(off_multiplet);
    m_NSWMM_off_gas_gap       ->push_back(off_gas_gap);
    m_NSWMM_off_channel       ->push_back(off_channel);


    // Fill ntuple with the hit/surface/digit positions
    m_NSWMM_detector_globalPositionX->push_back( detpos.x() );
    m_NSWMM_detector_globalPositionY->push_back( detpos.y() );
    m_NSWMM_detector_globalPositionZ->push_back( detpos.z() );
    m_NSWMM_detector_globalPositionR->push_back( detpos.perp() );
    m_NSWMM_detector_globalPositionP->push_back( detpos.phi() );

    m_NSWMM_hitToDsurfacePositionX->push_back( dSurface_pos.x() );
    m_NSWMM_hitToDsurfacePositionY->push_back( dSurface_pos.y() );
    m_NSWMM_hitToDsurfacePositionZ->push_back( dSurface_pos.z() );

    m_NSWMM_hitToRsurfacePositionX->push_back( rSurface_pos.x() );
    m_NSWMM_hitToRsurfacePositionY->push_back( rSurface_pos.y() );
    m_NSWMM_hitToRsurfacePositionZ->push_back( rSurface_pos.z() );

    m_NSWMM_FastDigitRsurfacePositionX->push_back( posOnSurf.x() );
    m_NSWMM_FastDigitRsurfacePositionY->push_back( posOnSurf.y() );

    ATH_MSG_DEBUG("---------- Hit processing ends!");

    m_NSWMM_nSimHits++;
  }

  ATH_MSG_INFO("processed " << m_NSWMM_nSimHits << " MicroMegas hits");
  return StatusCode::SUCCESS;
}


StatusCode MMSimHitVariables::clearVariables() 
{
  m_NSWMM_nSimHits = 0;
  m_NSWMM_trackId->clear();
  m_NSWMM_truthEl->clear();
  m_NSWMM_globalTime->clear();
  m_NSWMM_hitGlobalPositionX->clear();
  m_NSWMM_hitGlobalPositionY->clear();
  m_NSWMM_hitGlobalPositionZ->clear();
  m_NSWMM_hitGlobalPositionR->clear();
  m_NSWMM_hitGlobalPositionP->clear();
  m_NSWMM_hitGlobalDirectionX->clear();
  m_NSWMM_hitGlobalDirectionY->clear();
  m_NSWMM_hitGlobalDirectionZ->clear();

  m_NSWMM_hitLocalPositionX->clear();
  m_NSWMM_hitLocalPositionY->clear();
  m_NSWMM_hitLocalPositionZ->clear();

  m_NSWMM_detector_globalPositionX->clear();
  m_NSWMM_detector_globalPositionY->clear();
  m_NSWMM_detector_globalPositionZ->clear();
  m_NSWMM_detector_globalPositionR->clear();
  m_NSWMM_detector_globalPositionP->clear();

  m_NSWMM_hitToDsurfacePositionX->clear();
  m_NSWMM_hitToDsurfacePositionY->clear();
  m_NSWMM_hitToDsurfacePositionZ->clear();

  m_NSWMM_hitToRsurfacePositionX->clear();
  m_NSWMM_hitToRsurfacePositionY->clear();
  m_NSWMM_hitToRsurfacePositionZ->clear();

  m_NSWMM_FastDigitRsurfacePositionX->clear();
  m_NSWMM_FastDigitRsurfacePositionY->clear();


  m_NSWMM_particleEncoding->clear();
  m_NSWMM_kineticEnergy->clear();
  m_NSWMM_depositEnergy->clear();
  m_NSWMM_StepLength->clear();


  m_NSWMM_sim_stationName->clear();
  m_NSWMM_sim_stationEta->clear();
  m_NSWMM_sim_stationPhi->clear();
  m_NSWMM_sim_multilayer->clear();
  m_NSWMM_sim_layer->clear();
  m_NSWMM_sim_side->clear();

  m_NSWMM_off_stationName->clear();
  m_NSWMM_off_stationEta->clear();
  m_NSWMM_off_stationPhi->clear();
  m_NSWMM_off_multiplet->clear();
  m_NSWMM_off_gas_gap->clear();
  m_NSWMM_off_channel->clear();

  return StatusCode::SUCCESS;
}

StatusCode MMSimHitVariables::deleteVariables() 
{ 
  delete m_NSWMM_globalTime;
  delete m_NSWMM_hitGlobalPositionX;
  delete m_NSWMM_hitGlobalPositionY;
  delete m_NSWMM_hitGlobalPositionZ;
  delete m_NSWMM_hitGlobalPositionR;
  delete m_NSWMM_hitGlobalPositionP;
  delete m_NSWMM_hitGlobalDirectionX;
  delete m_NSWMM_hitGlobalDirectionY;
  delete m_NSWMM_hitGlobalDirectionZ;

  delete m_NSWMM_hitLocalPositionX;
  delete m_NSWMM_hitLocalPositionY;
  delete m_NSWMM_hitLocalPositionZ;

  delete m_NSWMM_detector_globalPositionX;
  delete m_NSWMM_detector_globalPositionY;
  delete m_NSWMM_detector_globalPositionZ;
  delete m_NSWMM_detector_globalPositionR;
  delete m_NSWMM_detector_globalPositionP;

  delete m_NSWMM_hitToDsurfacePositionX;
  delete m_NSWMM_hitToDsurfacePositionY;
  delete m_NSWMM_hitToDsurfacePositionZ;

  delete m_NSWMM_hitToRsurfacePositionX;
  delete m_NSWMM_hitToRsurfacePositionY;
  delete m_NSWMM_hitToRsurfacePositionZ;

  delete m_NSWMM_FastDigitRsurfacePositionX;
  delete m_NSWMM_FastDigitRsurfacePositionY;


  delete m_NSWMM_particleEncoding;
  delete m_NSWMM_kineticEnergy;
  delete m_NSWMM_depositEnergy;
  delete m_NSWMM_StepLength;

  delete m_NSWMM_sim_stationName;
  delete m_NSWMM_sim_stationEta;
  delete m_NSWMM_sim_stationPhi;
  delete m_NSWMM_sim_multilayer;
  delete m_NSWMM_sim_layer;
  delete m_NSWMM_sim_side;

  delete m_NSWMM_off_stationName;
  delete m_NSWMM_off_stationEta;
  delete m_NSWMM_off_stationPhi;
  delete m_NSWMM_off_multiplet;
  delete m_NSWMM_off_gas_gap;
  delete m_NSWMM_off_channel;

  m_NSWMM_nSimHits = 0;

  m_NSWMM_globalTime = nullptr;
  m_NSWMM_hitGlobalPositionX = nullptr;
  m_NSWMM_hitGlobalPositionY = nullptr;
  m_NSWMM_hitGlobalPositionZ = nullptr;
  m_NSWMM_hitGlobalPositionR = nullptr;
  m_NSWMM_hitGlobalPositionP = nullptr;

  m_NSWMM_hitGlobalDirectionX = nullptr;
  m_NSWMM_hitGlobalDirectionY = nullptr;
  m_NSWMM_hitGlobalDirectionZ = nullptr;

  m_NSWMM_hitLocalPositionX = nullptr;
  m_NSWMM_hitLocalPositionY = nullptr;
  m_NSWMM_hitLocalPositionZ = nullptr;

  m_NSWMM_detector_globalPositionX = nullptr;
  m_NSWMM_detector_globalPositionY = nullptr;
  m_NSWMM_detector_globalPositionZ = nullptr;
  m_NSWMM_detector_globalPositionR = nullptr;
  m_NSWMM_detector_globalPositionP = nullptr;

  m_NSWMM_hitToDsurfacePositionX = nullptr;
  m_NSWMM_hitToDsurfacePositionY = nullptr;
  m_NSWMM_hitToDsurfacePositionZ = nullptr;

  m_NSWMM_hitToRsurfacePositionX = nullptr;
  m_NSWMM_hitToRsurfacePositionY = nullptr;
  m_NSWMM_hitToRsurfacePositionZ = nullptr;

  m_NSWMM_FastDigitRsurfacePositionX = nullptr;
  m_NSWMM_FastDigitRsurfacePositionY = nullptr;

  m_NSWMM_particleEncoding = nullptr;
  m_NSWMM_kineticEnergy = nullptr;
  m_NSWMM_depositEnergy = nullptr;
  m_NSWMM_StepLength = nullptr;

  m_NSWMM_sim_stationName = nullptr;
  m_NSWMM_sim_stationEta = nullptr;
  m_NSWMM_sim_stationPhi = nullptr;
  m_NSWMM_sim_multilayer = nullptr;
  m_NSWMM_sim_layer = nullptr;
  m_NSWMM_sim_side = nullptr;

  m_NSWMM_off_stationName = nullptr;
  m_NSWMM_off_stationEta = nullptr;
  m_NSWMM_off_stationPhi = nullptr;
  m_NSWMM_off_multiplet = nullptr;
  m_NSWMM_off_gas_gap = nullptr;
  m_NSWMM_off_channel = nullptr;

  return StatusCode::SUCCESS;
}

StatusCode MMSimHitVariables::initializeVariables() 
{
  m_NSWMM_nSimHits = 0;
  m_NSWMM_trackId  = new std::vector<int>;
  m_NSWMM_truthEl  = new std::vector<int>;
  m_NSWMM_globalTime = new std::vector<double>;
  m_NSWMM_hitGlobalPositionX = new std::vector<double>;
  m_NSWMM_hitGlobalPositionY = new std::vector<double>;
  m_NSWMM_hitGlobalPositionZ = new std::vector<double>;
  m_NSWMM_hitGlobalPositionR = new std::vector<double>;
  m_NSWMM_hitGlobalPositionP = new std::vector<double>;
  m_NSWMM_hitGlobalDirectionX = new std::vector<double>;
  m_NSWMM_hitGlobalDirectionY = new std::vector<double>;
  m_NSWMM_hitGlobalDirectionZ = new std::vector<double>;

  m_NSWMM_hitLocalPositionX = new std::vector<double>;
  m_NSWMM_hitLocalPositionY = new std::vector<double>;
  m_NSWMM_hitLocalPositionZ = new std::vector<double>;

  m_NSWMM_detector_globalPositionX = new std::vector<double>;
  m_NSWMM_detector_globalPositionY = new std::vector<double>;
  m_NSWMM_detector_globalPositionZ = new std::vector<double>;
  m_NSWMM_detector_globalPositionR = new std::vector<double>;
  m_NSWMM_detector_globalPositionP = new std::vector<double>;

  m_NSWMM_hitToDsurfacePositionX = new std::vector<double>;
  m_NSWMM_hitToDsurfacePositionY = new std::vector<double>;
  m_NSWMM_hitToDsurfacePositionZ = new std::vector<double>;

  m_NSWMM_hitToRsurfacePositionX = new std::vector<double>;
  m_NSWMM_hitToRsurfacePositionY = new std::vector<double>;
  m_NSWMM_hitToRsurfacePositionZ = new std::vector<double>;

  m_NSWMM_FastDigitRsurfacePositionX = new std::vector<double>;
  m_NSWMM_FastDigitRsurfacePositionY = new std::vector<double>;


  m_NSWMM_particleEncoding = new std::vector<int>;
  m_NSWMM_kineticEnergy = new std::vector<double>;
  m_NSWMM_depositEnergy = new std::vector<double>;
  m_NSWMM_StepLength = new std::vector<double>;


  m_NSWMM_sim_stationName = new std::vector<std::string>;
  m_NSWMM_sim_stationEta  = new std::vector<int>;
  m_NSWMM_sim_stationPhi  = new std::vector<int>;
  m_NSWMM_sim_multilayer  = new std::vector<int>;
  m_NSWMM_sim_layer       = new std::vector<int>;
  m_NSWMM_sim_side        = new std::vector<int>;

  m_NSWMM_off_stationName = new std::vector<std::string>;
  m_NSWMM_off_stationEta  = new std::vector<int>;
  m_NSWMM_off_stationPhi  = new std::vector<int>;
  m_NSWMM_off_multiplet   = new std::vector<int>;
  m_NSWMM_off_gas_gap     = new std::vector<int>;
  m_NSWMM_off_channel     = new std::vector<int>;

  if(m_tree) {
    m_tree->Branch("Hits_MM_n", &m_NSWMM_nSimHits, "Hits_MM_n/i");
    m_tree->Branch("Hits_MM_trackId", &m_NSWMM_trackId);
    m_tree->Branch("Hits_MM_truthEl", &m_NSWMM_truthEl);
    m_tree->Branch("Hits_MM_globalTime", &m_NSWMM_globalTime);
    m_tree->Branch("Hits_MM_hitGlobalPositionX", &m_NSWMM_hitGlobalPositionX);
    m_tree->Branch("Hits_MM_hitGlobalPositionY", &m_NSWMM_hitGlobalPositionY);
    m_tree->Branch("Hits_MM_hitGlobalPositionZ", &m_NSWMM_hitGlobalPositionZ);
    m_tree->Branch("Hits_MM_hitGlobalPositionR", &m_NSWMM_hitGlobalPositionR);
    m_tree->Branch("Hits_MM_hitGlobalPositionP", &m_NSWMM_hitGlobalPositionP);
    m_tree->Branch("Hits_MM_hitGlobalDirectionX", &m_NSWMM_hitGlobalDirectionX);
    m_tree->Branch("Hits_MM_hitGlobalDirectionY", &m_NSWMM_hitGlobalDirectionY);
    m_tree->Branch("Hits_MM_hitGlobalDirectionZ", &m_NSWMM_hitGlobalDirectionZ);

    m_tree->Branch("Hits_MM_hitLocalPositionX", &m_NSWMM_hitLocalPositionX);
    m_tree->Branch("Hits_MM_hitLocalPositionY", &m_NSWMM_hitLocalPositionY);
    m_tree->Branch("Hits_MM_hitLocalPositionZ", &m_NSWMM_hitLocalPositionZ);

    m_tree->Branch("Hits_MM_detector_globalPositionX", &m_NSWMM_detector_globalPositionX);
    m_tree->Branch("Hits_MM_detector_globalPositionY", &m_NSWMM_detector_globalPositionY);
    m_tree->Branch("Hits_MM_detector_globalPositionZ", &m_NSWMM_detector_globalPositionZ);
    m_tree->Branch("Hits_MM_detector_globalPositionR", &m_NSWMM_detector_globalPositionR);
    m_tree->Branch("Hits_MM_detector_globalPositionP", &m_NSWMM_detector_globalPositionP);

    m_tree->Branch("Hits_MM_hitToDsurfacePositionX", &m_NSWMM_hitToDsurfacePositionX);
    m_tree->Branch("Hits_MM_hitToDsurfacePositionY", &m_NSWMM_hitToDsurfacePositionY);
    m_tree->Branch("Hits_MM_hitToDsurfacePositionZ", &m_NSWMM_hitToDsurfacePositionZ);

    m_tree->Branch("Hits_MM_hitToRsurfacePositionX", &m_NSWMM_hitToRsurfacePositionX);
    m_tree->Branch("Hits_MM_hitToRsurfacePositionY", &m_NSWMM_hitToRsurfacePositionY);
    m_tree->Branch("Hits_MM_hitToRsurfacePositionZ", &m_NSWMM_hitToRsurfacePositionZ);

    m_tree->Branch("Hits_MM_FastDigitRsurfacePositionX", &m_NSWMM_FastDigitRsurfacePositionX);
    m_tree->Branch("Hits_MM_FastDigitRsurfacePositionY", &m_NSWMM_FastDigitRsurfacePositionY);
     

    m_tree->Branch("Hits_MM_particleEncoding", &m_NSWMM_particleEncoding);
    m_tree->Branch("Hits_MM_kineticEnergy", &m_NSWMM_kineticEnergy);
    m_tree->Branch("Hits_MM_depositEnergy", &m_NSWMM_depositEnergy);
    m_tree->Branch("Hits_MM_StepLength", &m_NSWMM_StepLength);


    m_tree->Branch("Hits_MM_sim_stationName", &m_NSWMM_sim_stationName);
    m_tree->Branch("Hits_MM_sim_stationEta", &m_NSWMM_sim_stationEta);
    m_tree->Branch("Hits_MM_sim_stationPhi", &m_NSWMM_sim_stationPhi);
    m_tree->Branch("Hits_MM_sim_multilayer", &m_NSWMM_sim_multilayer);
    m_tree->Branch("Hits_MM_sim_layer", &m_NSWMM_sim_layer);
    m_tree->Branch("Hits_MM_sim_side", &m_NSWMM_sim_side);

    m_tree->Branch("Hits_MM_off_stationName", &m_NSWMM_off_stationName);
    m_tree->Branch("Hits_MM_off_stationEta", &m_NSWMM_off_stationEta);
    m_tree->Branch("Hits_MM_off_stationPhi", &m_NSWMM_off_stationPhi);
    m_tree->Branch("Hits_MM_off_multiplet", &m_NSWMM_off_multiplet);
    m_tree->Branch("Hits_MM_off_gas_gap", &m_NSWMM_off_gas_gap);
    m_tree->Branch("Hits_MM_off_channel", &m_NSWMM_off_channel);

  }

  return StatusCode::SUCCESS;
}
