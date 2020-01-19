/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloGeometryFromCaloDDM_h
#define CaloGeometryFromCaloDDM_h

#include "ISF_FastCaloSimParametrization/CaloGeometry.h"

class CaloDetDescrManager;

class CaloGeometryFromCaloDDM:public CaloGeometry {
public :
   CaloGeometryFromCaloDDM();
   virtual ~CaloGeometryFromCaloDDM();
   
   virtual bool LoadGeometryFromCaloDDM(const CaloDetDescrManager* calo_dd_man);
};

#endif

