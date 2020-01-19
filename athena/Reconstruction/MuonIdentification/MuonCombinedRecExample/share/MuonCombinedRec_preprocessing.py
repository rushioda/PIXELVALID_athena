#
# === configuration of tools/algs done BEFORE combined muon identification,
# === such as container initialisation, track splitting and ID track extrapolation
#
include.block ("MuonCombinedRecExample/MuonCombinedRec_preprocessing.py")
from AthenaCommon import CfgMgr
#
# Muon Calo Energy Container is needed even if Calo is Off: use the parametrized energy loss in that case - KAA
if rec.doMuonCombined() and muonCombinedRecFlags.doAnyMuons() and DetFlags.Muon_on():
    # Needed by MuonIsolationTools - whether calo is ON or OFF. In calo is Off use parametrized energy loss - KAA
  
    InitializeMuonCaloEnergy = CfgMgr.Rec__InitializeMuonCaloEnergy(
        name                    = "InitializeMuonCaloEnergy",
        MuonCaloEnergyContainer = "MuonCaloEnergyCollection"
        )
    topSequence += InitializeMuonCaloEnergy
    if muonCombinedRecFlags.printConfigurables():
        print InitializeMuonCaloEnergy

if rec.doMuonCombined() and muonCombinedRecFlags.doMuonClusters() and ( rec.readAOD() or rec.readESD() or DetFlags.haveRIO.Calo_on() ):
    # Needed by MuonIsolationTools
    InitializeMuonClusters = CfgMgr.Rec__InitializeMuonClusters (
        name                 = "InitializeMuonClusters",
        MuonClusterContainer = "MuonClusterCollection"
        )
    topSequence += InitializeMuonClusters
    if muonCombinedRecFlags.printConfigurables():
        print InitializeMuonClusters
    
    # hack until MuonClusterCollection is properly added to ObjKeyStore
    # Needed by CaloCellAODGetter.addClusterToCaloCellAOD()
    if jobproperties.Beam.beamType()!='cosmics' and jobproperties.Beam.beamType()!='singlebeam':
       from RecExConfig.ObjKeyStore import objKeyStore
       objKeyStore.addStreamAOD("CaloClusterContainer","MuonClusterCollection")


if rec.doMuonCombined() and jobproperties.Beam.beamType()=='cosmics' and DetFlags.haveRIO.ID_on() and \
       InDetFlags.Enabled() and not InDetFlags.disableInDetReco():
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    splittertool= InDet__InDetTrackSplitterTool(name='InDetTrackSplitterToolForMuonCombined',
                                                TrackFitter=ToolSvc.InDetTrackFitter)
    ToolSvc += splittertool

    from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator
    splitter=InDet__InDetSplittedTracksCreator(name='InDetTrackSplitterForMuonCombined',
                                               TrackSplitterTool     = splittertool,
                                               TrackCollection       = InDetKeys.Tracks(),
                                               OutputTrackCollection = InDetKeys.Tracks()+"_split")

    topSequence+=splitter

    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    InDetParticleCreatorTool_split = Trk__TrackParticleCreatorTool(name             = "InDetParticleCreatorTool_split",
                                                                   KeepParameters   = True,
                                                                   Extrapolator     = InDetExtrapolator,
                                                                   TrackSummaryTool = InDetTrackSummaryTool)
    ToolSvc += InDetParticleCreatorTool_split


    from InDetParticleCreation.InDetParticleCreationConf import InDet__TrackParticleCreator
    InDetParticleCreation_split = InDet__TrackParticleCreator(name                     = "InDetParticleCreation_split",
                                                              TrackParticleCreatorTool      = InDetParticleCreatorTool_split,
                                                              TracksName               = InDetKeys.Tracks()+"_split",
                                                              TrackParticlesOutputName = InDetKeys.TrackParticles()+"_split",
                                                              PRDAssociationTool       = None ,
                                                              DoSharedHits             = False,
                                                              VxCandidatesPrimaryName  = InDetKeys.PrimaryVertices())
    topSequence += InDetParticleCreation_split

    #truth tracks
    if rec.doTruth():
        include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
        
        InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.Tracks()+"_split",
                                                     InDetKeys.DetailedTracksTruth()+"_split",
                                                     InDetKeys.Tracks()+"_Truth_split")

