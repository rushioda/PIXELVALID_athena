# RunJetRec.py
#
# David Adams
# February 2014
#
# Job options for running jet reconstruction.
#
# Run with
# > athena.py test_RunJetRec.py
#

runJetGrooming = True

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags

# Import the standard jet tool manager.
from JetRec.JetRecStandardToolManager import jtm

# Set this true to test LAr bad HV fractions with data.
# See ATLJETMET-15
#useLArHVCorr = jetFlags.useTruth == False
useLArHVCorr = False

# Add Delta-R truth flavor tags.
useDRTruthFlavor = jetFlags.useTruth

#--------------------------------------------------------------
# Define the finders and groomers.
# Each line configures a finder or groomer and its associated jetrec tool.
# The first argument is the name of the output collection and the jetrec tool.
# A fifth argument may be added to change the list of modifiers.
#--------------------------------------------------------------

# Update the modifier lists.
# if useLArHVCorr:
#   jtm.modifiersMap["calib"] += [jtm.larhvcorr]
# jtm.modifiersMap["mycalib"] = jtm.modifiersMap["calib"]
# if jtm.haveShowerDeconstructionTool:
#   jtm.modifiersMap["mycalib"]

# Finders.
# Calibration for topo jets: calibOpt =
#   a - area corrn
#   r - residual pileup corrn
#   j - GES
#   g - GSC
#   i - Insitu
gatopo  = 0.01
gatruth = 0.00
gatrack = 0.00
if jetFlags.useTruth:
  jtm.addJetFinder("Run2AntiKt4TruthJets",    "AntiKt", 0.4,    "truth", ghostArea=gatruth, ptmin= 5000)
  jtm.addJetFinder("Run2AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ghostArea=gatruth, ptmin= 5000)
  jtm.addJetFinder("Run2AntiKt10TruthJets",   "AntiKt", 0.4,    "truth", ghostArea=gatruth, ptmin=40000)
  jtm.addJetFinder("Run2AntiKt10TruthWZJets", "AntiKt", 1.0,    "truth", ghostArea=gatruth, ptmin=40000)
  jtm.addJetFinder("Run2CamKt12TruthJets",     "CamKt", 1.2,    "truth", ghostArea=gatruth, ptmin=40000)
  jtm.addJetFinder("Run2CamKt12TruthWZJets",   "CamKt", 1.2,    "truth", ghostArea=gatruth, ptmin=40000)
if jetFlags.useTracks:
  jtm.addJetFinder("Run2AntiKt2PV0TrackJets", "AntiKt", 0.2, "pv0track", ghostArea=gatrack, ptmin= 2000)
  jtm.addJetFinder("Run2AntiKt3PV0TrackJets", "AntiKt", 0.3, "pv0track", ghostArea=gatrack, ptmin= 2000)
  jtm.addJetFinder("Run2AntiKt4TrackJets",    "AntiKt", 0.4,    "track", ghostArea=gatrack, ptmin= 2000)
  jtm.addJetFinder("Run2AntiKt4ZTrackJets",   "AntiKt", 0.4,   "ztrack", ghostArea=gatrack, ptmin= 2000)
  jtm.addJetFinder("Run2AntiKt4PV0TrackJets", "AntiKt", 0.4, "pv0track", ghostArea=gatrack, ptmin= 2000)
jtm.addJetFinder(  "Run2AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo", "emtopo_ungroomed", ptmin=2000, ptminFilter= 5000, ghostArea=gatopo, calibOpt="ar")
jtm.addJetFinder(  "Run2AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo", "lctopo_ungroomed", ptmin=2000, ptminFilter= 7000, ghostArea=gatopo, calibOpt="ar")
jtm.addJetFinder(  "Run2AntiKt4EMPFlowJets",  "AntiKt", 0.4,  "empflow", "pflow_ungroomed",  ptmin=2000, ptminFilter= 5000, ghostArea=gatopo, calibOpt="ar:pflow")
jtm.addJetFinder(  "Run2AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo", "lctopo_ungroomed", ptmin=2000, ptminFilter=50000, ghostArea=gatopo, calibOpt="none")
jtm.addJetFinder(  "Run2CamKt12LCTopoJets",    "CamKt", 1.2,   "lctopo", "lctopo_ungroomed", ptmin=2000, ptminFilter=50000, ghostArea=gatopo, calibOpt="none")

# Groomers.
if runJetGrooming:
  jtm.addJetSplitter(   "Run2Split040CamKt12LCTopoJets",  1.0,  0.04, "Run2CamKt12LCTopoJets")
  jtm.addJetTrimmer(     "Run2TrimR2AntiKt10LCTopoJets",  0.2,  0.05, "Run2AntiKt10LCTopoJets")
  jtm.addJetPruner( "Run2PruneR30Z10AntiKt10LCTopoJets",  0.30, 0.10, "Run2AntiKt10LCTopoJets")

#--------------------------------------------------------------
# Configure the jet algorithm.
#--------------------------------------------------------------

from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence()
