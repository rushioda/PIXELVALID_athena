# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from ParseEtaIntercalInput  import ReadEtaIntercalibrationHistograms
from ParseInsituInput_MJB   import ReadInSituHistograms
from ParsePileupInput       import ReadPileupHistograms
from ParseFlavourInput      import ReadFlavourHistograms
from ParsePunchthroughInput import ReadPunchthroughHistograms
from Parse2012Input         import Read2012Histograms

#http://stackoverflow.com/questions/2669059/how-to-sort-alpha-numeric-set-in-python
from itertools import groupby
def natural_sort(s):
    return [int(''.join(g)) if k else ''.join(g) for k, g in groupby(s[0], str.isdigit)]

# Option 2
# 
# Design: This is to make a file entirely from scratch. 
# All components are assumed available new.
# If that is not the case borrow parallel script from
# the Moriond 2016 directory (where punchthrough was kept from 2012)

# Full list of required inputs, distinguished by needing them
# from a different person
# - In situ terms
# - Eta intercalibration
# - Pileup
# - Flavour
# - Punchthrough


# NOTES
# Need to carefully check all flavour histograms in comparison to 2012 to make sure they look sensible!!
# Things below were done in pre-recs and are currently being done here:
#  - Freeze flavour histograms by hand wherever input histogram falls below 0: extrapolate forward in pT from around 2 TeV centrally and from lower pT as eta increases.
#  - FlavorResponse histogram inverted from input file to match Run I convention
freezeFlavourInPt = True

##########################################

# Check useage and store arguments
if len(sys.argv) < 3:
    print "USAGE: Expected the following arguments"
    print "     1. Directory path to V+jet calibrations"
    print "     2. Eta intercalibration directory path"
    print "     3. Pileup directory path"
    print "     5. Flavour directory path"
    print "	6. Punchthrough directory path"
    exit(1)

outBaselineFile = "JESUncertainty_forMJB.root"
theVPlusJetsDir      = sys.argv[1]
etaIntercalDir  = sys.argv[2]
pileupDir       = sys.argv[3]
flavourDir      = sys.argv[4]
punchthroughDir	= sys.argv[5]

# Ensure that all of the directories exist
if not outBaselineFile.endswith(".root"):
    print "Output baseline ROOT file doesn't appear to be a root file:",outBaselineFile
    exit(2)
if not os.path.exists(theVPlusJetsDir):
    print "V+jets directory does not exist:",theVPlusJetsDir
    exit(3)
if not os.path.exists(etaIntercalDir):
    print "Eta intercalibration directory does not exist:",etaIntercalDir
    exit(4)
if not os.path.exists(pileupDir):
    print "Pileup directory does not exist:",pileupDir
    exit(5)
if not os.path.exists(flavourDir):
    print "Flavour directory does not exist:",flavourDir
    exit(6)
if not os.path.exists(punchthroughDir):
    print "Punchthrough directory does not exist:",punchthroughDir
    exit(7)


# Store everything in memory!
currentDir = gDirectory
gROOT.cd()

# Now read the histograms
print "Reading inputs..."
# For now, everything except flavour and cross calibration inputs
# are read in from the final 2012 calibration files.
theVPlusJetsHistos = ReadInSituHistograms(theVPlusJetsDir)
etaIntercalHistos   = ReadEtaIntercalibrationHistograms(etaIntercalDir)
pileupHistos        = ReadPileupHistograms(pileupDir)
flavourHistos       = ReadFlavourHistograms(flavourDir,freezeFlavourInPt) # True flag freezes uncertainty above pT = 2TeV (lower at higher eta)
punchthroughHistos  = ReadPunchthroughHistograms(punchthroughDir)

# Make one mega-dictionary
print "Merging inputs..."
jetDefs = {"AntiKt4Topo_EMJES" : "AntiKt4EMTopo"}#, "AntiKt4Topo_LCJES" : "AntiKt4LCTopo"}#"AntiKt6Topo_EMJES","AntiKt6Topo_LCJES"]
systematics = {}
for aJetDef in jetDefs.keys():

    systematics[aJetDef] = {}
    dictsToUse = [punchthroughHistos,
                  etaIntercalHistos,
                  theVPlusJetsHistos,
                  pileupHistos,
                  flavourHistos]

    for dictionary in dictsToUse :

      # We don't have LC inputs for some of these
      if "LC" in aJetDef :
        if aJetDef not in dictionary.keys() :
          useInstead = dictionary["AntiKt4Topo_EMJES"]
          # Need to change name & copy hists so I don't have
          # memory problems down the road
          myNewDict = {}
          for item in useInstead.keys() :
            thisHist = useInstead[item].Clone()
            thisHistName = useInstead[item].GetName().replace("EM","LC")
            thisHist.SetName(thisHistName)
            myNewDict[item] = thisHist

          dictionary[aJetDef] = myNewDict

      systematics[aJetDef].update(dictionary[aJetDef].items())

# Loop over the mega-dictionary and write results
print "Writing to output file",outBaselineFile,"..."
baselineFile = TFile(outBaselineFile,"RECREATE")
for aJetDef,aSyst in sorted(systematics.iteritems(),key=natural_sort):
    for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
        baselineFile.cd()
        aSystHisto.SetTitle(aSystName+"_"+jetDefs[aJetDef])
        aSystHisto.Write(aSystHisto.GetTitle())

# Done, close the files, revert directory
baselineFile.Close()
gDirectory = currentDir
print "Done!"

