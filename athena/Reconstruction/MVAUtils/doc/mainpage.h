/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage

@section MVAUtilsIntro Introduction

This package is meant to provide general utilities for using MVAs in
reconstruction and analyses. The initial version extracts the
memory-efficient impementation for storing BDT trees from
/Reconstrcution/egamma/egammaMVACalib. It is an installed library when
compiled with athena. It contains no tools or algorithms. It is also
dual-use.

@section MVAUtilsOverview Class Overview
  The MVAUtils package contains of following classes:

  - BDT : A memory-efficient implementation of a BDT for applying the tree (not for training).
  - Node : An implementation of tree node. Not meant for external use.


@author Jovan Mitrevski
@author Christos Anastopoulos
@author RD Schaffer
@author Ruggero Turra
@author Bruno Lenzi

@section ExtrasMVAUtils Extra Pages
 - @ref UsedMVAUtils
 - @ref RequirementsMVAUtils
*/
/**
@page UsedMVAUtils Used Packages
@htmlinclude used_packages.html
*/
/**
@page RequirementsMVAUtils Requirements
@include requirements
*/
