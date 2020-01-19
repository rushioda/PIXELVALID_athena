/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonSimEventTPCnv/test/CSCSimHitCollectionCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for CSCSimHitCollectionCnv_p2.
 */


#undef NDEBUG
#include "MuonSimEventTPCnv/CSCSimHitCollectionCnv_p2.h"
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

void compare (const CSCSimHit& p1,
              const CSCSimHit& p2)
{
  assert (p1.CSCid() == p2.CSCid());
  assert (p1.globalTime() == p2.globalTime());
  assert (p1.energyDeposit() == p2.energyDeposit());
  assert (p1.getHitStart() == p2.getHitStart());
  assert (p1.getHitEnd() == p2.getHitEnd());
  assert (p1.particleID() == p2.particleID());
  compare(p1.particleLink(), p2.particleLink());
  assert (p1.particleLink() == p2.particleLink());
  assert (p1.kineticEnergy() == p2.kineticEnergy());
}


void compare (const CSCSimHitCollection& p1,
              const CSCSimHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const CSCSimHitCollection& trans1)
{
  MsgStream log (0, "test");
  CSCSimHitCollectionCnv_p2 cnv;
  Muon::CSCSimHitCollection_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  CSCSimHitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}

int lundCode(int pdgid) {
  if(pdgid==22) { return 1; }
  if(pdgid==11) { return 2; }
  if(pdgid==-11) { return 3; }
  if(pdgid==13) { return 5; }
  if(pdgid==-13) { return 6; }
  return -1;
}

void test1(std::vector<HepMC::GenParticle*>& genPartVector)
{
  std::cout << "test1\n";

  CSCSimHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    const HepMC::GenParticle* pGenParticle = genPartVector.at(i);
    trans1.Emplace (123, 10.5, 11.5,
                    Amg::Vector3D (12.5, 13.5, 14.5),
                    Amg::Vector3D (15.5, 16.5, 17.5),
                    lundCode(pGenParticle->pdg_id()), pGenParticle->barcode(), 20.5
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
