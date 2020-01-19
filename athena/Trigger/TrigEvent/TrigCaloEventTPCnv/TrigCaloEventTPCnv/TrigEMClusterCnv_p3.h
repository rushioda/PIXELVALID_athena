/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterCnv_p3
 *
 * @brief transient persistent converter for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterCnv_p2.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "DataModel/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

//#include "TrigCaloEventTPCnv/TrigEMClusterConverterBase.h"

class MsgStream;

class TrigEMClusterCnv_p3 : public T_AthenaPoolTPCnvBase<TrigEMCluster, TrigEMCluster_p3>  {
    
  public:

    TrigEMClusterCnv_p3() { }

    virtual void persToTrans(const TrigEMCluster_p3* persObj,  TrigEMCluster* transObj,   MsgStream& log);
    virtual void transToPers(const TrigEMCluster* transObj, TrigEMCluster_p3* persObj, MsgStream& log);

 protected:
    
    ElementLinkCnv_p3< ElementLink<RingerRingsContainer> > ELinkRingerRingsCnv;

//    ITPConverterFor<TrigCaloCluster>*   m_trigCaloClusterCnv;
};

//typedef TrigEMClusterConverterBase<TrigEMCluster_p2> TrigEMClusterCnv_p2;

#endif

