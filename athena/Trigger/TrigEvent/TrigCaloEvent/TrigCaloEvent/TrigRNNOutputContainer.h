/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENT_TRIGRNNOUTPUTCONTAINER_H 
#define TRIGCALOEVENT_TRIGRNNOUTPUTCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "TrigCaloEvent/TrigRNNOutput.h"
#include "SGTools/BaseInfo.h"

class TrigRNNOutputContainer : public DataVector<TrigRNNOutput> { 
};

CLASS_DEF( TrigRNNOutputContainer , 1245245010 , 1 )

SG_BASE(TrigRNNOutputContainer, DataVector<TrigRNNOutput>);
#endif
