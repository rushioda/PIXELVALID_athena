/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANAR_CLUSTER_P1_H
#define PLANAR_CLUSTER_P1_H

//--------------------------------------------------------------
//
// file:    PlanarCluster_p1.h
//
//--------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidth_p2.h"
#include "Identifier/Identifier.h"

#include <vector>

namespace iFatras {

  class PlanarCluster_p1
  {
  public:
    typedef int rdo_diff_type;

    PlanarCluster_p1() {}

    // m_clustId - stores the pixel cluster id only for 32-bit ids,
    // otherwise the cluster id is stored in the m_rdoList
       unsigned int                          m_clusId; 
       std::vector<rdo_diff_type>            m_rdoList;
       float                                 m_localPosX;
       float                                 m_localPosY;
       float                                 m_mat00;
       float                                 m_mat01; 
       float                                 m_mat11;

       InDet::SiWidth_p2                     m_width;

  };
}

#endif // PLANAR_CLUSTER_P1_H
