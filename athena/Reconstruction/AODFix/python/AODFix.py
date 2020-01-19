# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""@package AODFix 

This package is the only one meant to be used externally (mainly by
RecExCommon). Only the AODFix_* functions should be called.

When adding a new AODFix for a release family (where a release family
is defined by the first two numbers in a release, e.g., 20.1) that
does not yet have AODFix, the user needs to create a new
AODFix_rXXY.py file with an AODFix_rXXY class inheriting from the
AODFix_base class and overwrite the appropriate member
functions. Furthermore, the AODFix_Init() function below needs to set
_aodFixInstance approprately, as explained in the comments below.

When modifying the AODFix for a release family for which
AODFix_rXXY.py exists, most likely the changes would be restricted to
the AODFix_rXXY.py class. The only exception is if the release family
definition changes (e.g., add 17.3 to the 17.2 family).

"""

from AthenaCommon.Logging import logging

logAODFix = logging.getLogger( 'AODFix' )

from AODFix_base import AODFix_base
from AODFix_r210 import AODFix_r210

_aodFixInstance = AODFix_base()

def run_once(f):
    def wrapper(*args, **kwargs):
        if not wrapper.has_run:
            wrapper.has_run = True
            return f(*args, **kwargs)
    wrapper.has_run = False
    return wrapper

@run_once
def AODFix_Init():
    ''' This function is called to correctly setup AODFix.'''

    global _aodFixInstance

    from RecExConfig.RecFlags import rec

    ##################
    # exit if locked to false
    ##################
    if rec.doApplyAODFix.is_locked() and rec.doApplyAODFix.get_Value() == False:
        logAODFix.info("doApplyAODFix is locked to False; not applying anything")
        return


    ##################
    # deterimine in what release we are running (and fill some rec variables
    ##################

    import os
    curRelease=os.getenv("AtlasVersion") or ''

    logAODFix.debug("curRelease set to " + curRelease)
    
    
    ##################
    # determine athena with which input file was created and previous AODFix
    ##################
   
    # RDO doesn't have MetaData
    prevAODFix =''
    prevRelease = ''
    isMC = False
    isHI = rec.doHeavyIon()
    isHIP = rec.doHIP()

    # is it better to do this or to look at GlobalFlags?
    from RecExConfig.InputFilePeeker import inputFileSummary
    try:
        isMC = (inputFileSummary['evt_type'][0] == "IS_SIMULATION")
    except Exception:
        logAODFix.warning("Could not tell if the input file is data or MC; setting to data")
        isMC = False

    if not rec.readRDO():

        try:
            AtlasReleaseVersionString=inputFileSummary['metadata']['/TagInfo']['AtlasRelease']
            rv = AtlasReleaseVersionString.split('-')
            if len(rv) > 1:
                prevRelease = rv[1]
        except Exception:
            logAODFix.warning("no AthenRelease found in input file, setting to <none> !!")
            prevRelease=''


        ##################
        # determine which is the previous verion of the AODFix
        ##################
        try:
            prevAODFix=inputFileSummary['tag_info']['AODFixVersion']
            if prevAODFix.startswith("AODFix_"):
                prevAODFix=prevAODFix[7:]
                prevAODFix=prevAODFix.split("_")[0]
        except Exception:
            logAODFix.debug("no AODFixVersion found in input file, setting to <none>.")
            prevAODFix=''

    logAODFix.info("Summary of MetaData for AODFix:")
    if isMC:
        logAODFix.info(" Input file is MC")
    else:
        logAODFix.info(" Input file is DATA")
    logAODFix.info(" Input file produced with Athena version <%s>." % prevRelease)
    logAODFix.info(" AODFix version <%s> was previously applied." % prevAODFix)
    if rec.doApplyAODFix.is_locked():
        logAODFix.info(" AODFix is forced to run!")

    ##################
    # determine which AODFix to run (if actually running--to be determined later)
    ##################

    doAODFix = False
    if rec.readAOD():  # change in policy: no AODFix if reading ESD.
        doAODFix = True
        # check to see if it's a DAOD; do not run by default in DAODs
        from RecExConfig.InputFilePeeker import inputFileSummary
        try:
            streamNames = inputFileSummary['stream_names']
            if any(map(lambda x: "StreamDAOD" in x, streamNames)) and not rec.doApplyAODFix.is_locked():
                logAODFix.info("Running on a DAOD, so not scheduling AODFix")
                doAODFix = False
        except Exception:
            logAODFix.warning("Could not determine the stream name")

    elif rec.readESD() and rec.doApplyAODFix.is_locked():
        # only run on ESDs if forcing
        doAODFix = True

    #exit if not doing AODFix
    if not doAODFix:
        return  

    curReleaseSplit = curRelease.split('.')
    prevReleaseSplit = prevRelease.split('.')
    if len(curReleaseSplit) >= 2 and (rec.doApplyAODFix.is_locked() or len(prevReleaseSplit) >= 2):
        ### If adding an AODFix for a release family that does not have it,
        ### please add it to the if-elif... statement below
        # if (curReleaseSplit[0] == '21' and (curReleaseSplit[1] == '0' or curReleaseSplit[1] == '2') and 
        #       (rec.doApplyAODFix.is_locked() or 
        #        (prevReleaseSplit[0] == '21' and (prevReleaseSplit[1] == '0' or prevReleaseSplit[1] == '2')))):
        #     _aodFixInstance = AODFix_r210(prevAODFix, isMC, rec.doApplyAODFix.is_locked(), isHI, isHIP)
        # else:
        #     logAODFix.info("No AODFix scheduled for this release.")
        logAODFix.info("AODFix disabled.")

    else:
        logAODFix.info("Current release is of strange form: %s" % curRelease)

# To inquire whether AODFix will be run

def AODFix_willDoAODFix():
    '''Returns a boolean on whether AOD to AOD fix will be performed'''
    return _aodFixInstance.willDoAODFix()

# The interface routines
def AODFix_addMetaData():
    '''This adds metadata both when AODFix is run or when "fixed" reco is performed'''
    _aodFixInstance.addMetaData()

# inside RecExCommon_topOptions.py:
def AODFix_preInclude():
    _aodFixInstance.preInclude()

def AODFix_postAtlfast():
    _aodFixInstance.postAtlfast()

def AODFix_postTrigger():
    _aodFixInstance.postTrigger()

def AODFix_postSystemRec():
    _aodFixInstance.postSystemRec()

def AODFix_postCombinedRec():
    _aodFixInstance.postCombinedRec()

def AODFix_postHeavyIon():
    _aodFixInstance.postHeavyIon()

# inside SystemRec_config.py:
def AODFix_postCaloRec():
    _aodFixInstance.postCaloRec()

def AODFix_postInDetRec():
    _aodFixInstance.postInDetRec()

def AODFix_postMuonRec():
    _aodFixInstance.postMuonRec()

def AODFix_postForwardRec():
    _aodFixInstance.postForwardRec()

# inside CombinedRec_config.py:
def AODFix_postEgammaRec():
    _aodFixInstance.postEgammaRec()

def AODFix_postMuonCombinedRec():
    _aodFixInstance.postMuonCombinedRec()

def AODFix_postJetRec():
    _aodFixInstance.postJetRec()

# shoudl ideally capitalize tau, like the others
def AODFix_posttauRec():
    _aodFixInstance.postTauRec()

def AODFix_postMissingETRec():
    _aodFixInstance.postMissingETRec()
