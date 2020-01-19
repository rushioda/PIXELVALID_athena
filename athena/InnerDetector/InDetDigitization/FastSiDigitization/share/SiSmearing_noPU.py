
#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.SkipEvents=0
athenaCommonFlags.EvtMax=-1
athenaCommonFlags.PoolHitsInput=["Hits.pool.root"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil')

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'Input'

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-SIM-00-00-00'

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion='ATLAS-GEO-20-00-01'
globalflags.DetDescrVersion='ATLAS-PX-ITK-00-00-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()

include("FastSiDigitization/SiSmearing_jobOptions.py")

DetFlags.pixel_setOff()
DetFlags.detdescr.pixel_setOn()

DetFlags.SCT_setOff() # setOn to use the standard digi
DetFlags.detdescr.SCT_setOn()

#DetFlags.SCT_setOff()
DetFlags.TRT_setOff() # setOn to use the standard digi

include("Digitization/Digitization.py")

StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.Dump = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.PixelManager = "Pixel"

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc += PixelCalibDbTool()

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
OutputNameString = 'CheckSmearing'

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
 
#ServiceMgr.THistSvc.Output  = ["val DATAFILE='"+OutputNameString+".root' TYPE = 'ROOT' OPT='RECREATE'"];


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.SystemOfUnits import *
import math

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

topSequence = AlgSequence()

topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.CheckSmear=True
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.OutputLevel=VERBOSE
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_X=10.0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_X=0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_Y=55.0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_Y=0*micrometer
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.TrackingGeometrySvc = AtlasTrackingGeometrySvc
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.UseCustomGeometry = True
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.DetectorElementMapName="Pixel_IdHashDetElementMap"
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.MergeClusters = False
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.PlanarClusterContainerName="Pixel_PlanarClusters"
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.PRD_TruthPlanarContainerName="PRD_MultiTruthPlanarPixel"
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.Nsigma = 1.0

topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.CheckSmear=True
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.SmearPixel = False
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.OutputLevel=VERBOSE
#topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.pitch_X= 70.0*micrometer
#topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.pitch_X= 0*micrometer
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.TrackingGeometrySvc = AtlasTrackingGeometrySvc
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.UseCustomGeometry = True
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.DetectorElementMapName="SCT_IdHashDetElementMap"
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.MergeClusters = False
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.PlanarClusterContainerName="SCT_PlanarClusters"
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.PRD_TruthPlanarContainerName="PRD_MultiTruthPlanarSCT"
##topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.pitch_X= 0*micrometer


topSequence.StreamRDO.ItemList += ["iFatras::PlanarClusterContainer#Pixel_PlanarClusters"]
topSequence.StreamRDO.ItemList += ["iFatras::PlanarClusterContainer#SCT_PlanarClusters"]

topSequence.StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPlanarPixel"]
topSequence.StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPlanarSCT"]

#from VP1Algs.VP1AlgsConf import VP1Alg 
#topSequence += VP1Alg()

