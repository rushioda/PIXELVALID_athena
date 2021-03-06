/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TileTPCnv/test/TileDigitsCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileDigitsCnv_p2.
 */


#undef NDEBUG
#include "TileTPCnv/TileDigitsCnv_p2.h"
#include "TileConditions/TileCablingService.h"
#include "TileIdentifier/TileHWID.h"
#include "IdDictParser/IdDictParser.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


TileHWID hwid;
TileTBID tbid;
TileID   tileid;

class TileCablingSvc
{
public:
  static
  void init_idhelpers()
  {
    tileid.set_do_neighbours (false);
    IdDictParser parser;
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    assert (hwid.initialize_from_dictionary (idd) == 0);
    assert (tbid.initialize_from_dictionary (idd) == 0);
    assert (tileid.initialize_from_dictionary (idd) == 0);
    TileCablingService* svc = TileCablingService::getInstance();
    svc->setTileHWID (&hwid);
    svc->setTileTBID (&tbid);
    svc->setTileID (&tileid);
  }
};


void compare (const TileDigits& p1,
              const TileDigits& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.samples() == p2.samples());
}


void testit (const TileDigits& trans1)
{
  MsgStream log (0, "test");
  TileDigitsCnv_p2 cnv;
  TileDigits_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileDigits trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  std::vector<float> digits {10., 200., 5000., 32000., -1.};
  TileDigits trans1 (hwid.adc_id (1, 2, 3, 0), digits);

  testit (trans1);
}


int main()
{
  TileCablingSvc::init_idhelpers();
  test1();
  return 0;
}
