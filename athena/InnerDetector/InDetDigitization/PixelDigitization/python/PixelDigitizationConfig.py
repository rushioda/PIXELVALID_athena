# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from Digitization.DigitizationFlags import digitizationFlags
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags

# The earliest bunch crossing time for which interactions will be sent
# to the Pixel Digitization code.
def Pixel_FirstXing():
    FirstXing = -50
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        FirstXing = -25
    return FirstXing

# The latest bunch crossing time for which interactions will be sent
# to the Pixel Digitization code.
def Pixel_LastXing():
    LastXing = 100
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        if jobproperties.Beam.bunchSpacing.get_Value() > 50 :
            LastXing = 75
        else :
            LastXing = 25
    return LastXing

def ChargeCollProbSvc(name="ChargeCollProbSvc", **kwargs):
    return CfgMgr.ChargeCollProbSvc(name, **kwargs)

def SurfaceChargesTool(name="SurfaceChargesTool", **kwargs):
    if hasattr(digitizationFlags, "doBichselSimulation") and digitizationFlags.doBichselSimulation():
        kwargs.setdefault("PixelBarrelChargeTool","PixelBarrelBichselChargeTool")
        kwargs.setdefault("PixelECChargeTool","PixelECBichselChargeTool")
        kwargs.setdefault("IblPlanarChargeTool","IblPlanarBichselChargeTool")
        kwargs.setdefault("Ibl3DChargeTool","Ibl3DBichselChargeTool")
    else:
        kwargs.setdefault("PixelBarrelChargeTool","PixelBarrelChargeTool")
        kwargs.setdefault("PixelECChargeTool","PixelECChargeTool")
        kwargs.setdefault("IblPlanarChargeTool","IblPlanarChargeTool")
        kwargs.setdefault("Ibl3DChargeTool","Ibl3DChargeTool")
    kwargs.setdefault("DBMChargeTool","DBMChargeTool") # No separate implementation when using Bichsel model
    kwargs.setdefault("doITk", GeometryFlags.isSLHC())
    return CfgMgr.SurfaceChargesTool(name, **kwargs)

def DBMChargeTool(name="DBMChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.DBMChargeTool(name, **kwargs)

###############################################################################

def BichselSimTool(name="BichselSimTool", **kwargs):
    kwargs.setdefault("DeltaRayCut", 117.)
    kwargs.setdefault("nCols", 5)
    kwargs.setdefault("LoopLimit", 100000)
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.BichselSimTool(name, **kwargs)

def PixelBarrelBichselChargeTool(name="PixelBarrelBichselChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    kwargs.setdefault("doBichsel", hasattr(digitizationFlags, "doBichselSimulation") and digitizationFlags.doBichselSimulation())
    kwargs.setdefault("doBichselBetaGammaCut", 0.7)   # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)            # needs validation
    kwargs.setdefault("doPU", True)
    kwargs.setdefault("BichselSimTool", "BichselSimTool")
    # kwargs.setdefault("OutputFileName", digitizationFlags.BichselOutputFileName())
    # kwargs.setdefault("doHITPlots", True)
    return CfgMgr.PixelBarrelBichselChargeTool(name, **kwargs)

def PixelECBichselChargeTool(name="PixelECBichselChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    kwargs.setdefault("doBichsel", hasattr(digitizationFlags, "doBichselSimulation") and digitizationFlags.doBichselSimulation())
    kwargs.setdefault("doBichselBetaGammaCut", 0.7)   # dEdx not quite consistent below this
    kwargs.setdefault("doPU", True)
    kwargs.setdefault("BichselSimTool", "BichselSimTool")
    return CfgMgr.PixelECBichselChargeTool(name, **kwargs)

def IblPlanarBichselChargeTool(name="IblPlanarBichselChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    kwargs.setdefault("doBichsel", hasattr(digitizationFlags, "doBichselSimulation") and digitizationFlags.doBichselSimulation())
    kwargs.setdefault("doBichselBetaGammaCut", 0.7)   # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)            # needs validation
    kwargs.setdefault("doPU", True)
    kwargs.setdefault("BichselSimTool", "BichselSimTool")
    return CfgMgr.IblPlanarBichselChargeTool(name, **kwargs)

def Ibl3DBichselChargeTool(name="Ibl3DBichselChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    kwargs.setdefault("doBichsel", hasattr(digitizationFlags, "doBichselSimulation") and digitizationFlags.doBichselSimulation())
    kwargs.setdefault("doBichselBetaGammaCut", 0.7)   # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)            # needs validation
    kwargs.setdefault("doPU", True)
    kwargs.setdefault("BichselSimTool", "BichselSimTool")
    return CfgMgr.Ibl3DBichselChargeTool(name, **kwargs)


###############################################################################


def PixelBarrelChargeTool(name="PixelBarrelChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.PixelBarrelChargeTool(name, **kwargs)

def PixelECChargeTool(name="PixelECChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.PixelECChargeTool(name, **kwargs)

def IblPlanarChargeTool(name="IblPlanarChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.IblPlanarChargeTool(name, **kwargs)

def Ibl3DChargeTool(name="Ibl3DChargeTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.Ibl3DChargeTool(name, **kwargs)

def SubChargesTool(name="SubChargesTool", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.SubChargesTool(name, **kwargs)

def PixelNoisyCellGenerator(name="PixelNoisyCellGenerator", **kwargs):
    from AthenaCommon.BeamFlags import jobproperties
    kwargs.setdefault("MergeCharge", True)
    NoiseShape = [0.00596, 0.03491, 0.07058, 0.11991, 0.17971, 0.24105, 0.29884, 0.35167, 0.39912, 0.44188, 0.48016, 0.51471, 0.54587, 0.57405, 0.59958, 0.62288, 0.64411, 0.66360, 0.68159, 0.69823, 0.71362, 0.72781, 0.74096, 0.75304, 0.76415, 0.77438, 0.78383, 0.79256, 0.80066, 0.80821, 0.81547, 0.82246, 0.82918, 0.83501, 0.84054, 0.84576, 0.85078, 0.85558, 0.86018, 0.86455, 0.86875, 0.87273, 0.87653, 0.88020, 0.88369, 0.88705, 0.89027, 0.89336, 0.89633, 0.89921, 0.90195, 0.90460, 0.90714, 0.90961, 0.91198, 0.91426, 0.91644, 0.91853, 0.92055, 0.92250, 0.92435, 0.92611, 0.92782, 0.92947, 0.93105, 0.93257, 0.93404, 0.93547, 0.93688, 0.93822, 0.93953, 0.94079, 0.94201, 0.94318, 0.94432, 0.94542, 0.94649, 0.94751, 0.94851, 0.94949, 0.95045, 0.95137, 0.95227, 0.95314, 0.95399, 0.95483, 0.95563, 0.95646, 0.95729, 0.95812, 0.95896, 0.95980, 0.96063, 0.96144, 0.96224, 0.96301, 0.96377, 0.96451, 0.96521, 0.96590, 0.96657, 0.96722, 0.96787, 0.96849, 0.96911, 0.96971, 0.97031, 0.97090, 0.97148, 0.97204, 0.97260, 0.97314, 0.97367, 0.97421, 0.97474, 0.97525, 0.97576, 0.97627, 0.97676, 0.97722, 0.97769, 0.97815, 0.97861, 0.97906, 0.97950, 0.97992, 0.98033, 0.98073, 0.98111, 0.98147, 0.98182, 0.98216, 0.98249, 0.98281, 0.98312, 0.98343, 0.98374, 0.98402, 0.98430, 0.98456, 0.98482, 0.98507, 0.98532, 0.98555, 0.98579, 0.98602, 0.98624, 0.98646, 0.98668, 0.98690, 0.98711, 0.98732, 0.98753, 0.98773, 0.98793, 0.98813, 0.98832, 0.98851, 0.98870, 0.98888, 0.98907, 0.98925, 0.98943, 0.98961, 0.98979, 0.98996, 0.99014, 0.99031, 0.99048, 0.99064, 0.99081, 0.99098, 0.99114, 0.99131, 0.99147, 0.99163, 0.99179, 0.99194, 0.99210, 0.99225, 0.99240, 0.99256, 0.99271, 0.99286, 0.99300, 0.99315, 0.99329, 0.99344, 0.99358, 0.99372, 0.99386, 0.99400, 0.99414, 0.99427, 0.99440, 0.99453, 0.99466, 0.99479, 0.99491, 0.99503, 0.99515, 0.99527, 0.99538, 0.99549, 0.99560, 0.99571, 0.99582, 0.99592, 0.99602, 0.99613, 0.99623, 0.99633, 0.99643, 0.99653, 0.99662, 0.99672, 0.99682, 0.99691, 0.99701, 0.99710, 0.99719, 0.99728, 0.99737, 0.99746, 0.99755, 0.99764, 0.99772, 0.99781, 0.99790, 0.99798, 0.99806, 0.99814, 0.99823, 0.99831, 0.99839, 0.99847, 0.99855, 0.99863, 0.99871, 0.99879, 0.99887, 0.99895, 0.99902, 0.99910, 0.99918, 0.99925, 0.99933, 0.99940, 0.99948, 0.99955, 0.99963, 0.99971, 0.99978, 0.99985, 0.99993, 1.00000]
    kwargs.setdefault("NoiseShape",NoiseShape)
    #print "Noise Shape = ",NoiseShape
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    if jobproperties.Beam.beamType != "cosmics" :
         kwargs.setdefault("RndNoiseProb", 1.e-10)
    return CfgMgr.PixelNoisyCellGenerator(name, **kwargs)

def SpecialPixelGenerator(name="SpecialPixelGenerator", **kwargs):
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        # random noise probability
        kwargs.setdefault("SpmNoiseProb", 1e-5)
        kwargs.setdefault("SpmNoBumpProb", 5e-4)
        kwargs.setdefault("SpmDisableProb", 2e-5)
        #kwargs.setdefault("SpmBadTOTProb", 5e-4)
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.SpecialPixelGenerator(name, **kwargs)



def PixelGangedMerger(name="PixelGangedMerger", **kwargs):
    return CfgMgr.PixelGangedMerger(name, **kwargs)

def PixelChargeSmearer(name="PixelChargeSmearer", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    kwargs.setdefault("ThermalNoise", 160.0)
    return CfgMgr.PixelChargeSmearer(name, **kwargs)

def PixelDiodeCrossTalkGenerator(name="PixelDiodeCrossTalkGenerator", **kwargs):
    return CfgMgr.PixelDiodeCrossTalkGenerator(name, **kwargs)

def TimeSvc(name="TimeSvc", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    timeBCN = 5
    timePerBCO = 25.0
    if digitizationFlags.bunchSpacing.get_Value() > 0:
        timeBCN = max(int(digitizationFlags.bunchSpacing.get_Value()/timePerBCO),timeBCN)
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        ## Timing for cosmics run
        kwargs.setdefault("TimeJitter", 25.)
        kwargs.setdefault("TimeZero", 100.)
        timeBCN = 8
    else:
        kwargs.setdefault("TimeZero", 5.)
        timeBCN = 1
    kwargs.setdefault("TimePerBCO", 25.0)
    kwargs.setdefault("TimeBCN",timeBCN)
    return CfgMgr.TimeSvc(name, **kwargs)

def PixelCellDiscriminator(name="PixelCellDiscriminator", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.PixelCellDiscriminator(name, **kwargs)

def PixelRandomDisabledCellGenerator(name="PixelRandomDisabledCellGenerator", **kwargs):
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    kwargs.setdefault("RndmEngine", "PixelDigitization")
    return CfgMgr.PixelRandomDisabledCellGenerator(name, **kwargs)

def BasicPixelDigitizationTool(name="PixelDigitizationTool", **kwargs):
    from AthenaCommon import CfgGetter
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    streamName = kwargs.setdefault("RndmEngine", "PixelDigitization")
    if not digitizationFlags.rndmSeedList.checkForExistingSeed(streamName):
        digitizationFlags.rndmSeedList.addSeed(streamName, 10513239, 492615104 )
    from AthenaCommon.BeamFlags import jobproperties
    from AthenaCommon.Resilience import protectedInclude
    from AthenaCommon.Include import include
    from AthenaCommon.AppMgr import ServiceMgr
    protectedInclude( "PixelConditionsServices/SpecialPixelMapSvc_jobOptions.py" )
    include.block( "PixelConditionsServices/SpecialPixelMapSvc_jobOptions.py" )
    protectedInclude( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )
    include.block( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )
    protectedInclude("PixelConditionsServices/PixelCalibSvc_jobOptions.py")
    from IOVDbSvc.CondDB import conddb
    conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed")
    kwargs.setdefault("PixelCablingSvc","PixelCablingSvc")
    if not hasattr(ServiceMgr, "PixelSiPropertiesSvc"):
        from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc
        from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesSvc
        pixelSiPropertiesSvc = SiPropertiesSvc(name = "PixelSiPropertiesSvc",DetectorName="Pixel",SiConditionsServices = lorentzAngleSvc.pixelSiliconConditionsSvc)
        ServiceMgr += pixelSiPropertiesSvc
    kwargs.setdefault("InputObjectName", "PixelHits")
    pixTools = []
    pixTools += ['PixelDiodeCrossTalkGenerator']
    pixTools += ['PixelChargeSmearer']
    if digitizationFlags.doInDetNoise.get_Value():
        pixTools += ['PixelNoisyCellGenerator']
    pixTools += ['PixelGangedMerger']
    pixTools += ['SpecialPixelGenerator']
    pixTools += ['PixelRandomDisabledCellGenerator']
    pixTools += ['PixelCellDiscriminator']
    kwargs.setdefault("PixelTools", pixTools)
    # Start of special cosmics tuning:
    if jobproperties.Beam.beamType == "cosmics" :
        kwargs.setdefault("UseComTime", True)
    if GeometryFlags.isSLHC():
        LVL1Latency = [255, 255, 255, 255, 255, 16, 255]
        ToTMinCut = [0, 0, 0, 0, 0, 0, 0]
        ApplyDupli = [False, False, False, False, False, False, False]
        LowTOTduplication = [0, 0, 0, 0, 0, 0, 0]
        kwargs.setdefault("LVL1Latency", LVL1Latency)
        kwargs.setdefault("ToTMinCut", ToTMinCut)
        kwargs.setdefault("ApplyDupli", ApplyDupli)
        kwargs.setdefault("LowTOTduplication", LowTOTduplication)
    else:
        # For LVL1Latency, ToTMinCut, ApplyDupli and LowTOTduplication, first component [0] is always for IBL, even for run 1 production.
        # The order is IBL, BL, L1, L2, EC, DBM
        # For IBL and DBM, values of LVL1Latency and LowToTDupli are superseded by values driven by HitDiscCnfg settings, in PixelDigitizationTool.cxx
        LVL1Latency = [16, 150, 255, 255, 255, 16]
        ToTMinCut = [0, 6, 6, 6, 6, 0]
        ApplyDupli = [True, False, False, False, False, False]
        LowTOTduplication = [0, 7, 7, 7, 7, 0]
        kwargs.setdefault("LVL1Latency", LVL1Latency)
        kwargs.setdefault("ToTMinCut", ToTMinCut)
        kwargs.setdefault("ApplyDupli", ApplyDupli)
        kwargs.setdefault("LowTOTduplication", LowTOTduplication)
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", Pixel_FirstXing() )
        kwargs.setdefault("LastXing", Pixel_LastXing() )
    return CfgMgr.PixelDigitizationTool(name, **kwargs)

def PixelDigitizationTool(name="PixelDigitizationTool", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    from IOVDbSvc.CondDB import conddb
    if conddb.dbmc == "OFLP200" and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
      conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg")
    if not conddb.folderRequested('PIXEL/PixReco'):
      conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelGeantinoTruthDigitizationTool(name="PixelGeantinoTruthDigitizationTool", **kwargs):
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return PixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolHS(name="PixelDigitizationToolHS", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolPU(name="PixelDigitizationToolPU", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 2)
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolSplitNoMergePU(name="PixelDigitizationToolSplitNoMergePU", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("InputObjectName", "PileupPixelHits")
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelOverlayDigitizationTool(name="PixelOverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.evtStore())
    kwargs.setdefault("RDOCollName", overlayFlags.evtStore()+"/PixelRDOs")
    kwargs.setdefault("RDOCollNameSPM", overlayFlags.evtStore()+"/PixelRDOs_SPM")
    kwargs.setdefault("SDOCollName", overlayFlags.evtStore()+"/PixelSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return BasicPixelDigitizationTool(name,**kwargs)

def getPixelRange(name="PixelRange" , **kwargs):
    kwargs.setdefault('FirstXing', Pixel_FirstXing() )
    kwargs.setdefault('LastXing',  Pixel_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#PixelHits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def PixelDigitizationHS(name="PixelDigitizationHS",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelDigitizationToolHS")
    return CfgMgr.PixelDigitization(name,**kwargs)

def PixelDigitizationPU(name="PixelDigitizationPU",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelDigitizationToolPU")
    return CfgMgr.PixelDigitization(name,**kwargs)

def PixelOverlayDigitization(name="PixelOverlayDigitization",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelOverlayDigitizationTool")
    return CfgMgr.PixelDigitization(name,**kwargs)
