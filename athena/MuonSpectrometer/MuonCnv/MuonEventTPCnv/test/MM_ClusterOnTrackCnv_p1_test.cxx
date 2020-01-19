/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MM_ClusterOnTrackCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonRIO_OnTrack/MM_ClusterOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonMeasurementsCnv_tlp2.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


#include "make_dd.icc"


void compare (const Trk::Surface& s1,
              const Trk::Surface& s2)
{
  if (s1.name() == "Trk::ConeSurface") {
    assert (s2.name() == "Trk::PlaneSurface");
  }
  else {
    assert (s1.name() == s2.name());
    if (s1.name() == "Trk::PerigeeSurface" ||
        s1.name() == "Trk::PlaneSurface" ||
        s1.name() == "Trk::StraightLineSurface")
    {
      for (int i=0; i < 4; i++)
        for (int j=0; j < 4; j++)
          assert (Athena_test::isEqual (s1.transform()(i,j), s2.transform()(i,j)));
    }
    else {
      std::cout << "Unknown surface type: " << s1.name() << "\n";
      std::abort();
    }
  }
}


void compare (const Trk::MeasurementBase& p1,
              const Trk::MeasurementBase& p2)
{
  assert (p1.localParameters().size() == p2.localParameters().size());
  for (int i = 0; i < p1.localParameters().size(); i++) {
    Trk::ParamDefs ii = static_cast<Trk::ParamDefs>(i);
    assert (Athena_test::isEqual (p1.localParameters()[ii], p2.localParameters()[ii]));
  }
  assert (p1.localCovariance() == p2.localCovariance());

#if 0
  for (int i = 0; i < 3; i++) {
    assert (Athena_test::isEqual (p1.globalPosition()[i], p2.globalPosition()[i]));
  }

  compare (p1.associatedSurface(), p2.associatedSurface());
#endif
}


void compare (const Trk::RIO_OnTrack& p1,
              const Trk::RIO_OnTrack& p2)
{
  compare (static_cast<const Trk::MeasurementBase&>(p1),
           static_cast<const Trk::MeasurementBase&>(p2));
  assert (p1.identify() == p2.identify());
  
}


void compare (const Muon::MuonClusterOnTrack& p1,
              const Muon::MuonClusterOnTrack& p2)
{
  compare (static_cast<const Trk::RIO_OnTrack&>(p1),
           static_cast<const Trk::RIO_OnTrack&>(p2));
  assert (p1.positionAlongStrip() == p2.positionAlongStrip());
}


void compare (const Muon::MMClusterOnTrack& p1,
              const Muon::MMClusterOnTrack& p2)
{
  compare (static_cast<const Muon::MuonClusterOnTrack&>(p1),
           static_cast<const Muon::MuonClusterOnTrack&>(p2));
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.prepRawDataLink().key() == p2.prepRawDataLink().key());
  assert (p1.prepRawDataLink().index() == p2.prepRawDataLink().index());
  
}



void testit (const Muon::MMClusterOnTrack& trans1)
{
  MsgStream log (0, "test");
  MM_ClusterOnTrackCnv_p1 cnv;
  MuonMeasurementsCnv_tlp2 exttlcnv;
  cnv.setRuntimeTopConverter (&exttlcnv);
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Muon::MM_ClusterOnTrack_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::MMClusterOnTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 (const MuonGM::MuonDetectorManager& muo_dd)
{
  std::cout << "test1\n";

  Trk::LocalParameters locpos (1.5, 2.5, 3.5, 4.5, 5.5);

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  ElementLinkToIDC_MM_Container rio (333, 222);

  // 55 == MMS
  Identifier clusId = muo_dd.mmIdHelper()->channelID (55, 1, 2, 1, 1, 1, 1);

  Muon::MMClusterOnTrack trans1 (rio,
                                 locpos,
                                 cov,
                                 clusId,
                                 muo_dd.getMMReadoutElement (clusId),
                                 2.5);

  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  const MuonGM::MuonDetectorManager& muo_dd = make_dd();

  test1 (muo_dd);
  return 0;
}
