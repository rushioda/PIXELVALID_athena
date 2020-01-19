#
# $Id$
#
# File: share/xAODTestWrite_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test writing xAOD objects.
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
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool



#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

from DataModelTestDataWrite.DataModelTestDataWriteConf import \
     DMTest__xAODTestWriteCVec, \
     DMTest__xAODTestWriteHVec, \
     DMTest__xAODTestWriteCView, \
     DMTest__xAODTestWriteCInfo, \
     DMTest__xAODTestWriteCVecConst, \
     DMTest__xAODTestWrite
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec")
topSequence += DMTest__xAODTestWriteHVec ("xAODTestWriteHVec")
topSequence += DMTest__xAODTestWriteCView ("xAODTestWriteCView")
topSequence += DMTest__xAODTestWriteCInfo ("xAODTestWriteCInfo")
topSequence += DMTest__xAODTestWrite ("xAODTestWrite")
topSequence += DMTest__xAODTestWriteCVecConst ("xAODTestWriteCVecConst")


#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataWriteDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataWriteCnvPoolCnv.so')

theApp.CreateSvc += ['xAODMaker::EventFormatSvc']

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::CVec#cvec"]
fullItemList+=["DMTest::CAuxContainer#cvecAux."]
fullItemList+=["DMTest::CVecWithData#cvecWD"]
fullItemList+=["DMTest::CView#cview"]
fullItemList+=["DMTest::CAuxContainer#cvecWDAux."]
fullItemList+=["DMTest::GVec#gvec"]
fullItemList+=["DMTest::GAuxContainer#gvecAux."]
fullItemList+=["DMTest::CVec#ctrig"]
fullItemList+=["DMTest::CTrigAuxContainer#ctrigAux."]
fullItemList+=["DMTest::C#cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer#cinfoAux."]
fullItemList+=["DMTest::HVec#hvec"]
fullItemList+=["DMTest::HAuxContainer#hvecAux."]
fullItemList+=["DMTest::HView#hview"]

from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatSvc
fmtsvc = xAODMaker__EventFormatSvc (FormatNames = 
                                    ['DataVector<DMTest::C_v1>',
                                     'DMTest::CVecWithData_v1',
                                     'DMTest::CAuxContainer_v1',
                                     'DMTest::CTrigAuxContainer_v1',
                                     'ViewVector<DataVector<DMTest::C_v1,DataModel_detail::NoBase> >',
                                     'DMTest::C_v1',
                                     'DMTest::CInfoAuxContainer_v1',
                                     'DataVector<DMTest::G_v1>',
                                     'DMTest::GAuxContainer_v1',
                                     'DMTest::G_v1',
                                     'DMTest::H_v1',
                                     'DataVector<DMTest::H_v1>',
                                     'ViewVector<DataVector<DMTest::H_v1,DataModel_detail::NoBase> >',
                                     'DMTest::HAuxContainer_v1',
                                     ])
ServiceMgr += fmtsvc

# Stream's output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
Stream1_Augmented = MSMgr.NewPoolStream ('Stream1', 'xaoddata.root', asAlg=True)
Stream1_Augmented.AddMetaDataItem ('xAOD::EventFormat#EventFormat')
Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
# List of DO's to write out
Stream1.ItemList   += fullItemList
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_SPLITLEVEL='1'"]


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

# Explicitly specify the output file catalog
# to avoid races when running tests in parallel.
PoolSvc = Service( "PoolSvc" )
PoolSvc.WriteCatalog = "file:xAODTestWrite_catalog.xml"
