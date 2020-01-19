/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigParticleTPCnv/test/TrigPhotonCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigPhotonCnv_p3.
 */


#undef NDEBUG
#include "TrigParticleTPCnv/TrigPhotonCnv_p3.h"
#include "SGTools/TestStore.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const P4PtEtaPhiM& p1,
              const P4PtEtaPhiM& p2)
{
  assert (Athena_test::isEqual (p1.pt(), p2.pt()));
  assert (Athena_test::isEqual (p1.eta(), p2.eta()));
  assert (Athena_test::isEqual (p1.phi(), p2.phi()));
  assert (Athena_test::isEqual (p1.m(), p2.m()));
}


void compare (const TrigPhoton& p1,
              const TrigPhoton& p2)
{
  compare (static_cast<const P4PtEtaPhiM&>(p1),
           static_cast<const P4PtEtaPhiM&>(p2));

  assert (p1.roiId() == p2.roiId());
  assert (p1.isValid() == p2.isValid());
  assert (p1.HadEt() == p2.HadEt());
  assert (p1.energyRatio() == p2.energyRatio());
  assert (p1.rCore() == p2.rCore());
  assert (p1.dPhi() == p2.dPhi());
  assert (p1.dEta() == p2.dEta());
  assert (p1.F0() == p2.F0());
  assert (p1.F1() == p2.F1());
  assert (p1.F2() == p2.F2());
  assert (p1.F3() == p2.F3());
  assert (p1.Fside() == p2.Fside());
  assert (p1.Weta2() == p2.Weta2());
  assert (p1.clusterLink() == p2.clusterLink());
}


void testit (const TrigPhoton& trans1)
{
  MsgStream log (0, "test");
  TrigPhotonCnv_p3 cnv;
  TrigPhoton_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigPhoton trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigPhoton trans1 (80000, 1.5, 2.5,
                     123, //roi
                     true,
                     ElementLink< TrigEMClusterContainer > ("foo", 10),
                     6.5, 7.5, 8.5, 9.5, 10.5, 11.5, 12.5, 13.5, 14.5,
                     15.5, 16.5);
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
