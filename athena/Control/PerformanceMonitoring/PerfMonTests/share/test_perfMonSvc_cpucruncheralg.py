###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

if not 'doMonitoring' in dir():
    doMonitoring = True
    pass
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = doMonitoring
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING,ERROR
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 100
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

## schedule a noop algorithm: it just print where it is
from PerfMonTests.PerfMonTestsConf import PerfMonTest__CpuCruncherAlg
cpuCruncher = PerfMonTest__CpuCruncherAlg( "CpuCruncherAlg", OutputLevel = INFO )
cpuCruncher.MeanCpu = 100. # ms
cpuCruncher.RmsCpu  =   5. # ms
topSequence += cpuCruncher

svcMgr.MessageSvc.OutputLevel = ERROR

from PerfMonComps.PerfMonCompsConf import PerfMon__CallGraphBuilderSvc
svcMgr += PerfMon__CallGraphBuilderSvc( "CallGraphSvc" )
theApp.CreateSvc += [ svcMgr.CallGraphSvc.getFullJobOptName() ]
svcMgr.AuditorSvc.Auditors += [ "PerfMon::CallGraphAuditor" ]
#==============================================================
#
# End of job options file
#
###############################################################