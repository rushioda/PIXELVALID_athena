/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"

//Gaudi Includes
#include "StoreGate/StoreGateSvc.h"

// Calo include
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"  
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"


CalibHitToCaloCellTool::CalibHitToCaloCellTool(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p),
    m_caloGain((int)CaloGain::LARLOWGAIN),
    m_store_Tot(false),
    m_store_Vis(false),
    m_store_Em(false),
    m_store_NonEm(false),
    m_caloCell_Tot("TotalCalibCell"), m_caloCell_Vis("VisCalibCell"), 
    m_caloCell_Em(""), m_caloCell_NonEm(""),
    m_caloCell_ID(0),
    m_caloDM_ID(0),
    m_caloDDMgr(0),
    m_nchan(0)
{
  declareInterface<CalibHitToCaloCellTool>(this);

  declareProperty("CaloGain", m_caloGain);
  declareProperty("CalibHitContainers", m_calibHitContainerNames);
  declareProperty("SingleParticleBarcode", m_singleParticleBarcode = 10001);
  
  declareProperty("CellTotEne",    m_caloCell_Tot);
  declareProperty("CellVisEne",    m_caloCell_Vis);
  declareProperty("CellEmEne",     m_caloCell_Em);
  declareProperty("CellNonEmEne",  m_caloCell_NonEm);
  declareProperty("DoTile",        m_doTile=false);

  declareProperty("OutputCellContainerName", m_outputCellContainerName = "TruthCells");
  declareProperty("OutputClusterContainerName", m_outputClusterContainerName = "TruthClusters");
  declareProperty("WriteTruthCellContainers", m_writeTruthCellContainers=false);
  declareProperty("WriteTruthClusterContainers", m_writeTruthClusterContainers=false);
  
  m_tileActiveHitCnt   = "TileCalibHitActiveCell";
  m_tileInactiveHitCnt = "TileCalibHitInactiveCell";
  m_tileDMHitCnt       = "TileCalibHitDeadMaterial";
  m_larActHitCnt   = "LArCalibrationHitActive";
  m_larInactHitCnt = "LArCalibrationHitInactive";
  m_larDMHitCnt    = "LArCalibrationHitDeadMaterial";

}


CalibHitToCaloCellTool::~CalibHitToCaloCellTool() {}


////////////////   INITIALIZE   ///////////////////////
StatusCode CalibHitToCaloCellTool::initialize() 
{ 
  // retrieve ID helpers from det store
  ATH_MSG_INFO("initialisation ID helpers" );

  ATH_CHECK(  detStore()->retrieve(m_caloCell_ID) );
  ATH_CHECK(  detStore()->retrieve(m_caloDM_ID) );
  ATH_CHECK(  detStore()->retrieve(m_caloDDMgr, "CaloMgr") );
    
  ATH_MSG_INFO("initialisation completed" );
  return StatusCode::SUCCESS;
}


/////////////////   EXECUTE   //////////////////////
StatusCode CalibHitToCaloCellTool::processCalibHitsFromParticle(int barcode)
{
  ATH_MSG_DEBUG("in calibHitToCaloCellTool");

  std::string EnergyTypeToStr[] = {"Eem","Evis","Etot"};

  //protection - tool should run once per event
  if (evtStore()->contains<CaloCellContainer>((m_outputCellContainerName+EnergyTypeToStr[0]).c_str()) &&
      evtStore()->contains<xAOD::CaloClusterContainer>((m_outputClusterContainerName+EnergyTypeToStr[0]).c_str())) {
    ATH_MSG_DEBUG("containers for truth calo cells and truth clusters already exist");
    return StatusCode::SUCCESS;
  }
    
  // register containers for cells and clusters
  for (unsigned int i=0; i<CalibHitUtils::nEnergyTypes; i++) {
    m_truthCells[i] = new CaloCellContainer();
    m_truthClusters[i] = CaloClusterStoreHelper::makeContainer(&(*evtStore()),(m_outputClusterContainerName+EnergyTypeToStr[i]).c_str(),msg());    
    if (!m_truthClusters[i]) {
      ATH_MSG_FATAL("Cannot make cluster container");
      return StatusCode::FAILURE;
    }
  }
  
  // retrieve calibration hit containers
  const unsigned int nCont = m_calibHitContainerNames.size();
  std::vector<const CaloCalibrationHitContainer*> calibHitContainers(nCont,0);
  for (unsigned int i=0; i<nCont; i++) {
    ATH_MSG_DEBUG("Retrieving " << m_calibHitContainerNames[i]);
    ATH_CHECK( evtStore()->retrieve(calibHitContainers[i], m_calibHitContainerNames[i].c_str()) );
    ATH_MSG_DEBUG("  Retrieved container " << calibHitContainers[i]->Name() << " with size " << calibHitContainers[i]->Size() );
    if( calibHitContainers[i]->Size() == 0 ) {
      ATH_MSG_DEBUG("Container " << calibHitContainers[i]->Name() << " is empty");
    }
  }  
  ATH_MSG_DEBUG("CaloCalibrationHitContainers retrieved successfuly" );

  //count
  m_nchan=0;
  int em_nchan=0;
  int hec_nchan=0;
  int fcal_nchan=0;
  int tile_nchan=0; 
  int unknown_nchan = 0 ;
  
  std::vector<Identifier> ID;
  if (barcode<0) barcode = m_singleParticleBarcode; // if no barcode is specified for this event, use the default
  
  //clean up CaloCell vectors for this event 
  m_CellsEtot.clear();
  m_CellsEvis.clear();
  m_CellsEem.clear();
  	    
  int nhitsInactive = 0;

  for (unsigned int i=0; i<calibHitContainers.size(); i++) {
    for( const auto calibhit: *(calibHitContainers[i])) {
      //care only for deposits of the given truth particle
      if ((int)calibhit->particleID()!=barcode) continue;

      double Etot   = calibhit->energyTotal();
      double Eem    = calibhit->energy(0);
      double Enonem = calibhit->energy(1);
      double Evis   = Eem + Enonem;
      
      Identifier id=calibhit->cellID();

      //merge inactive and active hits from the same cell together
      if (i>0) { 
	//find if have already created cell..
	bool isNewId = true;
	for (int n=0; n<nhitsInactive; n++) {
	  if( id == ID[n] ) { //found
	    m_CellsEtot[n]->addEnergy(Etot);
	    m_CellsEvis[n]->addEnergy(Evis);
	    m_CellsEem[n]->addEnergy(Eem);
	    isNewId = false;
	    break;
	  }
	}
	if(!isNewId) continue; //go to next hit, else create new cell for this hit
      }

      //check if this ID is LAr or Tile
      if(m_caloCell_ID->is_lar(id)) {
	ATH_MSG_VERBOSE( "Found LAr cell" );	
	const CaloDetDescrElement* caloDDE = m_caloDDMgr->get_element(id);	  
	m_CellsEtot.push_back(new LArCell(caloDDE, id, Etot, 0., 0, 0, (CaloGain::CaloGain)m_caloGain)) ;
	m_CellsEvis.push_back(new LArCell(caloDDE, id, Evis, 0., 0, 0, (CaloGain::CaloGain)m_caloGain));
	m_CellsEem.push_back(new LArCell(caloDDE, id, Eem, 0., 0, 0, (CaloGain::CaloGain)m_caloGain)); 
	ID.push_back(id);
	++m_nchan;
      }
      else if(m_caloCell_ID->is_tile(id)) {
	ATH_MSG_VERBOSE( "Found Tile cell" );
	const CaloDetDescrElement* caloDDE = m_caloDDMgr->get_element(id);
	m_CellsEtot.push_back(new TileCell(caloDDE, id, Etot, 0., 0, 0, (CaloGain::CaloGain)m_caloGain)) ;
	m_CellsEvis.push_back(new TileCell(caloDDE, id, Evis, 0., 0, 0, (CaloGain::CaloGain)m_caloGain));
	m_CellsEem.push_back(new TileCell(caloDDE, id, Eem, 0., 0, 0, (CaloGain::CaloGain)m_caloGain)); 
	ID.push_back(id);
	++m_nchan;
      }
      else { //other, DeadMaterial
	//// FIXME DeadMaterial deposits not used; need to use m_caloDDMg;	
	ATH_MSG_VERBOSE( "Found unknown cell" );
	continue;
      }
    }    
    if (i==0) nhitsInactive = (int)ID.size();
  }
  
  //Now, put cells in the containers keeping the order. First goes EM, then HEC and so on
  // if(m_CellsEtot.size()==0) {
  //   ID.clear();
  //   return StatusCode::SUCCESS;
  // }

  ATH_MSG_DEBUG("N cells : " << m_nchan );
  
  for(int itr=0; itr!=m_nchan; itr++) {
    if(m_caloCell_ID->is_em(m_CellsEtot[itr]->ID())) {
      m_truthCells[CalibHitUtils::EnergyTotal]->push_back(m_CellsEtot[itr]);
      m_truthCells[CalibHitUtils::EnergyVisible]->push_back(m_CellsEvis[itr]);
      m_truthCells[CalibHitUtils::EnergyEM]->push_back(m_CellsEem[itr]);
      ++em_nchan;
    }
  }
  if(em_nchan) {
    for (int i=0;i<CalibHitUtils::nEnergyTypes;i++) m_truthCells[i]->setHasCalo(CaloCell_ID::LAREM);
  }

  for(int itr=0; itr!=m_nchan; itr++)  {
    if(m_caloCell_ID->is_hec(m_CellsEtot[itr]->ID())) {
      m_truthCells[CalibHitUtils::EnergyTotal]->push_back(m_CellsEtot[itr]);
      m_truthCells[CalibHitUtils::EnergyVisible]->push_back(m_CellsEvis[itr]);
      m_truthCells[CalibHitUtils::EnergyEM]->push_back(m_CellsEem[itr]);
      ++hec_nchan;
    }
  }
  if(hec_nchan){
    for (int i=0;i<CalibHitUtils::nEnergyTypes;i++) m_truthCells[i]->setHasCalo(CaloCell_ID::LARHEC);
  }

  for(int itr=0; itr!=m_nchan; itr++) {
    if(m_caloCell_ID->is_fcal(m_CellsEtot[itr]->ID())) {
      m_truthCells[CalibHitUtils::EnergyTotal]->push_back(m_CellsEtot[itr]);
      m_truthCells[CalibHitUtils::EnergyVisible]->push_back(m_CellsEvis[itr]);
      m_truthCells[CalibHitUtils::EnergyEM]->push_back(m_CellsEem[itr]);
      ++fcal_nchan;
    }
  }
  if(fcal_nchan) {
    for (int i=0;i<CalibHitUtils::nEnergyTypes;i++) m_truthCells[i]->setHasCalo(CaloCell_ID::LARFCAL);
  }

  for(int itr=0; itr!=m_nchan; itr++) {
    if((m_caloCell_ID->is_tile(m_CellsEtot[itr]->ID()))) {
      m_truthCells[CalibHitUtils::EnergyTotal]->push_back(m_CellsEtot[itr]);
      m_truthCells[CalibHitUtils::EnergyVisible]->push_back(m_CellsEvis[itr]);
      m_truthCells[CalibHitUtils::EnergyEM]->push_back(m_CellsEem[itr]);
      ++tile_nchan;
    }
  }
  if(tile_nchan) {
    for (int i=0;i<CalibHitUtils::nEnergyTypes;i++) m_truthCells[i]->setHasCalo(CaloCell_ID::TILE);
  }
  if(unknown_nchan) {
    for (int i=0;i<CalibHitUtils::nEnergyTypes;i++) m_truthCells[i]->setHasCalo(CaloCell_ID::NOT_VALID);
  }
  ATH_MSG_DEBUG("--- LAr INFO --- "<<m_nchan );
  ATH_MSG_DEBUG("LArCells  = "<<m_nchan );
  ATH_MSG_DEBUG("EMCells   = "<<em_nchan );
  ATH_MSG_DEBUG("HECCells  = "<<hec_nchan );
  ATH_MSG_DEBUG("FCALCells = "<<fcal_nchan );
  ATH_MSG_DEBUG("TileCells = "<<tile_nchan );
  ATH_MSG_DEBUG("NOT_VALID = "<<unknown_nchan );
    
  ID.clear();

  ///..........................................................

  ATH_MSG_DEBUG("making truth cluster");
  xAOD::CaloCluster* truthCluster[3] = {0,0,0};
  for (int i=0;i<CalibHitUtils::nEnergyTypes;i++) {
    truthCluster[i] = CaloClusterStoreHelper::makeCluster(m_truthClusters[i],m_truthCells[i]);
    if (!truthCluster[i]) {
      ATH_MSG_FATAL("makeCluster failed");
      return StatusCode::FAILURE;
    }
    for (const auto cell: *m_truthCells[i]) {
      if(m_caloCell_ID->is_lar(cell->ID()) || m_caloCell_ID->is_tile(cell->ID())) 
	truthCluster[i]->addCell( m_truthCells[i]->findIndex(cell->caloDDE()->calo_hash()) , 1.);
    }
    
    truthCluster[i]->setClusterSize(xAOD::CaloCluster::CSize_Unknown);
    CaloClusterKineHelper::calculateKine(truthCluster[i], true, true);
    ATH_MSG_INFO("Created truth cluster with " << EnergyTypeToStr[i] <<" " << truthCluster[i]->e());
  }

  // record containers
  if (m_writeTruthCellContainers) {
    for (int i=0;i<3;i++) {
      ATH_MSG_DEBUG("recording truth cells container for " << EnergyTypeToStr[i] <<" deposits, size: " << m_truthCells[i]->size());
      ATH_CHECK(evtStore()->record(m_truthCells[i], (m_outputCellContainerName+EnergyTypeToStr[i]).c_str()));
    }
  }
  if (m_writeTruthClusterContainers) {
    for (int i=0;i<3;i++) {
      ATH_MSG_DEBUG("finalizing truth cluster");
      ATH_CHECK( CaloClusterStoreHelper::finalizeClusters(&*evtStore(), m_truthClusters[i], (m_outputClusterContainerName+EnergyTypeToStr[i]).c_str(), msg()) );
    }
  }
  
  ATH_MSG_DEBUG("execute() completed successfully" );
  return StatusCode::SUCCESS;
}


/////////////////   FINALIZE   //////////////////////
StatusCode CalibHitToCaloCellTool::finalize()
{
  ATH_MSG_INFO("finalize() successfully" );
  return StatusCode::SUCCESS;
}

