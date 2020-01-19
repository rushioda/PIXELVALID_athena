/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_LOLUMRAWDATACONTAINERCNV_P1_H
#define TRT_LOLUMRAWDATACONTAINERCNV_P1_H

// TRT_LoLumRawDataContainerCnv_p1, T/P separation of TRT Raw data
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetRawData/TRT_RDO_Container.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetEventAthenaPool/InDetRawDataContainer_p1.h"
class TRT_ID;


// We do NOT use T_AthenaPoolTPCnvIDCont because we do all of the
// conversion in the .cxx. Same as for the LAr Raw Channels
// Note that this is used for a container of TRT Raw Data
// that containes only TRT_LoLumRawData concrete types
// TRT_RDO_ContainerCnv.cxx delegates the conversion to this
// converter if the objects are of the correct type


class TRT_LoLumRawDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<TRT_RDO_Container, InDetRawDataContainer_p1>
{
 private:
  const TRT_ID *m_trtId = nullptr;
 public:
  TRT_LoLumRawDataContainerCnv_p1() {};
  
  virtual void	persToTrans(const InDetRawDataContainer_p1* persCont,
			    TRT_RDO_Container* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const TRT_RDO_Container* transCont,
			    InDetRawDataContainer_p1* persCont,
			    MsgStream &log) ;
  virtual TRT_RDO_Container* createTransient(const InDetRawDataContainer_p1* persObj, MsgStream& log) ;


  // ID helper can't be used in the constructor, need initialize()
  void initialize(const TRT_ID *idhelper) { m_trtId = idhelper; }
  
};

#endif
