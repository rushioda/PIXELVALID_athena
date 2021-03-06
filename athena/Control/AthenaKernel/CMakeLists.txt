# $Id: CMakeLists.txt 787829 2016-12-02 10:10:58Z krasznaa $
################################################################################
# Package: AthenaKernel
################################################################################

# Declare the package name:
atlas_subdir( AthenaKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CxxUtils
   Control/DataModelRoot
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS program_options regex filesystem thread )
find_package( ROOT COMPONENTS Core )
find_package( UUID )

# Only link agains the RT library on Linux:
set( rt_library )
if( UNIX AND NOT APPLE )
   set( rt_library rt )
endif()

# Libraries in the package:
atlas_add_library( AthenaKernel
   AthenaKernel/*.h AthenaKernel/*.icc src/*.cxx
   PUBLIC_HEADERS AthenaKernel
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${UUID_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${UUID_LIBRARIES} CxxUtils DataModelRoot
   GaudiKernel ${rt_library}
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AthenaKernelDict
   AthenaKernel/AthenaKernelDict.h
   AthenaKernel/selection.xml
   LINK_LIBRARIES GaudiKernel AthenaKernel )

# Test(s) in the package:
atlas_add_test( getMessageSvc_test
   SOURCES test/getMessageSvc_test.cxx
   LINK_LIBRARIES GaudiKernel TestTools AthenaKernel
   EXTRA_PATTERNS "^=========+|^ApplicationMgr +SUCCESS|^HistogramPersis.*INFO.*CnvServices|^StatusCodeSvc +INFO initialize|^ *Welcome to ApplicationMgr|^ *running on|^Wall clock time" )

atlas_add_test( MsgStreamMember_test
   SOURCES test/MsgStreamMember_test.cxx
   LINK_LIBRARIES TestTools AthenaKernel
   EXTRA_PATTERNS "^=========+|^ApplicationMgr +SUCCESS|^HistogramPersis.*INFO.*CnvServices|^StatusCodeSvc +INFO initialize|^ *Welcome to ApplicationMgr|^ *running on|^Wall clock time |ref count" )

atlas_add_test( AthenaPackageInfo_test
   SOURCES test/AthenaPackageInfo_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( DirSearchPath_test
   SOURCES test/DirSearchPath_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel )

atlas_add_test( Chrono_test
   SOURCES test/Chrono_test.cxx
   LINK_LIBRARIES GaudiKernel AthenaKernel )

atlas_add_test( errorcheck_test
   SOURCES test/errorcheck_test.cxx
   LINK_LIBRARIES GaudiKernel TestTools AthenaKernel
   EXTRA_PATTERNS "^=========+|^ApplicationMgr +SUCCESS|^HistogramPersis.*INFO.*CnvServices|^StatusCodeSvc +INFO initialize|^ *Welcome to ApplicationMgr|^ *running on|^Wall clock time" )

atlas_add_test( type_tools_test
   SOURCES test/type_tools_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( Units_test
   SOURCES test/Units_test.cxx
   LINK_LIBRARIES GaudiKernel TestTools AthenaKernel )

atlas_add_test( DataObjectSharedPtr_test
   SOURCES test/DataObjectSharedPtr_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( IRCUSvc_test
   SOURCES test/IRCUSvc_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( RCUObject_test
   SOURCES test/RCUObject_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel )
