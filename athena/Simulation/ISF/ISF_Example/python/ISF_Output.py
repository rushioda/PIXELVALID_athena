# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## Get the logger
from AthenaCommon.Logging import *
isfoplog = logging.getLogger('ISF_Output')

def getHITSStreamItemList():
    hitsItemList=[]
    ## EventInfo & TruthEvent always written by default
    hitsItemList = ["EventInfo#*",
                    "McEventCollection#TruthEvent",
                    "JetCollection#*"]
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.SimulationFlavour() is not None and 'ongLived' in simFlags.SimulationFlavour(): # to catch LongLived and longLived
        hitsItemList += ["McEventCollection#GEN_EVENT"]
    from PyJobTransforms.trfUtils import releaseIsOlderThan
    if releaseIsOlderThan(20,0):
        #Hack to maintain compatibility of G4AtlasApps trunk with
        #19.2.X.Y after EDM changes in release 20.0.0.
        hitsItemList += ["xAOD::JetContainer_v1#*",
                         "xAOD::JetAuxContainer_v1#*"]
    else:
        hitsItemList += ["xAOD::JetContainer#*",
                           "xAOD::JetAuxContainer#*"]
    ## Detectors
    from AthenaCommon.DetFlags import DetFlags
    ## Inner Detector
    if DetFlags.ID_on():
        hitsItemList += ["SiHitCollection#*",
                         "TRTUncompressedHitCollection#*",
                         "TrackRecordCollection#CaloEntryLayer"]
    ## Calo
    if DetFlags.Calo_on():
        hitsItemList += ["CaloCalibrationHitContainer#*",
                         "LArHitContainer#*",
                         "TileHitVector#*",
                         #"SimpleScintillatorHitCollection#*",
                         "TrackRecordCollection#MuonEntryLayer"]
    ## Muon
    if DetFlags.Muon_on():
        hitsItemList += ["RPCSimHitCollection#*",
                         "TGCSimHitCollection#*",
                         "CSCSimHitCollection#*",
                         "MDTSimHitCollection#*",
                         "TrackRecordCollection#MuonExitLayer"]
    ## FwdRegion
    if DetFlags.FwdRegion_on():
        hitsItemList += ["SimulationHitCollection#*"]

    ## Lucid
    if DetFlags.Lucid_on():
        hitsItemList += ["LUCID_SimHitCollection#*"]

    ## ZDC
    if DetFlags.ZDC_on():
        hitsItemList += ["ZDC_SimPixelHit_Collection#*",
                             "ZDC_SimStripHit_Collection#*"]
    ## ALFA
    if DetFlags.ALFA_on():
        hitsItemList += ["ALFA_HitCollection#*","ALFA_ODHitCollection#*"]

    ## AFP
    if DetFlags.AFP_on():
        hitsItemList += ["AFP_TDSimHitCollection#*","AFP_SIDSimHitCollection#*"]

    ## Ancillary scintillators
    #hitsItemList += ["ScintillatorHitCollection#*"]

    ## TimingAlg
    hitsItemList +=["RecoTimingObj#EVNTtoHITS_timings"]

    if simFlags.RecordStepInfo.get_Value():
        hitsItemList +=["ISF_FCS_Parametrization::FCS_StepInfoCollection#MergedEventSteps"]

    ## add xAOD::TrackParticles output collection Parametric Simulation
    if simFlags.SimulationFlavour == "ParametricSimulation":
        hitsItemList +=["xAOD::TrackParticleContainer#*",
                            "xAOD::TrackParticleAuxContainer#*"]
    ## Add cosmics and test beam configuration hit persistency if required cf. geom tag
    layout = simFlags.SimLayout.get_Value()
    if "tb" not in layout:
        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType() == 'cosmics' or \
                (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn) or \
                (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
            hitsItemList += ["TrackRecordCollection#CosmicRecord", "TrackRecordCollection#CosmicPerigee"]
    else:
        ## CTB-specific
        if layout.startswith("ctb"):
            if simFlags.LArFarUpstreamMaterial.statusOn and simFlags.LArFarUpstreamMaterial.get_Value():
                hitsItemList.append("TrackRecordCollection#LArFarUpstreamMaterialExitLayer")
        ## Persistency of test-beam layout
        if layout.startswith('ctb') or layout.startswith('tb_Tile2000_'):
            hitsItemList += ["TBElementContainer#*"]
    return hitsItemList

def getEVNTStreamItemList():
    evntItemList = ["EventInfo#*"]
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.CavernBG.statusOn and 'Write' in simFlags.CavernBG.get_Value():
        evntItemList += ["TrackRecordCollection#NeutronBG"]
    elif hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        evntItemList += ["TrackRecordCollection#StoppingPositions"]
    else:
        evntItemList += ["TrackRecordCollection#CosmicRecord"]
    return evntItemList

class ISF_HITSStream:
    """
     Place to handle the persistency.
    """

    isfoplog.info("in ISF_HITSStream")

    from G4AtlasApps.SimFlags import simFlags
    from ISF_Config.ISF_jobProperties import ISF_Flags
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.PoolHitsOutput.statusOn or (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn) or (hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn):
        ## Write hits in POOL
        isfoplog.info("ISF_HITSStream starting")

        ## The following used to be in G4AtlasApps/HitAthenaPoolWriteOptions
        from AthenaCommon.Configurable import Configurable
        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

        ## Default setting for one output stream
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_BUFFERSIZE = '2048'"]

        ## Write geometry tag info
        import EventInfoMgt.EventInfoMgtInit

        ## Patch metadata if required
        from ISF_Example.ISF_Metadata import patch_mc_channel_numberMetadata
        patch_mc_channel_numberMetadata()

        ## Instantiate StreamHITS
        ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
        stream1 = None
        if athenaCommonFlags.PoolHitsOutput.statusOn:
            stream1 = AthenaPoolOutputStream("StreamHITS", athenaCommonFlags.PoolHitsOutput())
            stream1.ForceRead = True
            stream1.ItemList = getHITSStreamItemList()
            ## Make stream aware of aborted events
            stream1.AcceptAlgs = [ISF_Flags.Simulator.KernelName()]

        ## StreamEVGEN: needed for cosmic simulations and cavern BG
        ## Separate stream of track record (TR) info -- it does not apply to the CTB simulations.
        # TODO: Can this be merged into the cosmics sec above, or do the AthenaPool includes *need* to be in-between?
        layout = simFlags.SimLayout.get_Value()
        if "tb" not in layout:
            if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn or\
               hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
                streamName = simFlags.WriteTR.get_Value() if simFlags.WriteTR.statusOn else simFlags.StoppedParticleFile.get_Value()
                stream2 = AthenaPoolOutputStream("StreamEVGEN", streamName)
                stream2.ItemList = getEVNTStreamItemList()
                ## Make stream aware of aborted events
                stream2.AcceptAlgs = [ISF_Flags.Simulator.KernelName()]
        #
        #  Setup and add metadata to the HITS file.
        #  ( heavily based on G4AtlasApps/python/SimAtlasKernel.py )
        #
        #  causes problems with cosmics currently:
        #    "IOVDbSvc ERROR Duplicate request for folder /Simulation/Parameters associated to already"
        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
        stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]

        isfoplog.info("ISF_HITSStream done")
    else:
        isfoplog.info("ISF_HITSStream nothing done")
