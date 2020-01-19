# -*- coding: iso-8859-1 -*-
#######################################################
# SIMPLE JOB CONFIGURATION FOR E OVER P ANALYSES
# SPECIFY THE FOLLOWING JOB PARAMETERS
#######################################################

if 'EvtMax' not in dir():
    EvtMax              = -1              # NUMBER OF EVENTS TO PROCESS
if 'skipEvt' not in dir():
    skipEvt             = 0               # NUMBER OF EVENTS TO SKIP
if 'input_type' not in dir():
    input_type          = "data"          # data, geant4
if 'input_format' not in dir():
    input_format        = "ESD"           # AOD, ESD
if 'beam_type' not in dir():
    beam_type           = "collisions"    # BEAMTYPE: cosmics, collisions
if 'monitor_performance' not in dir():
    monitor_performance = False           # WHETHER (MEMORY) MONITORING SHOULD BE ENABLED
if "outputFile" not in dir():
    outputFile          = "output.root"   # OUTPUT FILE


if "inputFiles" not in dir():             # INPUT FILES
    if 'inputFile' not in dir() or inputFile=="":
        inputFiles = ['/afs/cern.ch/user/l/lcerdaal/work/public/EoverP/data12_8TeV.00209550.physics_MinBias.merge.DESD_MBIAS.r4065_p1278_tid01069837_00/DESD_MBIAS.01069837._000010.pool.root.1']
    else:
        if inputFile.endswith(".py"):
            include(inputFile)
        else:
            inputFiles = [inputFile]


CellsContainer = "AODCellContainer"
if input_format=="ESD":
  CellsContainer = "AllCalo"

#######################################################
# ATHENA CONFIGURATION FILE(S)
#######################################################
include("TileD3PDMaker/configurator_Ep.py")
