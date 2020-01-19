/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS2DFunctionHistogram_h
#define ISF_FASTCALOSIMEVENT_TFCS2DFunctionHistogram_h

#include "ISF_FastCaloSimEvent/TFCS2DFunction.h"
#include <vector>

class TH2;

class TFCS2DFunctionHistogram:public TFCS2DFunction
{
  public:
    TFCS2DFunctionHistogram(TH2* hist=nullptr) {if(hist) Initialize(hist);};
    ~TFCS2DFunctionHistogram() {};

    virtual void Initialize(TH2* hist);

    using TFCS2DFunction::rnd_to_fct;
    virtual void rnd_to_fct(float& valuex,float& valuey,float rnd0,float rnd1) const;

    const std::vector<float>& get_HistoBordersx() const {return m_HistoBorders;};
    std::vector<float>& get_HistoBordersx() {return m_HistoBorders;};
    const std::vector<float>& get_HistoBordersy() const {return m_HistoBordersy;};
    std::vector<float>& get_HistoBordersy() {return m_HistoBordersy;};
    const std::vector<float>& get_HistoContents() const {return m_HistoContents;};
    std::vector<float>& get_HistoContents() {return m_HistoContents;};
    
    static void unit_test(TH2* hist=nullptr);
  protected:
    std::vector<float> m_HistoBorders;
    std::vector<float> m_HistoBordersy;
    std::vector<float> m_HistoContents;

  private:

  ClassDef(TFCS2DFunctionHistogram,1)  //TFCS2DFunctionHistogram
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCS2DFunctionHistogram+;
#endif

#endif
