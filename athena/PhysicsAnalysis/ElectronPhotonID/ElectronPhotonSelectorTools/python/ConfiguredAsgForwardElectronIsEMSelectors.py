# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ConfiguredAsgForwardElectronIsEMSelectors
##
## Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
## Created:     Dec. 2011
##
## Description: Apply the default configurations for the AsgForwardElectronIsEMSelector,  but allow for overwriting them with user-defined values.
##
##=============================================================================

# Import the needed general stuff
from PATCore.HelperUtils import *
from AthenaCommon import CfgMgr
import sys

# Import the needed stuff specific to the ElectronPhotonSelectorTools
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgForwardElectronIsEMSelector
from ElectronPhotonSelectorTools.ForwardElectronIsEMSelectorMapping import ForwardElectronIsEMMap, forwardelectronPIDmenu

def ConfiguredAsgForwardElectronIsEMSelector( name, quality, menu=forwardelectronPIDmenu.menuMC15, **kw ):
    """
    Configure the AsgForwardElectronIsEMSelector with the quality cuts
    and allow for (re-)setting of all provided cuts.
    """
    try:
        ntuple = ForwardElectronIsEMMap(quality, menu)
    except KeyError:
        sys.stderr.write("Fwd Electron quality not found. Please use an egammaIDQuality (ElectronPhotonSelectorTools/egammaPIDdefs.h).\n This function only supports standard electron IDs, and not photon or forward IDs\n")
        raise

    # Get the label for user data
    tmpName = (ntuple[1]).func_name
    labelName = "is" + ((tmpName.split("Selector")[0]).split("IsEM")[1])

    # Create and instance of the tool
    tool = CfgMgr.AsgForwardElectronIsEMSelector(name, **kw)

    # Configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    # Get all provided properties and overwrite the default values with them
    SetToolProperties( tool, **kw )

    #print tool
    return tool




