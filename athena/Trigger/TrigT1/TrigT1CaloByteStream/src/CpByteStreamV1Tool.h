/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPBYTESTREAMV1TOOL_H
#define TRIGT1CALOBYTESTREAM_CPBYTESTREAMV1TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "DataModel/DataVector.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolHandle.h"

class IInterface;
class InterfaceID;
class StatusCode;

template <typename> class FullEventAssembler;

namespace LVL1 {
  class CMMCPHits;
  class CPMHits;
  class CPMTower;
  class CPBSCollectionV1;
  class IL1CaloMappingTool;
  class TriggerTowerKey;
}

namespace LVL1BS {

class CmmCpSubBlock;
class CpmSubBlockV1;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to CPM towers, CPM hits and CMM-CP hits,
 *  and CP container to raw data conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class CpByteStreamV1Tool : public AthAlgTool {

 public:
   CpByteStreamV1Tool(const std::string& type, const std::string& name,
                                               const IInterface* parent);
   virtual ~CpByteStreamV1Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to CPM towers
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMTower>* ttCollection);
   /// Convert ROB fragments to CPM hits
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMHits>* hitCollection);
   /// Convert ROB fragments to CMM-CP hits
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMMCPHits>* hitCollection);

   /// Convert CP Container to bytestream
   StatusCode convert(const LVL1::CPBSCollectionV1* cp, RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

 private:

   enum CollectionType { CPM_TOWERS, CPM_HITS, CMM_CP_HITS };

   typedef DataVector<LVL1::CPMTower>                    CpmTowerCollection;
   typedef DataVector<LVL1::CPMHits>                     CpmHitsCollection;
   typedef DataVector<LVL1::CMMCPHits>                   CmmCpHitsCollection;
   typedef std::map<unsigned int, LVL1::CPMTower*>       CpmTowerMap;
   typedef std::map<int, LVL1::CPMHits*>                 CpmHitsMap;
   typedef std::map<int, LVL1::CMMCPHits*>               CmmCpHitsMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   /// Convert bytestream to given container type
   StatusCode convertBs(const IROBDataProviderSvc::VROBFRAG& robFrags,
                        CollectionType collection);
   /// Unpack CMM-CP sub-block
   void decodeCmmCp(CmmCpSubBlock* subBlock, int trigCmm);
   /// Unpack CPM sub-block
   void decodeCpm(CpmSubBlockV1* subBlock, int trigCpm, CollectionType collection);

   /// Find a CPM tower for given key
   LVL1::CPMTower*  findCpmTower(unsigned int key);
   /// Find CPM hits for given crate, module
   LVL1::CPMHits*   findCpmHits(int crate, int module);
   /// Find CMM-CP hits for given crate, data ID
   LVL1::CMMCPHits* findCmmCpHits(int crate, int dataID);

   /// Set up CPM tower map
   void setupCpmTowerMap(const CpmTowerCollection* ttCollection);
   /// Set up CPM hits map
   void setupCpmHitsMap(const CpmHitsCollection* hitCollection);
   /// Set up CMM-CP hits map
   void setupCmmCpHitsMap(const CmmCpHitsCollection* hitCollection);

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem);

   /// Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_cpmMaps;
   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// Hardware crate number offset
   int m_crateOffsetHw;
   /// Software crate number offset
   int m_crateOffsetSw;
   /// Sub_block header version
   int m_version;
   /// Data compression format
   int m_dataFormat;
   /// Number of channels per module
   int m_channels;
   /// Number of crates
   int m_crates;
   /// Number of CPM modules per crate
   int m_modules;
   /// Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Default number of slices in simulation
   int m_dfltSlices;
   /// Force number of slices in bytestream
   int m_forceSlices;
   /// Minimum crate number when writing out bytestream
   int m_crateMin;
   /// Maximum crate number when writing out bytestream
   int m_crateMax;
   /// Tower channels to accept (1=Core, 2=Overlap)
   int m_coreOverlap;
   /// Unpacking error code
   unsigned int m_rodErr;
   /// ROB source IDs
   std::vector<uint32_t> m_sourceIDs;
   /// Sub-detector type
   eformat::SubDetector m_subDetector;
   /// Source ID converter
   L1CaloSrcIdMap* m_srcIdMap;
   /// Trigger tower key provider
   LVL1::TriggerTowerKey* m_towerKey;
   /// CPM sub-block for unpacking
   CpmSubBlockV1* m_cpmSubBlock;
   /// CMM-CP sub-block for unpacking
   CmmCpSubBlock* m_cmmCpSubBlock;
   /// Hits0 vector for unpacking
   std::vector<unsigned int> m_hitsVec0;
   /// Hits1 vector for unpacking
   std::vector<unsigned int> m_hitsVec1;
   /// Error0 vector for unpacking
   std::vector<int> m_errVec0;
   /// Error1 vector for unpacking
   std::vector<int> m_errVec1;
   /// EM data vector for unpacking
   std::vector<int> m_emVec;
   /// Had data vector for unpacking
   std::vector<int> m_hadVec;
   /// EM error data vector for unpacking
   std::vector<int> m_emErrVec;
   /// Had error data vector for unpacking
   std::vector<int> m_hadErrVec;
   /// Vector for current CPM sub-blocks
   DataVector<CpmSubBlockV1> m_cpmBlocks;
   /// Vector for current CMM-CP hit0 sub-blocks
   DataVector<CmmCpSubBlock> m_cmmHit0Blocks;
   /// Vector for current CMM-CP hit1 sub-blocks
   DataVector<CmmCpSubBlock> m_cmmHit1Blocks;
   /// Current CPM tower collection
   CpmTowerCollection*  m_ttCollection;
   /// Current CPM hits collection
   CpmHitsCollection*   m_hitCollection;
   /// Current CMM-CP hits collection
   CmmCpHitsCollection* m_cmmHitCollection;
   /// CPM tower map
   CpmTowerMap  m_ttMap;
   /// CPM hits map
   CpmHitsMap   m_hitsMap;
   /// CMM-CP hits map
   CmmCpHitsMap m_cmmHitsMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;

};

} // end namespace

#endif
