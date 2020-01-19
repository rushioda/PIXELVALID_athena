# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class 

def setup_eflowObjectBuilder(Configured, nameModifier,mlog):
    print "eflowObjectBuilderSetup.py : setup_eflowObjectBuilder"

    if nameModifier != "EM" and nameModifier != "LC":
        mlog.error("Invalid calorimeter scale was specified : should be LC or EM, but was "+nameModifier)
        return False

    try:
        from eflowRec.eflowRecConf import eflowObjectBuilder
        ObjectBuilder=eflowObjectBuilder("eflowObjectBuilder_"+nameModifier)
    except:
        mlog.error("could not import eflowRec.eflowObjectBuilder")
        print traceback.format_exc()
        return False

    Configured._eflowObjectBuilderHandle = ObjectBuilder

    from eflowRec.eflowRecFlags import jobproperties

    ObjectBuilder.PrivateToolList  = []

    try:
        from eflowRec.eflowMomentCalculatorToolDefault import eflowMomentCalculatorToolDefault
        MomentCalculatorTool = eflowMomentCalculatorToolDefault("eflowMomentCalculatorTool_"+nameModifier)
    except:
        mlog.error("could not import eflowRec.eflowMomentCalculatorTool")
        print traceback.format_exc()
        return False

    if nameModifier == "LC":
        MomentCalculatorTool.LCMode = True

    ObjectBuilder.PrivateToolList += [MomentCalculatorTool]

    if nameModifier == "EM" and not jobproperties.eflowRecFlags.CalType == "NoCal":        
        try:
            from eflowRec.eflowLCCalibToolDefault import eflowLCCalibToolDefault
            LCCalibTool = eflowLCCalibToolDefault("eflowLCCalibTool_"+nameModifier)
        except:
            mlog.error("could not import eflowRec.eflowLCCalibTool")
            print traceback.format_exc()
            return False

        ObjectBuilder.PrivateToolList += [LCCalibTool]
        
    try:
        from eflowRec.eflowObjectCreatorToolDefault import eflowObjectCreatorToolDefault
        ObjectCreatorTool=eflowObjectCreatorToolDefault("eflowObjectCreatorTool_"+nameModifier)
    except:
        mlog.error("could not import eflowRec.eflowObjectCreatorTool")
        print traceback.format_exc()
        return False

    if "LC" == nameModifier:
        ObjectCreatorTool.LCMode=True
        ObjectBuilder.EflowClustersOutputName="PFOClusters_JetETMiss_LC"
        ObjectBuilder.EflowCaloObjectsName="eflowCaloObjects_LC"
        if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
            ObjectCreatorTool.PFOOutputName = 'EOverP'
            ObjectCreatorTool.EOverPMode = True
        else:
            ObjectCreatorTool.PFOOutputName="JetETMiss_LCPFO"

    if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
        ObjectCreatorTool.PFOOutputName = 'EOverP'
        ObjectCreatorTool.EOverPMode = True
        ObjectBuilder.EflowClustersOutputName="eflowCluster01"

    ObjectBuilder.PrivateToolList += [ObjectCreatorTool]

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += ObjectBuilder

    return True
