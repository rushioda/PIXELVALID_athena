# TrigDB modifier
# Usage: athenaHLT.py --postcommand 'include("TriggerRelease/dbmod_BFieldAutoConfig.py")' ...
#
# Use DCS currents to configure magnetic field
#
from GaudiPython.Bindings import iProperty
from AthenaCommon.Logging import logging
log = logging.getLogger('dbmod_BFieldAutoConfig.py')

db = 'CONDBR2'
if '_run_number' in dir() and _run_number<222222: db = 'COMP200'

# Add the DCS folder (note the double-use of quotes)
f = "'<db>COOLOFL_DCS/%s</db> /EXT/DCS/MAGNETS/SENSORDATA'" % db
log.info("Adding folder %s to IOVDbSvc" % f)
iProperty('IOVDbSvc').Folders.push_back(f)
iProperty('AtlasFieldSvc').UseDCS = True
