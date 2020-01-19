/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage InDetRawData Package
@author David Candlin, Veronique Boisvert, Maria.Jose.Costa@cern.ch

@section InDetRawDataIntro Introduction

This package is meant to contain the classes defining the Inner Detector
RawDataObjects, their collections and containers. There are different
classes for DC1/DC2 and combined test beam RawDataObjects (all of them
deriving from the same base class).

@section InDetRawDataOverview Class Overview
  The InDetRawData package contains the following classes:

   - InDetRawData: base class of the Inner Detector RDO

   - InDetRawDataCollection: Class to contain all the Raw Data Objects of a 
   detector element (module for Pixel or wafer for SCT or layer for TRT)

   - InDetRawDataContainer: Container for Raw Data Objects collections for 
   Pixel, SCT and TRT

   - PixelRDORawData: Class to implement RawData for Pixel

   - Pixel1RawData: Class to implement DC1/DC2 RawData for Pixel
   
   - PixelTB04RawData: Class to implement test beam 2004 RawData for Pixel

   - SCT_RDORawData: Class to implement RawData for SCT

   - SCT1_RawData: Class to implement DC1/DC2 RawData for SCT
   
   - SCT_TB04_RawData: Class to implement test beam 2004 RawData for SCT

   - TRT_RDORawData: Class to implement RawData for TRT

   - TRT_LoLumRawData: Class to implement DC1/DC2 RawData for TRT

   - TRT_TB04_RawData: Class to implement test beam 2004 RawData for TRT
   
@section InDetRawDataReq Requirements

@include requirements
*/
