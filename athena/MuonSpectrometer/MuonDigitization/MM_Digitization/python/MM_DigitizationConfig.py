# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Import MM_Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

# The earliest bunch crossing time for which interactions will be sent 
# to the MdtDigitizationTool. 
def MM_FirstXing(): 
    return -250 

# The latest bunch crossing time for which interactions will be sent 
# to the MdtDigitizationTool. 
def MM_LastXing(): 
    return 200 

def MmDigitizationTool(name="MmDigitizationTool",**kwargs):
    kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc() )   
    # set rndm seeds
    mmRndm = kwargs.setdefault("RndmEngine","MM_Digitization")
    jobproperties.Digitization.rndmSeedList.addSeed(mmRndm, 49261510, 105132394 )
    if jobproperties.Digitization.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", MM_FirstXing() ) # this should match the range for the MM in Digitization/share/MuonDigitization.py
        kwargs.setdefault("LastXing",  MM_LastXing() )  # this should match the range for the MM in Digitization/share/MuonDigitization.py
    kwargs.setdefault("CheckSimHits", True)
    kwargs.setdefault("InputObjectName", "MicromegasSensitiveDetector")
    kwargs.setdefault("OutputObjectName", "MM_DIGITS")
    kwargs.setdefault("OutputSDOName", "MM_SDO")
    kwargs.setdefault("ValidationSetup", False)
    ## Strip response
    kwargs.setdefault("qThreshold", 0.001)
    kwargs.setdefault("DiffusSigma", 0.036)
    kwargs.setdefault("LogitundinalDiffusSigma", 0.019)
    kwargs.setdefault("DriftVelocity", 0.047)
    kwargs.setdefault("crossTalk1", 0.0)
    kwargs.setdefault("crossTalk2", 0.0)
    ## Chip/electronics response
    kwargs.setdefault("alpha", 2.5)
    kwargs.setdefault("RC", 20.)
    
    return CfgMgr.MmDigitizationTool(name,**kwargs)
       #return CfgMgr.MM_PileUpTool(name,**kwargs)
    #else:
       #return CfgMgr.MdtDigitizationTool(name,**kwargs)

def getMMRange(name="MMRange", **kwargs): 
    # bunch crossing range in ns 
    kwargs.setdefault('FirstXing', MM_FirstXing() ) 
    kwargs.setdefault('LastXing',  MM_LastXing() ) 
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset 
    kwargs.setdefault('ItemList', ["GenericMuonSimHitCollection#MicromegasSensitiveDetector"] ) 
    return CfgMgr.PileUpXingFolder(name, **kwargs) 

	
def MM_Response_DigitTool(name="MM_Response_DigitTool",**kwargs):
    kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc())
    mmRndm = kwargs.setdefault("RndmEngine", "MMResponse")
    jobproperties.Digitization.rndmSeedList.addSeed(mmRndm, 49261510,105132394 )
    return CfgMgr.MM_Response_DigitTool(name,**kwargs)


