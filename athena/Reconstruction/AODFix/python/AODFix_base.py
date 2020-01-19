# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

logAODFix = logging.getLogger( 'AODFix_base' )

#The AODFix base class
class AODFix_base(object):
    ''' This is the AODFix base class, which does nothing
    by default. Specific AODFix types should inherit from it.

    Derived classes should override do-nothing member functions,
    like preInclude, to implement the actual AODFix.

    Instance variables
      addMetadata (bool): whether to add metaData
      doAODFix (bool): whether to do AODFix
      prevAODFix (string): previous AODFix applied
      newAODFix (string): new AODFix to apply
      isMC (bool): is the input MC?
    '''

    @staticmethod
    def latestAODFixVersion():
        """ Must override this member function in derived classes
        """
        return []

    @staticmethod
    def excludeFromMetadata():
        """ Override if you want to remove something from metadata to be written
        """
        return []

    def __init__(self, prevVersion = "", isMC = False, force = False, isHI = False, isHIP = False):
        """ The default constructor. It implements the default behavior of setting
        up the latest AODFix to run. Only needs to be overriden if doing something
        more complicated.
        """
        self.isMC = isMC
        self.isHI = isHI
        self.isHIP = isHIP
        self.prevAODFix = prevVersion if not force else 'none' # if forcing, ignore old AODFix
        self.newAODFix = "-".join(self.latestAODFixVersion())
        logAODFix.debug( "latestAODFixVersion() = " +  self.newAODFix)
        logAODFix.debug( "prevVersion = " +  prevVersion)
        logAODFix.debug( "force = " +  str(force))
        if self.newAODFix == "":
            # the AODFix is empty: do nothing
            self.doAODFix = False
        elif not force:
            if prevVersion == self.newAODFix:
                self.doAODFix = False
            else:
                self.doAODFix = True

        else:  # force running
            self.doAODFix = True

        if self.doAODFix:
            logAODFix.info("AODFix with version %s scheduled" % self.newAODFix)

    def addMetaData(self):
        '''standard function to apply metadata, can overload if necessary for complicated scenarios.'''
        if self.doAODFix:
            logAODFix.debug("in addMetaData")

            from RecExConfig.RecFlags import rec

            suffix="_RAW"
            if rec.readESD():
                suffix="_ESD"
            elif rec.readAOD():
                suffix="_AOD"

            # remove any metadata we don't want to write out (in order to rerun again)
            metadataList = self.newAODFix.split("-")
            excludeFromMetadata = self.excludeFromMetadata()

            for excl in excludeFromMetadata:
                if excl in metadataList:
                    metadataList.remove(excl)

            str = "AODFix_" + "-".join(metadataList) + suffix

            logAODFix.info("executing addMetaData, will add as AODFixVersion %s" % str)
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AODFixVersion", str]

    def willDoAODFix(self):
        '''Returns a boolean on whether AOD to AOD fix will be performed'''
        return self.doAODFix

    # The functions below do nothing in the base class. Override them as necessary to implement AODFix.

    def preInclude(self):
        pass

    def postAtlfast(self):
        pass

    def postTrigger(self):
        pass

    def postSystemRec(self):
        pass

    def postCombinedRec(self):
        pass

    def postHeavyIon(self):
        pass

    # inside SystemRec_config.py:
    def postCaloRec(self):
        pass

    def postInDetRec(self):
        pass

    def postMuonRec(self):
        pass

    def postForwardRec(self):
        pass

    # inside CombinedRec_config.py:
    def postEgammaRec(self):
        pass

    def postMuonCombinedRec(self):
        pass

    def postJetRec(self):
        pass

    def postTauRec(self):
        pass

    def postMissingETRec(self):
        pass
