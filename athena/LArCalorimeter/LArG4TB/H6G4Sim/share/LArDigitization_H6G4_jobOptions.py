################################################################
#
# job Options fragment to create LArDigits  from LArHits which
#  should be already in StoreGate
# to be used on G4 output
#
################################################################

# overall calorimeter noise options (could be set by top jobOptions)
if not 'doCaloNoise' in dir():
    doCaloNoise = True

# We also need the conditions svc for MC constants:
# to use new calibration classes (WorkMode=1)
# not needed for new mode
# include( "LArG4TBH6/LArCondCnv_H6G4_jobOptions.py" )
#include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#theApp.Dlls += [ "LArTools","LArCalibUtils" ]
#ToolSvc = Service( "ToolSvc" )
from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.ConfigurableDb import getConfigurable
from AthenaCommon.AppMgr import ToolSvc
from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
theADC2MeVTool = LArADC2MeVToolDefault()
theADC2MeVTool.MCSym=TRUE
ToolSvc += theADC2MeVTool
# The following are properties for LArDigitization:
isOverlay = False
useLArFloat = True
# get main tool which does the real work
from LArDigitization.LArPileUpToolDefault import LArPileUpToolDefault
theLArPileUpTool = LArPileUpToolDefault(useLArFloat,isOverlay,"LArDigitContainer_MC","LArPileUpTool")
theLArPileUpTool.UseTriggerTime = TRUE
theLArPileUpTool.TriggerTimeToolName = "LArTBH6TriggerTimeTool"
# ................... electronic noise
if doCaloNoise:
    theLArPileUpTool.NoiseOnOff = TRUE    # if true, noise added in all subdetectors
else:
    theLArPileUpTool.NoiseOnOff = FALSE

#
# --------------------------------------------------------
# ............ declare the used top algo.
# --------------------------------------------------------
#theApp.Dlls += ["LArDigitization"]
#theApp.TopAlg += [ "LArDigitMaker/digitmaker1"]
from LArDigitization.LArDigitizationConf import LArDigitMaker
digitmaker1 = LArDigitMaker("digitmaker1")
topSequence += digitmaker1
#
digitmaker1.LArPileUpTool = theLArPileUpTool
#
# --------------------------------------------------------
#  all properties now to tool
# --------------------------------------------------------
# 
# .................. sub-detector hit containers
# 
#
theLArPileUpTool.SubDetectors = "LAr_EndCap"
#
#
# .................. set the Low gain threshold (ADC counts)
#
theLArPileUpTool.LowGainThreshEM = 3900
theLArPileUpTool.LowGainThreshHEC = 2500
theLArPileUpTool.LowGainThreshFCAL = 2000
#
# .................. set the high gain threshold (ADC counts)
#
theLArPileUpTool.HighGainThreshEM = 1300
theLArPileUpTool.HighGainThreshHEC = 0
theLArPileUpTool.HighGainThreshFCAL = 1100
#
# .................. set High gain value: applied if E < HighGainThreshrPileUpTool
#
#digitmaker1.HighGainValue = 100.
#
# .................. set Medium gain value: applied if HighGainThresh E < LowGainThresh
#
#digitmaker1.MediumGainValue = 10.
#
# .................. set Low gain value: applied if E > LowGainThresh
#
#digitmaker1.LowGainValue = 1.
#
# .................. set pedestal
#
#digitmaker1.AdcPedestal = 1000.
#
# .................. set Adc to GeV constant 
# .................. (adc = UnCalibretedEnergy*Gain/AdcPerGeV + Pedestal)
# .................. this number is correlated to ped and gain
#
#digitmaker1.AdcPerGev = 5.
#
# .................. set the energy threshold for zero suppression 
#
theLArPileUpTool.EnergyThresh = -99.
#
# .................. switches on the noise 
#put false if you want cancel the noise in one or several sub-detectors
theLArPileUpTool.NoiseInEMB   = FALSE; # if false, noise in Barrel is off 
theLArPileUpTool.NoiseInEMEC  = TRUE; # if false, noise in EndCap is off
theLArPileUpTool.NoiseInHEC   = TRUE; # if false, noise in HEC  is off 
theLArPileUpTool.NoiseInFCAL  = TRUE; # if false, noise in FCAL is off
#
# .................... turn on cross-talk in EM calorimeter
theLArPileUpTool.CrossTalk = TRUE
#
# ..................... Pile up turned off
# ... use new calibration classes
#digitmaker1.WorkMode = 1
# ... use the trigger time tool
#########################################################################
#
# end of LArDigitization job Options fragment
#
##########################################################################"
