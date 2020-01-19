infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #Rec::TrackParticleContainer_tlp2
    'SegmentTagTrackParticles',
    'HLT_InDetTrigParticleCreation_Photon_EFID',
    'HLT_InDetTrigParticleCreation_CosmicsN_EFID',
    'HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
    'HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
    'HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',
    'HLT_InDetTrigParticleCreationCombined_Electron_EFID',
    'HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
    'RefittedExtrapolatedMuonSpectrometerParticles',
    'HLT_InDetTrigParticleCreation_Bphysics_EFID',
    'ResolvedForwardTrackParticles',
    'ExtrapolatedMuonSpectrometerParticles',
    'MuonSpectrometerParticles',
    'StatCombinedMuonParticles',
    'CombinedFitMuonParticles',
    'HLT_InDetTrigParticleCreation_Muon_EFID',
    'MuGirlRefittedTrackParticles',
    'HLT_InDetTrigParticleCreation_FullScan_EFID',
    'HLT_InDetTrigParticleCreation_MuonIso_EFID',
    'HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
    'HLT_InDetTrigParticleCreation_Tau_EFID',
    'HLT_InDetTrigParticleCreation_Bjet_EFID',
    'GSFTrackParticleCandidate',
    'HLT_InDetTrigParticleCreation_Electron_EFID',
    'TrackParticleCandidate',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')

