/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  LArRecUtils/src/LArFCalTowerBuilderToolTestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression test for LArFCalTowerBuilderTool.
 */

#undef NDEBUG

#include "LArFCalTowerBuilderToolTestAlg.h"
#include "LArRecUtils/LArFCalTowerBuilderTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "boost/foreach.hpp"
#include <cstdlib>
#include <iostream>
#include <cmath>


using CLHEP::GeV;


namespace {


// Dufus-quality RNG, using LCG.  Constants from numerical recipies.
// I don't particularly care about RNG quality here, just about
// getting something that's reproducible.
#include <stdint.h>
uint32_t seed = 1;
uint32_t rngmax = static_cast<uint32_t> (-1);
uint32_t rng()
{
  seed = (1664525*seed + 1013904223);
  return seed;
}

float randf (float rmax, float rmin = 0)
{
  return static_cast<double>(rng()) / rngmax * (rmax-rmin) + rmin;
}
int randi (int rmax, int rmin = 0)
{
  return static_cast<int> (randf(rmax, rmin));
}

struct RNG
{
  int operator() (int n) const { return randi (n); }
};
//RNG stlrand;


bool comp (double x1, double x2, double thresh = 1e-6)
{
  double den = std::abs(x1) + std::abs(x2);
  if (den == 0) return true;
  double num = std::abs (x1 - x2);
  return num / den < thresh;
}


} // anonymous namespace


/**
 * @brief Constructor.
 */
LArFCalTowerBuilderToolTestAlg::LArFCalTowerBuilderToolTestAlg
  (const std::string& name,
   ISvcLocator* pSvcLocator)
    : AthAlgorithm (name, pSvcLocator),
      m_builder ("LArFCalTowerBuilderTool")
{
}


/** 
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArFCalTowerBuilderToolTestAlg::initialize()
{
  m_calos.push_back (CaloCell_ID::LARFCAL);

  CHECK( m_builder.retrieve() );

  m_seg = CaloTowerSeg (100, 64, -5.0, 5.0);
  m_builder->setTowerSeg (m_seg);
  return StatusCode::SUCCESS;
}


CaloCellContainer*
LArFCalTowerBuilderToolTestAlg::make_cells()
{
  CaloCellContainer* cells = new CaloCellContainer;
  const CaloDetDescrManager* ddman = 0;
  if ( detStore()->retrieve (ddman, "CaloMgr").isFailure() )
    std::abort();
  BOOST_FOREACH (CaloCell_ID::SUBCALO subcalo, m_calos) {
    BOOST_FOREACH (const CaloDetDescrElement* dde,
                   ddman->element_range (subcalo))
    {
      float energy = randf (100*GeV);
      cells->push_back (new CaloCell (dde, energy, 0, 0, 0, 
                                      CaloGain::LARMEDIUMGAIN) );
    }
  }
  cells->updateCaloIterators();
  return cells;
}


StatusCode
LArFCalTowerBuilderToolTestAlg::test_subseg (const CaloTowerSeg::SubSeg& subseg,
                                             const CaloCellContainer* cells,
                                             const CaloTowerContainer* tow0)
{
  CaloTowerContainer* tow = new CaloTowerContainer (subseg.segmentation());
  CHECK( m_builder->execute (tow, cells, &subseg) );

  for (size_t i = 0; i < tow->size(); i++) {
    typedef CaloTowerContainer::index_t index_t;
    index_t ieta1 = tow->getTowerEtaIndex (i);
    index_t iphi1 = tow->getTowerPhiIndex (i);
    index_t ieta0 = ieta1 + subseg.etamin()-1;
    index_t iphi0 = iphi1 + subseg.phimin()-1;
    if (iphi0 > m_seg.nphi())
      iphi0 -= m_seg.nphi();
    const CaloTower* t0 = tow0->getTower (ieta0, iphi0);
    const CaloTower* t1 = tow->getTower (i);
    if (!comp (t0->energy(), t1->energy()) ||
        !comp (t0->eta(), t1->eta()) ||
        !comp (t0->phi(), t1->phi()))
    {
      std::cout << "Energy mismatch " << i << " "
                << ieta1 << " " << iphi1 << " "
                << t0->eta() << " " << t0->phi() << " "
                << t1->eta() << " " << t1->phi() << " "
                << t0->energy() << " " << t1->energy() << "\n";
    }

    std::vector<unsigned int> h0;
    for (CaloTower::cell_iterator cit = t0->cell_begin();
         cit != t0->cell_end();
         ++cit)
    {
      h0.push_back ((*cit)->caloDDE()->calo_hash());
    }

    std::vector<unsigned int> h1;
    for (CaloTower::cell_iterator cit = t1->cell_begin();
         cit != t1->cell_end();
         ++cit)
    {
      h1.push_back ((*cit)->caloDDE()->calo_hash());
    }

    if (h0 != h1) {
      std::cout << "Cell list mismatch.\n";
      for (size_t i = 0; i < h0.size(); i++)
        std::cout << " " << h0[i];
      std::cout << "\n";
      for (size_t i = 0; i < h1.size(); i++)
        std::cout << " " << h1[i];
      std::cout << "\n";
    }
  }

  delete tow;
  
  return StatusCode::SUCCESS;
}


StatusCode LArFCalTowerBuilderToolTestAlg::test1()
{
  std::cout << "test1\n";

  const CaloCellContainer* cells = make_cells();
  CaloTowerContainer* tow1 = new CaloTowerContainer (m_seg);
  CHECK( m_builder->execute (tow1, cells) );

#if 0
  std::cout << "cells\n";
  for (size_t i = 0; i < cells->size(); i++) {
    const CaloCell* c = (*cells)[i];
    std::cout << i << " "
              << (int)(c->caloDDE()->calo_hash()) << " "
              << c->eta() << " " << c->phi() << " " << c->energy() << "\n";
  }
#endif

  std::cout << "towers\n";
  for (size_t i = 0; i < tow1->size(); i++) {
    const CaloTower* t = (*tow1)[i];
    std::cout << i << " "
              << tow1->getTowerEtaIndex(i) << " "
              << tow1->getTowerPhiIndex(i) << " "
              << t->energy() << " ";
    for (CaloTower::cell_iterator cit = t->cell_begin();
         cit != t->cell_end();
         ++cit)
    {
      std::cout << (*cit)->caloDDE()->calo_hash() << " ";
    }
    std::cout << "\n";
  }

  CHECK( test_subseg (m_seg.subseg ( 4.5, 0.3, -0.2, 0.4), cells, tow1) );
  CHECK( test_subseg (m_seg.subseg (-4.5, 0.3,  3.1, 0.4), cells, tow1) );

  delete cells;
  delete tow1;
  return StatusCode::SUCCESS;
}


/** 
 * @brief Standard Gaudi execute method.
 */
StatusCode LArFCalTowerBuilderToolTestAlg::execute()
{
  CHECK( test1() );
  return StatusCode::SUCCESS;
}
