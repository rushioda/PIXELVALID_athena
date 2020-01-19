
#use muon alignments
print "Haas: Reading muon alignment constants from DB for reco"

from IOVDbSvc.CondDB import conddb
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC')

#from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
#MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
#MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
#                                      "/MUONALIGN/MDT/ENDCAP/SIDEA",
#                                      "/MUONALIGN/MDT/ENDCAP/SIDEC",
#                                      "/MUONALIGN/TGC/SIDEA",
#                                      "/MUONALIGN/TGC/SIDEC"]

#ToolSvc += MuonAlignmentDbTool
MGM_AlignmentDbTool = ToolSvc.MGM_AlignmentDbTool
#MGM_AlignmentDbTool.OutputLevel=DEBUG
#print MGM_AlignmentDbTool
from AtlasGeoModel.MuonGM import GeoModelSvc
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
MuonDetectorTool.UseConditionDb = 1
#MuonDetectorTool.OutputLevel=DEBUG

MuonDetectorTool.EnableFineClashFixing = 0 #this should be on for g4?
#print MuonDetectorTool
#print GeoModelSvc
#print ToolSvc

#Now use MC T0 for TRT during overlay digi, but MC or data T0/Rt during reco, depending on where digit came from... so this stuff is just for reco:
#conddb.blockFolder("/TRT/Calib/MCRT")
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=OFLP200</dbConnection> /TRT/Calib/MCRT" + "<tag> TrtCalibMCRt-13TeV_ArgonScenario1_00-00 </tag>" , force=True)
#conddb.blockFolder("/TRT/Calib/MCT0")
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=OFLP200</dbConnection> /TRT/Calib/MCT0" + "<tag> TrtCalibMCT0-13TeV_ArgonScenario1_00-00 </tag>" , force=True)
#conddb.blockFolder("/TRT/Calib/MCRT")
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=OFLP200</dbConnection>/TRT/Calib/MCRT" + "<tag> TrtCalibMCRt-13TeV_ArgonScenario1_00-00</tag>" , force=True)
#conddb.blockFolder("/TRT/Calib/MCT0")
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=OFLP200</dbConnection>/TRT/Calib/MCT0" + "<tag> TrtCalibMCT0-13TeV_ArgonScenario1_00-00</tag>" , force=True)
conddb.blockFolder("/TRT/Calib/MC/RT")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/MC/RT","TrtCalibRt-R2-MC-run2-scenario1_03-00",force=True,forceData=True)
conddb.addMarkup("/TRT/Calib/MC/RT","<forceRunNumber>295000</forceRunNumber>")
conddb.blockFolder("/TRT/Calib/MC/T0")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/MC/T0","TrtCalibT0-R2-MC-run2-scenario1_03-00",force=True,forceData=True)
conddb.addMarkup("/TRT/Calib/MC/T0","<forceRunNumber>295000</forceRunNumber>")
#conddb.blockFolder("/TRT/Calib/MCerrors2d")
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=OFLP200</dbConnection> /TRT/Calib/MCerrors2d" + "<tag> TrtCalibMCErrors2d-IOVdep-2Dfit-00-00 </tag>" , force=True)
#conddb.blockFolder("/TRT/Calib/MCslopes")
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=OFLP200</dbConnection> /TRT/Calib/MCslopes" + "<tag> TrtCalibMCSlopes-mc_25ns-2Dfit-00-00 </tag>" , force=True)
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
TRTMCCalibDBSvc=TRT_CalDbSvc(name="TRTMCCalibDBSvc",RtFolderName="/TRT/Calib/MC/RT",T0FolderName="/TRT/Calib/MC/T0",ErrorSlopeFolderName="/TRT/Calib/slopes",ErrorFolderName ="/TRT/Calib/errors2d")
#TRTMCCalibDBSvc=TRT_CalDbSvc(name="TRTMCCalibDBSvc",RtFolderName="/TRT/Calib/MC/RT",T0FolderName="/TRT/Calib/MC/T0",ErrorFolderName="/TRT/Calib/MCerrors2d") #,ErrorSlopeFolderName="/TRT/Calib/MCslopes"
ServiceMgr += TRTMCCalibDBSvc

#This would add another trtdriftfuntiontool, using the new trtcaldb:
#from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
#InDetTRT_MCDriftFunctionTool = TRT_DriftFunctionTool(name="InDetTRT_MCDriftFunctionTool",TRTCalDbTool=TRTMCCalibDBSvc)
#ToolSvc += InDetTRT_MCDriftFunctionTool
#but instead we'll just add the new trtcaldb to the existing trtdriftfunctiontool:
ToolSvc.InDetTRT_DriftFunctionTool.IsOverlay=True
ToolSvc.InDetTRT_DriftFunctionTool.TRTCalDbTool2=TRTMCCalibDBSvc

if "EOJT_noLorentz" in globals():
    print "EOJT_noLorentz found in globals(), so not doing Lorentz corrections"
    svcMgr.PixelSiliconConditionsSvc.ForceUseGeoModel=True
    svcMgr.PixelSiliconConditionsSvc.CheckGeoModel=True
    svcMgr.PixelSiliconConditionsSvc.UseDBForHV=False
    svcMgr.PixelLorentzAngleSvc.usePixelDefaults=True
else:
    print "EOJT_noLorentz not found in globals(), so doing standard Lorentz corrections"

