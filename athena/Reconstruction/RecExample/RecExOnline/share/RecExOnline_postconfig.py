# #########################################

# ----------------------------- Catalog file
Service("PoolSvc").SortReplicas = False                                  # NEEDS TO BE CHECKED
PoolSvc = Service( "PoolSvc" )
if isOnline and useEmon:
  PoolSvc.ReadCatalog += ["xmlcatalog_file:/sw/DbData/poolcond/PoolCat_comcond.xml"]
#  PoolSvc.ReadCatalog += ["xmlcatalog_file:/sw/DbSuppForMx/poolcond/PoolCat_comcond.xml"]
#  PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/lar/lar/project/databases/cond08_data.000001.lar.COND/PoolFileCatalog.xml"]
#  PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/lar/lar/project/databases/comcond.000006.lar_conditions.recon.pool.v0000/PoolFileCatalog.xml"]

# AK: 10-07-2014: fixing this error
#AthenaPoolConverter                                 ERROR poolToObject: caught error: FID "22CA8C5E-C583-DD11-A2EC-000423D94D30" is not existing in the catalog ( POOL : "PersistencySvc::UserDatabase::connectForRead" from "PersistencySvc" )
#AthenaPoolConverter                                 ERROR createObj PoolToDataObject() failed, Token = [DB=22CA8C5E-C583-DD11-A2EC-000423D94D30][CNT=CollectionTree(CaloRec::ToolConstants/H1WeightsCone4Topo)][CLID=5A73ABF8-5095-405B-A296-3435BB52A5FD][TECH=00000202][OID=0000000000000003-0000000000000000]
#DataProxy                                         WARNING accessData: conversion failed for data object 250904980//CALO/H1Weights/H1WeightsCone4Topo
###  PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/dqm/GlobalMonitoring/AtlasProduction-19.1.0.1/PoolFileCatalog.xml"]

# SMW: 19-11-2014: fixing this error
# PoolCollectionConverter                           WARNING Unable to create Collection: FID:BC292F26-AE73-9041-BF5C-BCE6C5C651EC
# PoolCollectionConverter                           WARNING FID "BC292F26-AE73-9041-BF5C-BCE6C5C651EC" is not existing in the catalog ( POOL : "PersistencySvc::UserDatabase::connectForRead" from "PersistencySvc" )
# CondProxyProvider                                   ERROR Unable to open: FID:BC292F26-AE73-9041-BF5C-BCE6C5C651EC

### PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/dqm/GlobalMonitoring/SMW_test/AtlasProduction-19.3.0.1_19nov14/PoolFileCatalog.xml"]
  PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/dqm/GlobalMonitoring/PoolFileCatalog_M7/PoolFileCatalog.xml"]

#ToolSvc.InDetCosmicsEventPhaseTool.GlobalOffset = 0


###
### Fix problem in PixelConditionsTools
### From Fares Djama
### " the RUN2 pixel tags have been copied from RUN1,
###   while we have a new detector, the IBL.
###   We need to add a new IOV to our tags in the online.
###   I guess that's the same for all other condb folders"
###


#
# This part is only for online (SMWANG : Nov 17th 2015)
#

#from IOVDbSvc.CondDB import conddb
#
#if not conddb.folderRequested("PIXEL/PixReco"):
#        conddb.addFolder('PIXEL','/PIXEL/PixReco')
#if not conddb.folderRequested("PIXEL/PixCalib"):
#        conddb.addFolder('PIXEL','/PIXEL/PixCalib')
#conddb.blockFolder("/PIXEL/PixCalib")
#conddb.blockFolder("/PIXEL/PixReco")
#
#conddb.addFolderWithTag("/det/indet/M8/AtlasProduction-20.1.0.1/NewHLT.db;dbname=CONDBR2","/PIXEL/PixReco","PixReco-HLT-RUN12-000-01",force=True) 
#conddb.addFolderWithTag("/det/indet/M8/AtlasProduction-20.1.0.1/NewHLT.db;dbname=CONDBR2","/PIXEL/PixCalib","PixReco-HLT-RUN12-000-01",force=True) 


#
# Need to get this part to work online and offline !!! (SMWANG : Nov 17th 2015)
#

if (isOnline and not isOfflineTest):
  from IOVDbSvc.CondDB import conddb
  if not conddb.folderRequested("PIXEL/PixReco"):
    conddb.addFolder('PIXEL','/PIXEL/PixReco')
  if not conddb.folderRequested("PIXEL/PixCalib"):
    conddb.addFolder('PIXEL','/PIXEL/PixCalib')
  conddb.blockFolder("/PIXEL/PixCalib")
  conddb.blockFolder("/PIXEL/PixReco")
  conddb.addFolderWithTag("/det/indet/M8/AtlasProduction-20.1.0.1/NewHLT.db;dbname=CONDBR2","/PIXEL/PixReco","PixReco-HLT-RUN12-000-01",force=True) 
  conddb.addFolderWithTag("/det/indet/M8/AtlasProduction-20.1.0.1/NewHLT.db;dbname=CONDBR2","/PIXEL/PixCalib","PixReco-HLT-RUN12-000-01",force=True) 


if (isOnline and isOfflineTest):
  from IOVDbSvc.CondDB import conddb
  if not conddb.folderRequested("PIXEL/PixReco"):
    conddb.addFolder('PIXEL','/PIXEL/PixReco')
  if not conddb.folderRequested("PIXEL/PixCalib"):
    conddb.addFolder('PIXEL','/PIXEL/PixCalib')
  conddb.blockFolder("/PIXEL/PixCalib")
  conddb.blockFolder("/PIXEL/PixReco")
  conddb.addFolderWithTag("/afs/cern.ch/work/s/smwang/public/DQM/InDet_online_db/NewHLT.db;dbname=CONDBR2","/PIXEL/PixReco","PixReco-HLT-RUN12-000-01",force=True) 
  conddb.addFolderWithTag("/afs/cern.ch/work/s/smwang/public/DQM/InDet_online_db/NewHLT.db;dbname=CONDBR2","/PIXEL/PixCalib","PixReco-HLT-RUN12-000-01",force=True) 


# ----------------------------- Printout
print "CHECK POINT PRINTING"
globalflags.print_JobProperties()


