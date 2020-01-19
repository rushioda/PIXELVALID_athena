/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage MuonCalibMath Package
@author Oliver.Kortner@cern.ch 
                                                                                
@section MuonCalibMathIntro Introduction
This package collects all the utilities used to fit and parametrize sets of sample points.
                                                                                
@section MuonCalibMathOverview Class Overview
The MuonCalibMath package contains the following classes:

- MuonCalib::BaseFunction: This is an abstract base class for a set of base functions for fits to sample points
- MuonCalib::BaseFunctionFitter: This class performs a fit of a linear combination of base functions to a set of sample points
- MuonCalib::ChebyshevPolynomial: This class class provides a tschebyscheff polynomial of order k
- MuonCalib::Legendre_polynomial: This class provides a legendre polynomial of order k
- MuonCalib::SamplePoint: This class provides a sample point for the MuonCalib::BaseFunctionFitter
- MuonCalib::Tschebyscheff_polynomial: This class provides a tschebyscheff polynomial of order k
                                                                                
@ref used_MuonCalibMath
                                                                                
@ref requirements_MuonCalibMath
*/
                                                                                
/**
@page used_MuonCalibMath Used Packages
@htmlinclude used_packages.html
*/
                                                                                
/**
@page requirements_MuonCalibMath Requirements
@include requirements
*/
 
