/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "TileRecAlgs/TileDigitsThresholdFilter.h"
#include "TileConditions/ITileCondToolDspThreshold.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"

#include "IdDictParser/IdDictParser.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/setupStoreGate.h"

// ATLAS C++
#include "CxxUtils/make_unique.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


static const float THRESHOLD = 5;

static const std::vector<std::vector<float> > TESTDIGITS = {
  {30, 30, 30, 30, 30, 30, 30}, // max - min = 0
  {50, 53, 52, 52, 52, 52, 52}, // max - min = 3
  {40, 44, 41, 41, 41, 41, 41}, // max - min = 5 
  {60, 66, 65, 65, 65, 65, 65}, // max - min = 6
  { 0, 70, 10, 10, 10, 10, 10}, // max - min = 70
};

class TileCablingSvc {
 public:
   static void init_idhelpers(IdDictParser& parser) {

     TileHWID* tileHWID = new TileHWID();
     TileID* tileID = new TileID();
     TileTBID* tileTBID = new TileTBID();

     tileID->set_do_neighbours (false);
     parser.register_external_entity ("TileCalorimeter", "IdDictTileCalorimeter.xml");
     IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
     assert (tileHWID->initialize_from_dictionary (idd) == 0);
     assert (tileTBID->initialize_from_dictionary (idd) == 0);
     assert (tileID->initialize_from_dictionary (idd) == 0);

     ServiceHandle<StoreGateSvc> detStore("DetectorStore", "");
     assert(detStore.retrieve().isSuccess());

     assert( (detStore->record(tileHWID, "TileHWID")).isSuccess() );
     assert( (detStore->record(tileTBID, "TileTBID")).isSuccess() );
     assert( (detStore->record(tileID, "TileID")).isSuccess() );

     TileCablingService* svc = TileCablingService::getInstance();
     svc->setTileHWID(tileHWID);
     svc->setTileTBID(tileTBID);
     svc->setTileID(tileID);
  }
};

class TileCondToolDspThresholdMock: public AthAlgTool, virtual public ITileCondToolDspThreshold {
  
  public:
  
    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_TileCondToolDspThresholdMock("TileCondToolDspThresholdMock", 1, 0);    
      return IID_TileCondToolDspThresholdMock;
    };
  
    TileCondToolDspThresholdMock(const std::string& type, const std::string& name, const IInterface* parent)
      : AthAlgTool(type, name, parent) 
    {
      declareInterface<ITileCondToolDspThreshold>(this);
    };
    virtual ~TileCondToolDspThresholdMock() {};
  
    StatusCode initialize() {return StatusCode::SUCCESS;};
    StatusCode finalize() {return StatusCode::SUCCESS;};
  
    float getMinimumAmplitudeThreshold(unsigned int /*drawerIdx*/, unsigned int /*channel*/, unsigned int /*adc*/) const {return 0;};
    float getMaximumAmplitudeThreshold(unsigned int /*drawerIdx*/, unsigned int /*channel*/, unsigned int /*adc*/) const {return 0;};
    void getAmplitudeThresholds(unsigned int /*drawerIdx*/, unsigned int /*channel*/, unsigned int /*adc*/,
                                float& /*minimumThreshold*/, float& /*maximumThreshold*/) const {};
    float getDspThreshold(unsigned int /*drawerIdx*/, unsigned int /*channel*/, unsigned int /*adc*/) const {return THRESHOLD;};

};


DECLARE_TOOL_FACTORY( TileCondToolDspThresholdMock )

void test1() {

  std::cout << "test1\n";

  ISvcLocator* svcLoc = Gaudi::svcLocator();

  IdDictParser parser;
  TileCablingSvc::init_idhelpers(parser);

  ServiceHandle<StoreGateSvc> evtStore("StoreGateSvc", "");
  ServiceHandle<StoreGateSvc> detStore("DetectorStore", "");

  assert(evtStore.retrieve().isSuccess());
  assert(detStore.retrieve().isSuccess());

  TileHWID* tileHWID(nullptr);
  assert( detStore->retrieve(tileHWID).isSuccess() );

  TileID* tileID(nullptr);
  assert( detStore->retrieve(tileID).isSuccess() );


  std::unique_ptr<TileDigitsThresholdFilter> alg = CxxUtils::make_unique<TileDigitsThresholdFilter>("TileDigitsThresholdFilterTest", svcLoc);
  assert( (alg->setProperty("TileCondToolDspThreshold", "TileCondToolDspThresholdMock/TileCondToolDspThresholdMock")).isSuccess() );
  assert( (alg->initialize()).isSuccess() );


  TileDigitsContainer* inputContainer = new TileDigitsContainer(true);

  inputContainer->set_unit(TileRawChannelUnit::OnlineADCcounts);
  inputContainer->set_type(TileFragHash::Beam);
  inputContainer->set_bsflags(3);

  unsigned int ros = 1;
  unsigned int drawer = 1;
  for (unsigned int adc = 0; adc < 2; ++adc) {
    unsigned int channel = 0;
    for (const std::vector<float>& digits : TESTDIGITS) {
      HWIdentifier id = tileHWID->adc_id(ros, drawer, channel, adc);
      inputContainer->push_back(new TileDigits(id, digits));
      ++channel;
      std::cout << ((tileHWID->is_tile(id) ? "TILE" : "NOT TILE")) << " => " << tileHWID->to_string(id) << std::endl;
    }
    ++drawer;
  }

  /*
  for (const TileDigitsCollection* constInputCollection : *inputContainer) {
    if (constInputCollection->identify() == 0x101) {
      TileDigitsCollection* inputCollection = const_cast<TileDigitsCollection*>(constInputCollection);
      inputCollection->setLvl1Id(10100);
      inputCollection->setLvl1Type(10101);
      inputCollection->setDetEvType(10102);
      inputCollection->setRODBCID(10103);
    }
  }
  */

  assert( evtStore->record(inputContainer, "TileDigitsCnt", false).isSuccess() );


  assert( (alg->execute()).isSuccess() );


  const TileDigitsContainer* outputContainer;
  assert( evtStore->retrieve(outputContainer, "TileDigitsFiltered").isSuccess() );

  assert( inputContainer->get_unit() == outputContainer->get_unit() );
  assert( inputContainer->get_type() == outputContainer->get_type() );
  assert( inputContainer->get_bsflags() == outputContainer->get_bsflags() );


  int nTileDigitsPassedFitler(0);
  for (const TileDigitsCollection* outputCollection : *outputContainer) {
    int fragId = outputCollection->identify();
    unsigned int drawer = (fragId & 0x3F);
    unsigned int ros = fragId >> 8;

    /*
    IdentifierHash fragHash = (inputContainer->hashFunc())(fragId);
    const TileDigitsCollection* inputCollection = *(inputContainer->indexFind(fragHash));


    assert( inputCollection->getLvl1Id() == outputCollection->getLvl1Id() );
    assert( inputCollection->getLvl1Type() == outputCollection->getLvl1Type() );
    assert( inputCollection->getDetEvType() == outputCollection->getDetEvType() );
    assert( inputCollection->getRODBCID() == outputCollection->getRODBCID() );

    assert( inputCollection->getFragSize() == outputCollection->getFragSize() );
    assert( inputCollection->getFragExtraWords() == outputCollection->getFragExtraWords() );
    assert( inputCollection->getFragBCID() == outputCollection->getFragBCID() );
    assert( inputCollection->getFragChipHeaderWords() == outputCollection->getFragChipHeaderWords() );
    assert( inputCollection->getFragChipHeaderWordsHigh() == outputCollection->getFragChipHeaderWordsHigh() );
    assert( inputCollection->getFragChipCRCWords() == outputCollection->getFragChipCRCWords() );
    assert( inputCollection->getFragChipCRCWordsHigh() == outputCollection->getFragChipCRCWordsHigh() );
    */

    for (const TileDigits* digits : *outputCollection) {
      ++nTileDigitsPassedFitler;
      HWIdentifier adc_id = digits->adc_HWID();
      int channel = tileHWID->channel(adc_id);
      unsigned int adc = tileHWID->adc(adc_id);

      assert(ros == 1);
      assert(drawer == adc + 1);
      assert( (channel == 3) || (channel == 4) );
      assert(digits->samples() == TESTDIGITS[channel]);

    }
  }

  assert(nTileDigitsPassedFitler == 4);

  // delete alg;

}


int main(int /*argc*/, char** argv) {

  Athena_test::setupStoreGate (argv[0]);

  test1();

  return 0;
}


// This package builds a component library, so we don't normally
// link against the library.  So just include this here.
#include "../src/TileDigitsThresholdFilter.cxx"
