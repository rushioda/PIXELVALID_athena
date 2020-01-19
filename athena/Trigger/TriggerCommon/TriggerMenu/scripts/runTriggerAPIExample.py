#!/usr/bin/env python

from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

# List of lowest-unprescaled for a given type and period
# Loading periods is slow the first time and can take up to minutes,
# the information is cached and pickled and takes seconds for subsequent calls
# See TriggerEnums for all possibilities of types and periods
for triggerType in TriggerType:
    print "\n------------- TriggerType:",triggerType
    for triggerPeriod in TriggerPeriod:
        unprescaled = TriggerAPI.getLowestUnprescaled(triggerPeriod, triggerType)
        print "- TriggerPeriod:",triggerPeriod
        print unprescaled

# Further options:
# - ask for specific runs and/or specific patterns
print "Pattern match :",TriggerAPI.getLowestUnprescaled(337833, TriggerType.mu_bphys, matchPattern="bJpsi|bUpsi")
# - combined items, example bjet + ht, bjet + met
#                           bj = bj_single | bj_multi
print "Bjet combined items:",TriggerAPI.getLowestUnprescaled(337833, TriggerType.bj, TriggerType.xe | TriggerType.ht)
# - combined items, example electron + anything
print "Ele combined items:",TriggerAPI.getLowestUnprescaled(337833, TriggerType.el, TriggerType.ALL)
# - lowest unprescaled in at least one period. 
#   Allows to get items that were accidentally prescaled in some period or changed over time
#   E.g. no multi b-jet trigger is unprescaled over the full 2017
print "Lowest in at least one period:",TriggerAPI.getLowestUnprescaledAnyPeriod(TriggerPeriod.y2017periodAll, TriggerType.bj_multi)
# - retrieve items above a certain live fraction instead of unprescaled
#   HLT_j225_gsc420_boffperf_split is unprescaled
#   HLT_j225_gsc400_boffperf_split recorded >95% of the available luminosity
print TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017periodB, TriggerType.j_single, matchPattern="j225_gsc")
print TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017periodB, TriggerType.j_single, matchPattern="j225_gsc", livefraction=0.95)

# - combined items with more than 2 types:
print "Muon+jet+met items:",TriggerAPI.getLowestUnprescaled(337833, TriggerType.mu, [TriggerType.j, TriggerType.xe])
# - items that are expected to be lowest-unprescaled at higher luminosities
print "Single muon lowest-unprescaled 2.0e34 items:",TriggerAPI.getLowestUnprescaled(TriggerPeriod.future2e34, TriggerType.mu_single)
# - can also retrieve all unprescaled items, including higher thresholds
print "Single muon unprescaled 2.0e34 items:"       ,TriggerAPI.getUnprescaled(TriggerPeriod.future2e34, TriggerType.mu_single)

# Use the lists that are build with the release with the expected unprescaled items of the current/future menu
# Will return the same as TriggerAPI.getUnprescaled(TriggerPeriod.future2e34, ...)
# The list has been already processed and takes therefore ~0 time
from TriggerMenu.api import list_unprescaled1p8e34, list_unprescaled2e34
print "Precomputed single muon 2.0e34 items:",list_unprescaled2e34.list_mu_single
print "Precomputed single muon 1.8e34 items:",list_unprescaled1p8e34.list_mu_single

# Set a custom GRL, affects *ALL* calls after it has been set 
print "Multi-bjet in periods GHIK (default GRL)", TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017periodGHIK, TriggerType.bj_multi)
TriggerAPI.setCustomGRL("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20171130/data17_13TeV.periodAllYear_DetStatus-v97-pro21-13_Unknown_PHYS_StandardGRL_All_Good_25ns_BjetHLT_Normal2017.xml")
print "Multi-bjet in periods GHIK (Bjet GRL)   ", TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017periodGHIK, TriggerType.bj_multi)

