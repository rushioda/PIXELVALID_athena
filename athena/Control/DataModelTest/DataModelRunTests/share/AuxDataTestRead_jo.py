#
# $Id$
#
# File: share/AuxDataTestRead_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test reading objects with aux data.
#

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool
import AthenaPoolCnvSvc.ReadAthenaPool

#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
svcMgr.EventSelector.InputCollections        = [ "auxdata.root" ]

#--------------------------------------------------------------
# Define output
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::BAuxVec#bauxvec"]
fullItemList+=["DMTest::BAuxStandalone#b"]
fullItemList+=["DMTest::BAuxVec#copy_bauxvec"]
fullItemList+=["DMTest::BAuxStandalone#copy_b"]
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__AuxDataTestRead, \
     DMTest__AuxDataTestDecor, \
     DMTest__AuxDataTestClearDecor


topSequence += DMTest__AuxDataTestRead ('AuxDataTestRead',
                                        WritePrefix = 'copy_')

topSequence += DMTest__AuxDataTestDecor ('AuxDataTestDecor1',
                                         DecorName = 'dInt1',
                                         Offset = 100)

# Stream's output file
Stream1 = AthenaPoolOutputStream( "Stream1", asAlg = True )
Stream1.OutputFile =   "auxdata2.root"
Stream1.ItemList   += fullItemList # List of DO's to write out

topSequence += DMTest__AuxDataTestClearDecor ('AuxDataTestClearDecor')

topSequence += DMTest__AuxDataTestDecor ('AuxDataTestDecor2',
                                         DecorName = 'dInt2',
                                         Offset = 200)

# Stream's output file
Stream2 = AthenaPoolOutputStream( "Stream2", asAlg = True )
Stream2.OutputFile =   "auxdata2b.root"
Stream2.ItemList   += fullItemList # List of DO's to write out

# Note: can't autoload these.
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataReadCnvPoolCnv.so')


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

#svcMgr.ExceptionSvc.Catch = "None"

# Explicitly specify the output file catalog
# to avoid races when running tests in parallel.
PoolSvc = Service( "PoolSvc" )
PoolSvc.WriteCatalog = "file:AuxDataTestRead_catalog.xml"
