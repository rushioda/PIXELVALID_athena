#!/bin/sh

# art-description: DAOD building TOPQ5 mc16
# art-type: build

pwd
athena.py -c "REO_doAutoConfiguration=False; beamType='cosmics'" RecExOnline/RecExOnline_User_Offline_isOnlineTrue.py >& log_RecExOnline_User_Offline_isOnlineTrue_py_cosmic.log
grep "successful run" log_RecExOnline_User_Offline_isOnlineTrue_py_cosmic.log
