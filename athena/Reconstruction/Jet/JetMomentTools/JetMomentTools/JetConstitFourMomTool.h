/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetConstitFourMomTool.h

#ifndef JetMomentTools_JetConstitFourMomTool_H
#define JetMomentTools_JetConstitFourMomTool_H

/// Russell Smith
/// May 2015
///
/// Tool to attach the LC constituent level 4-vector to EM Jets

#include "JetRec/JetModifierBase.h"
#include <vector>

class JetConstitFourMomTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetConstitFourMomTool, IJetModifier)

 public:

  // Constructor from tool name.
  JetConstitFourMomTool(std::string myname);

  StatusCode initialize();

  // From IJetModifier base class
  int modify(xAOD::JetContainer& jets) const;
  int modifyJet(xAOD::Jet&) const {return 0;}

 private:
  
  int m_constitScale;
  std::vector<std::string> m_jetScaleNames;
  std::vector<std::string> m_altColls;
  std::vector<int>         m_altConstitScales;
  std::vector<std::string> m_altJetScales;

  std::vector<bool> m_isDetectorEtaPhi;

};

#endif
