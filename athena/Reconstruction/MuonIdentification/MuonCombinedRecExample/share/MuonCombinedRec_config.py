#
# Configuration jobOptions for setting up Muon Combined Reconstruction
#
include.block("MuonCombinedRecExample/MuonCombinedRec_config.py")

from AthenaCommon.Resilience import treatException,protectedInclude
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

muonCombinedRecFlags.setDefaults()

include ("MuonCombinedRecExample/MuonCombinedRec_preprocessing.py")

include ("MuonCombinedRecExample/MuonCombinedRec_identification.py")

if not rec.doAODMerging():    
    include ("MuonCombinedRecExample/MuonCombinedRec_postprocessing.py")
