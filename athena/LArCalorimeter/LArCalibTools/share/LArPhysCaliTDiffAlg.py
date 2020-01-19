if 'FebMG' not in dir():
   FebMG=False

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-20-00-01"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit 

#Get identifier mapping (needed by LArConditionsContainer)
                           
svcMgr.IOVDbSvc.GlobalTag="LARCALIB-000-02"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = 500000

dbname="<db>COOLOFL_LAR/COMP200</db>"

conddb.addFolder("","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>"+dbname)
conddb.addFolder("","/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>"+dbname)
conddb.addFolder("","/LAR/ElecCalibOfl/PhysCaliTdiff<key>input</key><tag>LARElecCalibOflPhysCaliTdiff-gaincorr-01</tag>"+dbname)


from LArCalibTools.LArCalibToolsConf import LArPhysCaliTDiffAlg
theLArPhysCaliTDiffAlg=LArPhysCaliTDiffAlg()
theLArPhysCaliTDiffAlg.OutputLevel=DEBUG
theLArPhysCaliTDiffAlg.AddFEBTempInfo=False
theLArPhysCaliTDiffAlg.InputContainer="input"
theLArPhysCaliTDiffAlg.FileName="FebOffsets_gain.txt"
theLArPhysCaliTDiffAlg.PerFebMGCorr=FebMG

topSequence+=theLArPhysCaliTDiffAlg


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='timediff.root' OPT='NEW'" ]

#svcMgr.DetectorStore.Dump=True
#svcMgr.MessageSvc.OutputLevel = DEBUG

svcMgr.IOVDbSvc.DBInstance=""
svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml",
                               ]


#svcMgr.MessageSvc.debugLimit = 5000000


OutputPoolFile="AdjustedTimeOffsets.pool.root"
outObject="LArPhysCaliTdiffComplete#LArPhysCaliTdiff#/LAR/ElecCalibOfl/PhysCaliTdiff"
outTag="LARElecCalibOflPhysCaliTdiff-gaincorr-02"

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",OutputPoolFile,
                                          [outObject],[outTag],True)


from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True


svcMgr.IOVDbSvc.dbConnection="sqlite://;schema=tdiff_run2.db;dbname=CONDBR2"
