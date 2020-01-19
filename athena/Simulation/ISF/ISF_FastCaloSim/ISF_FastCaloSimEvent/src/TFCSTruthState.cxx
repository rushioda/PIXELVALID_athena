/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include <iostream>

//=============================================
//======= TFCSTruthState =========
//=============================================

TFCSTruthState::TFCSTruthState():TLorentzVector(),m_pdgid(0),m_vertex(0,0,0,0)
{
}

TFCSTruthState::TFCSTruthState(Double_t x, Double_t y, Double_t z, Double_t t, int pdgid):TLorentzVector(x,y,z,t),m_vertex(0,0,0,0)
{
  m_pdgid=pdgid;
}

void TFCSTruthState::Print(Option_t *) const
{
  std::cout<<"PDGID="<<m_pdgid<<" pT="<<Pt()<<" eta="<<Eta()<<" phi="<<Phi()<<" E="<<E()<<std::endl;
}
