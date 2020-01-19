# jobOptions fragment to compute lar time in end-cap for collisions 

#David Cote: added include block to avoid duplication ERROR when TAG_COMM and DESDs are executed in the same job (both including this fragment).
include.block("LArCellRec/LArCollisionTime_jobOptions.py")

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool

from LArCellRec.LArCellRecConf import LArCollisionTimeAlg
theLArCollisionTimeAlg = LArCollisionTimeAlg("LArCollisionTimeAlg")
theLArCollisionTimeAlg.NoiseTool = theCaloNoiseTool

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data' :
  theLArCollisionTimeAlg.isMC = False
else :
  theLArCollisionTimeAlg.isMC = True

# dont require anymore explicitely that the time iteration converges
theLArCollisionTimeAlg.cutIteration = False

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theLArCollisionTimeAlg

