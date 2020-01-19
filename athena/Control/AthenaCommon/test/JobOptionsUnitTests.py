#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Author: Wim Lavrijsen (LBNL, WLavrijsen@lbl.gov)

"""Unit tests for verifying setting of Gaudi/Athena job properties setting.
This assumes that (modern versions of) AthExJobOptions and AthExHelloWorld
are available to play with."""

import unittest, sys

# NOTE: need to instantiate theApp to have the JobOptionsSvc created
try:
   import GaudiPython.Bindings as GaudiPython
except ImportError:
   import gaudimodule as GaudiPython
theApp = GaudiPython.AppMgr()

from AthenaCommon.AppMgr import theApp, ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.ServicesPythonize import *


### data ---------------------------------------------------------------------
__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'BasicConfigurableWorkingsTestCase',
            'BasicJobOptionsTestCase' ]


### helper class for removing properties =====================================
class JobOptionsTestBase( unittest.TestCase ):
   def tearDown( self ):
      JobOptionsSvc.clean()
      for client in JobOptionsSvc.getClients():
         assert( len(JobOptionsSvc.getProperties( client )) == 0 ),\
           '%s still has %d properties' %\
           (client,len(JobOptionsSvc.getProperties( client )))
      del client


### setting of basic properties (a la HelloWorld) ============================
class BasicConfigurableWorkingsTestCase( unittest.TestCase ):
   """Additional test cases for Configurables, with life ones"""

   def test1ParameterAccess( self ):
      """Test proper behavior of property access"""

      from AthExHelloWorld.AthExHelloWorldConf import HelloAlg

      HelloWorld = HelloAlg( 'test1ParameterAccess' )

    # a not-yet-set variable should raise
      self.assertRaises( AttributeError, getattr, HelloWorld, 'MyInt' )

    # a non-existing variable should raise
      self.assertRaises( AttributeError, setattr, HelloWorld, 'MyMy', 1 )

    # a not-yet-assigned to list should raise (can't be done today ... )
    # self.assertRaises( AttributeError, getattr, HelloWorld, 'MyStringVec' )

   def test2ParameterValidation( self ):
      """Test type checking of property setting"""

      from AthExHelloWorld.AthExHelloWorldConf import HelloAlg

      HelloWorld = HelloAlg( 'test2ParameterValidation' )

    # not-allowed conversions
      self.assertRaises( ValueError, setattr, HelloWorld, 'MyInt',  1. )
      self.assertRaises( ValueError, setattr, HelloWorld, 'MyInt', '1' )
      self.assertRaises( ValueError, setattr, HelloWorld, 'MyInt', [1] )

      self.assertRaises( ValueError, setattr, HelloWorld, 'MyDouble', '1' )
      self.assertRaises( ValueError, setattr, HelloWorld, 'MyDouble', '1.' )
      self.assertRaises( ValueError, setattr, HelloWorld, 'MyDouble', [1.] )

   def test3NonTransfers( self ):
      """Verify that temporary configurables leave nothing in the catalogue"""

      from AthExHelloWorld.AthExHelloWorldConf import HelloAlg

      name = 'test3NonTransfers'
      HelloWorld = HelloAlg( name )
      HelloWorld.MyInt = 666

    # adding, using, and subsequently removing to sequence should be a no-op
      seq = AlgSequence( name + 'Sequence' )
      seq += HelloWorld
      getattr(seq,name).MyDouble = 2.71828
      exec 'del seq.%s' % name
      seq.setup()

      self.assert_( name not in JobOptionsSvc.getClients() )

      del HelloWorld

    # oddity: give locals a kick to get the frame cleaned up
      locals()

    # after complete removal of the configurable, it should be history-less
      HelloWorld = HelloAlg( name )

      self.assertRaises( AttributeError, getattr, HelloWorld, 'MyInt' )
      self.assertRaises( AttributeError, getattr, HelloWorld, 'MyDouble' )

   def test4HistoryBehavior( self ):
      """Verify that name-uniqueness is preserved"""

      from AthExHelloWorld.AthExHelloWorldConf import HelloAlg

      name, value = 'test3NonTransfers', 888888
      HelloWorld = HelloAlg( name )
      HelloWorld.MyInt = value

      Hello2 = HelloAlg( name )

      self.assertEqual( HelloWorld.MyInt, Hello2.MyInt )

      del HelloWorld
      self.assertEqual( Hello2.MyInt, value )


### setting of basic properties (a la HelloWorld) ============================
class BasicJobOptionsTestCase( JobOptionsTestBase ):
   """Verify basic property setting"""

   def test1SetBuiltinTypes( self ):
      """Test setting of builtin types"""

      from AthExHelloWorld.AthExHelloWorldConf import HelloAlg

      HelloWorld = HelloAlg( 'HelloWorld' )

      HelloWorld.MyInt = 42
      HelloWorld.MyBool = True
      HelloWorld.MyDouble = 3.14159
      HelloWorld.MyStringVec = [ "Welcome", "to", "Athena", "Framework", "Tutorial" ]
      HelloWorld.MyStringVec += [ "!" ]
      HelloWorld.MyDict = { 'Bonjour'      : 'Guten Tag',
                            'Good Morning' : 'Bonjour' , 'one' : 'uno' }
      HelloWorld.MyDict[ "Goeiedag" ] = "Ni Hao"
      HelloWorld.MyTable = [ ( 1 , 1 ) , ( 2 , 4 ) , ( 3 , 9 ) ]
      HelloWorld.MyTable += [ ( 4, 16 ) ]
      HelloWorld.MyMatrix = [ [ 1, 2, 3 ],
                              [ 4, 5, 6 ] ]
      HelloWorld.MyMatrix += [ [ 7, 8, 9 ] ]

      HelloWorld.setup()

      self.assertEqual( HelloWorld.MyInt, 42 )
      self.assert_( JobOptionsSvc.verify( 'HelloWorld', 'MyInt', '42' ) )

      self.assertEqual( HelloWorld.MyBool, True )
      self.assert_( JobOptionsSvc.verify( 'HelloWorld', 'MyBool', 'True' ) )

      self.assertEqual( round( HelloWorld.MyDouble - 3.14159, 8 ), 0. )
      self.assert_( JobOptionsSvc.verify( 'HelloWorld', 'MyDouble', '3.14159' ) )

    # the following may be too sensitive to non-consequential changes in formatting
      self.assertEqual( HelloWorld.MyStringVec,
         [ "Welcome", "to", "Athena", "Framework", "Tutorial", "!" ] )
      self.assert_( JobOptionsSvc.verify( 'HelloWorld', 'MyStringVec',
         "['Welcome', 'to', 'Athena', 'Framework', 'Tutorial', '!']" ) )

      self.assertEqual( HelloWorld.MyDict,
         {'Bonjour': 'Guten Tag', 'one': 'uno', 'Goeiedag': 'Ni Hao', 'Good Morning': 'Bonjour'} )
      self.assert_( JobOptionsSvc.verify( 'HelloWorld', 'MyDict',
         "{'Bonjour': 'Guten Tag', 'one': 'uno', 'Goeiedag': 'Ni Hao', 'Good Morning': 'Bonjour'}" ) )
      self.assertEqual( HelloWorld.MyTable, [(1, 1), (2, 4), (3, 9), (4, 16)] )
      self.assert_( JobOptionsSvc.verify( 'HelloWorld', 'MyTable',
         "[(1, 1), (2, 4), (3, 9), (4, 16)]" ) )

      self.assertEqual( HelloWorld.MyMatrix, [[1, 2, 3], [4, 5, 6], [7, 8, 9]] )
      self.assert_( JobOptionsSvc.verify( 'HelloWorld', 'MyMatrix',
         "[[1, 2, 3], [4, 5, 6], [7, 8, 9]]" ) )

   def test2SetToolProperties( self ):
      """Test setting of tool properties"""

      from AthExHelloWorld.AthExHelloWorldConf import HelloAlg
      from AthExHelloWorld.AthExHelloWorldConf import HelloTool

      HelloWorld = HelloAlg( 'HelloWorld' )

      msg1 = "A Private Message!"

      HelloWorld.MyPrivateHelloTool = HelloTool( "HelloTool" )
      HelloWorld.MyPrivateHelloTool.MyMessage = msg1

      HelloWorld.setup()

      global ToolSvc

      msg2 = "A Public Message!"

      ToolSvc += HelloTool( "PublicHello" )
      ToolSvc.PublicHello.MyMessage = msg2

      ToolSvc.setup()

      self.assertEqual( HelloWorld.MyPrivateHelloTool.MyMessage, msg1 )
      client = HelloWorld.getName() + '.' + HelloWorld.MyPrivateHelloTool.getName()
      self.assert_( JobOptionsSvc.verify( client, 'MyMessage', msg1 ) )

      self.assertEqual( ToolSvc.PublicHello.MyMessage, msg2 )
      self.assert_( JobOptionsSvc.verify( 'ToolSvc.PublicHello', 'MyMessage', msg2 ) )

   def test3SetAuditorProperties( self ):
      """Test setting of Auditor properties"""

      from GaudiAud.GaudiAudConf import NameAuditor
      from AthenaCommon.AppMgr import ServiceMgr, theAuditorSvc
      from AthenaCommon.Constants import FATAL

      import AthenaCommon.AtlasUnixStandardJob

      theAuditorSvc += NameAuditor()
      self.assert_( theAuditorSvc.NameAuditor.getFullName()\
                    in ServiceMgr.AuditorSvc.Auditors )

      theAuditorSvc.NameAuditor.OutputLevel = FATAL

      theAuditorSvc.setup()

      self.assert_( JobOptionsSvc.verify( 'AuditorSvc', 'Auditors', str(theAuditorSvc.Auditors) ) )
      self.assert_( JobOptionsSvc.verify( 'NameAuditor', 'OutputLevel', str(FATAL) ) )


## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   sys.exit( result )
