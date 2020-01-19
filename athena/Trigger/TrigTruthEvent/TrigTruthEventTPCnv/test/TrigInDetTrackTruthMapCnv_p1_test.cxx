/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTruthEventTPCnv/test/TrigInDetTrackTruthMapCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for TrigInDetTrackTruthMapCnv_p1.
 */


#undef NDEBUG
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp2.h"
#include "SGTools/TestStore.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigIDHitStats& p1,
              const TrigIDHitStats& p2)
{
  assert (p1.pixhits() == p2.pixhits());
  assert (p1.scthits() == p2.scthits());
  assert (p1.trthits() == p2.trthits());
}


class TrigInDetTrackTruthCnv_p1_test
{
public:
  static void compare (const TrigInDetTrackTruth& p1,
                       const TrigInDetTrackTruth& p2)
  {
    assert (p1.m_best_match_hits == p2.m_best_match_hits);
    assert (p1.m_best_Si_match_hits == p2.m_best_Si_match_hits);
    assert (p1.m_best_TRT_match_hits == p2.m_best_TRT_match_hits);
    assert (p1.m_true_part_vec == p2.m_true_part_vec);
    assert (p1.m_nr_common_hits.size() == p2.m_nr_common_hits.size());
    for (size_t i = 0; i < p1.m_nr_common_hits.size(); i++)
      ::compare (p1.m_nr_common_hits[i], p2.m_nr_common_hits[i]);
    assert (p1.m_family_tree == p2.m_family_tree);
    
  }

  static void set (TrigInDetTrackTruth& p, int ndx)
  {
    int o = ndx*10;
    p.m_true_part_vec.emplace_back (1001, 10+o);
    p.m_true_part_vec.emplace_back (1002, 11+o);
    p.m_true_part_vec.emplace_back (1003, 12+o);

    int nstat = 4;
    p.m_nr_common_hits.resize (nstat);
    for (int i=0; i < nstat; i++) {
      p.m_nr_common_hits[i][TrigIDHitStats::PIX] = 12 + i*10 + o*10;
      p.m_nr_common_hits[i][TrigIDHitStats::SCT] = 13 + i*10 + o*10;
      p.m_nr_common_hits[i][TrigIDHitStats::TRT] = 14 + i*10 + o*10;
    }

    p.m_family_tree.emplace_back (0, 1);
    p.m_family_tree.emplace_back (0, 2);

    p.m_best_match_hits = 0;
    p.m_best_Si_match_hits = 1;
    p.m_best_TRT_match_hits = 2;
  }
};


void compare (const TrigInDetTrackTruth& p1,
              const TrigInDetTrackTruth& p2)
{
  TrigInDetTrackTruthCnv_p1_test::compare (p1, p2);
}


void compare (const TrigInDetTrackTruthMap& p1,
              const TrigInDetTrackTruthMap& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1.truthi(i), *p2.truthi(i));
    assert (p1.trackiLink(i) == p2.trackiLink(i));
  }
}


void testit (const TrigInDetTrackTruthMap& trans1)
{
  MsgStream log (0, "test");
  TrigInDetTrackTruthMapCnv_p1 cnv;
  TrigInDetTrackTruthMapCnv_tlp2 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigInDetTrackTruthMap_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigInDetTrackTruthMap trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigInDetTrackTruthMap trans1;
  //TrigInDetTrackTruthMapCnv_p1_test::set (trans1);

  auto coll = CxxUtils::make_unique<TrigInDetTrackCollection>();
  for (int i=0; i<10; i++) {
    int o = i*10;
    auto param = CxxUtils::make_unique<TrigInDetTrackFitPar>
      (2.5+o, 3.5+o, 4.5+o, 5.5+o, 6.5+o, TrigInDetTrackFitPar::PERIGEE, 7.5+o);
    auto trk = CxxUtils::make_unique<TrigInDetTrack> (param.release());
    trk->algorithmId (static_cast<TrigInDetTrack::AlgoId>(i+1));
    coll->push_back (std::move (trk));
  }
  const TrigInDetTrackCollection* collp = coll.get();
  SGTest::store.record (coll.release(), "coll");

  TrigInDetTrackTruth t1;
  TrigInDetTrackTruthCnv_p1_test::set (t1, 1);
  trans1.addMatch (collp, 1, t1);

  TrigInDetTrackTruth t2;
  TrigInDetTrackTruthCnv_p1_test::set (t2, 2);
  trans1.addMatch (collp, 2, t2);

  TrigInDetTrackTruth t3;
  TrigInDetTrackTruthCnv_p1_test::set (t3, 3);
  trans1.addMatch (collp, 3, t3);
  
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
