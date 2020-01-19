/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetDiscriminatorTool.h"

namespace{
  struct  Discrim{
    Discrim(float maxOverMean, float ETMin) :  _maxOverMean(maxOverMean),
					       _ETMin(ETMin){};
    
    float _maxOverMean;
    float _ETMin;

    //return true if jet should be removed
    /// %% FIX add check on attribute
    bool operator()(const xAOD::Jet* j1)
    {
      float max_ET=j1->getAttribute<float>("MaxConstituentET");
      float disc=j1->getAttribute<float>("MaxOverMean");
      if( max_ET < _ETMin ) return true;
      if( disc < _maxOverMean ) return true;
      return false;
    }
  };
} //annonymous namespace

HIJetDiscriminatorTool::HIJetDiscriminatorTool(const std::string& t) : JetModifierBase(t)
{
  declareProperty("MaxOverMeanCut",m_MaxOverMean_cut=4.);
  declareProperty("MinimumETMaxCut",m_ET_min_cut=3000.);
}

int HIJetDiscriminatorTool::modify(xAOD::JetContainer& jets) const
{
  xAOD::JetContainer::iterator itB = jets.begin();
  xAOD::JetContainer::iterator itE = jets.end();
  xAOD::JetContainer::iterator lastFiltered=std::remove_if(itB, itE, Discrim(m_MaxOverMean_cut,m_ET_min_cut));
  jets.erase( lastFiltered, itE );
  return 1;
}
