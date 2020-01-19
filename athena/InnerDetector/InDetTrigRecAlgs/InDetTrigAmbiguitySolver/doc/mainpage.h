/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage InDetTrigAmbiguitySolver


@section Introduction

InDetTrigAmbiguitySolver is a trigger interface to the offline
TrkAmbiguityProcessor which resolves ambiguities in a given
TrackCollection.

If m_resolveTracks is set it uses an ambiguity processor
specified by m_ambitool (by default
Trk::SimpleAmbiguityProcessorTool/InDetTrigAmbiguityProcessor) to find
ambiguities in the track collection associated with SPSeeded.
Otherwise tracks are just copied on the output.

More information on processing ambiguities is given in the
TrkAmbiguityProcessor documentation.

 
@section used_packagesInDetTrigAmbiguitySolver Used packages
@htmlinclude used_packages.html

@section ReqInDetTrigAmbiguitySolver Requirements
@include requirements

*/
