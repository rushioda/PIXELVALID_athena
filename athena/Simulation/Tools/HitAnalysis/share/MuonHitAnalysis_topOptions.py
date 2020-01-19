#from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py" )

#Use these lines if the NSW is included in the simulation
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"

import os
from glob import glob
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = glob( "/tmp/"+os.environ['USER']+"/"+"HITS*root*" )
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput() # This is stupid and redundant, but necessary

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from HitAnalysis.HitAnalysisConf import RPCHitAnalysis
topSequence += RPCHitAnalysis('RPCHitAnalysis')
topSequence.RPCHitAnalysis.HistPath = '/MuonHitAnalysis/'
from HitAnalysis.HitAnalysisConf import MDTHitAnalysis
topSequence += MDTHitAnalysis('MDTHitAnalysis')
topSequence.MDTHitAnalysis.HistPath = '/MuonHitAnalysis/'
from HitAnalysis.HitAnalysisConf import CSCHitAnalysis
topSequence += CSCHitAnalysis('CSCHitAnalysis')
topSequence.CSCHitAnalysis.HistPath = '/MuonHitAnalysis/'
from HitAnalysis.HitAnalysisConf import TGCHitAnalysis
topSequence += TGCHitAnalysis('TGCHitAnalysis')
topSequence.TGCHitAnalysis.HistPath = '/MuonHitAnalysis/'

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "MuonHitAnalysis DATAFILE='MuonHitAnalysis.root' OPT='RECREATE'" ]

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

theApp.EvtMax = -1

ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = FALSE

# To set up a geometry
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo() # Configure the settings for the geometry
include("RecExCond/AllDet_detDescr.py") # Actually load the geometry
#include("TrkDetDescrSvc/AtlasTrackingGeometrySvc.py") # Tracking geometry, handy for ID work
