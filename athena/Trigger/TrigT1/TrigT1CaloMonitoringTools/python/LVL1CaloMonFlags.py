# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package LVL1CaloMonFlags
#
# Flags to steer L1Calo monitoring
#
# @author Peter Faulkner
#
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer


###
### property container class
###
## Container for L1Calo monitoring setup
class LVL1CaloMonFlagsCont(JobPropertyContainer):

    pass


from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(LVL1CaloMonFlagsCont)
LVL1CaloMonFlags = TriggerFlags.LVL1CaloMonFlagsCont


###
### flags
###

## Flag for turning on PPrStabilityMon and everything else off
class doPPrStabilityMon(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Flag for turning on/off FineTime Monitoring
class doFineTimeMonitoring(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Flag for turning on/off Pedestal Monitoring
class doPedestalMonitoring(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Flag for turning on/off Pedestal Correction Monitoring
class doPedestalCorrectionMonitoring(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Flag for turning on/off EtCorrelation Monitoring
class doEtCorrelationMonitoring(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

LVL1CaloMonFlags.add_JobProperty(doPPrStabilityMon)
LVL1CaloMonFlags.add_JobProperty(doFineTimeMonitoring)
LVL1CaloMonFlags.add_JobProperty(doPedestalMonitoring)
LVL1CaloMonFlags.add_JobProperty(doEtCorrelationMonitoring)
