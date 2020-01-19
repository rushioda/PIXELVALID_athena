# TrigDB modifier
# Usage: athenaMT/PT -C 'include("TriggerRelease/dbmod_chronoAuditor.py")' ...
#
# Enable ChronoStat auditor
#
from TrigCommon.TrigPyHelper import trigApp
from GaudiPython.Bindings import iProperty
trigApp.changeJobProperties('.*', 'AuditAlgorithms', True)
trigApp.changeJobProperties('.*', 'AuditServices', True)
trigApp.changeJobProperties('.*', 'AuditTools', True)
trigApp.changeJobProperties('.*', 'AuditInitialize', True)

iProperty('AuditorSvc').Auditors.push_back("'ChronoAuditor/ChronoAuditor'")
iProperty('ChronoStatSvc').ChronoDestinationCout = True
iProperty('ChronoStatSvc').PrintEllapsedTime = True
