# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/share/AppMgr.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Application manager and other global Gaudi components."""

import sys, os
import ExitCodes

import AlgSequence, Configurable, Logging
import GaudiSvc.GaudiSvcConf as GaudiSvcConf


### data ---------------------------------------------------------------------
__version__ = '3.2.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'theApp', 'ServiceMgr', 'ToolSvc', 'AuditorSvc', 'theAuditorSvc',
            'athMasterSeq',
            'athFilterSeq',
            'athAlgSeq',    'topSequence',
            'athOutSeq',
            'athRegSeq',
            ]

### helpers ------------------------------------------------------------------
def _type_and_name(n):
   """`n` is the usual gaudi string identifier ('KlassName/Name' or 'KlassName')
   """
   s = n.split('/')
   t = s[0]
   if len(s)==2: n=s[1]
   return t,n

def release_metadata():
   """`release_metadata` returns informations about the current release being used
   """
   import ConfigParser
   import os
   d = {
      'project name': '?',
      'release': '?',
      'base release': '?',
      'nightly release': '?',
      'nightly name': '?',
      'date': '?',
      'platform': os.getenv('CMTCONFIG', '?'),
      }

   for ld_path in os.environ['LD_LIBRARY_PATH'].split(os.pathsep):
      release_data = os.path.join(ld_path, '..', 'ReleaseData')
      if os.path.exists(release_data):
         d1=d
         cfg = ConfigParser.SafeConfigParser()
         try:
            cfg.read( release_data )
            if cfg.has_section( 'release_metadata' ):
               d1.update( dict( cfg.items( 'release_metadata' ) ) )
               release = d1['release'].split('.')
               base_release = d1['base release'].split('.')
               if len(release)>=3 or len(base_release)>=3:
                  return d1
         except Exception:
            pass
   return d

### associator for public tools ----------------------------------------------
def iadd( self, tool ):
 # only add once (allow silently)
   if tool in self.getChildren():
      return self

 # this is only allowed for new-style AlgTools
   if not not isinstance( tool, Configurable.ConfigurableService ):
      raise TypeError( '"%s" is not an AlgTool' %\
         (hasattr(tool,'name') and tool.name() or str(value) ) )

   super( GaudiSvcConf.ToolSvc, self ).__iadd__( tool )

   return self

GaudiSvcConf.ToolSvc.__iadd__ = iadd
del iadd

### associator for services --------------------------------------------------
class AthServiceManager( Configurable.Configurable ):
   def getHandle( self ):
      return None

   def getGaudiType ( self ):
      return 'ServiceManager' 

   def getType( self ):
      return ''

   def getDlls ( self ):
      return None

   def setup( self ):
      children = self.getChildren()
      for svc in children:
         svc.setup()
      
   def __iadd__( self, service ):
    # only add once (allow silently)
      if service in self.getChildren():
         return self

    # this is only allowed for new-style Services
      if not isinstance( service, Configurable.ConfigurableService ):
         msg = 'attempt to add an %s (%s) to the ServiceManager' %\
               (type(service).__name__, service.name())
         Logging.log.error( msg )
         raise TypeError( msg )

      super( AthServiceManager, self ).__iadd__( service )

    # make all names explicitly known
      global theApp
      theApp.ExtSvc += [ service.getFullName() ]

      return self

### integrator for TopAlg and stream sequences -------------------------------
class OldToNewSequenceProxy( object ):
   def __init__( self, seq ):
      self.sequence = seq

   def __get__( self, obj, type = None ):
      return map( lambda x: x.getFullName(), self.sequence.getChildren() )

   def __set__( self, obj, value ):
      from OldStyleConfig import Algorithm

    # explicit removal/addition is required to maintain copy identity
      current = self.sequence.getChildren()
      names = dict( zip( map( lambda x: x.getFullName(), current ), current ) )

      self.sequence.removeAll()
      current = self.sequence._Configurable__children

      for name in value:
         try:                # can't use get() b/c of Configurable caches
            current.append( names[ name ] )      # hack to prevent dupe parents
         except KeyError:
            self.sequence += Algorithm( name )
   pass # class OldToNewSequenceProxy

### retro application manager ------------------------------------------------
from GaudiSvc.GaudiSvcConf import ApplicationMgr as AppMgr
class AthAppMgr( AppMgr ):
   class State:
      """Python equivalent of IService::State enum (kind of silly to load a whole
      dictionary library for just this quartet of integers
      """

    # for reference, the numbers below can also be obtained through a dictionary:
    #    import cppyy 
    #    melm = cppyy.makeClass( 'MinimalEventLoopMgr' )
    #    print melm.OFFLINE, melm.CONFIGURED, melm.FINALIZED, melm.INITIALIZED

      OFFLINE     = 0
      CONFIGURED  = 1
      INITIALIZED = 2
      RUNNING     = 3
      
   def __init__( self, name = "ApplicationMgr", **kw ):
      kw['name'] = name
      if not kw.has_key('outputLevel'): kw['outputLevel'] = 3
      if not kw.has_key('jobOptions') : kw['jobOptions']  = None

    # some Atlas defaults
      if not kw.has_key('JobOptionsPath'): kw['JobOptionsPath'] = ""
      if not kw.has_key('JobOptionsType'): kw['JobOptionsType'] = "NONE"
      if not kw.has_key('EventLoop'):      kw['EventLoop']="AthenaEventLoopMgr"
      if not kw.has_key('OutStreamType'):
         kw['OutStreamType'] = "AthenaOutputStream"
      if not kw.has_key('StatusCodeCheck'): kw['StatusCodeCheck'] = True

    # always the case in ATLAS (need early or ExtSvc should be a no-op, too)
      kw['ExtSvcCreates'] = False

      super(AthAppMgr,self).__init__( **kw )
      self.__dict__[ '_cppApp' ]    = None        # proxy to C++/App.
      self.__dict__[ '_sequences' ] = []
      self.__dict__[ '_streams' ]   = AlgSequence.AlgSequence( "Streams" )
      self.__dict__[ 'CreateSvc' ]  = []          # block the property
      self.__dict__[ '_exitstate' ] = ExitCodes.ALL_OK

      self.__dict__['state'] = lambda : AthAppMgr.State.OFFLINE
      self.__dict__['Dlls'] = []
      
    # install sequence proxies
      self.__class__.TopAlg    = OldToNewSequenceProxy( AlgSequence.AlgSequence( "TopAlg" ) )
      self.__class__.OutStream = OldToNewSequenceProxy( self.__dict__[ '_streams' ] )

    # install services
      svcMgr = self.serviceMgr()

    # external option (TODO: receive this cleanly; AthOptionsParser doesn't manage results, and
    # can't be called directly due to transforms etc.)
      self.__dict__[ '_opts' ] = None

    # this code is to be factored out; for now flag it with 'minimal' to test
    # the effects of its removal

    # figure out which release are we running, for logging purposes
      d = release_metadata()
      msg = Logging.log.info
      msg( 'using release [%(project name)s-%(release)s] [%(platform)s] [%(nightly name)s/%(nightly release)s] -- built on [%(date)s]' % d )
    # -- end of (proposed) minimal

      return

   def __setattr__( self, name, value ):
    # required b/c the lookup is otherwise intercepted by iProperty
      if name[0] == '_':           # private properties
         return object.__setattr__( self, name, value )
    # in case the C++ app has already been instantiated...
      if hasattr(self, '_cppApp') and self._cppApp and \
         name in AthAppMgr.__slots__:
         handle = self.getHandle()
         return handle.__setattr__(name, value)
      elif name == "TopAlg":       # special case
         return self.__class__.__dict__[ 'TopAlg' ].__set__( self, value )
      elif name == "Dlls":         # block Dlls calls
         return
      elif name == "OutStream":    # idem TopAlg
         return self.__class__.__dict__[ 'OutStream' ].__set__( self, value )
      elif name == "CreateSvc":    # for delaying creation of services
         self.__dict__[ name ] = value
      else:
         return super( AppMgr, self ).__setattr__( name, value )

   def __getattribute__( self, name ):
      if name[0] == '_':
         return object.__getattribute__( self, name )
      if hasattr(self, '_cppApp') and self._cppApp and \
         name in AthAppMgr.__slots__:
         return getattr(self._cppApp, name)
      else:
         if name == "Dlls": return []      # block Dlls calls
         return super( AthAppMgr, self ).__getattribute__(name)

   def __build_master_sequence (self):
      """helper method to build the top-level AthSequencer from all bits and
         pieces : AthMasterSeq, AthFilterSeq, AthAlgSeq, AthOutSeq, AthRegSeq
      """
      from . import AlgSequence as _as
      def _build():
         Logging.log.debug ("building master sequence...")
         athMasterSeq = _as.AthSequencer ("AthMasterSeq")
         athFilterSeq = _as.AthSequencer ("AthFilterSeq"); 
         athAlgSeq    = _as.AthSequencer ("AthAlgSeq")
         athOutSeq    = _as.AthSequencer ("AthOutSeq")
         athRegSeq    = _as.AthSequencer ("AthRegSeq")

         # transfer old TopAlg to new AthAlgSeq
         _top_alg = _as.AlgSequence("TopAlg")
         # first transfer properties
         for n,prop in _top_alg.properties().iteritems():
            if hasattr(_top_alg, n) and n != "Members":
               setattr(athAlgSeq, n, prop)
               
         # then sub-configurables
         children = [c for c in _top_alg.getChildren()]
         for c in children:
            athAlgSeq += c
            delattr(_top_alg, c.getName())
         del _top_alg, children
         
         
         # unroll AthFilterSeq to save some function calls and
         # stack size on the C++ side
         for c in athFilterSeq.getChildren():
            athMasterSeq += c

         # XXX: should we discard empty sequences ?
         #      might save some CPU and memory...
         athMasterSeq += athAlgSeq
         athMasterSeq += athOutSeq
         athMasterSeq += athRegSeq
         Logging.log.debug ("building master sequence... [done]")
         return athMasterSeq
      # prevent hysteresis effect
      if not hasattr (self, '__master_seq_built'):
         self.__master_seq_built = True
         return _build()
      return _as.AthSequencer ("AthMasterSeq")
   
   def algorithm( self, name ):
      if self._cppApp:
         return self.getHandle().algorithm( name )
      # might be lucky...
      return super( AppMgr, self ).algorithm( name )
   
   def service( self, name ):
      if self._cppApp: return self._cppApp.service(name)
      svcMgr = self.serviceMgr()
      if not hasattr( svcMgr, name ):
         import CfgMgr
         svcMgr += getattr( CfgMgr, name )()
      return getattr( svcMgr, name )
   
   def setOutputLevel( self, outputLevel ):
      if outputLevel != -1 :
         if self.state() == AthAppMgr.State.OFFLINE :    # not yet configured
            self.OutputLevel = outputLevel
         svcMgr = self.serviceMgr()
         if not hasattr( svcMgr, 'MessageSvc' ):
            svcMgr += GaudiSvcConf.MessageSvc()
         svcMgr.MessageSvc.OutputLevel = outputLevel

 # explicit user calls
   def addSequence( self, seq ):
      if not seq in self._sequences:
         self._sequences.append( seq )

   def removeSequence( self, seq ):
      self._sequences.remove( seq )

   def addOutputStream( self, stream ):
      if not stream in self._streams.getChildren():
         self._streams += stream

   def removeOutputStream( self, stream ):
      self._streams.remove( stream )
         
 # override toolSvc to handle the transitional one
   def toolSvc( self, name='ToolSvc' ):
      if not '_toolsvc' in self.__dict__:
         self.__dict__[ '_toolsvc' ] = GaudiSvcConf.ToolSvc( name )
      return self._toolsvc
   toolsvc = toolSvc

 # same for serviceMgr
   def serviceMgr( self ):
      if not '_servicemgr' in self.__dict__:
         self.__dict__[ '_servicemgr' ] = AthServiceManager( 'ServiceManager' )
      return self._servicemgr
   servicemgr = serviceMgr

   def bootProps(self):
      props = {}
      for k in self.getProperties().keys():
         if not k in [ "Go", "Exit", "AuditInitialize", "AuditFinalize" ]:
            props[k] = self.getDefaultProperty(k)
            if hasattr(self, k):
               props[k] = getattr(self, k)
      props['Dlls'] = [ 'AthenaServices' ]
      props['CreateSvc'] = []
      return props

   def getHandle( self, selfOptions = {} ):
      if not self._cppApp:
       # temporarily store this list as it will be touched by the C++ app
       # through the __getattribute__ method
         _createSvc = self.__dict__['CreateSvc']
         if not selfOptions:
            selfOptions = self.bootProps()
            for k,v in selfOptions.items(): setattr(self, k, v)
         svcMgr = self.serviceMgr()
       # the following code is stopped when minimal to prevent the loading of
       # ConfigurableDb when no configuration is done; FIXME: minimal is set
       # to go when all code has been cleaned up
         if self._opts and not self._opts.minimal:
            from ConfigurableDb import getConfigurable
            if not hasattr(svcMgr, 'JobOptionsSvc'):
               svcMgr += getConfigurable(self.JobOptionsSvcType)("JobOptionsSvc")
            if not hasattr(svcMgr, 'MessageSvc'):
               svcMgr += getConfigurable(self.MessageSvcType)("MessageSvc")
       # -- end minimal
         try:
            from GaudiPython import AppMgr as GaudiAppMgr
         except ImportError:
            from gaudimodule import AppMgr as GaudiAppMgr
         self._cppApp = GaudiAppMgr( outputlevel = self.outputLevel,
                                     joboptions  = None,
                                     selfoptions = selfOptions )
         self.__dict__['state'] = getattr(self._cppApp, 'state')
         for k,v in selfOptions.items():
            setattr(self._cppApp,k,v)
         self.__dict__['CreateSvc'] = _createSvc; del _createSvc

      import GaudiPython # this module might have disappeared b/c of cleansing
      return self._cppApp
   
   @property
   def _evtSeek(self):
      """ retrieve a handle to the IEventSeek interface of the event loop mgr
      """
      import AthenaPython.PyAthena as PyAthena
      return PyAthena.py_svc(self.EventLoop, iface='IEventSeek')

   @property
   def _evtSize(self):
      """ retrieve a handle to the ICollectionSize interface of the event loop mgr
      """
      import AthenaPython.PyAthena as PyAthena
      return PyAthena.py_svc(self.EventLoop, iface='ICollectionSize')

 # Configurable call
   def setup( self, recursive = False ):
      if not recursive and (self._opts and (self._opts.drop_reload or self._opts.config_only)):
       # store configuration on disk
         import os, sys
         import ConfigurationShelve

         if self._opts.config_only:
            fn = self._opts.config_only
         else:
            fn = 'TempAthenaConfig.' + str(os.getpid()) + '.pkl'
         fn = ConfigurationShelve.storeJobOptionsCatalogue( fn )
         del ConfigurationShelve

         if self._opts.profile_python:
            import cProfile, pstats
            cProfile._athena_python_profiler.disable()
            pstats.Stats(cProfile._athena_python_profiler,
                         stream=open(self._opts.profile_python, 'w')).sort_stats("time").print_stats()

         if self._opts.drop_reload:
          # build the new set of options; replace the .py by the .pkl,
          # and remove '-c arg'
            del sys.argv[ sys.argv.index( '--drop-and-reload' ) ]
            for arg in sys.argv[1:]:
               if arg[-3:] == '.py':
                  del sys.argv[ sys.argv.index( arg ) ]

               elif arg == '-c':
                  idx = sys.argv.index( arg )
                  del sys.argv[ idx : idx + 2 ]

          # dump profiling (if any) in temporary file
            if self._opts.profile_python:
               import cProfile
               cProfile._athena_python_profiler.disable()
               cProfile._athena_python_profiler.dump_stats(
                  self._opts.profile_python + ".athena.tmp" )

          # fire ourselves up anew
            Logging.log.info( 'restarting athena.py from %s ... ', fn )
            sys.argv.insert( 1, fn )
            sys.argv.append( '--minimal' )
            os.execvp( sys.argv[0], sys.argv )

         else:
          # running config-only, so we're done
            self.__report_python_profile()
            Logging.log.info( "configuration complete, now exiting ... " )
            os._exit( self._exitstate )

    # normal setup() (non drop-and-reload) continues here:

    # first we bring the real C++ application to life...
      handle = self.getHandle()

    # temporarily disable GaudiAppMgr.Dlls and .CreateSvc calls
      handle.__dict__['CreateSvc'] = self.__dict__.get('CreateSvc',
                                                       handle.CreateSvc)
      
      try:
         from GaudiPython import AppMgr as GaudiAppMgr
      except ImportError:
         from gaudimodule import AppMgr as GaudiAppMgr

    # Likely the first (or at least the first important) place if we're
    # running in compatibility mode where gaudimodule will be loaded. And
    # even if not, still ok. Remove the gaudimodule exit handlers as to
    # prevent them from clobbering Athena ones.
      import atexit
      for handler in atexit._exithandlers[:]:
         if hasattr(handler[0], '__module__') and handler[0].__module__:
            if 'audi' in handler[0].__module__:  # removes gaudimodule and GaudiPython handlers
               #print "removed ", handler[0].__module__
               atexit._exithandlers.remove( handler )
      del handler, atexit

      def _setattr( self, n, v ):
         if n == 'CreateSvc':
            self.__dict__[n] = v
            return
         if n == 'Dlls':
            return
         return super( GaudiAppMgr, self ).__setattr__( n, v )
      GaudiAppMgr_old_setattr = GaudiAppMgr.__setattr__
      GaudiAppMgr.__setattr__ = _setattr
      del _setattr
      
      def _getattr( self, n ):
         if n == 'CreateSvc':
            return self.__dict__[n]
         return super( GaudiAppMgr, self ).__getattr__( n )
      GaudiAppMgr_old_getattr = GaudiAppMgr.__getattr__
      GaudiAppMgr.__getattr__ = _getattr
      del _getattr
      
    # public tools (they're less likely to be instantiated early, and
    # thus rely on job options load order)
      self.toolSvc().setup()

    # new-style services (also sets up AuditorSvc)
      self.serviceMgr().setup()

    # setup top-level job seq
      master_seq = self.__build_master_sequence()
      master_seq.setup()

    # tell self about the algs
      handle.TopAlg = [ master_seq.getFullName() ]
                           
    # setup output streams, and tell self about them
      streams = self.__dict__[ '_streams' ]
      streams.setup()
      handle.OutStream = map( lambda x: x.getFullName(), streams.getChildren() )

    # synchronize 'Dlls'
    # note: we synchronize in the opposite direction wrt CreateSvc
      super(AppMgr,self).__setattr__('Dlls', handle.Dlls)

    # synchronize 'CreateSvc'  
      svcMgr = self.serviceMgr()
      _createSvc = handle.__dict__['CreateSvc']
      if len(_createSvc) > 0 and \
         _createSvc[0] != svcMgr.ToolSvc.getFullName():
         _createSvc = [ svcMgr.ToolSvc.getFullName() ] + _createSvc

      if self.__dict__.has_key('CreateSvc'):
         del self.__dict__['CreateSvc']
      handle.__dict__['CreateSvc'] = [ s for s in _createSvc ]
      
    # reinstall old __?etattr__
      del handle.__dict__['CreateSvc']
      GaudiAppMgr.__getattr__ = GaudiAppMgr_old_getattr
      GaudiAppMgr.__setattr__ = GaudiAppMgr_old_setattr
      
    # only now allow CreateSvc, so that there are no ordering problems
      Logging.log.debug( 'Updating (C++) "CreateSvc" property...' )
      handle.CreateSvc = _createSvc
      Logging.log.debug( 'Updating (C++) "CreateSvc" property... [ok]' )
      
    # finally, print a warning for the generic Algorithms that were left over
      names  = map( lambda x: x[x.find( '/' )+1:], handle.TopAlg )
      names += map( lambda x: x[x.find( '/' )+1:], handle.OutStream )
      from OldStyleConfig import Algorithm
      for grc in Algorithm.configurables.values():
         if not (grc._flags & Configurable.Configurable._fSetupOk):      # debugging variable
          # first, back-hack ... this will go rather wrong, so issue an error no matter what
            if grc.getName() in names:
               detail = 'type missing'
            else:
               detail = 'not in TopAlg or other known list'
            Logging.log.error( 'Algorithm "%s": %s, no properties set', grc.getFullName(), detail )

    # if requested, print a (summary of) the list of potential property clashes
      if self._opts and self._opts.check_properties:
         import PropertyHistoryCheck
         PropertyHistoryCheck.check( self._opts.check_properties )
         del PropertyHistoryCheck

    # if requested, dump the current state of the configuration to an ASCII file
      if self._opts and self._opts.config_dump_file:
         import AthenaCommon.ConfigurationShelve as cs
         cs.saveToAscii(self._opts.config_dump_file)

    # need to miminize this to configurables only
      import ConfigurationCleanup
      ConfigurationCleanup.Cleanse()
      del ConfigurationCleanup

    # for good measure, after removal of ConfigurationCleanup itself
      import gc
      gc.collect()
      del gc

      return
   
 # redefines to take into acount setup of Configurables
   def initialize( self ):
      # Touch these types early, before dictionaries are loaded,
      # to prevent spurious error messages from ROOT.
      # See ATLASRECTS-3486.
      import cppyy
      getattr(cppyy.gbl, 'vector<bool>')
      getattr(cppyy.gbl, 'vector<float>')
      getattr(cppyy.gbl, 'vector<unsigned short>')
      getattr(cppyy.gbl, 'vector<short>')
      getattr(cppyy.gbl, 'vector<unsigned long>')
      getattr(cppyy.gbl, 'vector<ULong64_t>')
      getattr(cppyy.gbl, 'map<string,string>')

    # build configuration
      self.setup()

    # create C++-side AppMgr
      try:
         sc = self.getHandle().initialize()
         if sc.isFailure():
            self._exitstate = ExitCodes.INI_ALG_FAILURE
      except Exception:
         self._exitstate = ExitCodes.INI_ALG_EXCEPTION
         raise
      return sc

   def reinitialize( self ):
    # since we're going to run python again, may have to re-enable to profiler
      if self._opts and self._opts.profile_python:
         import cProfile
         cProfile._athena_python_profiler.enable()

    # first, rebuild configuration
      self.setup()

    # ApplicationMgr::reinitialize does not reinit algorithms ...
      for name in self.getHandle().TopAlg:
         try:
            parts = name.split('/')
            algname = len(parts) == 2 and parts[1] or parts[0]
            sc = self.algorithm( algname )._ialg.reinitialize()
            if sc.isFailure() and not self._exitstate:
               return sc
         except AttributeError:
            pass

    # normal reinitialize kicks services, tools left out ...
      return self.getHandle().reinitialize()

 # redefine run to improve interface
   def run( self, nEvt = None ):
    # initialize as appropriate
      if self.state() in ( AthAppMgr.State.OFFLINE,
                           AthAppMgr.State.CONFIGURED, ):
         Logging.log.debug( 'Initializing application manager' )
         sc = self.initialize()
      else:
         Logging.log.debug( 'Re-initializing application manager' )
         sc = self.reinitialize()

      if sc.isFailure():
         return sc

      sc = self.start()
      if sc.isFailure():
         return sc

    # determine number of events
      if nEvt == None:
         nEvt = self.EvtMax            # late, as sequences may have changed it

    # another communication that needs improving (TODO) ...
      try:
         from Debugging import DbgStage
         if DbgStage.value == "exec":
            from Debugging import hookDebugger
            hookDebugger()
      except ImportError:
         pass

    # actual run (FIXME: capture beginRun() exceptions and failures, which is
    #               not currently supported by IEventProcessor interface)
      try:
         sc = self.getHandle()._evtpro.executeRun( nEvt )
         if sc.isFailure() and not self._exitstate:
            self._exitstate = ExitCodes.EXE_ALG_FAILURE   # likely, no guarantee
      except Exception:
         self._exitstate = ExitCodes.EXE_ALG_EXCEPTION    # idem
         raise

      sc = self.stop()
      if sc.isFailure():
         return sc
      
      return sc

   def start( self ):
      import GaudiPython
      sc = GaudiPython.SUCCESS
      import cppyy
      #backward compatibility v19 (no StateMachine)
      try:
         canStart = cppyy.gbl.Gaudi.StateMachine.INITIALIZED == self.getHandle().FSMState()
      except AttributeError:
         canStart=False # no start available

      if canStart:
         Logging.log.debug( 'Starting application manager' )
         sc = self.getHandle().start()
      return sc

   def stop( self ):
      import GaudiPython
      sc = GaudiPython.SUCCESS
      import cppyy
      #backward compatibility v19 (no StateMachine)
      try:
         canStop = cppyy.gbl.Gaudi.StateMachine.RUNNING == self.getHandle().FSMState()
      except AttributeError:
         canStop=False # no stop available

      if canStop:
         Logging.log.debug( 'Stopping application manager' )
         sc = self.getHandle().stop()
      return sc

 # forward call to underlying C++/app
   def finalize( self ): 
      #stop theApp if not already done
      sc = theApp.stop()
      if sc.isFailure() and not self._exitstate:
         self._exitstate = ExitCodes.FIN_ALG_FAILURE
      try:
         if not self._cppApp:
            raise RuntimeError, \
                  "C++ application not instantiated : Nothing to finalize !"
         sc = self.getHandle()._appmgr.finalize()
         if sc.isFailure():
            self._exitstate = ExitCodes.FIN_ALG_FAILURE
      except Exception:
         self._exitstate = ExitCodes.FIN_ALG_EXCEPTION
         raise
      return sc

 # convenience functions to move around events
   def curEvent( self ):
      try:
         return self._evtSeek.curEvent()
      except AttributeError:
         raise TypeError( '%s does not support IEventSeek' % theApp.EventLoop )

   def seekEvent( self, n ):
      try:
         sc = self._evtSeek.seek( n )
         # prevents any unchecked statuscode leftover
         if not sc.isSuccess():
            return sc
         sc = self.nextEvent()
         # prevents any unchecked statuscode leftover
         if not sc.isSuccess():
            return sc
         return sc
      except AttributeError:
         raise TypeError( '%s does not support IEventSeek' % theApp.EventLoop )

   def seek( self, n ):
      try:
         sc = self._evtSeek.seek( n )
         # prevents any unchecked statuscode leftover
         if not sc.isSuccess():
            return sc
         return sc
      except AttributeError:
         raise TypeError( '%s does not support IEventSeek' % theApp.EventLoop )

   def nextEvent( self, nEvt = None ):
      #start theApp if not already done
      sc = theApp.start()
      if sc.isFailure() and not self._exitstate:
         self._exitstate = ExitCodes.INI_ALG_FAILURE

      if nEvt == None:
         nEvt = self.curEvent() + 1

      try:
         sc = self.getHandle()._evtpro.nextEvent( nEvt )
         if sc.isFailure() and not self._exitstate:
            self._exitstate = ExitCodes.EXE_ALG_FAILURE
      except Exception:
         self._exitstate = ExitCodes.EXE_ALG_EXCEPTION
         raise

      return sc

   def size( self ):
      try:
         return self._evtSize.size()
      except AttributeError:
         raise TypeError( '%s does not support ICollectionSize' % theApp.EventLoop )
     
   def replacePFC(self):
     """ if we have MP version of EventLoopMgr 
      replace PoolFileCatalog.xml by MP version"""
     if (self.EventLoop == 'AthMpEventLoopMgr/EventLoopMgr'):
         if os.path.exists("MP_PoolFileCatalog.xml"):
             Logging.log.info ("replacing PoolFileCataloG.xml by MP version")
             import shutil
             shutil.copy2("MP_PoolFileCatalog.xml", "PoolFileCatalog.xml")

   def __report_python_profile( self ):
      if self._opts and self._opts.profile_python:
         import cProfile, pstats
         cProfile._athena_python_profiler.disable()
         stats = pstats.Stats(cProfile._athena_python_profiler,
                              stream=open(self._opts.profile_python, 'w'))
       # NOTE: tmpname has to match same in setup()
         tmpname = self._opts.profile_python + ".athena.tmp"
         try:
            added_stats = stats.add( tmpname )
         except (OSError, IOError):
            added_stats = None
         pos = self._opts.profile_python.rfind('.')
         if self._opts.profile_python[pos+1:] == "pkl":
            stats.dump_stats( self._opts.profile_python )
         else:
            stats.strip_dirs().sort_stats("time").print_stats()
         if added_stats:
            os.remove( tmpname )

 # exit includes leaving python
   def exit( self, code = None ):
      try:
         if self.state() > AthAppMgr.State.CONFIGURED:
            sc1 = self.finalize()
            self.replacePFC() #only for AthenaMP -> replace pfc by mp version
            sc2 = self.getHandle()._appmgr.terminate()
            if ( sc1.isFailure() or sc2.isFailure() ) and not self._exitstate:
               self._exitstate = ExitCodes.FIN_ALG_FAILURE
      except Exception:
         if not self._exitstate:
            self._exitstate = ExitCodes.FIN_ALG_EXCEPTION
         import traceback
         traceback.print_exc()     # no re-raise to allow sys.exit next

      self.__report_python_profile()

      Logging.log.info( 'leaving with code %d: "%s"',
         self._exitstate, ExitCodes.what( self._exitstate ) )
      sys.exit( code == None and self._exitstate or code )


### global objects for export ------------------------------------------------
theApp = AthAppMgr(
   outputLevel = Logging.AthenaLogger.mapLevelLoggingToGaudi( Logging.log.getEffectiveLevel() )
   )

ServiceMgr  = theApp.serviceMgr()
ServiceMgr += theApp.toolSvc()
ToolSvc     = ServiceMgr.ToolSvc

# old-style ...
def auditor( self, auditor ):
   Logging.log.warning( """AuditorSvc.auditor is deprecated, use instead:
   from GaudiAud import %s
   svcMgr.AuditorSvc += %s()""" % (auditor,auditor) )

   if type(auditor) == str:
      import ConfigurableDb
      auditor = ConfigurableDb.getConfigurable( auditor )()
   self.__iadd__( auditor )
   return auditor

GaudiSvcConf.AuditorSvc.auditor = auditor
del auditor

# convenience customization to deal with "Auditors" property
def iadd( self, config ):
   super( GaudiSvcConf.AuditorSvc, self ).__iadd__( config )

   if isinstance( config, Configurable.ConfigurableAuditor ):
      if not config.getName()     in self.Auditors and \
         not config.getFullName() in self.Auditors:
         self.Auditors += [ config.getFullName() ]

   return self

GaudiSvcConf.AuditorSvc.__iadd__ =iadd
del iadd


def delattr( self, attr ):
   try:
      c = getattr( self, attr )

      try:
         self.Auditors.remove( c.getFullName() )
      except ValueError:
         pass

      try:
         self.Auditors.remove( c.getName() )
      except ValueError:
         pass

   except AttributeError:
      pass

   super( GaudiSvcConf.AuditorSvc, self ).__delattr__( attr )

GaudiSvcConf.AuditorSvc.__delattr__ = delattr
del delattr


# AuditorSvc globals
ServiceMgr += GaudiSvcConf.AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc

def AuditorSvc():             # backwards compatibility
   global theAuditorSvc
   return theAuditorSvc

### create default sequences:
#      athMasterSeq
#         |
#         +-- athFilterSeq
#                |
#                +--- athAlgSeq == TopAlg
#                |
#                +--- athOutSeq
#                |
#                +--- athRegStreams
athMasterSeq = AlgSequence.AthSequencer( "AthMasterSeq" )
athFilterSeq = AlgSequence.AthSequencer( "AthFilterSeq" )
athAlgSeq    = AlgSequence.AthSequencer( "AthAlgSeq" )
athOutSeq    = AlgSequence.AthSequencer( "AthOutSeq" )
athRegSeq    = AlgSequence.AthSequencer( "AthRegSeq" )

topSequence  = AlgSequence.AlgSequence( "TopAlg" )     # for backward compatibility

# we set the override to True so algorithms
# setting the filterPassed to False won't stop the
# execution of the AthAlgSeq sequence
# this is mainly for backward compatibility reasons
athAlgSeq.StopOverride=True
