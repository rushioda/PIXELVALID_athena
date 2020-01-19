/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfig.h"

L1CaloReadoutConfig::L1CaloReadoutConfig(unsigned int channelId, const std::string& description, unsigned int baselinePointer, unsigned int numFadcSlices, unsigned int l1aFadcSlice, unsigned int numLutSlices, unsigned int l1aLutSlice, unsigned int numProcSlices, unsigned int l1aProcSlice, unsigned int numTopoSlices, unsigned int l1aTopoSlice, unsigned int latencyPpmFadc, unsigned int latencyPpmLut, unsigned int latencyCpmInput, unsigned int latencyCpmHits, unsigned int latencyCpmRoi, unsigned int latencyJemInput, unsigned int latencyJemRoi, unsigned int latencyCpCmxBackplane, unsigned int latencyCpCmxLocal, unsigned int latencyCpCmxCable, unsigned int latencyCpCmxSystem, unsigned int latencyCpCmxInfo, unsigned int latencyJetCmxBackplane, unsigned int latencyJetCmxLocal, unsigned int latencyJetCmxCable, unsigned int latencyJetCmxSystem, unsigned int latencyJetCmxInfo, unsigned int latencyJetCmxRoi, unsigned int latencyEnergyCmxBackplane, unsigned int latencyEnergyCmxLocal, unsigned int latencyEnergyCmxCable, unsigned int latencyEnergyCmxSystem, unsigned int latencyEnergyCmxInfo, unsigned int latencyEnergyCmxRoi, unsigned int latencyTopo, unsigned int internalLatencyJemJet, unsigned int internalLatencyJemSum, unsigned int bcOffsetJemJet, unsigned int bcOffsetJemSum, int bcOffsetCmx, int bcOffsetTopo, const std::string& formatTypePpm, const std::string& formatTypeCpJep, const std::string& formatTypeTopo, unsigned int compressionThresholdPpm, unsigned int compressionThresholdCpJep, unsigned int compressionThresholdTopo, unsigned int compressionBaselinePpm, unsigned int readout80ModePpm)
 : m_channelId(channelId)
 , m_description(description)
 , m_baselinePointer(baselinePointer)
 , m_numFadcSlices(numFadcSlices)
 , m_l1aFadcSlice(l1aFadcSlice)
 , m_numLutSlices(numLutSlices)
 , m_l1aLutSlice(l1aLutSlice)
 , m_numProcSlices(numProcSlices)
 , m_l1aProcSlice(l1aProcSlice)
 , m_numTopoSlices(numTopoSlices)
 , m_l1aTopoSlice(l1aTopoSlice)
 , m_latencyPpmFadc(latencyPpmFadc)
 , m_latencyPpmLut(latencyPpmLut)
 , m_latencyCpmInput(latencyCpmInput)
 , m_latencyCpmHits(latencyCpmHits)
 , m_latencyCpmRoi(latencyCpmRoi)
 , m_latencyJemInput(latencyJemInput)
 , m_latencyJemRoi(latencyJemRoi)
 , m_latencyCpCmxBackplane(latencyCpCmxBackplane)
 , m_latencyCpCmxLocal(latencyCpCmxLocal)
 , m_latencyCpCmxCable(latencyCpCmxCable)
 , m_latencyCpCmxSystem(latencyCpCmxSystem)
 , m_latencyCpCmxInfo(latencyCpCmxInfo)
 , m_latencyJetCmxBackplane(latencyJetCmxBackplane)
 , m_latencyJetCmxLocal(latencyJetCmxLocal)
 , m_latencyJetCmxCable(latencyJetCmxCable)
 , m_latencyJetCmxSystem(latencyJetCmxSystem)
 , m_latencyJetCmxInfo(latencyJetCmxInfo)
 , m_latencyJetCmxRoi(latencyJetCmxRoi)
 , m_latencyEnergyCmxBackplane(latencyEnergyCmxBackplane)
 , m_latencyEnergyCmxLocal(latencyEnergyCmxLocal)
 , m_latencyEnergyCmxCable(latencyEnergyCmxCable)
 , m_latencyEnergyCmxSystem(latencyEnergyCmxSystem)
 , m_latencyEnergyCmxInfo(latencyEnergyCmxInfo)
 , m_latencyEnergyCmxRoi(latencyEnergyCmxRoi)
 , m_latencyTopo(latencyTopo)
 , m_internalLatencyJemJet(internalLatencyJemJet)
 , m_internalLatencyJemSum(internalLatencyJemSum)
 , m_bcOffsetJemJet(bcOffsetJemJet)
 , m_bcOffsetJemSum(bcOffsetJemSum)
 , m_bcOffsetCmx(bcOffsetCmx)
 , m_bcOffsetTopo(bcOffsetTopo)
 , m_formatTypePpm(formatTypePpm)
 , m_formatTypeCpJep(formatTypeCpJep)
 , m_formatTypeTopo(formatTypeTopo)
 , m_compressionThresholdPpm(compressionThresholdPpm)
 , m_compressionThresholdCpJep(compressionThresholdCpJep)
 , m_compressionThresholdTopo(compressionThresholdTopo)
 , m_compressionBaselinePpm(compressionBaselinePpm)
 , m_readout80ModePpm(readout80ModePpm)
{
}