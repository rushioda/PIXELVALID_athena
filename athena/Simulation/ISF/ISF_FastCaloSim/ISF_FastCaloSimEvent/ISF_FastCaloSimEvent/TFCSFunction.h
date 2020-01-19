/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSFunction_h
#define ISF_FASTCALOSIMEVENT_TFCSFunction_h

#include "TObject.h"

class TFCSFunction: public TObject 
{
  public:
    TFCSFunction() {};
    virtual ~TFCSFunction() {};
    
    ///Gives the total memory size, including the size of additional memory allocated inside the class
    virtual std::size_t MemorySize() const {return sizeof(*this);};

    ///Return the number of dimensions for the function
    virtual int ndim() const {return 0;};

    ///Function gets array of random numbers rnd[] in the range [0,1) as arguments 
    ///and returns function value in array value.
    ///For a n-dimensional function, value and rnd should both have n elements.
    virtual void rnd_to_fct(float value[],const float rnd[]) const = 0;
   
  private:

  ClassDef(TFCSFunction,2)  //TFCSFunction

};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSFunction+;
#endif

#endif
