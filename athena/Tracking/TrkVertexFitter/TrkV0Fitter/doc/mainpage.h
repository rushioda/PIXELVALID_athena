/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
\mainpage The TrkV0FitterInterfaces package

@author Evelina.Bouhova@cern.ch

\section introductionTrkV0Fitter Introduction

This class implements a vertex fitting algorithm optimised for V0 finding.
The algorithm fits the full track information, incorporating the
possibility of performing full kinematic constraints at the same time.
The full covariance matrix from the fit, including track-track and track-vertex
correlations is calculated and returned via Trk::ExtendedVxCandidate

\section designTrkV0Fitter Design

\section requirementsTrkV0Fitter  Requirements

@htmlinclude used_packages.html
@include requirements

*/
