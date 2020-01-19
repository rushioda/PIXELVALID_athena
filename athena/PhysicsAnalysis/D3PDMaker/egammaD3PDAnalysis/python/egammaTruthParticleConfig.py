# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/egammaTruthParticleConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2011
# @brief Configure algorithms to build filtered TruthParticleContainer
#        for egamma truth.
#


import egammaD3PDAnalysis
import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from McParticleAlgs.JobOptCfg                 import createMcAodBuilder
from RecExConfig.RecFlags                     import rec
from AthenaCommon.AlgSequence                 import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from AthenaCommon                             import CfgMgr


def egammaTruthParticleConfig \
        (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
         sgkey = 'egammaTruth',
         prefix = '',
         doPileup     = D3PDMakerFlags.TruthDoPileup(),
         **kwargs):
    
    if not rec.doTruth():
        return

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('DataVector<xAOD::TruthParticle_v1>', sgkey):
        return

    algname = prefix + sgkey + 'Builder'
    if not hasattr (seq, algname):
        import AthenaCommon.CfgMgr as CfgMgr
        from egammaRec.Factories import ToolFactory
        exten = ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool,
                             name="GSFParticleCaloExtensionTool",
                             StartFromPerigee = True)()

        seq += egammaD3PDAnalysis.egammaTruthAlg (
            algname,
            InputKey = D3PDMakerFlags.TruthSGKey(),
            OutputKey = sgkey,
            ParticleCaloExtensionTool = exten,
            AuxPrefix = D3PDMakerFlags.EgammaUserDataPrefix())
            
        cfgKeyStore.addTransient ('DataVector<xAOD::TruthParticle_v1>', sgkey)

    return

    
    
