/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS1DFunctionInt16Histogram_h
#define ISF_FASTCALOSIMEVENT_TFCS1DFunctionInt16Histogram_h

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include <vector>

class TH2;

class TFCS1DFunctionInt16Histogram:public TFCS1DFunction
{
  public:
    TFCS1DFunctionInt16Histogram(const TH1* hist=nullptr) {if(hist) Initialize(hist);};
    ~TFCS1DFunctionInt16Histogram() {};

    virtual void Initialize(const TH1* hist);

    using TFCS1DFunction::rnd_to_fct;
    
    typedef uint16_t HistoContent_t;
    static const HistoContent_t s_MaxValue;

    ///Function gets random number rnd in the range [0,1) as argument 
    ///and returns function value according to a histogram distribution
    virtual double rnd_to_fct(double rnd) const;

    const std::vector<float>& get_HistoBordersx() const {return m_HistoBorders;};
    std::vector<float>& get_HistoBordersx() {return m_HistoBorders;};
    const std::vector<HistoContent_t>& get_HistoContents() const {return m_HistoContents;};
    std::vector<HistoContent_t>& get_HistoContents() {return m_HistoContents;};
    
    static void unit_test(TH1* hist=nullptr);
  protected:
    
    std::vector<float> m_HistoBorders;
    std::vector<HistoContent_t> m_HistoContents;

  private:

  ClassDef(TFCS1DFunctionInt16Histogram,1)  //TFCS1DFunctionInt16Histogram
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCS1DFunctionInt16Histogram+;
#endif

#endif
