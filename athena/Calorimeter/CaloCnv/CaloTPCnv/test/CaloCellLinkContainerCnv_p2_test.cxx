/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellLinkContainerCnv_p2_test.cxx,v 1.2 2008-12-18 19:34:49 ssnyder Exp $
/* @file CaloCellLinkContainerCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2008
 * @brief Regression tests for CaloCellLinkContainerCnv_p2.
 */

#undef NDEBUG

#include "CaloTPCnv/CaloCellLinkContainerCnv_p2.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <cstdio>

const std::string cont_name = "cells";


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
  return static_cast<float>(rng()) / rngmax * (rmax-rmin) + rmin;
}
int randi (int rmax, int rmin = 0)
{
  return static_cast<int> (randf(rmax, rmin));
}
struct randi_fn
{
  int operator() (int rmax) { return randi(rmax); }
};


struct cell_t
{
  cell_t (int the_cell = 0,
          float the_weight = 0) : cell (the_cell), weight (the_weight) {}
  bool operator< (const cell_t& other) const { return cell < other.cell; }
  bool operator== (const cell_t& other) const { return cell == other.cell; }
  int cell;
  float weight;
};


class CaloCellLinkTest
  : public CaloCellLink
{
public:
  void push (int icell,
             float weight,
             const std::string& name = cont_name);
};


void CaloCellLinkTest::push (int icell, float weight, const std::string& name)
{
  ElementLink<CaloCellContainer> el_link (name, icell);
  getConstituents().push_back (std::make_pair (el_link, weight));
}


CaloCellLink* make_cluslinks (int ncell_min,
                              int ncell_max,
                              int nrange_min,
                              int nrange_max,
                              int nwrange_min,
                              int nwrange_max,
                              float wmin,
                              float wmax)
{
  CaloCellLinkTest* lnk = new CaloCellLinkTest;
  int ncell = randi (ncell_max+1, ncell_min);
  if (ncell == 0) return lnk;

  std::vector<cell_t> cells (ncell);

  int nrange = randi (nrange_max+1, nrange_min);
  if (nrange < 1) nrange = 1;
  std::vector<int> ranges (nrange);
  for (int i = 0; i < nrange-1; i++)
    ranges[i] = randi (ncell);
  std::sort (ranges.begin(), ranges.end()-1);
  ranges.back() = ncell;

  int icell = 0;
  for (int i = 0; i < nrange; i++) {
    int nthis = ranges[i] - icell;
    assert (nthis >= 0);
    int istart = randi (200000);
    for (int i = 0; i < nthis; i++) {
      assert (icell < ncell);
      cells[icell++].cell = istart + i;
    }
  }
  assert (icell == ncell);

  nrange = randi (nwrange_max+1, nwrange_min);
  if (nrange < 1) nrange = 1;
  ranges.resize (nrange);
  for (int i = 0; i < nrange-1; i++)
    ranges[i] = randi (ncell);
  std::sort (ranges.begin(), ranges.end()-1);
  ranges.back() = ncell;

  icell = 0;
  for (int i = 0; i < nrange; i++) {
    int nthis = ranges[i] - icell;
    assert (nthis >= 0);
    float w = randf (wmax, wmin);
    for (int i = 0; i < nthis; i++) {
      assert (icell < ncell);
      cells[icell++].weight = w;
    }
  }
  assert (icell == ncell);

  std::sort (cells.begin(), cells.end());

  int uend = std::unique (cells.begin(), cells.end()) - cells.begin();
  randi_fn fn;
  std::random_shuffle (cells.begin(), cells.begin()+uend, fn);

  for (int i = 0; i < uend; i++) {
    lnk->push (cells[i].cell, cells[i].weight);
  }

  return lnk;
}


void dump_celllink (const CaloCellLink& lnk)
{
  for (CaloCellLink::cell_iterator beg = lnk.begin();
       beg != lnk.end();
       ++beg)
  {
    Navigable<CaloCellContainer,double>::constituent_const_iter citer =
      beg.getInternalIterator();
    std::cout << "  " << citer->first.dataID()
              << " " << citer->first.index() 
              << " " <<  citer->second << "\n";
  }
}


void dump_celllinkcontainer (const CaloCellLinkContainer& c)
{
  for (size_t i = 0; i < c.size(); i++) {
    std::cout << "Cluster " << i << "\n";
    dump_celllink (*c[i]);
  }
}


void dump_pers (const CaloCellLinkContainer_p2& c)
{
  std::cout << "Pers: " << c.m_nClusters << " clusts; container "
            << c.m_contName << "\n";

  std::cout << "  isizes:";
  for (size_t i = 0; i < c.m_vISizes.size(); i++)
    std::cout << " " << c.m_vISizes[i]; 
  std::cout << "\n";

  std::cout << "  wsizes:";
  for (size_t i = 0; i < c.m_vWSizes.size(); i++)
    std::cout << " " << c.m_vWSizes[i]; 
  std::cout << "\n";

  std::cout << "  ilinks:";
  for (size_t i = 0; i < c.m_linkI.size(); i++)
    std::cout << " "
              << (c.m_linkI[i]>>18) << "/" << (c.m_linkI[i]&((1<<18)-1)); 
  std::cout << "\n";

  std::cout << "  wlinks:";
  for (size_t i = 0; i < c.m_linkW.size(); i++)
    std::cout << " " << c.m_linkW[i]; 
  std::cout << "\n";
}


std::vector<cell_t> celllink_to_vec (const CaloCellLink& lnk)
{
  std::vector<cell_t> vec;
  vec.reserve (lnk.size());

  for (CaloCellLink::cell_iterator beg = lnk.begin();
       beg != lnk.end();
       ++beg)
  {
    Navigable<CaloCellContainer,double>::constituent_const_iter citer =
      beg.getInternalIterator();
    assert (citer->first.dataID() == cont_name);
    vec.push_back (cell_t (citer->first.index(), citer->second));
  }
  return vec;
}


void compare_float (float x1, float x2, const char* what, float thresh=0.01)
{
  float den = std::abs(x1) + std::abs(x2);
  if (den == 0) den = 1;
  if (std::abs(x1-x2) / den > thresh) {
    printf ("Float %s miscompare: %f %f\n", what, x1, x2);
    std::abort();
  }
}


void compare (const CaloCellLinkContainer& c1,
              const CaloCellLinkContainer& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    std::vector<cell_t> v1 = celllink_to_vec (*c1[i]);
    std::vector<cell_t> v2 = celllink_to_vec (*c2[i]);
    assert (v1.size() == v2.size());
    std::sort (v1.begin(), v1.end());
    std::sort (v2.begin(), v2.end());
    for (size_t j = 0; j < v1.size(); j++) {
      assert (v1[j].cell == v2[j].cell);
      compare_float (v1[j].weight, v2[j].weight, "weight");
    }
  }
}


void runtest  (int nclus_max,
               int ncell_min,
               int ncell_max,
               int nrange_min,
               int nrange_max,
               int nwrange_min,
               int nwrange_max,
               float wmin,
               float wmax,
               bool print = true)
{
  MsgStream log (0, "test");

  CaloCellLinkContainerCnv_p2 cnv;
  CaloCellLinkContainer_p2 pers;

  CaloCellLinkContainer c1;
  CaloCellLinkContainer c1out;
  int nclus = randi (nclus_max);
  for (int i = 0; i < nclus; i++)
    c1.push_back (make_cluslinks (ncell_min, ncell_max,
                                  nrange_min, nrange_max,
                                  nwrange_min, nwrange_max,
                                  wmin, wmax));
  if (print)
    dump_celllinkcontainer (c1);
  cnv.transToPers (&c1, &pers, log);
  if (print)
    dump_pers (pers);
  cnv.persToTrans (&pers, &c1out, log);
  if (print)
    dump_celllinkcontainer (c1out);
  compare (c1, c1out);
}



void test_pack_errors()
{
  MsgStream log (0, "test");
  CaloCellLinkContainerCnv_p2 cnv;

  {
    std::cout << "test_pack_errors1\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    CaloCellLinkTest* lnk = new CaloCellLinkTest;
    lnk->push (0, 1, "cells1");
    lnk->push (1, 1, "cells2");
    lnk->push (2, 1, "cells1");
    c1.push_back (lnk);
    cnv.transToPers (&c1, &p1, log);
    dump_pers (p1);
  }

  {
    std::cout << "test_pack_errors2\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    CaloCellLinkTest* lnk = new CaloCellLinkTest;
    lnk->push (0, 1);
    lnk->push (1, 1e10);
    lnk->push (2, 1);
    c1.push_back (lnk);
    cnv.transToPers (&c1, &p1, log);
    dump_pers (p1);
  }

  {
    std::cout << "test_pack_errors3\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    CaloCellLinkTest* lnk = new CaloCellLinkTest;
    lnk->push (0, 1);
    lnk->push (500000, 1);
    lnk->push (2, 1);
    c1.push_back (lnk);
    cnv.transToPers (&c1, &p1, log);
    dump_pers (p1);
  }
}


void test_unpack_errors()
{
  MsgStream log (0, "test");
  CaloCellLinkContainerCnv_p2 cnv;

  {
    std::cout << "test_unpack_errors1\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    p1.m_contName = "cellx";
    p1.m_nClusters = 1;
    cnv.persToTrans (&p1, &c1, log);
  }

  {
    std::cout << "test_unpack_errors2\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    p1.m_contName = "cellx";
    p1.m_nClusters = 1;
    p1.m_vISizes.push_back (1);
    p1.m_vWSizes.push_back (1);
    cnv.persToTrans (&p1, &c1, log);
  }

  {
    std::cout << "test_unpack_errors3\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    p1.m_contName = "cellx";
    p1.m_nClusters = 1;
    p1.m_vISizes.push_back (1);
    p1.m_vWSizes.push_back (1);
    p1.m_linkI.push_back ((1<<18) + 1);
    cnv.persToTrans (&p1, &c1, log);
  }

  {
    std::cout << "test_unpack_errors4\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    p1.m_contName = "cellx";
    p1.m_nClusters = 1;
    p1.m_vISizes.push_back (1);
    p1.m_vWSizes.push_back (1);
    p1.m_linkI.push_back ((1<<18) + 1);
    p1.m_linkW.push_back (2001);
    cnv.persToTrans (&p1, &c1, log);
  }

  {
    std::cout << "test_unpack_errors5\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    p1.m_contName = "cellx";
    p1.m_nClusters = 1;
    p1.m_vISizes.push_back (1);
    p1.m_vWSizes.push_back (2);
    p1.m_linkI.push_back ((1<<18) + 1);
    p1.m_linkW.push_back (1001);
    p1.m_linkW.push_back (1002);
    cnv.persToTrans (&p1, &c1, log);
  }

  {
    std::cout << "test_unpack_errors6\n";
    CaloCellLinkContainer c1;
    CaloCellLinkContainer_p2 p1;
    p1.m_nClusters = 1;
    p1.m_vISizes.push_back (1);
    p1.m_vWSizes.push_back (2);
    p1.m_linkI.push_back ((1<<18) + 1);
    p1.m_linkW.push_back (1001);
    cnv.persToTrans (&p1, &c1, log);
  }
}


void tests()
{
  std::cout << "test1\n";
  runtest (10, 100, 100, 4, 4, 4, 4, 0.5, 2);
  std::cout << "test2\n";
  runtest (10, 100, 100, 1, 1, 1, 1, 1, 1);
  for (int i = 0; i < 10; i++) {
    std::cout << "test3 " << i << "\n";
    runtest (10, 100, 10000, 1, 50, 1, 20, 0.5, 2, false);
  }
  std::cout << "test4\n";
  runtest (10, 20000, 20000, 1, 1, 4, 4, 0.5, 2,  false);
  std::cout << "test5\n";
  runtest (10, 0, 0, 1, 1, 1, 1, 0.5, 2,  false);

  test_pack_errors();
  test_unpack_errors();
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  SGTest::initTestStore();
  tests();
  return 0;
}
