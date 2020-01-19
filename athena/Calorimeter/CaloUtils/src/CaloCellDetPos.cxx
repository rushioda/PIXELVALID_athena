/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// for a given calorimeter layer, this class will convert detector eta,phi to Atlas eta,phi
//  and vice versa, taking into account calorimeter alignment
//  this assumes that the shift is the one of the cell to which the eta-phi position belongs
//  the methods return true if an element of the correct layer is found within the cell, false otherwise

#include "CaloUtils/CaloCellDetPos.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

CaloPhiRange CaloCellDetPos::s_range;

CaloCellDetPos::CaloCellDetPos()
{}

CaloCellDetPos::~CaloCellDetPos()
{}

bool CaloCellDetPos::getDetPosition(CaloCell_ID::CaloSample sam, double etaAtlas, double phiAtlas, double& etaDet, double& phiDet)
{

  const CaloDetDescrManager* calo_mgr;
  calo_mgr = CaloDetDescrManager::instance();

  const CaloDetDescrElement* elt = calo_mgr->get_element(sam,etaAtlas,phiAtlas);
  if (!elt) {
    etaDet = etaAtlas;
    phiDet = phiAtlas;
    return false;
  }
  etaDet = etaAtlas + elt->eta_raw()-elt->eta();
  phiDet = s_range.fix(phiAtlas + elt->phi_raw()-elt->phi());
  return true;

}

bool CaloCellDetPos::getAtlasPosition(CaloCell_ID::CaloSample sam, double etaDet, double phiDet, double& etaAtlas, double& phiAtlas)
{

  const CaloDetDescrManager* calo_mgr;
  calo_mgr = CaloDetDescrManager::instance();

  const CaloDetDescrElement* elt = calo_mgr->get_element_raw(sam,etaDet,phiDet);
  if (!elt) {
    etaAtlas = etaDet;
    phiAtlas = phiDet;
    return false;
  }
  etaAtlas = etaDet + elt->eta()-elt->eta_raw();
  phiAtlas = s_range.fix(phiDet + elt->phi()-elt->phi_raw());
  return true;

}
