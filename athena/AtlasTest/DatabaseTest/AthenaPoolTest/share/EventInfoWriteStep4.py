###############################################################
#
# Job options file - read EventInfo and add in Dummy tags
#
## @file EventInfoWriteStep4.py
##
## @brief For Athena POOL test: read in EventInfo with tags and write
## dummy tags with override EventInfoTags
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# Define the input parameters
#--------------------------------------------------------------

svcMgr.EventSelector.InputCollections        = [ "SimplePoolFile2.root" ]

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags import rec

# For general flags
rec.doAOD     = False
rec.doTrigger = False
rec.doWriteTAG  = False
DetDescrVersion = "ATLAS-GEO-17-00-00"
include( "RecExCond/RecExCommon_flags.py" )

# Set local flags - only need LAr DetDescr
#DetFlags.detdescr.ID_setOff()
#DetFlags.detdescr.Calo_setOff()
#DetFlags.detdescr.LAr_setOff()
#DetFlags.detdescr.Tile_setOff()
#DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()

# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")
include( "IOVDbSvc/IOVRecExCommon.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 25

#--------------------------------------------------------------
# Application: load TagInfoMgr
#--------------------------------------------------------------
import EventInfoMgt.EventInfoMgtInit

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolTest.AthenaPoolTestConf import EventInfoWriter
topSequence += EventInfoWriter( "EventInfoWriter" )

#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )

print fullItemList


# Run OutputStream as an algorithm
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1", "SimplePoolFile3.root", asAlg = True )

# List of DO's to write out
Stream1.ItemList   += fullItemList

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc = Service( "MessageSvc" )
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = INFO

EventInfoWriter.OutputLevel = DEBUG
#EventInfoWriter.CreateDummyTags = True
EventInfoWriter.CreateDummyOverrideTags = True


# TagInfoMgr r/w TagInfo
svcMgr.TagInfoMgr.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
