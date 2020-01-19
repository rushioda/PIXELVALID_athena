/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrkJiveXML
@author Nikos.Konstantinidis -at- cern.ch, Eric.Jansen -at- cern.ch, Juergen.Thomas -at- cern.ch, Sebastian.Boeser -at- cern.ch

@section TrkJiveXMLIntro Introduction

This package contains the @c AthAlgTools that retrieve tracking data (tracks, track segments and vertices) from @c StoreGate and 
forward them to the formatting tool. Each tool implements the IDataRetriever interface, through which it is called from the @c AlgoJiveXML.

@section TrkJiveXMLTools Retrievers

- JiveXML::SegmentRetriever @copydoc JiveXML::SegmentRetriever

- JiveXML::TrackRetriever : @copydoc JiveXML::TrackRetriever

- JiveXML::VertexRetriever : @copydoc JiveXML::VertexRetriever

@section TrkJiveXMLEnv Environment
 - @ref used_TrkJiveXML
 - @ref requirements_TrkJiveXML

*/

/**
@page used_TrkJiveXML Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_TrkJiveXML Requirements
@include requirements
*/

