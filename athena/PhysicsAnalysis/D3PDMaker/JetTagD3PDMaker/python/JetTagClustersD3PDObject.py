# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagClustersD3PDObject.py
## @brief pixel cluster D3PD object configured for the btagging D3PD
## @author Georges Aad
## @date Nov, 2010
##
import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from TrackD3PDMaker.PixelClusterD3PDObject import PixelClusterD3PDObject

def getJetTagClusterD3PDObject(level=0, **kw):

    ### fixme (needs new TrackD3PDMaker tag):
    ### 1. add link to track and gen particle
    ### 2. configure selection blayer/all pix

    return PixelClusterD3PDObject(level=level, **kw)

