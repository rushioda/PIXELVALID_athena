# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Streaming Chain Def Generations"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
mlog = logging.getLogger("TriggerMenu.calibcosmic.generateStreamingChainDefs")

from TriggerMenu.calibcosmicmon.StreamingDef import L2EFChain_SeededStreamerTemplate
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs

##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:      
        
        Streaming = L2EFChain_SeededStreamerTemplate(subChainDict)

        listOfChainDefs += [Streaming.generateHLTChainDef()]
    
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    return theChainDef

