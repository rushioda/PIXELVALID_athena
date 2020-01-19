#
#  extraMetadata.py
#  
#
#  Created by Alvin on 01/09/2009.
#  Copyright (c) 2009 University of Birmingham. All rights reserved.
#
import re

# Format of dictionary
#
# Dictionary must be called extraMetadataDict.
#
# key -  Arbitrary filter name
# value - When a compiled pattern is provided, it is used to match against lines in the log file.
#         Any other value will be treated as a forced insertion of a metadata.

extraMetadataDict = \
{ 
'geometryVersion' : re.compile( r'TrfJobReport metaData_geometryVersion = (ATLAS-(?:GEO|CSC)\w*-[0-9\-]+)\s*\Z' ),
'conditionsTag'   : re.compile( r'TrfJobReport metaData_conditionsTag = ((?:OFL|COM)COND-\w+-[0-9\-]+)\s*\Z' ),
'beamType'        : re.compile( r'TrfJobReport metaData_BeamType = (\w+)\s*\Z' ),
}
