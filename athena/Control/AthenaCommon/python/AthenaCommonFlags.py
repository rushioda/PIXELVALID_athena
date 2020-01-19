# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file AthenaCommon/python/AthenaCommonFlags.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" AthenaCommonFlags
    Python module to hold common flags to configure JobOptions.

    From the python prompt:
    >>> from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    >>> print athenaCommonFlags.EvtMax()
    >>> athenaCommonFlags.EvtMax = 50
    >>> assert( athenaCommonFlags.EvtMax() == 50 )
    >>> athenaCommonFlags.print_JobProperties('tree&value')

"""

__author__ = "S.Binet, M.Gallas"
__version__= "$Revision: 1.11 $"
__doc__    = "AthenaCommonFlags"

__all__    = [ "athenaCommonFlags" ]

##-----------------------------------------------------------------------------
## Import

from JobProperties import JobProperty, JobPropertyContainer
from JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class EvtMax(JobProperty):
    """Number of events to process or generate"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 5

class SkipEvents(JobProperty):
    """Number of events to skip when reading an input POOL file. This should
    be given to the EventSelector service.
    """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0

class FilesInput(JobProperty):
    """The list of input data files (if not empty override all the specific XYZInput) """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class PoolEvgenInput(JobProperty):
    """The list of input POOL files containing HepMC decay trees"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Evgen.pool.root"]

class PoolEvgenOutput(JobProperty):
    """The name of the output POOL files containing HepMC decay trees"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "Event.pool.root"

class PoolHitsInput(JobProperty):
    """The list of input POOL files containing collections of Hits"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Hits.pool.root"]

class PoolHitsOutput(JobProperty):
    """The name of the output POOL file containing collections of Hits"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "Hits.pool.root"
    
class PoolRDOInput(JobProperty):
    """The list of input POOL files containing collections of Raw data objects
    ['myfile.pool.root'] : file in run directory
    ['LFN:mylogicalfile.root'] : logical file name as specified in PoolFileCatalog.cml
    ['rfio:/castor/cern.ch/somepath/somefile.pool.root'] : file on castor (at CERN)
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["RDO.pool.root"]

class PoolRDOOutput(JobProperty):
    """The name of the output POOL file containing collections of Raw data
    objects
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "RDO.pool.root"
    
class BSRDOInput(JobProperty):
    """The list of input ByteStream files containing collections of Raw data
    objects
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [""]

class BSRDOOutput(JobProperty):
    """The name of the output ByteStream file containing collections of Raw
    data objects
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    
class PoolESDInput(JobProperty):
    """The list of input POOL files containing collections of ESD objects"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["ESD.pool.root"]

class PoolESDOutput(JobProperty):
    """The name of the output POOL file containing collections of ESD objects
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "ESD.pool.root"
    
class PoolAODInput(JobProperty):
    """The list of input POOL files containing collections of AOD objects"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["AOD.pool.root"]

class PoolAODOutput(JobProperty):
    """The name of the output POOL file containing collections of AOD objects
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "AOD.pool.root"
    
class PoolTAGInput(JobProperty):
    """The list of input POOL files containing collections of TAGs"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["TAG.pool.root"]

class PoolTAGOutput(JobProperty):
    """The name of the output POOL file containing collections of TAGs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "TAG.pool.root"

class PoolInputQuery(JobProperty):
    """A selection criteria one can submit to the EventSelector.
    A few examples, in an athena session:
      athenaCommonFalgs.PoolInputQuery = 'EventNumber <= 1 && RunNumber!=1234'
      athenaCommonFlags.PoolInputQuery = 'NLooseMuon>0 || NLooseElectron>0'

      # this will later on be carried to the EventSelector:
      EventSelector.Query = athenaCommonFlags.PoolInputQuery
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""

class DoFullChain(JobProperty):
    """ If True does full chain: simulation + digitisation + reconstruction
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AllowIgnoreExistingDataObject(JobProperty):
    """Allow an algorithm to ignore wether an input object is available or not.
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class AllowIgnoreConfigError(JobProperty):
    """Allow an algorithm to ignore return error code from upstream algorithm
    and tools.
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class isOnline(JobProperty):
    """ Set to True when running online
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class isOnlineStateless(JobProperty):
    """ Set to True when running emon bytestreamsvc online
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class UseLZMA(JobProperty):
    """ Set the compression algorith to LZMA
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class RuntimeStrictness(JobProperty):
    """Controls setting 
    """
    statusOn      = False    # there's a logic error in JobProperty, so set_On() in __init__
    allowedTypes  = ['str']
    allowedValues = ['abort', 'warn', 'none']
    StoredValue   = 'none'

    def __init__( self, *args, **kwds ):
        result = super( RuntimeStrictness, self ).__init__( *args, **kwds )
        self.set_On()
        return result

    def _do_action( self, *args, **kwds ):
        if self.StoredValue != 'none':
            import CfgMgr, AppMgr

            if not hasattr( AppMgr.ServiceMgr, 'FPEControlSvc' ):
                AppMgr.ServiceMgr += CfgMgr.FPEControlSvc()
                AppMgr.theApp.CreateSvc += [ AppMgr.ServiceMgr.FPEControlSvc.getFullJobOptName() ]
             # defaults are fine ...

         # the CoreDumpSvc, which catches SIGFPE by default, is forcefed from the
         # the standard unix options and not set again here

        else:
            self._undo_action()

        return super( RuntimeStrictness, self )._do_action( *args, **kwds )

    def _undo_action( self, *args, **kwds ):
        import AppMgr

        try:
            AppMgr.theApp.CreateSvc.remove( AppMgr.ServiceMgr.FPEControlSvc.getFullJobOptName() )
            delattr( AppMgr.ServiceMgr, 'FPEControlSvc' )
        except (ValueError, AttributeError):
         # not a logical error: this can legitimately happen if _do_action was
         # called with set value 'none', followed by set_Off()
            pass

         # there's no point in cleansing the JobOptionsSvc here, b/c CreateSvc
         # will be acted upon over the course of theApp.setup()

        return super( RuntimeStrictness, self )._do_action( *args, **kwds )

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the AthenaCommon flag container
class AthenaCommonFlags(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(AthenaCommonFlags)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the AthenaCommonFlags container
jobproperties.AthenaCommonFlags.add_JobProperty(EvtMax)
jobproperties.AthenaCommonFlags.add_JobProperty(SkipEvents)
jobproperties.AthenaCommonFlags.add_JobProperty(FilesInput )
jobproperties.AthenaCommonFlags.add_JobProperty(PoolEvgenInput )
jobproperties.AthenaCommonFlags.add_JobProperty(PoolEvgenOutput)
jobproperties.AthenaCommonFlags.add_JobProperty(PoolHitsInput )
jobproperties.AthenaCommonFlags.add_JobProperty(PoolHitsOutput)
jobproperties.AthenaCommonFlags.add_JobProperty(PoolRDOInput )
jobproperties.AthenaCommonFlags.add_JobProperty(PoolRDOOutput)
jobproperties.AthenaCommonFlags.add_JobProperty(BSRDOInput )
jobproperties.AthenaCommonFlags.add_JobProperty(BSRDOOutput)
jobproperties.AthenaCommonFlags.add_JobProperty(PoolESDInput )
jobproperties.AthenaCommonFlags.add_JobProperty(PoolESDOutput)
jobproperties.AthenaCommonFlags.add_JobProperty(PoolAODInput )
jobproperties.AthenaCommonFlags.add_JobProperty(PoolAODOutput)
jobproperties.AthenaCommonFlags.add_JobProperty(PoolTAGInput )
jobproperties.AthenaCommonFlags.add_JobProperty(PoolTAGOutput)
jobproperties.AthenaCommonFlags.add_JobProperty(PoolInputQuery)
jobproperties.AthenaCommonFlags.add_JobProperty(AllowIgnoreExistingDataObject)
jobproperties.AthenaCommonFlags.add_JobProperty(AllowIgnoreConfigError)
jobproperties.AthenaCommonFlags.add_JobProperty(isOnline)
jobproperties.AthenaCommonFlags.add_JobProperty(isOnlineStateless)
jobproperties.AthenaCommonFlags.add_JobProperty(RuntimeStrictness)
jobproperties.AthenaCommonFlags.add_JobProperty(DoFullChain)
jobproperties.AthenaCommonFlags.add_JobProperty(UseLZMA)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select AthenaCommonFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags = jobproperties.AthenaCommonFlags
