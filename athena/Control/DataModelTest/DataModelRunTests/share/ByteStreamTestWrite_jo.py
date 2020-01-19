#
# $Id$
#
# File: DataModelRunTests/share/ByteStreamTestWrite_jo.py
# Author: snyder@bnl.gov
# Date: Mar 2016
# Purpose: Test writing objects to bytestream.
#

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

bswrite = ['DMTest::CVec#cvec',
           'DMTest::CView#cview',
           'DMTest::HVec#hvec',
           'DMTest::HView#hview',
           ]

from TrigNavigation.TrigNavigationConf import HLT__Navigation
from DataModelTestDataWrite.DataModelTestDataWriteConf import \
     DMTest__xAODTestWriteCVec, \
     DMTest__xAODTestWriteHVec, \
     DMTest__xAODTestWriteCView, \
     DMTest__HLTResultWriter
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec",
                                          CVecKey = 'HLT_DMTest__CVec_cvec')
topSequence += DMTest__xAODTestWriteCView ("xAODTestWriteCView",
                                           CVecKey = 'HLT_DMTest__CVec_cvec',
                                           CViewKey = 'HLT_DMTest__CView_cview')
topSequence += DMTest__xAODTestWriteHVec ("xAODTestWriteHVec",
                                          HVecKey = 'HLT_DMTest__HVec_hvec',
                                          HViewKey = 'HLT_DMTest__HView_hview')
topSequence += DMTest__HLTResultWriter \
               ("HLTResultWriter",
                Nav = HLT__Navigation (ClassesToPayload = bswrite,
                                       ClassesToPreregister = bswrite))

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# Note: can't autoload these.
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataWriteDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataWriteCnvPoolCnv.so')
ROOT.DMTest.setTrigConverterLibrary ('libDataModelTestDataWriteSerCnv.so')

# ItemList:
fullItemList = []
fullItemList+=["HLT::HLTResult#HLTResult_HLT"]

# Stream's output file
import os
try:
    os.remove('test.bs')
except OSError:
    pass
import os
try:
    os.remove('test.bs.writing')
except OSError:
    pass
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.BSRDOOutput = 'test.bs'
from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
# List of DO's to write out
StreamBSFileOutput.ItemList   += fullItemList

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.ClassIDSvc.OutputLevel = 3

# No stats printout
ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoPrintOutTable = FALSE
ChronoStatSvc.PrintUserTime       = FALSE
ChronoStatSvc.StatPrintOutTable   = FALSE
