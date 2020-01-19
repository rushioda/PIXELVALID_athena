/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The TrkValInterfaces package
Interfaces for common tracking validation tools.

@author Andreas.Salzburger@cern.ch, Sebastian.Fleischmann@cern.ch

@section introductionTrkValInterfaces Introduction

This package contains the interfaces for common tracking validation tools.

@section TrkValInterfacesOverview Class Overview
  The TrkValInterfaces package contains the following classes:

  - Trk::IValidationNtupleTool : Interface for main validation ntuple tools which steer Trk::IValidationNtupleHelperTool.
  - Trk::IValidationNtupleHelperTool : Interface for helper tools which add special data to the validation ntuple.
  - Trk::ITrackDiff : Interface for track diff tools.
      
@section ExtrasTrkValInterfaces Extra Pages

 - @ref UsedTrkValInterfaces
 - @ref requirementsTrkValInterfaces
*/

/**
@page UsedTrkValInterfaces Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirementsTrkValInterfaces Requirements
@include requirements
*/

