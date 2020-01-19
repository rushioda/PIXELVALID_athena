/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_web_xaod_bunch_tool_test.cxx 625753 2014-11-03 13:40:15Z krasznaa $

// System include(s):
#include <iostream>
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// xAOD include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// Local include(s):
#include "TrigBunchCrossingTool/WebBunchCrossingTool.h"

/// A little helper macro
#define SIMPLE_CHECK( EXP )                                       \
   do {                                                           \
      if( ! EXP.isSuccess() ) {                                   \
         std::cerr << "Failed to execute: " << #EXP << std::endl; \
            return 1;                                             \
      }                                                           \
   } while( 0 )

/// Another helper macro for evaluating logical tests
#define SIMPLE_ASSERT( EXP )                                            \
   do {                                                                 \
      const bool result = EXP;                                          \
      if( ! result ) {                                                  \
         std::cerr << "Expression \"" << #EXP << "\" failed the evaluation" \
                   << std::endl;                                        \
            return 1;                                                   \
      }                                                                 \
   } while( 0 )

int main() {

   // The name of the application:
   static const char* APP_NAME = "ut_web_xaod_bunch_tool_test";

   // Initialise the environment for xAOD reading:
   SIMPLE_CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   static const char* FNAME =
      "/afs/cern.ch/atlas/project/PAT/xAODs/r5597/"
      "data12_8TeV.00204158.physics_JetTauEtmiss.recon.AOD.r5597/"
      "AOD.01495682._003054.pool.root.1";
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, "Couldn't open file: %s", FNAME );
      return 1;
   }
   ::Info( APP_NAME, "Opened file: %s", FNAME );

   // Set up the reading of an example file:
   xAOD::TEvent event;
   SIMPLE_CHECK( event.readFrom( ifile.get() ) );

   // Create the tool to be tested:
   Trig::WebBunchCrossingTool tool;
   SIMPLE_CHECK( tool.setProperty( "OutputLevel", MSG::INFO ) );
   SIMPLE_CHECK( tool.setProperty( "ServerAddress",
                                   "atlas-trigconf.cern.ch" ) );

   // Loop over the events in the file:
   const ::Long64_t entries = event.getEntries();
   for( ::Long64_t entry = 0; entry < entries; ++entry ) {

      // Load the event:
      if( event.getEntry( entry ) < 0 ) {
         ::Error( APP_NAME, "Couldn't load event %i from the input file",
                  static_cast< int >( entry ) );
         return 1;
      }

      // Print some status info:
      if( ! ( entry % 100 ) ) {
         ::Info( APP_NAME, "Processing event %i / %i",
                 static_cast< int >( entry ),
                 static_cast< int >( entries ) );
      }

      // Now ask some questions about the loaded configuration:
      SIMPLE_ASSERT( tool.isFilled( 7 ) );
      SIMPLE_ASSERT( tool.isInTrain( 13 ) );
      SIMPLE_ASSERT( tool.distanceFromFront( 146 ) == 0 );
      SIMPLE_ASSERT( tool.distanceFromFront( 238 ) == 300 );
      SIMPLE_ASSERT( tool.gapBeforeTrain( 148 ) == 250 );
      SIMPLE_ASSERT( tool.bunchTrainSpacing() == 50 );
   }

   // Return gracefully:
   return 0;
}
