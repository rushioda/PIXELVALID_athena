# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# configuration used for Tier-0 cosmic processing
# extracted from 14.2.23 RecExCommission_ATN.py setup 10/10/08
#
FolderList=['<db>COOLONL_INDET/COMP200</db> /Indet/Align', '<db>COOLONL_TRT/COMP200</db> /TRT/Align', '<db>COOLONL_INDET/COMP200</db> /Indet/PixelDist', '<db>COOLONL_LAR/COMP200</db> /LAR/Align', '<db>COOLONL_GLOBAL/COMP200</db> /GLOBAL/BField/Map <tag>BFieldMap-000</tag>', '<db>COOLONL_INDET/COMP200</db> /Indet/Beampos', '<db>COOLONL_LAR/COMP200</db> /LAR/Identifier/OnOffIdAtlas', '<db>COOLONL_LAR/COMP200</db> /LAR/Identifier/FebRodAtlas', '<db>COOLONL_LAR/COMP200</db> /LAR/BadChannels/BadChannels', '<db>COOLONL_LAR/COMP200</db> /LAR/BadChannels/MissingFEBs', '<db>COOLONL_LAR/COMP200</db> /LAR/ElecCalibOnl/DAC2uA', '<db>COOLONL_LAR/COMP200</db> /LAR/ElecCalibOnl/MphysOverMcal<channelSelection>3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022</channelSelection>', '<db>COOLONL_LAR/COMP200</db> /LAR/ElecCalibOnl/Pedestal<channelSelection>3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022</channelSelection>', '<db>COOLONL_LAR/COMP200</db> /LAR/ElecCalibOnl/Ramp<channelSelection>3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022</channelSelection>', '<db>COOLONL_LAR/COMP200</db> /LAR/ElecCalibOnl/uA2MeV', '<db>COOLONL_LAR/COMP200</db> /LAR/ElecCalibOnl/HVScaleCorr', '<db>COOLONL_LAR/COMP200</db> /LAR/ElecCalib/OFC/PhysWave/RTM/5samples3bins17phases<channelSelection>3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022</channelSelection>', '<db>COOLONL_LAR/COMP200</db> /LAR/Identifier/LArTTCellMapAtlas<tag> LARIdentifierLArTTCellMapAtlas-DC3-05 </tag>', '<db>COOLONL_CALO/COMP200</db> /CALO/Identifier/CaloTTOnOffIdMapAtlas', '<db>COOLONL_CALO/COMP200</db> /CALO/Identifier/CaloTTOnAttrIdMapAtlas', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/STATUS/ADC', '<db>COOLONL_TILE/COMP200</db> /TILE/ONL01/STATUS/ADC', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/CALIB/CIS/FIT/LIN', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/CALIB/CIS/FIT/NLN', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/CALIB/LAS/LIN', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/CALIB/LAS/NLN', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/CALIB/CES', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/CALIB/EMS', '<db>COOLONL_TILE/COMP200</db> /TILE/ONL01/CALIB/CIS/LIN', '<db>COOLONL_TILE/COMP200</db> /TILE/ONL01/CALIB/LAS/LIN', '<db>COOLONL_TILE/COMP200</db> /TILE/ONL01/CALIB/CES', '<db>COOLONL_TILE/COMP200</db> /TILE/ONL01/CALIB/EMS', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/NOISE/SAMPLE', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/NOISE/AUTOCR', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/TIME/CELLFIBERLENGTH', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/TIME/DRAWEROFFSET/LAS', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/TIME/CHANNELOFFSET/LAS', '<db>COOLONL_TILE/COMP200</db> /TILE/OFL01/PULSESHAPE/PHY', '<db>COOLONL_MDT/COMP200</db> /MDT/T0', '<db>COOLONL_MDT/COMP200</db> /MDT/RT', '<db>COOLOFL_GLOBAL/COMP200</db> /GLOBAL/DETSTATUS/LBSUMM', '<db>COOLONL_INDET/COMP200</db> /Indet/TrackingGeo/LayerMaterial<tag>TagInfo/InDetLayerMat_v6_/GeoAtlas</tag>', '<db>COOLONL_TRIGGER/COMP200</db> /TRIGGER/HLT/Menu <tag>HEAD</tag>', '<db>COOLONL_TRIGGER/COMP200</db> /TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>', '<db>COOLONL_TRIGGER/COMP200</db> /TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>', '<db>COOLONL_TRIGGER/COMP200</db> /TRIGGER/LVL1/Menu <tag>HEAD</tag>', '<db>COOLONL_TRIGGER/COMP200</db> /TRIGGER/LVL1/Prescales <tag>HEAD</tag>', '<db>COOLONL_CSC/COMP200</db> /CSC/PED', '<db>COOLONL_CSC/COMP200</db> /CSC/NOISE', '<db>COOLONL_CSC/COMP200</db> /CSC/PSLOPE', '<db>COOLONL_CALO/COMP200</db> /CALO/Noise/CellNoise', '<db>COOLONL_CALO/COMP200</db> /CALO/HadCalibration/CaloEMFrac <tag>CaloEMFRac-003-01</tag>', '<db>COOLONL_CALO/COMP200</db> /CALO/HadCalibration/H1ClusterCellWeights <tag>CaloH1CellWeights-003-01</tag>', '<db>COOLONL_CALO/COMP200</db> /CALO/HadCalibration/CaloOutOfCluster <tag>CaloHadOOCCorr-003-01</tag>', '<db>COOLONL_CALO/COMP200</db> /CALO/HadCalibration/CaloOutOfClusterPi0 <tag>CaloHadOOCCorrPi0-003-01</tag>', '<db>COOLONL_CALO/COMP200</db> /CALO/HadCalibration/CaloDMCorr2 <tag>CaloHadDMCorr-002-00</tag>', '<db>COOLONL_PIXEL/COMP200</db> /PIXEL/PixCalib', '<db>COOLONL_PIXEL/COMP200</db> /PIXEL/PixMap', '<db>COOLONL_PIXEL/COMP200</db> /PIXEL/NoiseMap', '<db>COOLONL_PIXEL/COMP200</db> /PIXEL/PixMapOverlay', '<db>COOLOFL_DCS/COMP200</db> /PIXEL/DCS/TEMPERATURE', '<db>COOLOFL_DCS/COMP200</db> /PIXEL/DCS/HV', '<db>COOLOFL_DCS/COMP200</db> /PIXEL/DCS/FSMSTATUS', '<db>COOLOFL_DCS/COMP200</db> /PIXEL/DCS/FSMSTATE', '<db>COOLONL_PIXEL/COMP200</db> /PIXEL/PixReco', '<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Chip', '<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Module', '<db>COOLONL_SCT/COMP200</db> /SCT/Derived/Monitoring', '<db>COOLONL_TRT/COMP200</db> /TRT/Calib/RT', '<db>COOLONL_TRT/COMP200</db> /TRT/Calib/T0', '<db>COOLONL_TRT/COMP200</db> /TRT/Cond/Status', '<db>COOLOFL_DCS/COMP200</db> /TRT/DCS/HV/BARREL <cache>600</cache>', '<db>COOLOFL_DCS/COMP200</db> /TRT/DCS/HV/ENDCAPA <cache>600</cache>', '<db>COOLOFL_DCS/COMP200</db> /TRT/DCS/HV/ENDCAPC <cache>600</cache>', '<db>COOLONL_INDET/COMP200</db> /Indet/TrkErrorScaling', '<db>COOLONL_TRT/COMP200</db> /TRT/Calib/PID', '<db>COOLONL_TRT/COMP200</db> /TRT/Calib/PID_RToT', '<db>COOLONL_INDET/COMP200</db> /Indet/Beampos']

# extra DCS data not yet being read in production
FolderList+=['<db>COOLOFL_DCS/COMP200</db> /LAR/DCS/HV/BARREL/I8','<db>COOLOFL_DCS/COMP200</db> /LAR/DCS/HV/BARREl/I16','<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/HV','<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/MPS/LV']