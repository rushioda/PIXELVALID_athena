# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#

##
# @short Function returning a pre-configured instance of GenObjectsFilterTool
#
# This configuration of the tool lets the user select stable charged particles
# from the truth record.
#
def StableChargedTruthFilterTool():

    # Name for this tool instance:
    toolName = "StableChargedTruthFilterTool"

    # Instantiate the tool if necessary:
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr( ToolSvc, toolName ):
        from TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
        ToolSvc += D3PD__GenObjectsFilterTool( toolName )

    # Get hold of the tool:
    tool = getattr( ToolSvc, toolName )

    # Set all of the tool's properties:
    tool.PtMin = 100.0
    tool.EtaMax = 5.0
    tool.RemoveEmptyEvents = True
    tool.RemoveDummyEvents = False
    tool.RemoveInTimePileUp = False
    tool.Remove2BCPileUp = False
    tool.Remove800nsPileUp = False
    tool.RemoveCavernBkg = False
    tool.SelectTruthTracks = True

    # Return this tool to the user:
    return tool

