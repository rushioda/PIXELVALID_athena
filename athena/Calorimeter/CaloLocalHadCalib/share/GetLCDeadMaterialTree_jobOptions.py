# File: CaloLocalHadCalib/share/GetLCDeadMaterialTree_jobOptions.py
# Created: Febryary 2009, G. Pospelov
# Purpose: Generate special small DeadMaterialTree from ESD

from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCDeadMaterialTree
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence

GetLCDM = GetLCDeadMaterialTree("GetLCDM")
GetLCDM.HadDMCoeffInitFile = "CaloHadDMCoeff_init_v2.txt"
GetLCDM.ClusterCollectionName = "CaloTopoCluster"
GetLCDM.ClusterCollectionNameCalib = "CaloCalTopoCluster"
GetLCDM.doSaveCalibClusInfo = False # to save additional info from collection with calibrated clusters
if vars().has_key('outFileNameLCDM'):
  GetLCDM.OutputFileName = outFileNameLCDM
else:
  GetLCDM.OutputFileName = "DeadMaterialTree.root"


topSequence += GetLCDM
