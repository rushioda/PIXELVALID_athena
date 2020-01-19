/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetModifier.cxx

#include "JetInterface/IJetModifier.h"

//**********************************************************************

IJetModifier::IJetModifier() : m_ppjr(nullptr) { }

//**********************************************************************

IJetModifier::~IJetModifier() {
  m_ppjr = nullptr;
}

//**********************************************************************

int IJetModifier::inputContainerNames(std::vector<std::string>&) {
  return 0;
}

//**********************************************************************

const IJetPseudojetRetriever* IJetModifier::pseudojetRetriever() const {
  return m_ppjr;
}

//**********************************************************************

void IJetModifier::setPseudojetRetriever(const IJetPseudojetRetriever* ppjr) {
  m_ppjr = ppjr;
}

//**********************************************************************
