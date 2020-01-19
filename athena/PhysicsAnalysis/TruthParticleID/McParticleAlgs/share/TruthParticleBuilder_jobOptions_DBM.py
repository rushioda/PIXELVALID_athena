####################################
#  TruthParticleBuilder Job Options
#  author: Sebastien Binet
####################################

import EventKernel.ParticleDataType
from ParticleBuilderOptions.AODFlags import AODFlags
from RecExConfig.ObjKeyStore import objKeyStore


# needed runtime dependancy for TruthParticleCnvTool
if not "PartPropSvc" in theApp.Dlls:
    include( "PartPropSvc/PartPropSvc.py" )
    pass

if not "ToolSvc"         in theApp.ExtSvc and \
   not "ToolSvc/ToolSvc" in theApp.ExtSvc:
    theApp.ExtSvc += [ "ToolSvc/ToolSvc"]
    pass

# make sure we have Configurable-aware jobO
# ie: hack that we'll remove someday... FIXME
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from McParticleAlgs.JobOptCfg import McAodBuilder,createMcAodBuilder,PileUpClassification

#if (objKeyStore.isInInput( "McEventCollection", "TruthEvent" ) and 
#    not objKeyStore.isInInput( "McEventCollection", "GEN_AOD" )):
job += McAodBuilder()
#    pass



def getMcAODBuilder(putype):
    """ putype is expected to be a string ! """
    builder =  createMcAodBuilder(
        name = "McAodBuilder"+putype,
        outMcEvtCollection  = "GEN_AOD", # this is the input to the CnvTool
        outTruthParticles   = "SpclMC"+putype,
        )
    builder.CnvTool.SelectSignalType =  PileUpClassification.fromString(putype) # min bias only

    objKeyStore.addStreamAOD("TruthParticleContainer","SpclMC"+putype)
    return builder


# below, build a new TruthParticleContainer for each type of pile-up.
# we get the list of existing ESD TruthParticleContainer, and guess its pile-up type
# according to its suffix. 
inputTPContainer = objKeyStore['inputFile'].list("TruthParticleContainer")
prefix = "TruthParticleContainer#INav4MomTruthEvent"
for cont in inputTPContainer:
    suffix = cont[len(prefix):]
    if suffix == "":
        # the truth SIGNAL has already been scheduled above
        continue
    builder = getMcAODBuilder(suffix)
    builder.DoFiltering = False    
    job += builder


if rec.doWritexAOD():
    from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
    job += xAODMaker__xAODTruthCnvAlg(name = "GEN_AOD2xAOD", xAODTruthEventContainerName = "TruthEvent")
