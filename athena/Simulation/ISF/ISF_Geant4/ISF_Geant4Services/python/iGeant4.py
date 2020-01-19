# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Gaudi.Configuration import *
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties

# confgetter
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

G4Eng = None

class iGeant4:
  def __init__(self, **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    ####################################
    ##
    ## the Athena part:
    ##
    ####################################

    ##FIXME: This should be configured using a CfgGetter method
    ##defined in the SubDetectorEnvelopes package.
    ##G4EnvelopeDefSvc is only used in G4DetectorEnvelopes/src/DefSvcEnvelope.cxx
    from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
    G4_DDDBEnvelopeDefSvc = DetDescrDBEnvelopeSvc( "G4EnvelopeDefSvc" )
    G4_DDDBEnvelopeDefSvc.DBBeamPipeNode = 'BPipeEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBInDetNode    = 'InDetEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBCaloNode     = 'CaloEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBMSNode       = 'MuonEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBCavernNode   = 'CavernEnvelopeG4'
    svcMgr += G4_DDDBEnvelopeDefSvc

    # configure Geant4SimSvc
    from ISF_Geant4Services.ISF_Geant4ServicesConf import iGeant4__Geant4SimSvc
    Geant4SimService = iGeant4__Geant4SimSvc('ISFG4SimSvc', **kwargs)

    # DEBUG: dump Geant4SimService
    print Geant4SimService

    svcMgr += Geant4SimService
    self.__simSvc__ = Geant4SimService

    ####################################
    ##
    ## the G4 Python part:
    ##
    ####################################

    # let the G4 configuration know that is an ISF run!
    simFlags.ISFRun.set_Value_and_Lock( True)


  def getSimSvc(self):
    return self.__simSvc__
