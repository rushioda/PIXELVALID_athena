/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD86EAF030B.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD86EAF030B.cm

//## begin module%3CD86EAF030B.cp preserve=no
//## end module%3CD86EAF030B.cp

//## Module: GeoElement%3CD86EAF030B; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoElement.cxx

//## begin module%3CD86EAF030B.additionalIncludes preserve=no
//## end module%3CD86EAF030B.additionalIncludes

//## begin module%3CD86EAF030B.includes preserve=yes
//## end module%3CD86EAF030B.includes

// GeoElement
#include "GeoModelKernel/GeoElement.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>
//## begin module%3CD86EAF030B.additionalDeclarations preserve=yes
//## end module%3CD86EAF030B.additionalDeclarations


// Class GeoElement 

GeoElement::GeoElement (const std::string &Name, const std::string &Symbol, double Z, double A):
  //## begin GeoElement::GeoElement%3CD87237027F.hasinit preserve=no
  //## end GeoElement::GeoElement%3CD87237027F.hasinit
  //## begin GeoElement::GeoElement%3CD87237027F.initialization preserve=yes
  m_name (Name),
  m_symbol (Symbol),
  m_z (Z),
  m_a (A)
  //## end GeoElement::GeoElement%3CD87237027F.initialization
{
  //## begin GeoElement::GeoElement%3CD87237027F.body preserve=yes
  //## end GeoElement::GeoElement%3CD87237027F.body
}


GeoElement::~GeoElement()
{
  //## begin GeoElement::~GeoElement%3CD86EAF030B_dest.body preserve=yes
  //## end GeoElement::~GeoElement%3CD86EAF030B_dest.body
}


double GeoElement::getRadTsai() const
{
  // Compute Coulomb correction factor (Phys Rev. D50 3-1 (1994) page 1254)
  const double k1 = 0.0083, k2 = 0.20206, k3 = 0.0020, k4 = 0.0369;
  double az2 = (CLHEP::fine_structure_const*m_z)*(CLHEP::fine_structure_const*m_z);
  double az4 = az2 * az2;

  double coulomb = (k1*az4 + k2 + 1./(1.+az2))*az2 - (k3*az4 + k4)*az4;

  //  Compute Tsai's Expression for the Radiation Length (Phys Rev. D50 3-1 (1994) page 1254)
  const double Lrad_light[]  = {5.31,  4.79,  4.74,  4.71};
  const double Lprad_light[] = {6.144, 5.621, 5.805, 5.924};

  const double logZ3 = std::log(m_z) * (1./3);

  double Lrad, Lprad;
  int iz = (int)(m_z+0.5) - 1 ;
  if (iz<=3) {
    Lrad = Lrad_light[iz];  
    Lprad = Lprad_light[iz]; 
  } else { 
    Lrad = std::log(184.15) - logZ3; 
    Lprad = std::log(1194.) - 2*logZ3;
  }

  double radTsai = 4*CLHEP::alpha_rcl2*m_z*(m_z*(Lrad-coulomb) + Lprad);
  return radTsai;
}

// Additional Declarations
  //## begin GeoElement%3CD86EAF030B.declarations preserve=yes
  //## end GeoElement%3CD86EAF030B.declarations

//## begin module%3CD86EAF030B.epilog preserve=yes
//## end module%3CD86EAF030B.epilog
