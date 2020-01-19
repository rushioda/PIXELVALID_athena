#!/usr/bin/env python
# $Id: d3pdReaderSFrameMaker.py 471380 2011-11-29 15:25:42Z krasznaa $
#
# This script can be used to quickly create an SFrame package out of the
# source files generated in a previous step.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

# Base module(s):
import sys
import os
import re

# Local module(s):
from D3PDMakerReader.Helpers import *

##
# @short C/C++ style main function
#
# This function executes all the code stored in this script.
#
# @returns <code>0</code> in case of success, something else in case
#          of failure
def main():

    # Create a logger object first of all:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( os.path.splitext( os.path.basename( sys.argv[ 0 ] ) )[ 0 ] )

    #
    # Specify the command line options:
    #
    desc = "This script can be used to generate an SFrame package out of the D3PDReader " \
           "sources generated by one of the D3PDReader code generators."
    vers = "$Revision: 471380 $"
    import optparse
    parser = optparse.OptionParser( description = desc,
                                    version = vers,
                                    usage = "%prog [options] <source files>" )
    parser.add_option( "-v", "--verbosity", dest = "verbosity",
                       action = "store", type = "int", default = 3,
                       help = "Message verbosity level" )
    parser.add_option( "-p", "--package-name", dest = "packagename",
                       action = "store", type = "string", default = "D3PDReader",
                       help = "Name for the RootCore package" )
    parser.add_option( "-o", "--output", dest = "output",
                       action = "store", type = "string", default = "./",
                       help = "Output directory for the RootCore package" )

    # Parse the command line options:
    ( options, files ) = parser.parse_args()

    # Do a small sanity check:
    if not len( files ):
        logger.error( "You have to define at least one source file!" )
        parser.print_help()
        return 255

    # Set the default logging level:
    logging.getLogger( "" ).setLevel( options.verbosity )

    # Print some welcome message:
    logger.info( "***********************************************************" )
    logger.info( "*             D3PDReader SFrame Package Maker" )
    logger.info( "* Version: $Revision: 471380 $" )
    logger.info( "*" )
    logger.info( "* Output level: " + str( options.verbosity ) )
    logger.info( "* Package name: " + options.packagename )
    logger.info( "* Output directory: " + options.output )
    logger.info( "* Source files: " + ", ".join( files ) )
    logger.info( "***********************************************************" )

    # Filter out the header and source files:
    ( header_files, source_files ) = separateSources( files )
    dictionary_header_files = dictionaryHeaders( files )

    # Create the package's skeleton:
    if makeSFramePackageSkeleton( options.output, options.packagename ):
        logger.error( "Failed to create RootCore package skeleton" )
        return 255

    # Copy the header files:
    import shutil
    for header in header_files:
        shutil.copy( header, options.output + "/" + options.packagename + "/include/" )
        pass

    # Copy the source files:
    for source in source_files:
        shutil.copy( source, options.output + "/" + options.packagename + "/src/" )
        pass

    # Transform the include paths in the source files:
    sframe_sources = [ ( options.output + "/" + options.packagename + "/src/" + \
                         os.path.basename( source ) ) for source in source_files ]
    for source in sframe_sources:
        # Read in the file's contents:
        sfile = open( source, "r" )
        cont = sfile.read()
        sfile.close()

        # Modify the file's contents:
        cont = re.sub( "#include \"(.*)\"",
                       "#include \"../include/\g<1>\"", cont )

        # Overwrite the original file:
        sfile = open( source, "w" )
        sfile.write( cont )
        sfile.close()
        pass

    # Extract the class names to generate a dictionary for:
    classnames = dictionaryClasses( dictionary_header_files )

    # Create the LinkDef file:
    if writeLinkDefFile( options.output + "/" + options.packagename + "/include/" + \
                         options.packagename + "_LinkDef.h", classnames ):
        logger.error( "Failed writing SFrame LinkDef.h file" )
        return 255

    # Write the ChangeLog, just for fun:
    import datetime
    clog = open( options.output + "/" + options.packagename + "/ChangeLog", "w" )
    clog.write( "%s d3pdReaderSFrameMaker.py <Attila.Krasznahorkay@cern.ch>\n" % \
                datetime.date.today() )
    clog.write( "\t* Created the package\n" )
    clog.close()

    return 0

# Execute the main function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
