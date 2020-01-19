/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TEasyFormula                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __TEasyFormula__
#define __TEasyFormula__

#include "TFormula.h"
#include "TString.h"
#include <vector>
#include <map>

namespace MET {

   class TEasyFormula : public TFormula {
      
   public:

      TEasyFormula();      
      TEasyFormula(const char* expression);
      TEasyFormula(const char* name, const char* expression);
      virtual ~TEasyFormula();

      TEasyFormula(const MET::TEasyFormula& other) ;
      TEasyFormula& operator=(const TEasyFormula& other) ;

      Int_t SetFormula(const char *expression);
      void  SetValue(const TString& name, const double& value);

      inline unsigned int GetNumberPars() const { return m_par.size(); }
      const std::vector<TString>& GetParList() const { return m_par; }

      inline double Evaluate()        { return this->EvalPar(0,0); }
      Bool_t Contains(const TString& parname) const;
      Bool_t Contains(const std::vector<TString>& parList) const;

      inline double GetDoubleValue()  { return this->EvalPar(0,0); } 
      inline float  GetFloatValue()   { return static_cast<float>( this->EvalPar(0,0) ); }
      inline bool   GetBoolValue()    { return static_cast<bool>( this->EvalPar(0,0) ); }
      inline int    GetIntValue()     { return static_cast<int>( this->EvalPar(0,0) ); }
      inline const char* GetFormula() const { return m_expr.Data(); }

      inline void   Reset()           { (void) this->SetFormula("1"); }

      void Summary() const;

   protected:

      // Interface to TFormula engine
      virtual       Double_t DefinedValue(Int_t code) ;  // ROOT 4
      Int_t         DefinedVariable(TString& name) ;     // ROOT 3
      virtual Int_t DefinedVariable(TString& name, int& action) ;

      TString m_expr;
      std::vector<TString> m_par;

      std::map<TString,double> m_stod;
      std::map<int,TString> m_itos;
      std::map<TString,int> m_stoi;

      ClassDef(TEasyFormula,0)

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
      enum { kDefinedVariable = 150 } ;
#endif

   };
}

#endif

