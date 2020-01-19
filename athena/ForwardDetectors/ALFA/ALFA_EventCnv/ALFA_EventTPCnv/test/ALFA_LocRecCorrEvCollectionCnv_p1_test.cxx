/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EvCollectionTPCnv/test/ALFA_LocRecCorrEvCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_LocRecCorrEvCollectionCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvCollectionCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_LocRecCorrEvent& p1,
              const ALFA_LocRecCorrEvent& p2)
{
  assert (p1.getAlgoNum() == p2.getAlgoNum());
  assert (p1.getPotNum() == p2.getPotNum());
  assert (p1.getXpositionLHC() == p2.getXpositionLHC());
  assert (p1.getYpositionLHC() == p2.getYpositionLHC());
  assert (p1.getZpositionLHC() == p2.getZpositionLHC());
  assert (p1.getXpositionPot() == p2.getXpositionPot());
  assert (p1.getYpositionPot() == p2.getYpositionPot());
  assert (p1.getXpositionStat() == p2.getXpositionStat());
  assert (p1.getYpositionStat() == p2.getYpositionStat());
  assert (p1.getXpositionBeam() == p2.getXpositionBeam());
  assert (p1.getYpositionBeam() == p2.getYpositionBeam());
}


void compare (const ALFA_LocRecCorrEvCollection& p1,
              const ALFA_LocRecCorrEvCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const ALFA_LocRecCorrEvCollection& trans1)
{
  MsgStream log (0, "test");
  ALFA_LocRecCorrEvCollectionCnv_p1 cnv;
  ALFA_LocRecCorrEvCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_LocRecCorrEvCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_LocRecCorrEvCollection trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new ALFA_LocRecCorrEvent
                      (123+o, 234+o,
                       10.5+o, 11.5+o, 12.5+o,
                       13.5+o, 14.5+o,
                       15.5+o, 16.5+o,
                       17.5+o, 18.5+o));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
