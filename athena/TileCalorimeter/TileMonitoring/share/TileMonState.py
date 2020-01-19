from AthenaCommon.Logging import logging
tilemon_log = logging.getLogger('TileMonState.py')

#########################################
#
# Set db
# 
#########################################

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock("bytestream")
globalflags.DetGeo.set_Value_and_Lock("atlas")
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")



#########################################
#
# Example setup for ByteStreamEmonSvc
# 
#########################################

# Common part, copy from here

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

if 'EvtMax' not in dir():
    EvtMax = -1


include("ByteStreamEmonSvc/ByteStreamEmonInputSvc_jobOptions.py")

# #####################################################
# If using the ERSBootstrap.py file, enable the output
# via ERS
# ####################################################
#MessageSvc.useErs = True

# Define the input
ByteStreamEmonInputSvc = svcMgr.ByteStreamInputSvc

# ############################################################
# The name of the partition you want to connect to is taken
# from 'TDAQ_PARTITION' if it exists, otherwise from here.
# ############################################################

if 'Partition' not in dir():
    Partition="ATLAS"
ByteStreamEmonInputSvc.Partition = Partition


from ipc import IPCPartition
from ispy import ISObject
ipc_partition = IPCPartition(Partition);

if not ipc_partition.isValid():
    tilemon_log.error( 'Partition: ' + ipc_partition.name() + ' is not valid' )
    sys.exit(1)

### ATLAS partition: Read Global Run Parameters to configure the jobs
if Partition == 'ATLAS' or Partition == 'Tile':
    try:
        run_params = ISObject(ipc_partition, 'RunParams.SOR_RunParams', 'RunParams')
    except:
        tilemon_log.warning( "Could not find Run Parameters in IS - Set default beam type to 'cosmics'")
        beamType = 'cosmics'
    else:
        run_params.checkout()
        beam_type = run_params.beam_type
        beam_energy = run_params.beam_energy
        RunNumber = run_params.run_number
        project = run_params.T0_project_tag
        run_type = run_params.run_type
        tilemon_log.info( "RUN CONFIGURATION: run type: %s, beam type: %i, beam energy: %i, run number: %i, project tag: %s" 
                          % (run_type, beam_type, beam_energy, RunNumber, project) )
        

        if Partition == 'ATLAS':
            # define beam type based on project tag name
            if project[7:] == "cos" or project[5:] == "test":
                beamType = 'cosmics'
            elif project[7:] == '1beam':
                beamType = 'singlebeam'
                StreamType = 'express'
                KeyCount = 20
            else:
                beamType = 'collisions'
        
        else:
            if 'CIS mono' in run_type:
                TileMonoRun = True
                TileRunType = 8
                TileBiGainRun = False
            elif 'CIS' in run_type:
                TileCisRun = True
                TileRunType = 8
                TileBiGainRun = True
            elif 'Laser' in run_type:
                TileLasRun = True
                TileRunType = 2
                TileBiGainRun = False
            elif 'Pedestals' in run_type:
                TilePedRun = True
                TileRunType = 4
                TileBiGainRun = True


    if Partition == 'ATLAS':
        tilemon_log.info( 'Set up beam type: ' + beamType )
        TileNoiseFilter = 1
    else:
        TileNoiseFilter = 0

else:
    TileNoiseFilter = 0

# #########################################
# The source of events, SFI for full events
# #########################################
#ByteStreamEmonInputSvc.Key = "ReadoutApplication"
if 'Key' not in dir():
    Key="dcm"
ByteStreamEmonInputSvc.Key = Key

# ############################################################
# A list of of key values, e.g. a list of SFIs to contact.
# If not defined, all event providers of this type (e.g. all SFIs)
# ############################################################
#ByteStreamEmonInputSvc.KeyValue = ["TileREB-ROS" ]
#if 'KeyValue' not in dir():
#    KeyValue=[""]
#ByteStreamEmonInputSvc.KeyValue = KeyValue

if 'KeyValue' in dir():
    ByteStreamEmonInputSvc.KeyValue = KeyValue

# ##############################
# one of 'Ignore', 'Or', 'And'
# ##############################

if 'LVL1Logic' in dir():
    ByteStreamEmonInputSvc.LVL1Logic = LVL1Logic

# ###########################################################
# A LVL1 bit pattern to select events on.
#
# From 1 to 4 64bit values, first entry = Bit 0..63 in CTP
# ###########################################################
#ByteStreamEmonInputSvc.LVL1Bits = [ 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff]

# ###########################################################
# A list of L1 bit names to select on. This requires the
# L1CT.TrigConfL1Items to be published in IS.
# 
# This can be used instead of or in addition to LVL1Bits.
# ###########################################################
#ByteStreamEmonInputSvc.LVL1Names = ['L1_MU2', 'L1_MU3', 'L1_EM10' ]

if 'LVL1Names' in dir():
    ByteStreamEmonInputSvc.LVL1Names = LVL1Names
elif Partition == 'ATLAS':
    # make sure that we do not read calibration events in physics run
    if 'StreamType' not in dir():
        StreamType = "physics"
    if 'StreamLogic' not in dir():
        StreamLogic = "Or"

# ###########################################
# Selection by stream tag:
# One of 'Or', 'And', 'Ignore' (default)
# ###########################################

if 'StreamLogic' in dir():
    ByteStreamEmonInputSvc.StreamLogic = StreamLogic

# ###########################################
# set stream type
# One of 'physics' or 'calibration' or 'express'
# ############################################

if 'StreamType' in dir():
    ByteStreamEmonInputSvc.StreamType = StreamType

# ############################################
# A list of stream tag names
# ############################################

if 'StreamNames' in dir():
    ByteStreamEmonInputSvc.StreamNames = StreamNames

if 'KeyCount' in dir():
    ByteStreamEmonInputSvc.KeyCount = KeyCount
elif 'TriggerType' in dir():
    ByteStreamEmonInputSvc.KeyCount = 1000
    import os
    os.environ['TDAQ_IPC_TIMEOUT'] = '240000'
else:
    ByteStreamEmonInputSvc.KeyCount = 50
    
if 'TriggerType' in dir():
    ByteStreamEmonInputSvc.TriggerType = TriggerType


if 'Dispersion' in dir():
    ByteStreamEmonInputSvc.Dispersion = Dispersion
else:
    ByteStreamEmonInputSvc.Dispersion = True

# #################################################
# Shall athena exit if the partition is shutdown ?
# For offline athena tasks mainly.
# #################################################
#ByteStreamEmonInputSvc.ExitOnPartitionShutdown = False

if 'ISServer' not in dir():
    ISServer="Histogramming"
ByteStreamEmonInputSvc.ISServer = ISServer

if 'PublishName' not in dir():
    PublishName="TilePT-stateless"
ByteStreamEmonInputSvc.PublishName = PublishName

if 'Frequency' in dir():
    ByteStreamEmonInputSvc.Frequency = 301 #histograms update in number of events
else:
    if 'UpdatePeriod' not in dir():
        UpdatePeriod=60 #histograms update time in seconds
    try:
        ByteStreamEmonInputSvc.UpdatePeriod = UpdatePeriod
    except Exception:
        treatException("Could not set UpdatePeriod")

if 'BufferSize' not in dir():
    BufferSize=10
try:
     ByteStreamEmonInputSvc.BufferSize = BufferSize
except Exception:
     treatException("Could not set BufferSize")

if 'PublishInclude' in dir():
    ByteStreamEmonInputSvc.Include = PublishInclude

print ByteStreamEmonInputSvc

ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
theApp.ExtSvc += [ "ByteStreamCnvSvc"]

include("TileMonitoring/TileRec_topOptions.py")

# -----------------------------------------------------
# An example algorithm in Python
# -----------------------------------------------------
import AthenaPython.PyAthena as PyAthena

class ShowLVL1(PyAthena.Alg):
    """
    An athena algorithm written in python.
    """

    def __init__(self, name="ShowLVL1", **kw):
        kw['name'] = name
        super(ShowLVL1,self).__init__(name=name)
        # How to access properties
        self.SomeProperty = kw.get('SomeProperty', 'DefaultValue')

        self.ROBDataProviderSvc = None

    def initialize(self):
        self.msg.info("initializing [%s]", self.name())

        self.sg = PyAthena.py_svc("StoreGateSvc")
        if not self.sg:
            self.msg.error("Could not get StoreGateSvc")
            return PyAthena.StatusCode.Failure

        self.ROBDataProviderSvc = PyAthena.py_svc("ROBDataProviderSvc")
        if not self.ROBDataProviderSvc:
            self.msg.error("Could not get ROBDataProviderSvc")
            return PyAthena.StatusCode.Failure
        return PyAthena.StatusCode.Success

    def execute(self):
        self.msg.debug("executing [%s]", self.name())

        # Get EventInfo
        ev = self.sg.retrieve('EventInfo')

        # Print some LVL1 information
        print "Run:",ev.event_ID().run_number(),"Event:",ev.event_ID().event_number()
        for x in  ev.trigger_info().level1TriggerInfo():
            print "0x%08x" % x

        return PyAthena.StatusCode.Success        

    def finalize(self):
        self.msg.debug("finalizing [%s]", self.name())
        return PyAthena.StatusCode.Success        

ShowLVL1Algo = ShowLVL1("ShowMyLVL1")

#--------------------------------------------------------------
# Private Application Configuration options, replace with
# your configuration.
#--------------------------------------------------------------
#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()
#topSequence += ShowLVL1Algo

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#ShowLVL1Algo.SomeProperty = 'Some Value'
