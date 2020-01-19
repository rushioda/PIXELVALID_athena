
from IOVDbSvc.CondDB import conddb

########################################
#These top ones are MC folders (and other random things) and shouldn't have to change much

#use MC TRT RT functions
conddb.blockFolder("/TRT/Calib/RT")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/RT","TrtCalibRt-MCnewDigi_ToTon_7TeV_00-00",force=True,forceMC=True)

#use MC MDT RT functions
#conddb.blockFolder("/MDT/RT")
#conddb.addFolderWithTag("MDT_OFL","/MDT/RT","MDTRT_Sim-00",force=True,forceMC=True)
#conddb.blockFolder("/MDT/RTBLOB")
#conddb.addFolderWithTag("MDT_OFL","/MDT/RTBLOB","MDTRT_Sim-00-BLOB",force=True,forceMC=True)

conddb.blockFolder("/PIXEL/HLT/DCS/HV")
conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/HV","PixDCSHV-UPD1-00",force=True,forceData=True)
conddb.blockFolder("/PIXEL/HLT/DCS/TEMPERATURE")
conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/TEMPERATURE","PixDCSTemp-UPD1-00",force=True,forceData=True)
conddb.blockFolder("/PIXEL/ReadoutSpeed")
conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed",force=True)
conddb.blockFolder("/PIXEL/PixReco")
conddb.addFolderWithTag("PIXEL_OFL","/PIXEL/PixReco","PixReco-SIM-RUN12-000-01",force=True,forceMC=True)

conddb.blockFolder("/TRT/Cond/DigVers")
conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True)

conddb.addOverride("/LAR/ElecCalibOfl/Shape/RTM/5samples1phase","LARElecCalibOflShapeRTM5samples1phase-UPD1-04")

###################################################
print "Haas: RT OVERRIDE, for COMCOND-BLKPA-006-11"
#See https://twiki.cern.ch/twiki/pub/AtlasComputing/ConditionsTagComCondBlkpa00611/checkDB_COMCOND-BLKPA-006-11-17.2.10.6.txt

#use data TRT T0 constants
conddb.blockFolder("/TRT/Calib/T0")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/T0","TrtCalibT0-Physics-BLK-UPD4-00-01",force=True,forceData=True)

#use data MDT T0 constants
#conddb.blockFolder("/MDT/T0")
#conddb.addFolderWithTag("MDT_OFL","/MDT/T0","MDTT0-UPD4-11",force=True,forceData=True)
conddb.blockFolder("/MDT/T0BLOB")
conddb.addFolderWithTag("MDT_OFL","/MDT/T0BLOB","MDTT0-UPD4-11",force=True,forceData=True)

conddb.blockFolder("/TRT/Align")
conddb.addFolderWithTag("TRT_OFL","/TRT/Align","TRTAlign-BLK-UPD4-07",force=True,forceData=True)
conddb.blockFolder("/LAR/Align")
conddb.addFolderWithTag("LAR_ONL","/LAR/Align","LARAlign-2012-00",force=True,forceData=True)
conddb.blockFolder("/Indet/Align")
conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign-BLK-UPD4-08",force=True,forceData=True)

conddb.blockFolder("/MUONALIGN/MDT/BARREL")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEA")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEC")
conddb.blockFolder("/MUONALIGN/TGC/SIDEA")
conddb.blockFolder("/MUONALIGN/TGC/SIDEC")

#use updated muon alignments
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-BA_ROLLING_04-BLKP-UPD4-00",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-ECA_ROLLING_2012_03_01-UPD4-01",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-ECC_ROLLING_2012_03_01-UPD4-01",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-BA_ROLLING_2010_03-BLKP-UPD4-00",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-ECA_ROLLING_2011_02_02-UPD4-01",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-ECC_ROLLING_2011_02_02-UPD4-01",force=True,forceData=True)
    
#the TGC alignments had overlap issues... caused G4 crash
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-TGCA_ROLLING_2011_01-ES1-UPD1-00",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-TGCC_ROLLING_2011_01-ES1-UPD1-00",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-0001-DEFAULT",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-0001-DEFAULT",force=True,forceData=True)

conddb.blockFolder("/SCT/DAQ/Configuration/Chip")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/Chip","/SCT/DAQ/Configuration/Chip",force=True)
conddb.blockFolder("/SCT/DAQ/Configuration/Module")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/Module","/SCT/DAQ/Configuration/Module",force=True)
