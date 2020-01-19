/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_CLUSTER_ON_TRACK_P1_TRK_H
#define STGC_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   STGC_ClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"


namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::STGC_ClusterOnTrack */
   class STGC_ClusterOnTrack_p1
   {
  public:
     STGC_ClusterOnTrack_p1() : m_id(0), m_positionAlongStrip(0.0) {}

      /** base RIO_OnTrack_p1 info*/
      Identifier32::value_type        m_id;
      TPObjRef                      m_localParams;
      TPObjRef                      m_localErrMat;
           
      /** Link to PRD */
      ElementLinkInt_p3  m_prdLink;
            
      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
   };
}

#endif // STGC_CLUSTER_ON_TRACK_P1_TRK_H
