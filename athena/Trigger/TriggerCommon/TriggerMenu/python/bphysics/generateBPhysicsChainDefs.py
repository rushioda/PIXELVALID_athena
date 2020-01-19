# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 

###########################################################################
# SliceDef file for Bphysics chains
###########################################################################
__author__  = 'M.Backes, C.Bernius, J.Walder'
__doc__="Definition of bphysics chains" 

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.menu.MenuUtils import setupTopoStartFrom
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence

log.bphysTrigWarning = True


###########################################################################
###########################################################################

def generateChainDefs(chainDict):
    
    
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if TriggerFlags.run2Config=='2016':
        thisIsBphysChain = False
    else :
        thisIsBphysChain = True


    # OI - this is really should be managed in RunTier0Tests and not here!! 
    menu_name = TriggerFlags.triggerMenuSetup()
    if 'MC_pp_v6' in menu_name and TriggerFlags.run2Config!='2016' :
        log.warning(menu_name+" is used with run2Config = "+str(TriggerFlags.run2Config)+" will use Bphys trigger config for 2016!!")
        thisIsBphysChain = False
        if log.bphysTrigWarning :
            log.bphysTrigWarning = False

    elif log.bphysTrigWarning :
        log.bphysTrigWarning = False
        log.info( "Bphys trigger will be setup for run2config = "+str(TriggerFlags.run2Config)+" as requested")
        
    
    if "bBmumuxv3" in chainDict['chainName'] :  # OI this mc 2016 chain was never enabled, and is not needed in future
        thisIsBphysChain = False
    if 'Zmumu' in chainDict['chainName'] or 'idperf' in chainDict['chainName']  : # keep idperf chains with default muon config
        thisIsBphysChain = False
    if 'legacyVtx' in chainDict['chainName'] :  # OI Drell-Yan chains are not migrated
        thisIsBphysChain = False
    if 'invm' in chainDict['chainName'] :  # OI Drell-Yan chains are not migrated
        thisIsBphysChain = False
    
    if 'bBeex' in chainDict['chainName'] :
        log.error( " bBeex chain should be part of EgammaSignature, not BphysicsSignature, to ensure egamma component " )
    else :
        from TriggerMenu.muon.generateMuonChainDefs import generateChainDefs as genMuonChainDefs
        theChainDef =     genMuonChainDefs(chainDict, thisIsBphysChain)

    if not chainDict["topo"]:
        log.error( "No topo given -> not a bphysics chain...")
    else:
        if thisIsBphysChain : # new setup 
            theChainDef = _addTopoInfo(theChainDef,chainDict, doL2MultiTrack = True)
        else :
            theChainDef = _addTopoInfo(theChainDef,chainDict, doL2MultiTrack = False)

    return theChainDef


###########################################################################
###########################################################################
def _addTopoInfo(theChainDef,chainDict,doAtL2AndEF=True, doL2MultiTrack = False):

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements'] 
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

    topoAlgs = chainDict["topo"]

    SameConfigTopos = ['bJpsi', 'bDimu', 'bDimu2700', 'bDimu6000', 'bTau', 'bTauTrk', 'trkTau', 'bTauTight',
                       'bDsPhiXtight','bDsPhiX', 'bPhi', 
                       'bBmumu', 'bJpsimumu', 'bUpsimumu', 'Zmumu','bUpsi',
                       'bNocut', 'bVertex2', 'bVertex3']
    ntopos = len(topoAlgs)

    topoThresh = chainDict['topoThreshold']
    topoStartFrom = setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None

            
    if ('bBmumux' in topoAlgs) | ('bBmumuxv2' in topoAlgs) | ('bBmumuxv3' in topoAlgs):
        theChainDef = bBmumuxTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom, doL2MultiTrack)
    #elif ( 'bBeex' in topoAlgs ) :
    #   B->ee X chain is setup in egamma/generateElectronChainDefs  but the Beex algo resides here, below
    #
    elif ('Trkloose' in topoAlgs):
        theChainDef = bMuTrack(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom)
    elif ('TrkPEBmon' in topoAlgs):
        theChainDef = bMuTrackPEB(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom, False)
    elif ('TrkPEB' in topoAlgs):
        theChainDef = bMuTrackPEB(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom, True)
    elif (ntopos ==1) & (topoAlgs[0] in SameConfigTopos):
        theChainDef = bSingleOptionTopos(theChainDef,chainDict, inputTEsL2, inputTEsEF, topoStartFrom, doL2MultiTrack)
    else:
        if 'PEB' in topoAlgs:
           theChainDef = bMultipleOptionTopos(theChainDef,chainDict,inputTEsL2, inputTEsEF, topoStartFrom, doL2MultiTrack, True)
        else:
           theChainDef = bMultipleOptionTopos(theChainDef,chainDict,inputTEsL2, inputTEsEF, topoStartFrom, doL2MultiTrack, False)

    return theChainDef


###################################################################################
def getBphysThresholds(chainDict) :
    mult = 0
    mult_without_noL1 = 0
    trkmuons = []
    fexNameExt = ""

    for part in chainDict['chainParts'] :
        mult = mult + int(part['multiplicity'])
        if not 'noL1' in  part['extra'] :
            mult_without_noL1 = mult_without_noL1 + int(part['multiplicity'])
    print "OI multiplicity ", mult, mult_without_noL1
    for dictpart in chainDict['chainParts']:
        #if 'noL1' in  dictpart['extra'] : continue
        if 'mu' in dictpart['trigType']:
            for x in range(0,int(dictpart['multiplicity'])):
                if dictpart['threshold']!='0':
                    dthr = float(dictpart['threshold'] ) 
                    thr= dthr * 1000.  # in MeV; 
                    #lower to match EF muon threshols
                    if dthr < 9.5 :
                        thr = thr - 350.
                    elif dthr < 11.5 :
                        thr = thr - 550. 
                    elif dthr < 21.5  :
                        thr = thr - 750.                         
                    else :
                        thr = thr -1000.

                else :
                    thr = 900.
                trkmuons.append(thr)
                fexNameExt = fexNameExt + "_"+str(int(dictpart['threshold']))
    return fexNameExt, trkmuons, mult, mult_without_noL1

###################################################################################
def getBphysElectronThresholds(chainDict) :
    mult = 0
    trkelectrons = []
    fexNameExt = ""

    for part in chainDict['chainParts'] :
        mult = mult + int(part['multiplicity'])
    
    pid = None
    maxThreshold = 0
    for dictpart in chainDict['chainParts']:
        #if 'noL1' in  dictpart['extra'] : continue
        if 'e' in dictpart['trigType']:
            for x in range(0,int(dictpart['multiplicity'])):
                if dictpart['threshold']!='0':
                    dthr = float(dictpart['threshold'] )
                    
                    if dthr > maxThreshold :
                        maxThreshold = dthr
                        pid = dictpart['IDinfo']
                    
                    thr= dthr * 1000.  # in MeV; 
                    #lower to match EF muon threshols
                    if dthr < 9.5 :
                        thr = thr - 350.
                    elif dthr < 11.5 :
                        thr = thr - 550. 
                    elif dthr < 21.5  :
                        thr = thr - 750.                         
                    else :
                        thr = thr -1000.

                else :
                    thr = 900.
                trkelectrons.append(thr)
                fexNameExt = fexNameExt + "_"+str(int(dictpart['threshold']))
    if pid == None :
        log.error( " failed to decode pid for chain "+chainDict['chainName'] )
    return fexNameExt, trkelectrons, mult, pid

###################################################################################

def bSingleOptionTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom, doL2MultiTrack = False):
    topoAlgs = chainDict["topo"]
    mtopo = topoAlgs[0]
    # TE naming deifnition
    TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    L2TEname = "L2_" + TEname+'_'+mtopo+'_'+chainDict['L1item'].replace(",","")
    EFTEname = "EF_" + chainDict['chainName']
    topo2StartFrom = None
    if topoStartFrom:
        L2TEname = "L2_" + TEname+'_'+mtopo+'_tsf_'+chainDict['L1item'].replace(",","")
        topo2StartFrom = L2TEname

    fexNameExt,trkmuons, mult, mult_without_noL1  = getBphysThresholds(chainDict)
    L2Fex = None
    L2Hypo = None
    
    if (mtopo == 'bJpsi'):
        if doL2MultiTrack :
            from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Jpsi
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Jpsi("L2MultiMuTrkHypo_Jpsi")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        else :
            from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_Jpsi
            from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_Jpsi
            L2Fex = L2MultiMuFex_Jpsi()
            L2Hypo = L2MultiMuHypo_Jpsi()
     
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Jpsi
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Jpsi
        EFFex = EFMultiMuFex_Jpsi()
        EFHypo = EFMultiMuHypo_Jpsi()
        
    elif (mtopo =='bDimu'):
        import re
        if ('3mu' in chainDict['chainName'] or re.search('2mu[0-9]+_mu[0-9]+',chainDict['chainName']) or re.search('mu[0-9]+_2mu[0-9]+',chainDict['chainName'])):
            if doL2MultiTrack :
                from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
                from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu
                L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
                L2Fex.setTrackThresholds( trkmuons )
                L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu")
                L2Hypo.bphysCollectionKey = "MultiTrkFex"
            else :
                from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_DiMu
                from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_DiMu
                L2Fex = L2MultiMuFex_DiMu()
                L2Hypo = L2MultiMuHypo_DiMu()

            from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu
            EFFex = EFMultiMuFex_DiMu()
            EFHypo = EFMultiMuHypo_DiMu()
        else:
            #This is a di-muon chain
            if doL2MultiTrack :
                from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
                from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu
                L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
                L2Fex.setTrackThresholds( trkmuons )
                L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu")
                L2Hypo.bphysCollectionKey = "MultiTrkFex"
            else :
                from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
                from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
                L2Fex  = L2BMuMuFex_DiMu()
                L2Hypo = L2BMuMuHypo_DiMu()

            from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
            from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu
            EFFex  = EFBMuMuFex_DiMu()
            EFHypo = EFBMuMuHypo_DiMu()

    elif (mtopo =='bDimu2700'):
        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_2700
        L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_bTau")

        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_DiMu
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu2700
        EFFex = EFMultiMuFex_DiMu()
        EFHypo = EFMultiMuHypo_DiMu2700()

    elif (mtopo =='bDimu6000'):
        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_L26000
        L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_L26000("L2MultiMuHypo_bDimu6000")

        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_DiMu
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu6000
        EFFex = EFMultiMuFex_DiMu()
        EFHypo = EFMultiMuHypo_DiMu6000()

    elif (mtopo == 'bTauTrk'):

        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkTau
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau3
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_2700

        L2Fex = TrigMultiTrkFex_trkTau("TrigMultiTrkFex_trkTau"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_trkTau")  
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        if mult == 2 :
            EFFex = EFMultiMuFex_Tau2()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_bTau")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"
        else :
            EFFex = EFMultiMuFex_Tau3()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_Tau3")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"

    elif (mtopo == 'bNocut'):

        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu_noCut
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu_noCut
        from TrigBphysHypo.TrigBphysMuonCounterConfig import  TrigBphysMuonCounter_bNmu
        # at level 2 just check that there is at least 2 tracks. One could cut this more down with multiplicity cut for 3mu_bNocut
        # but because of 3mu_bJpsi items we will not gain anything
        L2Fex = TrigMultiTrkFex_DiMu_noCut("TrigMultiTrkFex_DiMu_noCut"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_DiMu_noCut("L2MultiMuTrkHypo_noCut")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        EFFex = TrigBphysMuonCounter_bNmu("TrigBphysMuonCounter"+fexNameExt, trkmuons)
        EFHypo = None

    elif (mtopo == 'bVertex2'):

        from TrigBphysHypo.TrigMultiTrkFexConfig import  TrigMultiTrkFex_Vtx2
        from TrigBphysHypo.TrigBphysMuonCounterConfig import  TrigBphysMuonCounter_bNmu
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Vtx20
        # at level 2 just check that there is at least 2 tracks. One could cut this more down with multiplicity cut for 3mu_bNocut
        # but because of 3mu_bJpsi items we will not gain anything
        L2Fex = TrigMultiTrkFex_Vtx2("TrigMultiTrkFex_Vtx2"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_Vtx20("EFMultiMuHypo_Vtx20")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        EFFex = None #TrigBphysMuonCounter_bNmu("TrigBphysMuonCounter"+fexNameExt, trkmuons)
        #EFFex.setEFMuonThresholds( trkmuons )
        EFHypo = None

    elif (mtopo == 'bVertex3'):

        from TrigBphysHypo.TrigMultiTrkFexConfig import  TrigMultiTrkFex_Vtx3
        from TrigBphysHypo.TrigBphysMuonCounterConfig import  TrigBphysMuonCounter_bNmu
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Vtx20
        # at level 2 just check that there is at least 2 tracks. One could cut this more down with multiplicity cut for 3mu_bNocut
        # but because of 3mu_bJpsi items we will not gain anything
        L2Fex = TrigMultiTrkFex_Vtx3("TrigMultiTrkFex_Vtx3"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_Vtx20("EFMultiMuHypo_Vtx20")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        EFFex = None #TrigBphysMuonCounter_bNmu("TrigBphysMuonCounter"+fexNameExt, trkmuons)
        #EFFex.setEFMuonThresholds( trkmuons )       
        EFHypo = None

    elif (mtopo == 'bTau'):

        
        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkTau
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau3
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_2700

        if doL2MultiTrack :
            from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkTau
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu
            L2Fex = TrigMultiTrkFex_trkTau("TrigMultiTrkFex_trkTau"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_bTau")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        else :  # in 2016 there was no L2 part
            L2Fex = None
            L2Hypo = None

        if mult == 2 :
            EFFex = EFMultiMuFex_Tau2()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_bTau")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"

        else :
            EFFex = EFMultiMuFex_Tau3()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_Tau3")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"

    elif (mtopo == 'bTauTight'):
        # this option sets tracks pt cuts to 
        
        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkTau
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau3
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_2700

        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkTau
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu
        L2Fex = TrigMultiTrkFex_trkTau("TrigMultiTrkFex_trkTauTight"+fexNameExt)
        L2Fex.setNumberAndTrackThresholds( len(trkmuons), trkmuons )
        L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_bTau")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        if mult == 2 :
            EFFex = EFMultiMuFex_Tau2()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_bTau")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"

        else :
            EFFex = EFMultiMuFex_Tau3()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_Tau3")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"


            
    elif (mtopo == 'trkTau'):  # no EF muon hypo

        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkTau
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_2700
                
        L2Fex = TrigMultiTrkFex_trkTau("TrigMultiTrkFex_trkTau"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )        
        L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_trkTau")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"
        
        EFFex = TrigMultiTrkFex_trkTau("EFTrigMultiTrkFex_trkTau"+fexNameExt)
        EFFex.setTrackThresholds( trkmuons )  
        EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_trkTau")
        EFHypo.bphysCollectionKey = "MultiTrkFex"

    elif (mtopo == 'bDsPhiXtight'):
 
        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkPhiXTight
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_2700
        L2Fex = TrigMultiTrkFex_trkPhiXTight("TrigMultiTrkFex_trkPhiXTight"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_DsPhiXtight")  # OI can keep the name, then all histograms will be merged
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Phi

        EFFex = EFMultiMuFex_Tau2()
        EFHypo = EFMultiMuHypo_Phi()
        EFHypo.bphysCollectionKey = "EFMultiMuFex"

    elif (mtopo == 'bDsPhiX'):
 
        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkPhiX
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_2700
        L2Fex = TrigMultiTrkFex_trkPhiX("TrigMultiTrkFex_trkPhiX"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_DsPhiX")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Phi
        EFFex = EFMultiMuFex_Tau2()
        EFHypo = EFMultiMuHypo_Phi()
        EFHypo.bphysCollectionKey = "EFMultiMuFex"
        #        EFFex = TrigMultiTrkFex_trkPhiX("EFMultiTrkFex_trkPhiX"+fexNameExt)
        #EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_DsPhiX")
        #EFHypo.bphysCollectionKey = "EFMultiMuFex"

    elif (mtopo == 'bPhi'):

        from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_trkPhi
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Phi
        L2Fex = TrigMultiTrkFex_trkPhi("TrigMultiTrkFex_trkPhi"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_Phi("L2MultiMuHypo_Phi")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"

        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2

        EFFex = EFMultiMuFex_Tau2()
        EFHypo = EFMultiMuHypo_Phi("EFMultiMuHypo_Phi")
        EFHypo.bphysCollectionKey = "EFMultiMuFex"

    
    elif (mtopo == 'bUpsi'):
        if doL2MultiTrack :
            from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Upsi
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )            
            L2Hypo = EFMultiMuHypo_Upsi("L2MultiMuTrkHypo_Upsi")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        else:
            from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_Upsi
            from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_Upsi
            L2Fex = L2MultiMuFex_Upsi()
            L2Hypo = L2MultiMuHypo_Upsi()        
            
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Upsi
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Upsi
        EFFex = EFMultiMuFex_Upsi()
        EFHypo = EFMultiMuHypo_Upsi()
    
    elif (mtopo == 'bBmumu'):
        if doL2MultiTrack :
            from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_BMeson
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_BMeson("L2MultiMuTrkHypo_BMeson")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        else :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_B
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_B
            L2Fex  = L2BMuMuFex_B()
            L2Hypo = L2BMuMuHypo_B()
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B()
        
    elif (mtopo == 'bJpsimumu'):
        if doL2MultiTrack :
            from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Jpsi
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Jpsi("L2MultiMuTrkHypo_Jpsi")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        else :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
            L2Fex  = L2BMuMuFex_Jpsi()
            L2Hypo = L2BMuMuHypo_Jpsi()
            
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi()

    elif (mtopo == 'bUpsimumu'):
        if doL2MultiTrack :
            from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Upsi
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Upsi("L2MultiMuTrkHypo_Upsi")
        else :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Upsi
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Upsi
            L2Fex  = L2BMuMuFex_Upsi()
            L2Hypo = L2BMuMuHypo_Upsi()
            
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Upsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Upsi
        EFFex  = EFBMuMuFex_Upsi()
        EFHypo = EFBMuMuHypo_Upsi()

    elif (mtopo == 'Zmumu'):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import  L2BMuMuFex_noId_Z
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Z
        from TrigBphysHypo.TrigEFBMuMuFexConfig import  EFBMuMuFex_Z
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Z
        L2Fex  = L2BMuMuFex_noId_Z() 
        L2Hypo = L2BMuMuHypo_Z()
        EFFex  = EFBMuMuFex_Z()
        EFHypo = EFBMuMuHypo_Z()

    else:
        log.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], mtopo ))

    # OI make sure that L2Fex is not running, when only 1 muon and therefore only 1 ID RoI is processed at L2
    if  L2Fex != None :

        if "btrk" in chainDict['chainName'] : # add wider cone tracking
            [trkfastw, trkprecw] = TrigInDetSequence("BphysicsCone02", "muonBtrk", "IDTrig").getSequence()

            # OI: fast tracking seems to be enough to get tracks for muon seeding.
            # precision tracking is then done after MS segments are identified, with default muon ID cone
            theChainDef.addSequence( trkfastw ,inputTEsL2, L2TEname+"_widetrk", topo_start_from = None)
            theChainDef.addSignatureL2([L2TEname+"_widetrk"])
            inputTEsL2 = L2TEname+"_widetrk"

            
        if mult_without_noL1 == mult :  # no noL1 parts
            theChainDef.addSequence([L2Fex, L2Hypo], inputTEsL2, L2TEname, topo_start_from = topoStartFrom)
            theChainDef.addSignatureL2([L2TEname])
        else :  # insert this after Hypo that goes after EF ID
            position = -1
            for signature in theChainDef.signatureList:
                if signature['listOfTriggerElements'][0].startswith( "EF_FStracksMuon" ) or signature['listOfTriggerElements'][0].startswith( "EF_NStrkMu" ) :
                    if position == -1 :
                        position = signature['signature_counter']
                    else : 
                        position = min(position, signature['signature_counter'])
            if position > -1 :
                locInputTEs = theChainDef.signatureList[position]['listOfTriggerElements']
                locTEname = "EF_mTrk_" + TEname+'_'+mtopo+'_'+chainDict['L1item'].replace(",","")
                theChainDef.addSequence([L2Fex, L2Hypo], locInputTEs, locTEname, topo_start_from = None)
                theChainDef.insertSignature(position+1, [locTEname])
            else :
                log.error('Bphysics Chain %s  unknown noL1 configuration, please check!! ' %(chainDict['chainName'] ))

        # Now catch place in muon sequence that should be seeded by Bphys FEX :
        for sequence in theChainDef.sequenceList:
            if sequence['input'][0].startswith( "L2BPHYS_TOBEFILLED" ) :
                sequence['input'][0] = L2TEname
                
    else :
        # that is to make sure that L1 topo seed is not give to EF-only chains..
        topo2StartFrom = None

    # add step that counts EF muons above required thresholds
    from TrigBphysHypo.TrigBphysMuonCounterConfig import  TrigBphysMuonCounter_bNmu
    EFFexMu = TrigBphysMuonCounter_bNmu("TrigBphysMuonCounter"+fexNameExt, trkmuons)
    EFFexMu.setEFMuonThresholds( trkmuons )
    theChainDef.addSequence([EFFexMu],inputTEsEF, EFTEname+"_MuCounter", topo_start_from=topo2StartFrom)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname+"_MuCounter"])    


    # Here we need to use inputs from Muon sequence and not from TrigBphysMuonCounter_bNmu..
    if EFFex != None :
        if EFHypo != None :        
            theChainDef.addSequence([EFFex, EFHypo],inputTEsEF, EFTEname, topo_start_from=topo2StartFrom)
        else :
            theChainDef.addSequence([EFFex],inputTEsEF, EFTEname, topo_start_from=topo2StartFrom)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])    




    
    if 'idperf' in chainDict['chainName']:
      from TrigIDTPMonitor.TrigIDTPMonitorConfig import IDTPMonitor                                                                          
      IDTP = IDTPMonitor()
      myInputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
      theChainDef.addSequence([IDTP],myInputTEsEF,EFTEname+"_monit")
      theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1,[EFTEname+"_monit"])


    if "bTau" in chainDict['chainName'] :
        #print "OI btrk ", theChainDef
            
        print "OI resulting sequence ", sequence
      
    return theChainDef


###################################################################################
###################################################################################
def bMultipleOptionTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom, doL2MultiTrack = False, doPEB = False):

    topoAlgs = chainDict["topo"]
    TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    
    myTopoString = ''
    for mtopo in topoAlgs:
        myTopoString =myTopoString+'_'+mtopo
    L2TEname = "L2_" + TEname+myTopoString+'_'+chainDict['L1item'].replace(",","")
    EFTEname = "EF_" + chainDict['chainName']

    topo2StartFrom = None
    if topoStartFrom:
        L2TEname = "L2_" + TEname+myTopoString+'_tsf_'+chainDict['L1item'].replace(",","")
        topo2StartFrom = L2TEname

    fexNameExt,trkmuons, mult, mult_without_noL1  = getBphysThresholds(chainDict)

    inputToEFFEX = None

        #from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
        #from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu
    from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu,TrigMultiTrkFex_DiMu_noCut,TrigMultiTrkFex_DiMu_noVtx_noOS,TrigMultiTrkFex_DiMu_noVtx_noM_SS
    from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu_noCut,EFMultiMuHypo_DiMu_noVtx,EFMultiMuHypo_DiMu,EFMultiMuHypo_BMeson,EFMultiMuHypo_Jpsi,EFMultiMuHypo_Upsi,EFMultiMuHypo_2700

    if ('7invm9' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        #if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_7invm9_noOS
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_7invm9
            L2Fex  = L2BMuMuFex_DiMu_7invm9_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_7invm9()
        ## else :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu_7invm9_noVtx")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 7000.
        ##     L2Hypo.UpperMassCut= 9000.
        
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_7invm9_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_7invm9
        EFFex  = EFBMuMuFex_DiMu_7invm9_noOS()
        EFHypo = EFBMuMuHypo_DiMu_7invm9()
    elif ('7invm9' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_7invm9_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_7invm9_noVtx
#        if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_7invm9_noOS
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_7invm9_noVtx
            L2Fex  = L2BMuMuFex_DiMu_7invm9_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_7invm9_noVtx()
        ## else :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu_noVtx("L2MultiMuTrkHypo_DiMu_noVtx_7invm9")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 7000.
        ##     L2Hypo.UpperMassCut= 9000.

        EFFex  = EFBMuMuFex_DiMu_7invm9_noOS()
        EFHypo = EFBMuMuHypo_DiMu_7invm9_noVtx()
    elif ('18invm60' in topoAlgs) & ('noos' in topoAlgs):
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_18invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_18invm60_noVtx           
#        if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_18invm60_noOS
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_18invm60_noVtx           
            L2Fex  = L2BMuMuFex_DiMu_18invm60_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_18invm60_noVtx()
        ## else :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu_18invm60")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 18000.
        ##     L2Hypo.UpperMassCut= 60000.
        EFFex  = EFBMuMuFex_DiMu_18invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_18invm60_noVtx()
    elif ('11invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm60
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm60
#        if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            L2Fex  = L2BMuMuFex_DiMu_11invm60_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_11invm60()
        ## else:
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu_11invm60")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 11000.
        ##     L2Hypo.UpperMassCut= 60000.
        EFFex  = EFBMuMuFex_DiMu_11invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm60()                   
    elif ('11invm24' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm24
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm24
#        if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            L2Fex  = L2BMuMuFex_DiMu_11invm24_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_11invm24()
        ## else :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu_11invm24")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 11000.
        ##     L2Hypo.UpperMassCut= 24000.
        
        EFFex  = EFBMuMuFex_DiMu_11invm24_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm24()
    elif ('24invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_24invm60
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_24invm60
        ## if  doL2MultiTrack :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu_24invm60")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 24000.
        ##     L2Hypo.UpperMassCut= 60000.
        ## else :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            L2Fex  = L2BMuMuFex_DiMu_24invm60_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_24invm60()
        EFFex  = EFBMuMuFex_DiMu_24invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_24invm60()
    elif ('11invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm60_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm60_noVtx           
#        if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            L2Fex  = L2BMuMuFex_DiMu_11invm60_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_11invm60_noVtx()
        ## else :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu_noVtx("L2MultiMuTrkHypo_DiMu_noVtx_11invm60")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 11000.
        ##     L2Hypo.UpperMassCut= 60000.
        EFFex  = EFBMuMuFex_DiMu_11invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm60_noVtx()                   
    elif ('11invm24' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm24_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm24_noVtx           
#        if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            L2Fex  = L2BMuMuFex_DiMu_11invm24_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_11invm24_noVtx()
        ## else :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu_noVtx("L2MultiMuTrkHypo_DiMu_noVtx_11invm24")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 11000.
        ##     L2Hypo.UpperMassCut= 24000.
       
        EFFex  = EFBMuMuFex_DiMu_11invm24_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm24_noVtx()
    elif ('24invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_24invm60_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_24invm60_noVtx           
#        if not doL2MultiTrack :
        # OI do MultiTrk is not working yet for invm chains
        if 1 :
            L2Fex  = L2BMuMuFex_DiMu_24invm60_noOS()
            L2Hypo = L2BMuMuHypo_DiMu_24invm60_noVtx()
        ## else :
        ##     L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS()
        ##     L2Hypo = EFMultiMuHypo_DiMu_noVtx("L2MultiMuTrkHypo_DiMu_noVtx_24invm60")  
        ##     L2Hypo.bphysCollectionKey = "MultiTrkFex"
        ##     L2Hypo.LowerMassCut= 24000.
        ##     L2Hypo.UpperMassCut= 60000.
        EFFex  = EFBMuMuFex_DiMu_24invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_24invm60_noVtx()
   
    elif ('bTau' in topoAlgs) :
        
        chainParts = chainDict['chainParts']
        mult = 0
        for part in chainParts :
            mult = mult + int(part['multiplicity'])
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau3

        L2Fex = None
        L2Hypo = None
        if mult == 2 :
            EFFex = EFMultiMuFex_Tau2()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_bTau")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"

        else :
            EFFex = EFMultiMuFex_Tau3()
            EFHypo = EFMultiMuHypo_2700("EFMultiMuHypo_Tau3")
            EFHypo.bphysCollectionKey = "EFMultiMuFex"
     
    elif ('bDimu' in topoAlgs) & ('novtx' in topoAlgs) & ('noos' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigEFBMuMuFexConfig  import EFBMuMuFex_DiMu_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_noVtx
        if not doL2MultiTrack :
            from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_passL2
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
            L2Fex  = L2BMuMuFex_DiMu_passL2() 
            L2Hypo = L2BMuMuHypo_DiMu_passL2()
        else :
            L2Fex = TrigMultiTrkFex_DiMu_noCut("TrigMultiTrkFex_DiMu_noCut"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu_noCut("L2MultiMuTrkHypo_noCut")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_DiMu_noOS()
        EFHypo = EFBMuMuHypo_DiMu_noVtx()        
    elif ('bDimu' in topoAlgs) & ('novtx' in topoAlgs) & ('noos' in topoAlgs):
        if not doL2MultiTrack :
            from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_noOS
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_noVtx
            L2Fex  = L2BMuMuFex_DiMu_noOS() 
            L2Hypo = L2BMuMuHypo_DiMu_noVtx()
        else :
            L2Fex = TrigMultiTrkFex_DiMu_noVtx_noOS("TrigMultiTrkFex_DiMu_noVtx_noOS"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu_noVtx("L2MultiMuTrkHypo_noVtx")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        from TrigBphysHypo.TrigEFBMuMuFexConfig  import EFBMuMuFex_DiMu_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_noVtx
        EFFex  = EFBMuMuFex_DiMu_noOS()
        EFHypo = EFBMuMuHypo_DiMu_noVtx()        
    elif ('bDimu' in topoAlgs) & ('noinvm' in topoAlgs) & ('novtx' in topoAlgs) & ('ss' in topoAlgs):
        from TrigBphysHypo.TrigEFBMuMuFexConfig  import EFBMuMuFex_DiMu_noinvm_SS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_noinvm_noVtx
        if not doL2MultiTrack :
            from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_noinvm_SS
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_noinvm_noVtx
            L2Fex  = L2BMuMuFex_DiMu_noinvm_SS() 
            L2Hypo = L2BMuMuHypo_DiMu_noinvm_noVtx()
        else :
            L2Fex = TrigMultiTrkFex_DiMu_noVtx_noM_SS("TrigMultiTrkFex_DiMu_noVtx_noM_SS"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu_noCut("L2MultiMuTrkHypo_noCut")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_DiMu_noinvm_SS()
        EFHypo = EFBMuMuHypo_DiMu_noinvm_noVtx()    
    elif ('bJpsimumu' in topoAlgs) & ('noid' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_noId
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi_noId
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_noId
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi_noId
        

        if 'noL1' in chainDict['chainName'] : # OI then L2 is not available and L2Fex makes no sense
            L2Fex = None
            L2Hypo = None
            EFFex  = EFBMuMuFex_noId()
            EFHypo = EFBMuMuHypo_Jpsi_noId()

            # OI so noL1 logic is different from other EF muon chains, they are executed *after* other EF chains
            # so we need to re-attach last TE from default chain here.

            position = theChainDef.signatureList[-1]['signature_counter']
            normalLegTE = None
            # find last of default muon signature - noL1 starts with EF_dummy, so take two just before
            for signature in  theChainDef.signatureList :
              if signature['listOfTriggerElements'][0].startswith( "EF_dummy" ) :
                position = signature['signature_counter']
            normalLegTE =  theChainDef.signatureList[position-2]['listOfTriggerElements'] 
            # now re-attach TE to the sequence, put normal leg first, as this is what is expected by EFBMuMuFex
            theChainDef.signatureList[-1]['listOfTriggerElements']  = normalLegTE +  inputTEsEF 
            inputToEFFEX = theChainDef.signatureList[-1]['listOfTriggerElements']  

        else :
            L2Fex  = L2BMuMuFex_noId()
            L2Hypo = L2BMuMuHypo_Jpsi_noId()
            EFFex  = EFBMuMuFex_noId()
            EFHypo = EFBMuMuHypo_Jpsi_noId()

            



    elif ('bDimu' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_DiMu_passL2() 
            L2Hypo = L2BMuMuHypo_DiMu_passL2()
        else :
            L2Fex = TrigMultiTrkFex_DiMu_noCut("TrigMultiTrkFex_DiMu_noCut"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu_noCut("L2MultiMuTrkHypo_noCut")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_DiMu()
        EFHypo = EFBMuMuHypo_DiMu() 
        if ('nscan' in chainDict['chainName'] ): # do not do L2 at all; expect one TE
            L2Fex = None
            L2Hypo = None
            from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_oneTE
            EFFex  = EFBMuMuFex_DiMu_oneTE()
    elif ('bJpsimumu' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi_passL2
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_Jpsi_passL2()
            L2Hypo = L2BMuMuHypo_Jpsi_passL2()
        else :
            L2Fex = TrigMultiTrkFex_DiMu_noCut("TrigMultiTrkFex_DiMu_noCut"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu_noCut("L2MultiMuTrkHypo_noCut")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi() 
        if ('nscan' in chainDict['chainName'] ): # do not do L2 at all; expect one TE
            L2Fex = None
            L2Hypo = None
            from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi_oneTE
            EFFex  = EFBMuMuFex_Jpsi_oneTE()
    elif ('bBmumu' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_DiMu_passL2()
            L2Hypo = L2BMuMuHypo_DiMu_passL2()
        else :
            L2Fex = TrigMultiTrkFex_DiMu_noCut("TrigMultiTrkFex_DiMu_noCut"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu_noCut("L2MultiMuTrkHypo_noCut")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B() 
    elif ('bDimu' in topoAlgs) & ('noEFbph' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_passEF
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_passEF
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_DiMu() 
            L2Hypo = L2BMuMuHypo_DiMu()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu")
            L2Hypo.bphysCollectionKey = "MultiTrkFex"

        EFFex  = EFBMuMuFex_passEF()
        EFHypo = EFBMuMuHypo_passEF() 
    elif ('bJpsimumu' in topoAlgs) & ('noEFbph' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_passEF
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_passEF
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_Jpsi()
            L2Hypo = L2BMuMuHypo_Jpsi()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Jpsi("L2MultiMuTrkHypo_Jpsi")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_passEF()
        EFHypo = EFBMuMuHypo_passEF() 
    # Tight Chi2 variants
    elif ('bJpsimumu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi_tightChi2
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_Jpsi()
            L2Hypo = L2BMuMuHypo_Jpsi()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Jpsi("L2MultiMuTrkHypo_Jpsi")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi_tightChi2() 
    elif ('bDimu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_tightChi2
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_DiMu()
            L2Hypo = L2BMuMuHypo_DiMu()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"

        EFFex  = EFBMuMuFex_DiMu()
        EFHypo = EFBMuMuHypo_DiMu_tightChi2() 
    elif ('bBmumu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_B
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_B
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B_tightChi2
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_B()
            L2Hypo = L2BMuMuHypo_B()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_BMeson("L2MultiMuTrkHypo_BMeson")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B_tightChi2() 
    elif ('bUpsimumu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Upsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Upsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Upsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Upsi_tightChi2
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_Upsi()
            L2Hypo = L2BMuMuHypo_Upsi()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Upsi("L2MultiMuTrkHypo_Upsi")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"

        EFFex  = EFBMuMuFex_Upsi()
        EFHypo = EFBMuMuHypo_Upsi_tightChi2() 
    # Lxy > 0 variants
    elif ('bJpsimumu' in topoAlgs) & ('Lxy0' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi_Lxy0
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_Jpsi()
            L2Hypo = L2BMuMuHypo_Jpsi()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Jpsi("L2MultiMuTrkHypo_Jpsi")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi_Lxy0() 
    elif ('bDimu' in topoAlgs) & ('Lxy0' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_Lxy0
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_DiMu()
            L2Hypo = L2BMuMuHypo_DiMu()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu("L2MultiMuTrkHypo_DiMu") 
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_DiMu()
        EFHypo = EFBMuMuHypo_DiMu_Lxy0() 
    elif ('bDimu2700' in topoAlgs) & ('Lxy0' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu2700_Lxy0
        L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
        L2Fex.setTrackThresholds( trkmuons )
        L2Hypo = EFMultiMuHypo_2700("L2MultiMuHypo_bTau")
        L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_DiMu()
        EFHypo = EFBMuMuHypo_DiMu2700_Lxy0() 
    elif ('bBmumu' in topoAlgs) & ('Lxy0' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_B
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_B
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B_Lxy0
        if not doL2MultiTrack :
            L2Fex  = L2BMuMuFex_B()
            L2Hypo = L2BMuMuHypo_B()
        else :
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_BMeson("L2MultiMuTrkHypo_BMeson")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B_Lxy0() 
    # legacy vertexing
    elif ('bDimu' in topoAlgs) & ('legacyVtx' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_legacyVtx
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu
        L2Fex  = L2BMuMuFex_DiMu()
        L2Hypo = L2BMuMuHypo_DiMu()
        EFFex  = EFBMuMuFex_DiMu_legacyVtx()
        EFHypo = EFBMuMuHypo_DiMu() 
    else:
        log.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], topoAlgs ))

    # OI make sure that L2Fex is not running, when only 1 muon and therefore only 1 ID RoI is processed at L2
    if not doL2MultiTrack :
        theChainDef.addSequence([L2Fex, L2Hypo], inputTEsL2, L2TEname, topo_start_from = topoStartFrom)
        theChainDef.addSignatureL2([L2TEname])
    else :
     if  L2Fex != None :
        if mult_without_noL1 == mult :  # no noL1 parts
            theChainDef.addSequence([L2Fex, L2Hypo], inputTEsL2, L2TEname, topo_start_from = topoStartFrom)
            theChainDef.addSignatureL2([L2TEname])
        else :  # insert this after Hypo that goes after EF ID
            position = -1
            for signature in theChainDef.signatureList:
                if signature['listOfTriggerElements'][0].startswith( "EF_FStracksMuon" ) or signature['listOfTriggerElements'][0].startswith( "EF_NStrkMu" ) :
                    if position == -1 :
                        position = signature['signature_counter']
                    else : 
                        position = min(position, signature['signature_counter'])
            if position > -1 :
                locInputTEs = theChainDef.signatureList[position]['listOfTriggerElements']
                locTEname = "EF_mTrk_" + TEname+'_'+mtopo+'_'+chainDict['L1item'].replace(",","")
                theChainDef.addSequence([L2Fex, L2Hypo], locInputTEs, locTEname, topo_start_from = None)
                theChainDef.insertSignature(position+1, [locTEname])
            else :
                log.error('Bphysics Chain %s  unknown noL1 configuration, please check!! ' %(chainDict['chainName'] ))

    if not ('noEFbph' in topoAlgs) :
        from TrigBphysHypo.TrigBphysMuonCounterConfig import  TrigBphysMuonCounter_bNmu
        fexNameExt,trkmuons,mult, mult_without_noL1  = getBphysThresholds(chainDict)
        EFFexMu = TrigBphysMuonCounter_bNmu("TrigBphysMuonCounter"+fexNameExt, trkmuons)
        EFFexMu.setEFMuonThresholds( trkmuons )
        theChainDef.addSequence([EFFexMu],inputTEsEF, EFTEname+"_MuCounter", topo_start_from=topo2StartFrom)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname+"_MuCounter"])    

    # here we have to use inputs from Muon sequence and not TrigBphysMuonCounter_bNmu
    if inputToEFFEX :
        theChainDef.addSequence([EFFex, EFHypo],inputToEFFEX, EFTEname, topo_start_from = topo2StartFrom)
    else :
        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF, EFTEname, topo_start_from = topo2StartFrom)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])       

    if doPEB:
        from TrigDetCalib.TrigDetCalibConfig import TrigCheckForMuons_peb075
        bphysROBWriter = TrigCheckForMuons_peb075("bphysROBWriter_peb075")
        EFTEname = "EF_" + chainDict['chainName']+"_1"
        inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
        theChainDef.addSequence([bphysROBWriter],inputTEsEF, EFTEname)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname]+inputTEsEF)
        
    return theChainDef


###################################################################################
###################################################################################
def bMuTrackPEB(theChainDef,chainDict, inputTEsL2, inputTEsEF, topoStartFrom, doPEB):

    topoAlgs = chainDict["topo"]

    mtopo = topoAlgs[0]
    TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    if "lowpt" in chainDict['chainName'] :
        L2TEname = "L2_" + TEname+'_'+mtopo+'_lowpt_'+chainDict['L1item'].replace(",","")
    else :
        L2TEname = "L2_" + TEname+'_'+mtopo+'_'+chainDict['L1item'].replace(",","")

    
    #--- 1: L2 first add large cone before duing superEF
    #[trkfast, trkprec] = TrigInDetSequence("Bphysics", "bphysics", "IDTrig").getSequence()
    [trkfast, trkprec] = TrigInDetSequence("BphysHighPt", "bphysHighPt", "IDTrig").getSequence()
    L2TEcount = 0; L2outTEsfast = [];  L2outTEsprec = [];
    for L2inputTE in inputTEsL2:
        L2TEcount = L2TEcount + 1
        L2outputTEfast = L2inputTE+'_idfast_'+str(L2TEcount)
        L2outTEsfast.append(L2outputTEfast)
        theChainDef.addSequence(trkfast,L2inputTE, L2outputTEfast)
        if not "L2Trk2" in chainDict['chainName'] :
            L2outputTEprec = L2inputTE+'_idprec_'+str(L2TEcount)
            L2outTEsprec.append(L2outputTEprec)
            theChainDef.addSequence(trkprec,L2outputTEfast, L2outputTEprec)

    theChainDef.addSignatureL2( L2outTEsfast)
    theChainDef.addSignatureL2( L2outTEsprec)
    theChainDef.addSequence([L2outTEsfast,L2outTEsprec],L2outputTEprec, L2TEname+"Trk")


    #--- 2: then add L2 multi trk fex+hypo for Jpsi
        
    fexNameExt,trkmuons, mult, mult_without_noL1  = getBphysThresholds(chainDict)

    if len( trkmuons) == 1 :
        if "lowpt" in chainDict['chainName'] :
            trkmuons.append(2000)
            fexNameExt = fexNameExt + "_"+str(2)
        else :
            trkmuons.append(3500)
            fexNameExt = fexNameExt + "_"+str(4)
    else :
         log.error('Bphysics Chain %s can not be constructed, more than 2 muons are requested ' %(chainDict['chainName']))
            
    from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_Jpsi
    L2Fex = TrigMultiTrkFex_Jpsi("TrigMultiTrkFex_TrkPEB"+fexNameExt)
    L2Fex.setTrackThresholds( trkmuons )
    L2Fex.bphysCollectionKey = "MultiTrkFex_TrkPEB"
    L2Fex.outputTrackCollectionKey = "MultiTrkFex_TrkPEB"
    #L2Fex.trackCollectionKey = "InDetTrigTrackingxAODCnv_Bphysics_IDTrig"

    from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Jpsi
    L2Hypo = EFMultiMuHypo_Jpsi("L2MultiMuTrkHypo_pebJpsi")
    L2Hypo.bphysCollectionKey = L2Fex.bphysCollectionKey 
    theChainDef.addSequence([L2Fex, L2Hypo], L2outTEsprec , L2TEname+"MultiTrk")
    theChainDef.addSignatureL2([L2TEname+"MultiTrk"])

    if doPEB:
        #---- 3 : last step - setup PEB
        from TrigDetCalib.TrigDetCalibConfig import TrigCheckForMuons_peb075
        bphysROBWriter = TrigCheckForMuons_peb075("bphysROBWriter_peb075")

        EFTEname = "EF_" + chainDict['chainName']+"_1"
        inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
        theChainDef.addSequence([bphysROBWriter],inputTEsEF, EFTEname)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname]+inputTEsEF)

    return theChainDef

###################################################################################
def bMuTrack(theChainDef,chainDict, inputTEsL2, inputTEsEF, topoStartFrom):

    EFChainName = "EF_" + chainDict['chainName']
    topoAlgs = chainDict["topo"]

    [trkfast, trkprec] = TrigInDetSequence("Bphysics", "bphysics", "IDTrig").getSequence()


    if ('bJpsi' in topoAlgs) & ('Trkloose' in topoAlgs):
        from TrigBphysHypo.TrigEFTrkMassFexConfig  import EFTrkMassFex_Jpsimumu_loose
        from TrigBphysHypo.TrigEFTrkMassHypoConfig import EFTrkMassHypo_Jpsimumu_loose
        EFFex  = EFTrkMassFex_Jpsimumu_loose()
        EFHypo = EFTrkMassHypo_Jpsimumu_loose()

    else:
        log.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], topoAlgs ))

    # Commented out section for L2 and EF sequence - not working with current FTF and prec tracking
    #    L2TEcount = 0; L2outTEs = []
    #    for L2inputTE in inputTEsL2:
    #        L2TEcount = L2TEcount+1
    #        L2outputTE = L2inputTE+'_idl2fast_'+str(L2TEcount)
    #        L2outTEs.append(L2outputTE)
    #        #theChainDef.addSequence([theTrigIdscan],L2inputTE, L2outputTE)
    #    theChainDef.addSignatureL2(L2outTEs)
    #
    #    theChainDef.addSequence( [L2Fex,  L2Hypo], L2outTEs, L2ChainName)
    ##    theChainDef.addSequence( [L2Fex,  L2Hypo], inputTEsL2, L2ChainName)
    #    theChainDef.addSignatureL2([L2ChainName])
    #
    #
    #
    #    EFTEcount = 0; EFoutTEsprec = [];
    #    for EFinputTE in inputTEsEF:
    #        EFTEcount = EFTEcount + 1
    #        EFoutputTEprec = EFinputTE+'_idefprec_'+str(EFTEcount)
    #        EFoutTEsprec.append(EFoutputTEprec)
    #        theChainDef.addSequence(trkprec,EFinputTE, EFoutputTEprec)
    #    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsprec)
    #
    #    theChainDef.addSequence([EFFex, EFHypo],EFoutTEsprec,EFChainName)
    #    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])


    # Sequence where only the EF mu+track stage is run. FTF+prec tracking run first
    #------- EF Sequences -------
    EFTEcount = 0; EFoutTEsfast = [];
    for EFinputTE in inputTEsEF:
        EFTEcount = EFTEcount + 1
        EFoutputTEfast = EFinputTE+'_idfast_'+str(EFTEcount)
        EFoutTEsfast.append(EFoutputTEfast)
        theChainDef.addSequence(trkfast,EFinputTE, EFoutputTEfast)

    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsfast)
        
    
    EFTEcount = 0; EFoutTEsprec = [];
    for EFinputTE in inputTEsEF:
        EFTEcount = EFTEcount + 1
        EFinputTEprec  = EFinputTE+'_idfast_'+str(EFTEcount)
        EFoutputTEprec = EFinputTE+'_idprec_'+str(EFTEcount)
        EFoutTEsprec.append(EFoutputTEprec)
        theChainDef.addSequence(trkprec,EFinputTEprec, EFoutputTEprec)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsprec)

    theChainDef.addSequence([EFFex, EFHypo], EFoutTEsprec, EFChainName, topo_start_from = topoStartFrom)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])


    return theChainDef



###################################################################################
#
#  Algorithms to select B-> mu mu X
#
def bBmumuxTopos(theChainDef,chainDict, inputTEsL2, inputTEsEF, topoStartFrom, doL2MultiTrack = False):
#
###################################################################################
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    topoAlgs = chainDict["topo"]
    TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    
    myTopoString = ''
    for mtopo in topoAlgs:
        myTopoString =myTopoString+'_'+mtopo
    L2TEname = "L2_" + TEname+myTopoString+'_'+chainDict['L1item'].replace(",","")
    EFTEname = "EF_" + chainDict['chainName']

    fexNameExt,trkmuons, mult, mult_without_noL1  = getBphysThresholds(chainDict)

    
    topo2StartFrom = None
    if topoStartFrom:
        L2TEname = "L2_" + TEname+myTopoString+'_tsf_'+chainDict['L1item'].replace(",","")
        topo2StartFrom = L2TEname

    from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu_noCut
    from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu_noCut
    # replace L2 Te (and EF tsf) with bBmumuxv2 naming
    # same underlying sequence is used
    # Warning if ever switch to v3 implementation -- this should be changed / tested 
    if 'BcmumuDsloose' in topoAlgs:
        L2TEname = L2TEname.replace("bBmumux_BcmumuDsloose","bBmumuxv2")
        if topo2StartFrom:
            topo2StartFrom = L2TEname
    if 'BpmumuKp' in topoAlgs:
        L2TEname = L2TEname.replace("bBmumux_BpmumuKp","bBmumuxv2")
        if topo2StartFrom:
            topo2StartFrom = L2TEname
    if 'BsmumuPhi' in topoAlgs:
        L2TEname = L2TEname.replace("bBmumux_BsmumuPhi","bBmumuxv2")
        if topo2StartFrom:
            topo2StartFrom = L2TEname
    if 'BsJpsiPhi' in topoAlgs:
        L2TEname = L2TEname.replace("bBmumux_BsJpsiPhi","bBmumuxv2")
        if topo2StartFrom:
            topo2StartFrom = L2TEname
    if 'LbmumuLambda' in topoAlgs:
        L2TEname = L2TEname.replace("bBmumux_LbmumuLambda","bBmumuxv2")
        if topo2StartFrom:
            topo2StartFrom = L2TEname
        
    if 'Ftk' in topoAlgs:
        from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
        trkftk = TrigInDetFTKSequence("BeamSpot", "beamSpot", [""]).getSequence()
    else:
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Bphysics", "bphysics", "IDTrig").getSequence()

#    from InDetTrigRecExample.EFInDetConfig import *
#    theTrigEFIDInsideOut = TrigEFIDInsideOut_Bphysics().getSequence()


    if 'BcmumuDsloose' in topoAlgs:
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_BcMuMuDs
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_BcMuMuDs
        EFFex = EFBMuMuXFex_BcMuMuDs()
        EFHypo = EFBMuMuXHypo_BcMuMuDs()
        
    elif 'LbmumuLambda' in topoAlgs:
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_LbMuMuLambda
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_LbMuMuLambda
        EFFex = EFBMuMuXFex_LbMuMuLambda()
        EFHypo = EFBMuMuXHypo_LbMuMuLambda()
        
    elif 'BpmumuKp' in topoAlgs:
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_BplusMuMuKplus
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_BplusMuMuKplus
        EFFex = EFBMuMuXFex_BplusMuMuKplus()
        EFHypo = EFBMuMuXHypo_BplusMuMuKplus()
        
    elif 'BsmumuPhi' in topoAlgs:
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_BsMuMuPhi
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_BsMuMuPhi
        EFFex = EFBMuMuXFex_BsMuMuPhi()
        EFHypo = EFBMuMuXHypo_BsMuMuPhi()
        
    elif 'BsJpsiPhi' in topoAlgs:
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_BsJpsiPhi
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_BsJpsiPhi
        EFFex = EFBMuMuXFex_BsJpsiPhi()
        EFHypo = EFBMuMuXHypo_BsJpsiPhi()

    elif 'Taumumux' in topoAlgs:
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_TauMuMuX
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_TauMuMuX
        EFFex = EFBMuMuXFex_TauMuMuX()
        EFHypo = EFBMuMuXHypo_TauMuMuX()

    elif 'Dsmumux' in topoAlgs:

        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_TauMuMuX
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_TauMuMuX
        EFFex = EFBMuMuXFex_TauMuMuX()
        EFHypo = EFBMuMuXHypo_TauMuMuX()
        
    elif 'bBmumuxv2' in topoAlgs:
        if 'Ftk' in topoAlgs:
           from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_FTK
           from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_FTK
           EFFex  =  EFBMuMuXFex_FTK()
           EFHypo = EFBMuMuXHypo_FTK()
        else:
           from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_1
           from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_1  
           EFFex  =  EFBMuMuXFex_1()
           EFHypo = EFBMuMuXHypo_1()
           # legacy vertexing
           if 'legacyVtx' in topoAlgs:
               from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_1_legacyVtx
               EFFex  =  EFBMuMuXFex_1_legacyVtx()
        
    elif 'bBmumuxv3' in topoAlgs:
#        print 'MOOOO in bBmumuxv3'
        from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_EF
        from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_EF
        EFFex  =  L2BMuMuXFex_EF()
        EFHypo = L2BMuMuXHypo_EF()
        
    else:
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        EFFex = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi()
       
    # Use simple di-muon fex/hypo for L2
    # Note - may need to change oppsign and vtx requirements
    # noL2 option to skip dimuon selection at L2

    if 'noL2' in topoAlgs:
        if doL2MultiTrack :
            L2Fex = TrigMultiTrkFex_DiMu_noCut("TrigMultiTrkFex_DiMu_noCut"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_DiMu_noCut("L2MultiMuTrkHypo_DiMu_noCut")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        else :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_passL2
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
            L2Fex  = L2BMuMuFex_DiMu_passL2()
            L2Hypo = L2BMuMuHypo_DiMu_passL2()
    elif 'bBmumuxv3' in topoAlgs:
#        print 'MOOOO2 in bBmumuxv3'
        from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_1
        from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_1
        L2Fex = L2BMuMuXFex_1()
        L2Hypo = L2BMuMuXHypo_1()

    else :
        if  doL2MultiTrack :
            from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Bmumux
            L2Fex = TrigMultiTrkFex_DiMu("TrigMultiTrkFex_DiMu"+fexNameExt)
            L2Fex.setTrackThresholds( trkmuons )
            L2Hypo = EFMultiMuHypo_Bmumux("EFMultiMuHypo_Bmumux")  
            L2Hypo.bphysCollectionKey = "MultiTrkFex"
        else :
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_1
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_1
            L2Fex  = L2BMuMuFex_1()
            L2Hypo = L2BMuMuHypo_1()
        

    if 'bBmumuxv3' in topoAlgs:
        L2TEcount = 0; L2outTEs = []
        for L2inputTE in inputTEsL2:
            L2TEcount = L2TEcount+1
            L2outputTE = L2inputTE+'_id_'+str(L2TEcount)
            L2outTEs.append(L2outputTE)
            theChainDef.addSequence(trkfast,L2inputTE, L2outputTE)
        theChainDef.addSignatureL2(L2outTEs)
        theChainDef.addSequence( [L2Fex,  L2Hypo], L2outTEs, L2ChainName)
        theChainDef.addSignatureL2([L2ChainName])    
    #------- EF Sequences -------
        EFTEcount = 0; EFoutTEs = []
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1
            #EFoutputTE = EFinputTE+'_id_'+str(EFTEcount)
            #EFoutTEs.append(EFoutputTE)
            #theChainDef.addSequence(trkprec,L2outputTE, EFoutputTE)  
            EFoutputTE = "EF_bphysprec_"+L2outputTE+"_" + str(EFTEcount)
            EFoutTEs.append(EFoutputTE)
            theChainDef.addSequence(trkprec,L2outputTE, EFoutputTE)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEs)
        theChainDef.addSequence([EFFex, EFHypo], EFoutTEs, EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    elif 'Ftk' in topoAlgs:
    
    #------- L2 Sequences -------
    # create the individual outputTEs together with the first sequences that are run
        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2TEname, topo_start_from = topoStartFrom)
        theChainDef.addSignatureL2([L2TEname])


    #------- EF Sequences -------
        from TrigGenericAlgs.TrigGenericAlgsConf import  PESA__DummyUnseededAllTEAlgo
        dummyAlgo = PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")
        trkFTK=[dummyAlgo]+trkftk[0]
        EFTEcount = 0; EFoutputTEsftk = [];
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1
            EFoutputTEftk = EFinputTE+'_ftk'+str(EFTEcount)
            EFoutputTEsftk.append(EFoutputTEftk)
        EFoutputTEftk = inputTEsEF[0]+'_ftk'

        theChainDef.addSequence(trkFTK , L2TEname, EFoutputTEftk)

        inputTEs_ftk = [EFoutputTEftk]+inputTEsEF

        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFoutputTEftk])



        theChainDef.addSequence([EFFex, EFHypo], inputTEs_ftk , EFTEname, topo_start_from = topo2StartFrom)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])        


    else:
    #------- L2 Sequences -------
    # create the individual outputTEs together with the first sequences that are run
        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2TEname, topo_start_from = topoStartFrom)
        theChainDef.addSignatureL2([L2TEname])


    #------- EF Sequences -------
        EFTEcount = 0; EFoutTEsfast = []; 
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1
            EFoutputTEfast = EFinputTE+'_idfast_'+str(EFTEcount)
            EFoutTEsfast.append(EFoutputTEfast)
            theChainDef.addSequence(trkfast,EFinputTE, EFoutputTEfast)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsfast)
        EFTEcount = 0; EFoutTEsprec = []; 
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1        
            EFinputTEprec  = EFinputTE+'_idfast_'+str(EFTEcount)
            EFoutputTEprec = EFinputTE+'_idprec_'+str(EFTEcount)
            EFoutTEsprec.append(EFoutputTEprec)
            theChainDef.addSequence(trkprec,EFinputTEprec, EFoutputTEprec)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsprec)
        theChainDef.addSequence([EFFex, EFHypo], EFoutTEsprec, EFTEname, topo_start_from = topo2StartFrom)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])


    return theChainDef

    
###################################################################################
#
#  Algorithm to select B->eeX
#
def bBeexTopos(theChainDef,chainDict, inputTEsL2, inputTEsEF ):
#
###################################################################################

# the FTK part here was not tested


    #L2ChainName = "L2_" + chainDict['chainName']
    #EFChainName = "EF_" + chainDict['chainName']
    topoAlgs = chainDict["topo"]
    #TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    
    myTopoString = ''
    for mtopo in topoAlgs:
        myTopoString =myTopoString+'_'+mtopo
    L2TEname = "L2_" + chainDict['chainName'] + "_Beex"  # TEname+myTopoString+'_'+chainDict['L1item'].replace(",","")
    EFTEname = "EF_" + chainDict['chainName']


    fexNameExt,trkelectrons, mult, pid  = getBphysElectronThresholds(chainDict)

    if 'Ftk' in topoAlgs:
        log.error("BPhysicsChainDefs not setup for ftk in beex topos yet!") 
        #from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
        #trkftk = TrigInDetFTKSequence("BeamSpot", "beamSpot", [""]).getSequence()
    else:
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Bphysics", "bphysics", "IDTrig").getSequence()

#    from InDetTrigRecExample.EFInDetConfig import *
#    theTrigEFIDInsideOut = TrigEFIDInsideOut_Bphysics().getSequence()


      
    # Use simple di-electron fex/hypo for L2
    # Note - may need to change oppsign and vtx requirements
    # noL2 option to skip dimuon selection at L2

    from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_DiE
    L2Fex = TrigMultiTrkFex_DiE("TrigMultiTrkFex_DiE"+fexNameExt)  
    L2Fex.setElectronTrackThresholds( trkelectrons )   
    
    if  'bBeexv2' in topoAlgs  : #  here we have only L2 with MultiTrack doL2MultiTrack :
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Bmumux
        L2Hypo = EFMultiMuHypo_Bmumux("EFMultiMuHypo_Beexv2")    # this Hypo cuts only on mass of Bphys object, so not important to separate muons and electrons
        L2Hypo.bphysCollectionKey = "MultiTrkFex_DiE"
        
    elif  'bBeexM2700' in topoAlgs  : #  here we have only L2 with MultiTrack doL2MultiTrack :
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu2700
        L2Hypo = EFMultiMuHypo_DiMu2700("EFMultiMuHypo_BeeM2700")    # 
        L2Hypo.bphysCollectionKey = "MultiTrkFex_DiE"
        
    elif  'bBeexM6000' in topoAlgs or 'bBeexM6000t' in topoAlgs  : #  here we have only L2 with MultiTrack doL2MultiTrack :
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu6000
        L2Hypo = EFMultiMuHypo_DiMu6000("EFMultiMuHypo_BeeM6000")    # 
        L2Hypo.bphysCollectionKey = "MultiTrkFex_DiE"  # do not change collection key, as it is output to AOD

    else :
        log.error( " Unknown Bphysics B->eeX chain "+ chainDict['chainName']+" do not know how to set up")
        return theChainDef

    # setup EF FEX+Hypo
        
    #------- L2 Sequences -------
    # create the individual outputTEs together with the first sequences that are run
    theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2TEname)
    theChainDef.addSignatureL2([L2TEname])


    if  'bBeexv2' in topoAlgs  :
        from TrigBphysHypo.TrigEFBEEXFexConfig import EFBEEXFex_1
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_1  
        EFFex  =  EFBEEXFex_1()     # here we really need FEX specific to EE final state
        EFHypo = EFBMuMuXHypo_1("EFBEEXHypo_1")
        EFHypo.bphysCollectionKey = "EFBEEXFex"
    #------- EF Sequences -------
        EFTEcount = 0; EFoutTEsfast = []; 
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1
            EFoutputTEfast = EFinputTE+'_idfast_'+str(EFTEcount)
            EFoutTEsfast.append(EFoutputTEfast)
            theChainDef.addSequence(trkfast,EFinputTE, EFoutputTEfast)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsfast)
        EFTEcount = 0; EFoutTEsprec = []; 
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1        
            EFinputTEprec  = EFinputTE+'_idfast_'+str(EFTEcount)
            EFoutputTEprec = EFinputTE+'_idprec_'+str(EFTEcount)
            EFoutTEsprec.append(EFoutputTEprec)
            theChainDef.addSequence(trkprec,EFinputTEprec, EFoutputTEprec)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsprec)
        theChainDef.addSequence([EFFex, EFHypo], EFoutTEsprec, EFTEname)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])

    else :  # add BphysElectron counter\

        #add L1 name to make sure name is unique
        
    
        # add step that counts EF electrons above required thresholds
        from TrigBphysHypo.TrigBphysElectronCounterConfig import  TrigBphysElectronCounter_bBee
        EFFexE = TrigBphysElectronCounter_bBee("TrigBphysECounter"+chainDict['chainName'], trkelectrons, pid,  4650. )
        EFFexE.setEFElectronThresholds( trkelectrons, 4650. )
        EFFexE.outputTrackCollectionKey = "BphysElectronCounter" 
    
        theChainDef.addSequence([EFFexE],inputTEsEF, EFTEname+"_eCounter")
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname+"_eCounter"])    

        if  'bBeexM6000t' in topoAlgs :
            EFFexM = TrigMultiTrkFex_DiE("TrigMultiTrkFex_EFDiE"+fexNameExt) 
            EFFexM.setElectronTrackThresholds( trkelectrons )
            EFFexM.trackCollectionKey = EFFexE.outputTrackCollectionKey  # these are input tracks, we want to use those identified by BphysElectronContainer
            EFFexM.outputTrackCollectionKey = "EFEMultiTrkFex_DiE"  # these are selected tracks for monitoring
            EFFexM.bphysCollectionKey = "EFEMultiTrkFex_DiE" # this is output container with Bphys objects that will be used in Hypo
             # do not change collection key, as it is output to AOD

            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu6000
            EFHypoM = EFMultiMuHypo_DiMu6000("EFMultiMuHypo_EFBeeM6000")    # 
            EFHypoM.bphysCollectionKey = EFFexM.bphysCollectionKey 
            theChainDef.addSequence([EFFexM, EFHypoM],EFTEname+"_eCounter", EFTEname+"_EFMass")
            theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname+"_EFMass"])    

        

    return theChainDef

    

###################################################################################
###################################################################################
def findL2teBaseName(cName,topos):
    indices = []
    for topo in topos:
        indices.append(cName.find(topo))

    firstTopoOccurance = sorted(indices,key=int)[0]
    
    return cName[:firstTopoOccurance-1]

###################################################################################
