/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArG4BirksLaw.h"

double LArG4BirksLaw::operator () (double dE, double dX, double EField) const {
  if (dX<1e-5) return dE;
  if (EField<1e-3) return 0.;

  //  return dE * (1 + m_BirksLawK/EField*1.51) / (1 + m_BirksLawK/EField*dE/dX/m_LArDensity);

  // 22.10.2010
  // Corrections for high dE/dX (heavy ions, exotics HIPs)
  // based on heavy ion experimental data 
  // questions and comments to S.Burdin (burdin@cern.ch) and W.Taylor (taylorw@yorku.ca)

  double dEdX = dE/dX;

  const double kOverField = m_BirksLawK/EField;
  double dEcorr1 = dE * (1 + kOverField*1.51) / (1 + kOverField*dEdX/m_LArDensity);  // original corrections

  if (dEdX > 12000.0) dEdX = 12000.0;   // The experimental data is available only until dE/dX ~ 12000 MeV/cm

  double kHIP = (dEdX > 969.) ? 0.000754*dEdX+0.2692 : 1.0;     // No corrections for dE/dX < 969 MeV/cm

  return dEcorr1 * kHIP;
}
