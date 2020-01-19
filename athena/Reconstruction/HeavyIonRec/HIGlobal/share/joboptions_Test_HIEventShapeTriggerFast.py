#from AthenaCommon.AthenaCommonFlags import jobproperties as jp
#jp.AthenaCommonFlags.FilesInput = [ "01PoolFile/data12_hip.00210184.physics_MinBias.recon.ESD.r5762_tid01566321_00/ESD.01566321._001465.pool.root.1" ]

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [ "01PoolFile/data12_hip.00210184.physics_MinBias.recon.ESD.r5762_tid01566321_00/ESD.01566321._001465.pool.root.1" ]
theApp.EvtMax = 5

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
#GlobalFlags.DataSource.set_data()


svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=5)
include ("RecExCommon/AllDet_detDescr.py")
include ("/afs/cern.ch/user/s/soumya/testarea/19.2.1/Reconstruction/HeavyIonRec/HIRecExample/python/HIRecExampleFlags.py")
DetDescrVersion="ATLAS-GEO-10-00-00"
theApp.Dlls += [ "CaloTools", "TileRecAlgs", "LArClusterRec" ]



#The HIEventShapeFillerToolToolTriggerFast
HIEventShapeFillerToolTriggerFast_instance=CfgMgr.HIEventShapeFillerToolTriggerFast("HIEventShapeFillerToolTriggerFast")
HIEventShapeFillerToolTriggerFast_instance.OutputLevel=VERBOSE
ToolSvc += HIEventShapeFillerToolTriggerFast_instance


#the main algorithm sequence
algSeq = CfgMgr.AthSequencer("AthAlgSeq")


#Algorithm to Call the HIEventShapeFillerToolTriggerFast
HIEventShapeTriggerFastAlg=CfgMgr.HIEventShapeTriggerFast()
HIEventShapeTriggerFastAlg.OutputLevel                      =VERBOSE
HIEventShapeTriggerFastAlg.InputCellKey                     ="AllCalo"
HIEventShapeTriggerFastAlg.OutputContainerKey               ="HIEventShapeTriggerFast"
HIEventShapeTriggerFastAlg.HIEventShapeFillerToolTriggerFast=HIEventShapeFillerToolTriggerFast_instance
algSeq += HIEventShapeTriggerFastAlg


#Output file for histograms
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='myfile.root' OPT='RECREATE'"]


#Output xAOD and branches to keep
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outStream = MSMgr.NewPoolRootStream( "MyXAODStream", "myXAOD.pool.root");
outStream.AddItem(['xAOD::HIEventShapeContainer_v1#*'])
outStream.AddItem(['xAOD::HIEventShapeAuxContainer_v1#*'])
#outStream.AddItem(['CaloTowerContainer_v1#*'])
#outStream.AddItem(['CaloTowerAuxContainer_v1#*'])

