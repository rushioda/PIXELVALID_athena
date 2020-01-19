infile = 'rtt:valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.simul.HITS.e3099_s2578_tid04919495_00/HITS.04919495._000416.pool.root.1'
keys = [
    #TileHitVector_p1
    'MBTSHits@100',
    'TileHitVec@100',
         ]

theApp.EvtMax = 10

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.Calo_setOn()
noMuon = True
noID = True
include ('AthenaPoolUtilities/TPCnvTest.py')

# Disable LAr alignment to avoid spurious warnings
# when running in AthSimulation.
svcMgr.GeoModelSvc.AlignCallbacks = False
from IOVDbSvc.CondDB import conddb
conddb.blockFolder ('/LAR/Align')
