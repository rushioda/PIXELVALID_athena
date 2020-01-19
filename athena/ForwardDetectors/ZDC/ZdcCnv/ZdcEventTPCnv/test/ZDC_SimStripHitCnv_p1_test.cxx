/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ZdcEventTPCnv/test/ZDC_SimStripHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ZDC_SimStripHitCnv_p1.
 */


#undef NDEBUG
#include "ZdcEventTPCnv/ZDC_SimStripHitCnv_p1.h"
//#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void compare (const ZDC_SimStripHit& p1,
              const ZDC_SimStripHit& p2)
{
  assert (p1.GetSide() == p2.GetSide());
  assert (p1.GetMod() == p2.GetMod());
  assert (p1.GetEdep() == p2.GetEdep());
  assert (p1.GetNPhotons() == p2.GetNPhotons());
}


void testit (const ZDC_SimStripHit& trans1)
{
  MsgStream log (0, "test");
  ZDC_SimStripHitCnv_p1 cnv;
  ZDC_SimStripHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ZDC_SimStripHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  //Athena_test::Leakcheck check;

  ZDC_SimStripHit trans1 (123, 234, 21, 12345.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
