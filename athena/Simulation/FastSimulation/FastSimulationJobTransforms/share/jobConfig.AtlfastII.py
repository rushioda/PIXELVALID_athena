from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()

from FastSimulationConfig.FastSimulationFlags import jobproperties
jobproperties.FastSimulation.doFastCaloSim = True

from FastSimulationConfig.FastSimulationFlags import jobproperties
jobproperties.FastSimulation.doFastCaloSimCaloAddCells = True

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSimAddCells = True

include("FastSimulationJobTransforms/jobConfig.v14_Parametrisation.py")

#theFastShowerCellBuilderTool.OutputLevel=DEBUG
