/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARNOISYROSUMMARYCNV_P1_H
#define LARTPCNV_LARNOISYROSUMMARYCNV_P1_H

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummary_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class MsgStream;

class LArNoisyROSummaryCnv_p1: public T_AthenaPoolTPCnvBase<LArNoisyROSummary,LArNoisyROSummary_p1>
{
 public:
  LArNoisyROSummaryCnv_p1() {};

  virtual void   persToTrans(const LArNoisyROSummary_p1* pers, LArNoisyROSummary* trans, MsgStream &log) ;
  virtual void   transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p1* pers, MsgStream &log) ;
  
};


#endif
