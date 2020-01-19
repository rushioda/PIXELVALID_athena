/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSInitWithEkin_h
#define ISF_FASTCALOSIMEVENT_TFCSInitWithEkin_h

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

class TFCSInitWithEkin:public TFCSParametrization {
public:
  TFCSInitWithEkin(const char* name=nullptr, const char* title=nullptr);

  virtual bool is_match_Ekin_bin(int /*Ekin_bin*/) const override {return true;};
  virtual bool is_match_calosample(int /*calosample*/) const override {return true;};
  virtual bool is_match_all_Ekin_bin() const override {return true;};
  virtual bool is_match_all_calosample() const override {return true;};

  // Initialize simulstate with the kinetic energy Ekin from truth
  virtual FCSReturnCode simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) override;
private:

  ClassDefOverride(TFCSInitWithEkin,1)  //TFCSInitWithEkin
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSInitWithEkin+;
#endif

#endif
