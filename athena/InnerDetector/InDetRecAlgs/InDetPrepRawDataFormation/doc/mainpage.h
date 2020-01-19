/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage InDetPrepRawDataFormation Package
@author Tommaso.Lari@cern.ch

@section InDetPrepRawDataFormationIntro Introduction

This package manages the creation of InDetPrepRawData objects 
for the Inner Detector. 

@section InDetPrepRawDataFormationOverview Overview

There are three classes, all of them Algorithms:

PixelClusterization
-------------------
  For each module, retrieves the RDO collection, and calls an 
algorithm tool of SiClusterizationTool (default is MergedPixelTool, 
can be changed by job option) which clusters togheter neighbouring 
pixels and creates a cluster collection (actually, an 
InDetPrepRawDataCollection)

   - SCT_Clusterization
   For each module, retrieves the RDO collection, and calls an algorithm tool of SiClusterizationTool which clusters together adjacent 
strips and creates a cluster collection (actually, an InDetPrepRawDataCollection). 
Previous versions referenced SCT_ChannelStatusAlg, vestigial code from combined test beam 2004. This was removed July 2007, pending
introduction of the use of SCT_ConditionsAlgTools.


TRT_RIO_Maker
-------------

@ref used_InDetPrepRawDataFormation

@ref requirements_InDetPrepRawDataFormation

*/

/**

@page used_InDetPrepRawDataFormation Used Packages
@htmlinclude used_packages.html

*/

/**
@page requirements_InDetPrepRawDataFormation requirements
@include requirements
*/
