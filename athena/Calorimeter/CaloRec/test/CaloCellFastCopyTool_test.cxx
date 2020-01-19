/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * CaloCellFastCopyTool_CppUnit.cxx
 *
 *  Created on: Oct 7, 2013
 *      Author:
 */


#undef NDEBUG


#include "CaloRec/CaloCellFastCopyTool.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "IdDictParser/IdDictParser.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/setupStoreGate.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>



/** Class provides dummy algorithm
 *  to help construct CaloCellFastCopyTool to be tested.
 */

class DummyAlgorithm: public AthAlgorithm {
  public:
    DummyAlgorithm(const std::string& name, ISvcLocator* svcloc)
        : AthAlgorithm(name, svcloc) {
    }

    virtual StatusCode execute() {
      return StatusCode::SUCCESS;
    }

};

/** Class provides instance of CaloCell_ID
 *  and helps to construct CaloCell using cell hash.
 *  (code taken from: Calorimeter/CaloEvent/test/CaloCellContainer_test.cxx)
 *  The class designed using singleton pattern.
 */

class CaloHelper
{
public:

  static CaloHelper* GetInstance(void)
  {
    if (!s_instance) {
      s_instance = new CaloHelper();
      s_instance->Initialize();
    }
    ++s_refCount;
    return s_instance;
  }


  static void DeleteInstance(void)
  {
    --s_refCount;
    if (s_refCount == 0) delete s_instance;
    s_instance = NULL;
  }


  ~CaloHelper()
  {
    if (m_emID) delete m_emID;
    if (m_hecID) delete m_hecID;
    if (m_fcalID) delete m_fcalID;
    if (m_minifcalID) delete m_minifcalID;
    if (m_tileID) delete m_tileID;
    if (m_parser) delete m_parser;

    std::map<Identifier, CaloDetDescriptor*>::iterator it = m_ddmap.begin();
    for (; it != m_ddmap.end(); ++it) {
      if (it->second) delete it->second;
    }
  }

  CaloCell_ID* GetCaloID()
  {
    return m_caloID;
  }

  CaloCell* GetCell(IdentifierHash calo_hash)
  {
    Identifier reg_id = m_caloID->region_id(m_caloID->cell_id(calo_hash));
    CaloDetDescriptor* dd = m_ddmap[reg_id];
    if (!dd) {
      dd = new CaloDetDescriptor(reg_id, m_tileID, m_caloID);
      m_ddmap[reg_id] = dd;
    }
    int sub_calo;
    IdentifierHash subcalo_hash = m_caloID->subcalo_cell_hash(calo_hash, sub_calo);
    CaloDetDescrElement* dde = new DummyDetDescrElement(subcalo_hash, dd->caloCellMin(), 0, dd);
    return new CaloCell(dde, 1, 1, 1, CaloGain::LARHIGHGAIN);
  }

private:

  CaloHelper()
    : m_emID(0)
    , m_hecID(0)
    , m_fcalID(0)
    , m_minifcalID(0)
    , m_tileID(0)
    , m_parser(0)
    , m_caloID(0)
  {
  }


  void Initialize (void)
  {
    m_emID = new LArEM_ID;
    m_hecID = new LArHEC_ID;
    m_fcalID = new LArFCAL_ID;
    m_minifcalID = new LArMiniFCAL_ID;
    m_tileID = new TileID;

    m_parser = new IdDictParser;
    m_parser->register_external_entity("LArCalorimeter", "IdDictLArCalorimeter.xml");
    IdDictMgr& idd = m_parser->parse("IdDictParser/ATLAS_IDS.xml");
    m_emID->set_do_neighbours(false);
    m_emID->initialize_from_dictionary(idd);
    m_hecID->initialize_from_dictionary(idd);
    m_fcalID->set_do_neighbours(false);
    m_fcalID->initialize_from_dictionary(idd);
    m_minifcalID->set_do_neighbours(false);
    m_minifcalID->initialize_from_dictionary(idd);
    m_tileID->set_do_neighbours(false);
    m_tileID->initialize_from_dictionary(idd);

    m_caloID = new CaloCell_ID(m_emID, m_hecID, m_fcalID, m_minifcalID, m_tileID);
    m_caloID->initialize_from_dictionary(idd);
  }

  static CaloHelper* s_instance;

  LArEM_ID* m_emID;
  LArHEC_ID* m_hecID;
  LArFCAL_ID* m_fcalID;
  LArMiniFCAL_ID* m_minifcalID;
  TileID* m_tileID;
  IdDictParser* m_parser;
  CaloCell_ID* m_caloID;
  std::map<Identifier, CaloDetDescriptor*> m_ddmap;
  static int s_refCount;
};

CaloHelper* CaloHelper::s_instance = 0;
int CaloHelper::s_refCount = 0;


/** Class to test the "CaloCellFastCopyTool.h" class
 *   StoreGate service required
 *
 */
class CaloCellFastCopyToolTest
{
public:
  
  CaloCellFastCopyToolTest()
    : m_evtStore("StoreGateSvc","")
    , m_detStore("DetectorStore","")
    , m_alg(0)
    , m_caloHelper(0)
    , m_caloID(0)
  {
  }


  ~CaloCellFastCopyToolTest()
  {
    if (m_alg) delete m_alg;
  }

  
  void setUp()
  {
    ISvcLocator* svcloc = Gaudi::svcLocator();
    m_evtStore.retrieve();
    m_detStore.retrieve();
    m_alg = new DummyAlgorithm("DummyAlgorithm", svcloc);
    m_alg->addRef();
    m_caloHelper = CaloHelper::GetInstance();
    m_caloID = m_caloHelper->GetCaloID();

    if (!m_detStore->contains<CaloCell_ID>("CaloCell_ID"))
      m_detStore->record(m_caloID, "CaloCell_ID");

    m_tileGap3Hashes.push_back(186886); // TileGap3
    m_tileGap3Hashes.push_back(186887); // TileGap3
    m_allHashes = m_tileGap3Hashes;
    m_allHashes.push_back(186885); // TileGap1

    if (!m_evtStore->contains<CaloCellContainer>("AllCalo")) {
      CaloCellContainer* cellCont;
      cellCont = new CaloCellContainer(SG::OwnershipPolicy::OWN_ELEMENTS);
      m_evtStore->record(cellCont, "AllCalo", false);

      std::vector<IdentifierHash>::const_iterator it = m_allHashes.begin();
      for (; it != m_allHashes.end(); ++it) {
        cellCont->push_back(m_caloHelper->GetCell(*it));
      }
    }
  }


  void testViewNotAvoidingDuplicatesFindCellIsNotFast()
  {
    testViewNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[1]", false);
  }

  void testViewNotAvoidingDuplicatesFindCellIsFast()
  {
    testViewNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[1]", true);
  }

  void testViewNotAvoidingDuplicatesIsFindCellFast(std::string toolName,
                                                   bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates ,
                          isFindCellFast));

    std::unique_ptr<CaloConstCellContainer> destCont =
      CxxUtils::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap3 cells from source container has
    // been copied in destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that the pointers in both container is same.
    // because destination container has VIWE_ELEMENT own policy.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    std::pair<CaloCellContainer::CellVector::iterator, CaloCellContainer::CellVector::iterator> misPair;
    misPair = std::mismatch(srcCv.begin(), srcCv.end(), destCv.begin());
    assert( misPair.first == srcCv.end() );

    // Test that only Tile subcalo is set in destination container;
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test next event (imitated);
    assert(tool->process(destCont.get()).isSuccess());
    // Not avoiding duplicates, therefore
    // size of destination container has been doubled
    assert( destCont->size() == (2 * m_tileGap3Hashes.size()) );
  }


  void testViewAvoidingDuplicatesFindCellIsNotFast()
  {
    testViewAvoidingDuplicatesIsFindCellFast("CopyToolTest[3]", false);
  }


  void testViewAvoidingDuplicatesFindCellIsFast()
  {
    testViewAvoidingDuplicatesIsFindCellFast("CopyToolTest[4]", true);
  }


  void testViewAvoidingDuplicatesIsFindCellFast(std::string toolName,
                                                bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      CxxUtils::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);
    // Add one TileGap3 cell to destination in order to test
    // if tool is avoiding duplicates in destination container
    std::unique_ptr<CaloCell> cell (m_caloHelper->GetCell(m_tileGap3Hashes[0]));
    destCont->push_back(cell.get());

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap3 cells from source container
    // has been copied to destination container
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that all pointers to TileGap3 cells in both containers
    // is the same (destination container has VIWE_ELEMENTS own policy)
    // apart pointer to specially added cell before.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      if (destCv[i] != cell.get()) assert( destCv[i] == srcCv[i] );
    }

    // Test that only Tile sub calo is set in destination container
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert( tool->process(destCont.get()).isSuccess() );
    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == m_tileGap3Hashes.size() );
  }


  void testViewNotAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testViewNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[13]", false);
  }

  void testViewNotAvoidingDuplicatesFindCellIsFastConst()
  {
    testViewNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[14]", true);
  }

  void testViewNotAvoidingDuplicatesIsFindCellFastConst(std::string toolName,
                                                        bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates ,
                          isFindCellFast));

    std::unique_ptr<CaloCellContainer> destCont =
      CxxUtils::make_unique<CaloCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Test tool
    assert(tool->process(destCont.get()).isFailure());
  }


  void testViewAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testViewAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[15]", false);
  }


  void testViewAvoidingDuplicatesFindCellIsFastConst()
  {
    testViewAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[16]", true);
  }


  void testViewAvoidingDuplicatesIsFindCellFastConst(std::string toolName,
                                                     bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloCellContainer> destCont =
      CxxUtils::make_unique<CaloCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);
    // Test tool
    assert(tool->process(destCont.get()).isFailure());
  }


  void testCloneNotAvoidingDuplicatesFindCellIsNotFast()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[5]", false);
  }


  void testCloneNotAvoidingDuplicatesFindCellIsFast()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[6]", true);
  }

  
  void testCloneNotAvoidingDuplicatesIsFindCellFast(std::string toolName,
                                                    bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloCellContainer> destCont =
      CxxUtils::make_unique<CaloCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert( tool->process(destCont.get()).isSuccess() );

    // Test that size of destination container has been doubled
    // because of not avoiding duplicates
    assert( destCont->size() == (2 * m_tileGap3Hashes.size()) );
  }


  void testCloneAvoidingDuplicatesFindCellIsNotFast()
  {
    testCloneAvoidingDuplicatesIsFindCellFast("CopyToolTest[7]", false);
  }


  void testCloneAvoidingDuplicatesFindCellIsFast()
  {
    testCloneAvoidingDuplicatesIsFindCellFast("CopyToolTest[8]", true);
  }

  
  void testCloneAvoidingDuplicatesIsFindCellFast(std::string toolName,
                                                 bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));
    
    // Initialize destination container
    std::unique_ptr<CaloCellContainer> destCont =
      CxxUtils::make_unique<CaloCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);
    // Add one TileGap3 cell to destination in order to test
    // if tool is avoiding duplicates in destination container
    CaloCell* cell = m_caloHelper->GetCell(m_tileGap3Hashes[0]);
    destCont->push_back(cell);

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy,
    // apart of pointer to cell specially added before.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      if (destCv[i] != cell) assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert( tool->process(destCont.get()).isSuccess() );

    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == m_tileGap3Hashes.size() );
  }


  void testCloneNotAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[9]", false);
  }


  void testCloneNotAvoidingDuplicatesFindCellIsFastConst()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[10]", true);
  }

  
  void testCloneNotAvoidingDuplicatesIsFindCellFastConst(std::string toolName,
                                                         bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      CxxUtils::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert( tool->process(destCont.get()).isSuccess() );

    // Test that size of destination container has been doubled
    // because of not avoiding duplicates
    assert( destCont->size() == (2 * m_tileGap3Hashes.size()) );
  }


  void testCloneAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testCloneAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[11]", false);
  }


  void testCloneAvoidingDuplicatesFindCellIsFastConst()
  {
    testCloneAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[12]", true);
  }

  
  void testCloneAvoidingDuplicatesIsFindCellFastConst(std::string toolName,
                                                      bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(0);
    m_evtStore->retrieve(srcCont, "AllCalo");

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));
    
    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      CxxUtils::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);
    // Add one TileGap3 cell to destination in order to test
    // if tool is avoiding duplicates in destination container
    CaloCell* cell = m_caloHelper->GetCell(m_tileGap3Hashes[0]);
    destCont->push_back(cell);

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy,
    // apart of pointer to cell specially added before.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      if (destCv[i] != cell) assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert( tool->process(destCont.get()).isSuccess() );

    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == m_tileGap3Hashes.size() );
  }


  void testOverFullContainerFindCellIsNotFast()
  {
    testOverFullContainerIsFindCellFast("CopyToolTest[9]", false);
  }


  void testOverFullContainerFindCellIsFast()
  {
    testOverFullContainerIsFindCellFast("CopyToolTest[10]", true);
  }


  void testOverFullContainerIsFindCellFast(std::string toolName,
                                           bool isFindCellFast)
  {
    // Initialize full source container
    CaloCellContainer* srcCont;
    srcCont = new CaloCellContainer(SG::OwnershipPolicy::VIEW_ELEMENTS);
    IdentifierHash maxCellHash = m_caloID->calo_cell_hash_max();
    unsigned int nTileGap1(0);
    unsigned int nTileGap3(0);
    for (IdentifierHash cellHash = 0; cellHash < maxCellHash; cellHash += 1) {
      srcCont->push_back(m_caloHelper->GetCell(cellHash));
      if (m_caloID->calo_sample(cellHash) == CaloCell_ID::TileGap1) ++nTileGap1;
      if (m_caloID->calo_sample(cellHash) == CaloCell_ID::TileGap3) ++nTileGap3;
    }

    std::string caloCellName("AllCalloFull");
    m_evtStore->record(srcCont, caloCellName, false);

    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      CxxUtils::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast,
                          caloCellName));

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == nTileGap3 );

    // Test next event (imitated)
    assert(tool->process(destCont.get()).isSuccess());

    // Test that size of destination container has been doubled
    // because of not avoiding duplicates
    assert( destCont->size() == (2 * nTileGap3) );

    // Test with avoiding duplicates
    // -----------------------------

    // Initialize destination container
    destCont =
      CxxUtils::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Get initialized tool to be tested.
    includeSamplings.push_back("TileGap1");
    avoidDuplicates = true;
    tool = std::unique_ptr<CaloCellFastCopyTool>
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast,
                          caloCellName));

    // Test tool
    assert(tool->process(destCont.get()).isSuccess());

    // Test that all TileGap1 + TileGap3 cells has been copied to destination container.
    unsigned int nTileGap1AndTileGap3((nTileGap1 + nTileGap3));
    assert( destCont->size() == nTileGap1AndTileGap3 );

    // Test next event (imitated)
    assert(tool->process(destCont.get()).isSuccess());

    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == nTileGap1AndTileGap3 );
  }


private:

  CaloCellFastCopyTool*
  getInitializedTool(std::string name,
                     std::vector<std::string>& includeSamplings,
                     bool avoidDuplicates = false,
                     bool isFindCellFast = false,
                     std::string caloCellName = "AllCalo")
  {
    CaloCellFastCopyTool* tool = new CaloCellFastCopyTool("CaloCellFastCopyTool", name, m_alg);
    tool->addRef();
    assert(tool->setProperty("OutputLevel", "1").isSuccess());
    assert(tool->setProperty("AvoidDuplicates",avoidDuplicates).isSuccess());
    assert(tool->setProperty("IncludeSamplings", includeSamplings).isSuccess());
    assert(tool->setProperty("InputName", caloCellName).isSuccess());
    assert(tool->setProperty("IsFindCellFast", isFindCellFast).isSuccess());
    assert(tool->initialize().isSuccess());
    return tool;
  }


  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<StoreGateSvc> m_detStore;
  DummyAlgorithm* m_alg;
  CaloHelper* m_caloHelper;
  CaloCell_ID* m_caloID;

  std::vector<IdentifierHash> m_tileGap3Hashes;
  std::vector<IdentifierHash> m_tileGap1Hashes;
  std::vector<IdentifierHash> m_allHashes;
};



void test1()
{
  std::cout << "test1\n";

  CaloCellFastCopyToolTest tester;
  tester.setUp();

  tester.testViewNotAvoidingDuplicatesFindCellIsNotFast();
  tester.testViewNotAvoidingDuplicatesFindCellIsFast();
  tester.testViewAvoidingDuplicatesFindCellIsNotFast();
  tester.testViewAvoidingDuplicatesFindCellIsFast();

  tester.testViewNotAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testViewNotAvoidingDuplicatesFindCellIsFastConst();
  tester.testViewAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testViewAvoidingDuplicatesFindCellIsFastConst();

  tester.testCloneNotAvoidingDuplicatesFindCellIsNotFast();
  tester.testCloneNotAvoidingDuplicatesFindCellIsFast();
  tester.testCloneAvoidingDuplicatesFindCellIsNotFast();
  tester.testCloneAvoidingDuplicatesFindCellIsFast();

  tester.testCloneNotAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testCloneNotAvoidingDuplicatesFindCellIsFastConst();
  tester.testCloneAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testCloneAvoidingDuplicatesFindCellIsFastConst();

  tester.testOverFullContainerFindCellIsNotFast();
  tester.testOverFullContainerFindCellIsFast();
}


int main (int /*argc*/, char** argv)
{
  Athena_test::setupStoreGate (argv[0]);
  test1();
  return 0;
}
