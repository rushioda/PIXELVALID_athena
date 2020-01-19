/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS2DFunction_h
#define ISF_FASTCALOSIMEVENT_TFCS2DFunction_h

#include "ISF_FastCaloSimEvent/TFCSFunction.h"

class TH2;

class TFCS2DFunction:public TFCSFunction
{
  public:
    TFCS2DFunction() {};
    ~TFCS2DFunction() {};

    virtual int ndim() const {return 2;};
    
    virtual void rnd_to_fct(float& valuex,float& valuey,float rnd0,float rnd1) const = 0;
    virtual void rnd_to_fct(float value[],const float rnd[]) const;

  private:

  ClassDef(TFCS2DFunction,1)  //TFCS2DFunction
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCS2DFunction+;
#endif

#endif
