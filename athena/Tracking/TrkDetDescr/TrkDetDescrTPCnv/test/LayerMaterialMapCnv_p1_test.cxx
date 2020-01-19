/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkDetDescrTPCnv/test/LayerMaterialMapCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialMapCnv_p1.h"
#include "TrkDetDescrTPCnv/LayerMaterialMapCnv_tlp1.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>



using Athena_test::isEqual;


void compare (const Trk::BinningData& b1, const Trk::BinningData& b2)
{
  assert (b1.type == b2.type);
  assert (b1.option == b2.option);
  assert (b1.binvalue == b2.binvalue);
  assert (b1.bins == b2.bins);
  assert (b1.min == b2.min);
  assert (b1.max == b2.max);
  assert (b1.step == b2.step);
  assert (b1.subStep == b2.subStep);
  assert (b1.boundaries == b2.boundaries);
  assert (b1.refphi == b2.refphi);
  assert (b1.hbounds == b2.hbounds);
}


void compare (const Trk::BinUtility& b1, const Trk::BinUtility& b2)
{
  size_t sz = b1.binningData().size();
  assert (b2.binningData().size() == sz);
  for (size_t i = 0; i < sz; i++)
    compare (b1.binningData()[i], b2.binningData()[i]);
}


void compare (const Trk::Material& m1, const Trk::Material& m2)
{
  assert (m1.X0 == m2.X0);
  assert (m1.L0 == m2.L0);
  assert (m1.A == m2.A);
  assert (m1.Z == m2.Z);
  assert (m1.rho == m2.rho);
  assert (m1.dEdX == m2.dEdX);
  assert (m1.zOaTr == m2.zOaTr);
  if (m1.composition)
    assert (*m1.composition == *m2.composition);
  else
    assert (m2.composition == nullptr);
}


void compare (const Trk::MaterialProperties& m1,
              const Trk::MaterialProperties& m2)
{
  compare (m1.material(), m2.material());
  assert (isEqual (m1.thicknessInX0(), m2.thicknessInX0()));
  assert (isEqual (m1.thicknessInL0(), m2.thicknessInL0()));
  assert (isEqual (m1.zOverAtimesRhoTimesD(), m2.zOverAtimesRhoTimesD()));
}


void compare (const Trk::BinnedLayerMaterial& m1,
              const Trk::BinnedLayerMaterial& m2)
{
  compare (*m1.binUtility(), *m2.binUtility());
  assert (m1.alongPostFactor() == m2.alongPostFactor());

  const Trk::MaterialPropertiesMatrix& mm1 = m1.fullMaterial();
  const Trk::MaterialPropertiesMatrix& mm2 = m2.fullMaterial();
  assert (mm1.size() == mm2.size());
  for (size_t i=0; i < mm1.size(); i++) {
    const Trk::MaterialPropertiesVector& mv1 = mm1[i];
    const Trk::MaterialPropertiesVector& mv2 = mm2[i];
    assert (mv1.size() == mv2.size());
    for (size_t j = 0; j < mv1.size(); j++) {
      compare (*mv1[j], *mv2[j]);
    }
  }
}


void compare (const Trk::CompressedLayerMaterial& m1,
              const Trk::CompressedLayerMaterial& m2)
{
  compare (*m1.binUtility(), *m2.binUtility());
  assert (m1.alongPostFactor() == m2.alongPostFactor());

  const Trk::MaterialPropertiesVector& mv1 = m1.fullMaterial();
  const Trk::MaterialPropertiesVector& mv2 = m2.fullMaterial();
  assert (mv1.size() == mv2.size());
  for (size_t j = 0; j < mv1.size(); j++) {
    compare (*mv1[j], *mv2[j]);
  }
  assert (m1.materialBins() == m2.materialBins());
}


void compare (const Trk::ElementTable& e1, const Trk::ElementTable& e2)
{
  size_t sz = e1.size();
  assert (e2.size() == sz);

  for (size_t i=0; i < sz; i++) {
    assert (e1.contains(i) == e2.contains(i));
    if (e1.contains(i)) {
      assert (e1.elementName(i) == e2.elementName(i));
      compare (*e1.element(i), *e2.element(i));
    }
  }
}


void compare (const Trk::LayerMaterialMap& m1,
              const Trk::LayerMaterialMap& m2)
{
  compare (*m1.elementTable(), *m2.elementTable());
  assert (m1.size() == m2.size());
  for (const std::pair<const Trk::LayerIndex,
                       const Trk::LayerMaterialProperties*>& p : m1)
  {
    Trk::LayerMaterialMap::const_iterator it = m2.find (p.first);
    assert (it != m2.end());

    const Trk::LayerMaterialProperties* p1 = p.second;
    const Trk::LayerMaterialProperties* p2 = it->second;

    if (auto* pp1 = dynamic_cast<const Trk::BinnedLayerMaterial*> (p1))
    {
      auto* pp2 = dynamic_cast<const Trk::BinnedLayerMaterial*> (p2);
      assert (pp2 != nullptr);
      compare (*pp1, *pp2);
    }

    else if (auto* pp1 = dynamic_cast<const Trk::CompressedLayerMaterial*> (p1))
    {
      auto* pp2 = dynamic_cast<const Trk::CompressedLayerMaterial*> (p2);
      assert (pp2 != nullptr);
      compare (*pp1, *pp2);
    }

    else
      std::abort();

  }
}


void testit (const Trk::LayerMaterialMap& trans1)
{
  LayerMaterialMapCnv_p1 cnv;
  LayerMaterialMapCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::LayerMaterialMap_p1 pers;
  MsgStream log (0, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::LayerMaterialMap trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Trk::ElementTable* e = new Trk::ElementTable;
  e->addElement (Trk::Material ( 1.5,  2.5,  3.5, 10,  4.5), "ten");
  e->addElement (Trk::Material (11.5, 12.5, 13.5, 20, 14.5), "twenty");
  e->addElement (Trk::Material (21.5, 22.5, 23.5, 30, 24.5), "thirty");

  Trk::LayerMaterialMap lmm (e);

  {
    Trk::MaterialPropertiesMatrix mm;
    mm.resize(2);
    Trk::Material mat1 ( 1.5,  2.5,  3.5,  4.5,  5.5);
    Trk::Material mat2 (11.5, 12.5, 13.5, 14.5, 15.5);
    Trk::Material mat3 (21.5, 22.5, 23.5, 24.5, 25.5);
    Trk::Material mat4 (31.5, 32.5, 33.5, 34.5, 35.5);
    mm[0].push_back (new Trk::MaterialProperties (mat1, 1));
    mm[0].push_back (new Trk::MaterialProperties (mat2, 2));
    mm[1].push_back (new Trk::MaterialProperties (mat3, 3));
    mm[1].push_back (new Trk::MaterialProperties (mat4, 4));
    
    Trk::BinUtility binning (2, 0, 10);
    Trk::BinUtility binning2 (2, 0, 20);
    binning += binning2;
  
    lmm[Trk::LayerIndex(1)] = new Trk::BinnedLayerMaterial (binning, mm, 1.5);
  }

  {
    Trk::MaterialPropertiesVector mm;
    Trk::Material mat1 ( 1.5,  2.5,  3.5,  4.5,  5.5);
    Trk::Material mat2 (11.5, 12.5, 13.5, 14.5, 15.5);
    Trk::Material mat3 (21.5, 22.5, 23.5, 24.5, 25.5);
    Trk::Material mat4 (31.5, 32.5, 33.5, 34.5, 35.5);
    mm.push_back (new Trk::MaterialProperties (mat1, 1));
    mm.push_back (new Trk::MaterialProperties (mat2, 2));
    mm.push_back (new Trk::MaterialProperties (mat3, 3));
    mm.push_back (new Trk::MaterialProperties (mat4, 4));

    Trk::BinUtility binning (2, 0, 10);
    Trk::BinUtility binning2 (2, 0, 20);
    binning += binning2;

    std::vector<unsigned short> indices {0, 1, 2, 3};
  
    lmm[Trk::LayerIndex(1)] = new Trk::CompressedLayerMaterial (binning, mm, indices, 1.5);
  }

  testit (lmm);
}


int main()
{
  test1();
  return 0;
}
