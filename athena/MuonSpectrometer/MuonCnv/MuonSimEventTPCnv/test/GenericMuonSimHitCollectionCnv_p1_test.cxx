/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonSimEventTPCnv/test/GenericMuonSimHitCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for GenericMuonSimHitCollectionCnv_p1.
 */


#undef NDEBUG
#include "MuonSimEventTPCnv/GenericMuonSimHitCollectionCnv_p1.h"
#include <cassert>
#include <iostream>

#include "GeneratorObjectsTPCnv/initMcEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"


void compare (const HepMcParticleLink& p1,
              const HepMcParticleLink& p2)
{
  assert ( p1.isValid() == p2.isValid() );
  assert ( p1.barcode() == p2.barcode() );
  assert ( p1.eventIndex() == p2.eventIndex() );
  assert ( p1.cptr() == p2.cptr() );
  assert ( p1 == p2 );
}

void compare (const GenericMuonSimHit& p1,
              const GenericMuonSimHit& p2)
{
  assert (p1.GenericId() == p2.GenericId());
  assert (p1.globalTime() == p2.globalTime());
  assert (p1.globalpreTime() == p2.globalpreTime());
  assert (p1.globalPosition() == p2.globalPosition());
  assert (p1.localPosition() == p2.localPosition());
  assert (p1.globalPrePosition() == p2.globalPrePosition());
  assert (p1.localPrePosition() == p2.localPrePosition());
  assert (p1.particleEncoding() == p2.particleEncoding());
  assert (p1.kineticEnergy() == p2.kineticEnergy());
  assert (p1.globalDirection() == p2.globalDirection());
  assert (p1.depositEnergy() == p2.depositEnergy());
  assert (p1.StepLength() == p2.StepLength());
  compare(p1.particleLink(), p2.particleLink());
  assert (p1.particleLink() == p2.particleLink());
}


void compare (const GenericMuonSimHitCollection& p1,
              const GenericMuonSimHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const GenericMuonSimHitCollection& trans1)
{
  MsgStream log (0, "test");
  GenericMuonSimHitCollectionCnv_p1 cnv;
  Muon::GenericMuonSimHitCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  GenericMuonSimHitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1(std::vector<HepMC::GenParticle*> genPartVector)
{
  std::cout << "test1\n";

  GenericMuonSimHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    const HepMC::GenParticle* pGenParticle = genPartVector.at(i);
    trans1.Emplace (123, 10.5, 11.5,
                    Amg::Vector3D (12.5, 13.5, 14.5),
                    Amg::Vector3D (15.5, 16.5, 17.5),
                    Amg::Vector3D (18.5, 19.5, 20.5),
                    Amg::Vector3D (21.5, 22.5, 23.5),
                    pGenParticle->pdg_id(), 25.5,
                    Amg::Vector3D (26.5, 27.5, 28.5),
                    29.5, 30.5, pGenParticle->barcode()
                    );
  }

  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc = nullptr;
  std::vector<HepMC::GenParticle*> genPartVector;
  if (!Athena_test::initMcEventCollection(pSvcLoc,genPartVector)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1(genPartVector);
  return 0;
}
