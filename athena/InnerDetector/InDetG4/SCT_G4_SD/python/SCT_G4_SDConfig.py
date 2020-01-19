# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasApps.SimFlags import simFlags

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


def getSctSensorSD(name="SctSensorSD", **kwargs):
    bare_collection_name = "SCT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "SCTHits"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["SCT::BRLSensor","SCT::ECSensor0","SCT::ECSensor1",
                                             "SCT::ECSensor2","SCT::ECSensor3"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.SctSensorSDTool(name, **kwargs)


def getSLHC_SctSensorSD(name="SLHC_SctSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["SCT::BRLSensor","SCT::BRLSensorSS","SCT::BRLSensorMS",
                                             "SCT::ECSensor0","SCT::ECSensor1","SCT::ECSensor2",
                                             "SCT::ECSensor3","SCT::ECSensor4","SCT::ECSensor5"])
    return getSctSensorSD(name, **kwargs)


def getSLHC_SctSensorSD_Gmx(name="SLHC_SctSensorSD_Gmx", **kwargs):
    kwargs.setdefault("GmxSensor", True )
    return getSLHC_SctSensorSD(name, **kwargs)


def getSctSensor_CTB(name="SctSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["SCT::ECSensor0"])
    kwargs.setdefault("OutputCollectionNames", ["SCT_Hits"])
    #kwargs.setdefault("OutputLevel",  1 )
    return CfgMgr.SctSensor_CTBTool(name, **kwargs)
