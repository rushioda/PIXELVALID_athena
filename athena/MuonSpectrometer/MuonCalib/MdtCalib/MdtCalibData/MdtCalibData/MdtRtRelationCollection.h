/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtCalibDataCollection.h
//   Header file for class MdtCalibDataCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTRTRELATIONCOLLECTION_H
#define MUONCALIB_MDTRTRELATIONCOLLECTION_H

#include "DataModel/DataVector.h"
#include "MdtCalibData/MdtRtRelation.h"
#include "CLIDSvc/CLASS_DEF.h"

/**
This typedef representing:
- a collection of MdtRtRelation objects. 
It is a DataVector. It can be saved to storegate.
*/
typedef DataVector<MuonCalib::MdtRtRelation> MdtRtRelationCollection;
CLASS_DEF(MdtRtRelationCollection, 1270996316, 1 )

#endif
