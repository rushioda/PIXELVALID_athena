# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagPrimaryVertexD3PDObject.py
## @brief primary vertex D3PD object configured for the btagging D3PD
## @author Georges Aad
## @date Nov, 2010
##
import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from TrackD3PDMaker.VertexD3PDObject import BuildVertexD3PDObject
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags

from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags


## configure TrackD3PDFlags before calling this function
##eg:
# TrackD3PDFlags.vertexPositionLevelOfDetails = 2
# TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
# TrackD3PDFlags.storeVertexFitQuality = False
# TrackD3PDFlags.storeVertexKinematics = True
# TrackD3PDFlags.storeVertexPurity = False
# TrackD3PDFlags.storeVertexTrackAssociation = True
# TrackD3PDFlags.storeVertexTrackIndexAssociation = True

def getJetTagPrimaryVertexD3PDObject(level=10, **kw):


    if JetTagD3PDFlags.TrackAssocLabel() == "":
        TrackD3PDFlags.storeVertexTrackIndexAssociation=False

    JetTagPrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix=JetTagD3PDKeys.PrimaryVertexPrefix(),
                                                          _label=JetTagD3PDKeys.PrimaryVertexGetterLabel(),
                                                          _sgkey=JetTagD3PDFlags.PrimaryVertexSGKey(),
                                                          #_object_name='JetTagPrimaryVertexD3PDObject',
                                                          trackTarget=JetTagD3PDFlags.TrackAssocLabel(),
                                                          trackPrefix='trk_',
                                                          trackType='Rec::TrackParticleContainer')

    return JetTagPrimaryVertexD3PDObject(level=level, **kw)

