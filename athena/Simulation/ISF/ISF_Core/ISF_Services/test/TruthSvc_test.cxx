/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date June, 2016
 * @brief Tests for ISF::TruthSvc.
 */

#undef NDEBUG

// Tested service
#include "../src/TruthSvc.h"

// Framework
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Framework testing
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"
// Google Mock
#include "gmock/gmock.h"

// ISF
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthIncident.h"
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

// STL includes
#include <cstdlib> // quick_exit


namespace ISFTesting {

  // Athena Tool to emulate a TruthStrategy
  class MockTruthStrategy : public extends<AthAlgTool, ISF::ITruthStrategy> {
  public:
    MockTruthStrategy(const std::string& type, const std::string& name, const IInterface* parent)
      : base_class(type,name,parent)
    { };

    virtual ~MockTruthStrategy() {};

    // mock method which will be called by tested code
    MOCK_CONST_METHOD1(pass, bool(ISF::ITruthIncident&));
  };

  DECLARE_TOOL_FACTORY( MockTruthStrategy )

  class DummyTruthIncident : public ISF::ITruthIncident {
  public:
    DummyTruthIncident(AtlasDetDescr::AtlasRegion geoID, unsigned short numChildren)
      : ISF::ITruthIncident(geoID, numChildren)
    { };

    virtual ~DummyTruthIncident() {};
    virtual const HepMC::FourVector&  position() const {return m_myPosition;};
    virtual int                       physicsProcessCategory() const { return 1;};
    virtual Barcode::PhysicsProcessCode physicsProcessCode() const {return 1;};
    virtual double                    parentP2() const {return 1.0;};
    virtual double                    parentPt2() const {return 1.0;};
    /** Return Ekin of the parent particle */
    virtual double                    parentEkin() const {return 1.0;};
    /** Return the PDG Code of the parent particle */
    virtual int                       parentPdgCode() const {return 1;};
    /** Return the parent particle as a HepMC particle type
        (only called for particles that will enter the HepMC truth event) */
    virtual HepMC::GenParticle*       parentParticle() const {return nullptr;};
    /** Return the barcode of the parent particle */
    virtual Barcode::ParticleBarcode  parentBarcode() const {return 1;};
    /** Return the extra barcode of the parent particle */
    virtual Barcode::ParticleBarcode  parentBCID() const {return 1;};
    /** Return a boolean whether or not the parent particle survives the incident */
    virtual bool                      parentSurvivesIncident() const {return false;};
    /** Return the parent particle after the TruthIncident vertex (and assign
        a new barcode to it) */
    virtual HepMC::GenParticle*       parentParticleAfterIncident(Barcode::ParticleBarcode) {return nullptr;};

    /** Return p^2 of the i-th child particle */
    virtual double                    childP2(unsigned short) const {return 1.0;};
    /** Return pT^2 of the i-th child particle */
    virtual double                    childPt2(unsigned short) const {return 1.0;};
    /** Return Ekin of the i-th child particle */
    virtual double                    childEkin(unsigned short) const {return 1.0;};
    /** Return the PDG Code of the i-th child particle */
    virtual int                       childPdgCode(unsigned short) const {return 1;};
    /** Return the i-th child as a HepMC particle type and assign the given
        Barcode to the simulator particle (only called for particles that will
        enter the HepMC truth event) */
    virtual HepMC::GenParticle*       childParticle(unsigned short,
                                                    Barcode::ParticleBarcode) const {return nullptr;};
    /** Update the properties of a child particle from a pre-defined
        interaction based on the properties of the ith child of the
        current TruthIncident (only used in quasi-stable particle
        simulation). */
    virtual HepMC::GenParticle*       updateChildParticle(unsigned short,
                                                          HepMC::GenParticle*) const {return nullptr;};
    /** Set the the barcode of all child particles to the given bc */
    virtual void                      setAllChildrenBarcodes(Barcode::ParticleBarcode) {};
  private:
    const HepMC::FourVector m_myPosition{0.0, 40.0, 0.0, 40.0};
  };

  class MockTruthIncident : public DummyTruthIncident {
  public:
    MockTruthIncident(AtlasDetDescr::AtlasRegion geoID, unsigned short numChildren)
      : DummyTruthIncident(geoID, numChildren)
    { };

    virtual ~MockTruthIncident() {};
    MOCK_METHOD0(geoID, AtlasDetDescr::AtlasRegion());
    //MOCK_CONST_METHOD0(position, HepMC::FourVector&());
    MOCK_CONST_METHOD0(physicsProcessCode, Barcode::PhysicsProcessCode());
    MOCK_CONST_METHOD0(parentParticle, HepMC::GenParticle*());
    MOCK_CONST_METHOD0(parentBarcode, Barcode::ParticleBarcode());
    MOCK_METHOD1(parentParticleAfterIncident, HepMC::GenParticle*(Barcode::ParticleBarcode));
    MOCK_METHOD1(setPassWholeVertices, void(bool));
    MOCK_CONST_METHOD0(numberOfChildren, unsigned short());
  };

  // class AllGoodMatcher : public ::testing::MatcherInterface<ISF::ITruthIncident> {
  // public:
  //   virtual bool MatchAndExplain(ISF::ITruthIncident n, ::testing::MatchResultListener* listener) const {
  //     return true;
  //   }

  //   virtual void DescribeTo(::std::ostream* os) const {
  //     *os << "is divisible by 7";
  //   }

  //   virtual void DescribeNegationTo(::std::ostream* os) const {
  //     *os << "is not divisible by 7";
  //   }
  // };

  // inline ::testing::Matcher<ISF::ITruthIncident> AllGood() {
  //   return ::testing::MakeMatcher(new AllGoodMatcher);
  // }

  class TruthSvc_test: public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_appMgr = Gaudi::createApplicationMgr();
      ASSERT_TRUE( m_appMgr!=nullptr );

      m_svcLoc = m_appMgr;
      ASSERT_TRUE( m_svcLoc.isValid() );

      m_svcMgr = m_appMgr;
      ASSERT_TRUE( m_svcMgr.isValid() );

      m_propMgr = m_appMgr;
      ASSERT_TRUE( m_propMgr.isValid() );
      ASSERT_TRUE( m_propMgr->setProperty( "EvtSel", "NONE" ).isSuccess() );
      ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsType", "FILE" ).isSuccess() );
      ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsPath", "TruthSvc_test.txt" ).isSuccess() );

      m_toolSvc = m_svcLoc->service("ToolSvc");
      ASSERT_TRUE( m_toolSvc.isValid() );

      ASSERT_TRUE( m_appMgr->configure().isSuccess() );
      ASSERT_TRUE( m_appMgr->initialize().isSuccess() );

      // the tested AthenaService
      const auto& truthSvcTypeAndName = "ISF::TruthSvc/TruthSvc";
      SmartIF<IService> svc = m_svcLoc->service(truthSvcTypeAndName);
      m_svc = dynamic_cast<ISF::TruthSvc*>(svc.get());
      ASSERT_NE(nullptr, m_svc);

      ASSERT_TRUE( m_svc->setProperties().isSuccess() );
      ASSERT_TRUE( m_svc->configure().isSuccess() );
    }

    virtual void TearDown() override {
      m_svcMgr->removeService(m_svc);
      ASSERT_TRUE( m_svc->finalize().isSuccess() );
      ASSERT_TRUE( m_svc->terminate().isSuccess() );
      ReleaseSmartIFComponent(m_svc);

      ASSERT_TRUE( m_appMgr->finalize().isSuccess() );
      ASSERT_TRUE( m_appMgr->terminate().isSuccess() );
      Gaudi::setInstance( static_cast<IAppMgrUI*>(nullptr)) ;
    }

    void ReleaseSmartIFComponent(IInterface* comp) {
      size_t finalRefCount = 1; // keep one reference for the SmartIF destructor
      for (size_t refCount = comp->refCount(); refCount>finalRefCount; refCount--) {
        comp->release();
      }
    }

    //
    // accessors for private methods
    // NB: This works because TruthSvc_test is a friend
    //     of the tested TruthSvc service
    //
    template<typename... Args>
    HepMC::GenVertex* createGenVertexFromTruthIncident(Args&&... args) const {
      return m_svc->createGenVertexFromTruthIncident(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void recordIncidentToMCTruth(Args&&... args) const {
      return m_svc->recordIncidentToMCTruth(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void registerTruthIncident(Args&&... args) const {
      return m_svc->registerTruthIncident(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void deleteChildVertex(Args&&... args) const {
      return m_svc->deleteChildVertex(std::forward<Args>(args)...);
    }

    ToolHandleArray<ISF::ITruthStrategy>& getIDTruthStrategies() const {
      return m_svc->m_geoStrategyHandles[AtlasDetDescr::fAtlasID];
    }

    // void forceEndVertexInID() const {
    //   m_svc->m_forceEndVtx[1] = true;
    // }

    // protected member variables
    //

    // Core Gaudi components
    IAppMgrUI*             m_appMgr = nullptr;
    SmartIF<ISvcLocator>   m_svcLoc;
    SmartIF<ISvcManager>   m_svcMgr;
    SmartIF<IToolSvc>      m_toolSvc;
    SmartIF<IProperty>     m_propMgr;

    ISF::TruthSvc*   m_svc; // the tested AthenaService

  };  // TruthSvc_test fixture


  TEST_F(TruthSvc_test, initialize_empty) {
    ASSERT_TRUE( m_svc->initialize().isSuccess() );
  }


  TEST_F(TruthSvc_test, createGenVertexFromTruthIncident) {
    ASSERT_TRUE( m_svc->initialize().isSuccess() );
    /// Create dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    const int pdgid1(-13);
    const int pdgid2(13);
    std::unique_ptr<HepMC::GenEvent> anEvent = std::make_unique<HepMC::GenEvent>(process_id1, event_number1);
    const CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    const HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, pdgid1, 2);
    myVertex->add_particle_in(inParticle1);
    const HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, pdgid2, 2);
    myVertex->add_particle_in(inParticle2);
    const HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
    myVertex->add_particle_out(inParticle3);
    const HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, pdgid2, 1);
    myVertex->add_particle_out(inParticle4);
    anEvent->add_vertex( myVertex );
    anEvent->set_signal_process_vertex( myVertex );
    anEvent->set_beam_particles(inParticle1,inParticle2);

    MockTruthIncident ti(AtlasDetDescr::fAtlasID, 2);
    HepMC::FourVector vtxPosition(0.0, 40.0, 0.0, 40.0);
    EXPECT_CALL(ti, physicsProcessCode())
      .Times(1)
      .WillOnce(::testing::Return(21));
    EXPECT_CALL(ti, parentBarcode())
      .Times(1)
      .WillOnce(::testing::Return(inParticle3->barcode()));
    EXPECT_CALL(ti, parentParticle())
      .Times(1)
      .WillOnce(::testing::Return(inParticle3));
    HepMC::GenVertex *generated = createGenVertexFromTruthIncident(ti);

    ASSERT_EQ( vtxPosition, generated->position() );
    ASSERT_EQ( 1021, generated->id() );
    ASSERT_EQ( -2, generated->barcode() );
    ASSERT_EQ( 1, generated->particles_in_size());
    ASSERT_EQ( 0, generated->particles_out_size());
    ASSERT_EQ( inParticle3, *(generated->particles_in_const_begin()));
  }


  TEST_F(TruthSvc_test, recordIncidentToMCTruth_noChildParticles) {
    ASSERT_TRUE( m_svc->initialize().isSuccess() );
    /// Create dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    const int pdgid1(-13);
    const int pdgid2(13);
    std::unique_ptr<HepMC::GenEvent> anEvent = std::make_unique<HepMC::GenEvent>(process_id1, event_number1);
    const CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    const HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, pdgid1, 2);
    myVertex->add_particle_in(inParticle1);
    const HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, pdgid2, 2);
    myVertex->add_particle_in(inParticle2);
    const HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
    myVertex->add_particle_out(inParticle3);
    const HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, pdgid2, 1);
    myVertex->add_particle_out(inParticle4);
    anEvent->add_vertex( myVertex );
    anEvent->set_signal_process_vertex( myVertex );
    anEvent->set_beam_particles(inParticle1,inParticle2);

    MockTruthIncident ti(AtlasDetDescr::fAtlasID, 2);
    HepMC::FourVector vtxPosition(0.0, 40.0, 0.0, 40.0);
    EXPECT_CALL(ti, physicsProcessCode())
      .Times(2)
      .WillOnce(::testing::Return(21))
      .WillOnce(::testing::Return(21));
    EXPECT_CALL(ti, parentBarcode())
      .Times(2)
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()));
    EXPECT_CALL(ti, parentParticle())
      .Times(2)
      .WillOnce(::testing::Return(inParticle3))
      .WillOnce(::testing::Return(inParticle3));
    EXPECT_CALL(ti, parentParticleAfterIncident(1010003))
      .Times(1)
      .WillOnce(::testing::Return(nullptr));

    recordIncidentToMCTruth(ti);
    HepMC::GenVertex* generated = anEvent->barcode_to_vertex(-2); //Find a nicer way to get this.
    ASSERT_EQ( vtxPosition, generated->position() );
    ASSERT_EQ( 1021, generated->id() );
    ASSERT_EQ( -2, generated->barcode() ); // by construction at the moment
    ASSERT_EQ( 1, generated->particles_in_size());
    ASSERT_EQ( 0, generated->particles_out_size());
    ASSERT_EQ( inParticle3, *(generated->particles_in_const_begin()));
  }


  TEST_F(TruthSvc_test, registerTruthIncident_noStrat_noForceEndVtx) {
    ASSERT_TRUE( m_svc->initialize().isSuccess() );
    /// Create dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    const int pdgid1(-13);
    const int pdgid2(13);
    std::unique_ptr<HepMC::GenEvent> anEvent = std::make_unique<HepMC::GenEvent>(process_id1, event_number1);
    const CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    const HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, pdgid1, 2);
    myVertex->add_particle_in(inParticle1);
    const HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, pdgid2, 2);
    myVertex->add_particle_in(inParticle2);
    const HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
    myVertex->add_particle_out(inParticle3);
    const HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, pdgid2, 1);
    myVertex->add_particle_out(inParticle4);
    anEvent->add_vertex( myVertex );
    anEvent->set_signal_process_vertex( myVertex );
    anEvent->set_beam_particles(inParticle1,inParticle2);

    MockTruthIncident ti(AtlasDetDescr::fAtlasID, 2);
    EXPECT_CALL(ti, physicsProcessCode())
      .Times(1)
      .WillOnce(::testing::Return(21));
    EXPECT_CALL(ti, parentBarcode())
      .Times(2)
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()));

    registerTruthIncident(ti);
    HepMC::GenVertex* generated = anEvent->barcode_to_vertex(-2); //Find a nicer way to get this.
    HepMC::GenVertex* expectedVtx(nullptr);
    ASSERT_EQ( expectedVtx, generated);
  }


  TEST_F(TruthSvc_test, registerTruthIncident_noStrat_ForceEndVtx) {
    bool forceEndVtx[AtlasDetDescr::fNumAtlasRegions] = {true, true, true, true, true, true};
    ASSERT_TRUE( m_svc->setProperty( "ForceEndVtxInRegions", forceEndVtx ).isSuccess() );
    ASSERT_TRUE( m_svc->initialize().isSuccess() );
    /// Create dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    const int pdgid1(-13);
    const int pdgid2(13);
    std::unique_ptr<HepMC::GenEvent> anEvent = std::make_unique<HepMC::GenEvent>(process_id1, event_number1);
    const CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    const HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, pdgid1, 2);
    myVertex->add_particle_in(inParticle1);
    const HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, pdgid2, 2);
    myVertex->add_particle_in(inParticle2);
    const HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
    myVertex->add_particle_out(inParticle3);
    const HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, pdgid2, 1);
    myVertex->add_particle_out(inParticle4);
    anEvent->add_vertex( myVertex );
    anEvent->set_signal_process_vertex( myVertex );
    anEvent->set_beam_particles(inParticle1,inParticle2);

    MockTruthIncident ti(AtlasDetDescr::fAtlasID, 2);
    HepMC::FourVector vtxPosition(0.0, 40.0, 0.0, 40.0);
    EXPECT_CALL(ti, physicsProcessCode())
      .Times(2)
      .WillOnce(::testing::Return(21))
      .WillOnce(::testing::Return(21));
    EXPECT_CALL(ti, parentBarcode())
      .Times(3)
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()));
    EXPECT_CALL(ti, parentParticle())
      .Times(1)
      .WillOnce(::testing::Return(inParticle3));

    registerTruthIncident(ti);
    HepMC::GenVertex* generated = anEvent->barcode_to_vertex(-2); //Find a nicer way to get this.
    ASSERT_EQ( vtxPosition, generated->position() );
    ASSERT_EQ( 1021, generated->id() );
    ASSERT_EQ( -2, generated->barcode() ); // by construction at the moment
    ASSERT_EQ( 1, generated->particles_in_size());
    ASSERT_EQ( 0, generated->particles_out_size());
    ASSERT_EQ( inParticle3, *(generated->particles_in_const_begin()));
  }


  TEST_F(TruthSvc_test, registerTruthIncident_failMockStrat_ForceEndVtx) {
    bool forceEndVtx[AtlasDetDescr::fNumAtlasRegions] = {true, true, true, true, true, true};
    ASSERT_TRUE( m_svc->setProperty( "ForceEndVtxInRegions", forceEndVtx ).isSuccess() );
    // retrieve mockable TruthStrategy tool and point InputConverter to the same instance
    m_svc->setProperty("IDTruthStrategies", "['ISFTesting::MockTruthStrategy/DummyTruthStrategy']");
    ASSERT_TRUE( m_svc->initialize().isSuccess() );
    ToolHandleArray<ISF::ITruthStrategy>& idTruthStrategies = getIDTruthStrategies();
    const unsigned int expectedSize(1);
    ASSERT_EQ (idTruthStrategies.size(), expectedSize);
    MockTruthStrategy* truthStrategy = dynamic_cast<MockTruthStrategy*>(&*(idTruthStrategies[0]));
    ASSERT_TRUE( truthStrategy );

    /// Create dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    const int pdgid1(-13);
    const int pdgid2(13);
    std::unique_ptr<HepMC::GenEvent> anEvent = std::make_unique<HepMC::GenEvent>(process_id1, event_number1);
    const CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    const HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, pdgid1, 2);
    myVertex->add_particle_in(inParticle1);
    const HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, pdgid2, 2);
    myVertex->add_particle_in(inParticle2);
    const HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
    myVertex->add_particle_out(inParticle3);
    const HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, pdgid2, 1);
    myVertex->add_particle_out(inParticle4);
    anEvent->add_vertex( myVertex );
    anEvent->set_signal_process_vertex( myVertex );
    anEvent->set_beam_particles(inParticle1,inParticle2);


    MockTruthIncident ti(AtlasDetDescr::fAtlasID, 2);
    HepMC::FourVector vtxPosition(0.0, 40.0, 0.0, 40.0);
    EXPECT_CALL(ti, physicsProcessCode())
      .Times(2)
      .WillOnce(::testing::Return(21))
      .WillOnce(::testing::Return(21));
    EXPECT_CALL(ti, parentBarcode())
      .Times(3)
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()));
    EXPECT_CALL(ti, parentParticle())
      .Times(1)
      .WillOnce(::testing::Return(inParticle3));
    // _ is a matcher where the argument can be any value of the
    // correct type. Needed because the argument to the pass method is
    // a reference to an abstract interface class (ITruthIncident).
    EXPECT_CALL(*truthStrategy, pass(::testing::_))
      .Times(1)
      .WillOnce(::testing::Return(false));

    registerTruthIncident(ti);
    HepMC::GenVertex* generated = anEvent->barcode_to_vertex(-2); //Find a nicer way to get this.
    ASSERT_EQ( vtxPosition, generated->position() );
    ASSERT_EQ( 1021, generated->id() );
    ASSERT_EQ( -2, generated->barcode() ); // by construction at the moment
    ASSERT_EQ( 1, generated->particles_in_size());
    ASSERT_EQ( 0, generated->particles_out_size());
    ASSERT_EQ( inParticle3, *(generated->particles_in_const_begin()));
  }


  TEST_F(TruthSvc_test, registerTruthIncident_passMockStrat_parentSurvives) {
    // retrieve mockable TruthStrategy tool and point InputConverter to the same instance
    m_svc->setProperty("IDTruthStrategies", "['ISFTesting::MockTruthStrategy/DummyTruthStrategy']");
    ASSERT_TRUE( m_svc->initialize().isSuccess() );
    ToolHandleArray<ISF::ITruthStrategy>& idTruthStrategies = getIDTruthStrategies();
    const unsigned int expectedSize(1);
    ASSERT_EQ (idTruthStrategies.size(), expectedSize);
    MockTruthStrategy* truthStrategy = dynamic_cast<MockTruthStrategy*>(&*(idTruthStrategies[0]));
    ASSERT_TRUE( truthStrategy );

    /// Create dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    const int pdgid1(-13);
    const int pdgid2(13);
    std::unique_ptr<HepMC::GenEvent> anEvent = std::make_unique<HepMC::GenEvent>(process_id1, event_number1);
    const CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    const HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, pdgid1, 2);
    myVertex->add_particle_in(inParticle1);
    const HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, pdgid2, 2);
    myVertex->add_particle_in(inParticle2);
    const HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
    myVertex->add_particle_out(inParticle3);
    const HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, pdgid2, 1);
    myVertex->add_particle_out(inParticle4);
    anEvent->add_vertex( myVertex );
    anEvent->set_signal_process_vertex( myVertex );
    anEvent->set_beam_particles(inParticle1,inParticle2);
    HepMC::GenParticle* inParticle5 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
    inParticle5->suggest_barcode(1010003);
    inParticle5->set_status(1);

    MockTruthIncident ti(AtlasDetDescr::fAtlasID, 2);
    HepMC::FourVector vtxPosition(0.0, 40.0, 0.0, 40.0);
    EXPECT_CALL(ti, physicsProcessCode())
      .Times(2)
      .WillOnce(::testing::Return(21))
      .WillOnce(::testing::Return(21));
    EXPECT_CALL(ti, parentBarcode())
      .Times(3)
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()))
      .WillOnce(::testing::Return(inParticle3->barcode()));
    EXPECT_CALL(ti, parentParticle())
      .Times(2)
      .WillOnce(::testing::Return(inParticle3))
      .WillOnce(::testing::Return(inParticle3));
    EXPECT_CALL(ti, parentParticleAfterIncident(1010003))
      .Times(1)
      .WillOnce(::testing::Return(inParticle5));
    // _ is a matcher where the argument can be any value of the
    // correct type. Needed because the argument to the pass method is
    // a reference to an abstract interface class (ITruthIncident).
    EXPECT_CALL(*truthStrategy, pass(::testing::_))
      .Times(1)
      .WillOnce(::testing::Return(true));

    registerTruthIncident(ti);
    HepMC::GenVertex* generated = anEvent->barcode_to_vertex(-2); //Find a nicer way to get this.
    ASSERT_EQ( vtxPosition, generated->position() );
    ASSERT_EQ( 1021, generated->id() );
    ASSERT_EQ( -2, generated->barcode() ); // by construction at the moment
    ASSERT_EQ( 1, generated->particles_in_size());
    ASSERT_EQ( inParticle3, *(generated->particles_in_const_begin()));
    ASSERT_EQ( 1, generated->particles_out_size());
    ASSERT_EQ( inParticle5, *(generated->particles_out_const_begin()));
  }

  //TODO Add tests for the following cases:
  // 1) parent destroyed, all child particles pass cuts
  // 2) parent destroyed, some child particles pass cuts
  // 3) parent survives, all child particles pass cuts
  // 4) parent survives, some child particles pass cuts
  // 5) quasi-stable particle, pre-defined decay
  // 6) quasi-stable particle, additional interaction, survives, no child particles pass cuts
  // 7) quasi-stable particle, additional interaction, survives, some child particles pass cuts
  // 8) quasi-stable particle, additional interaction, destroyed, no child particles pass cuts
  // 9) quasi-stable particle, additional interaction, destroyed, some child particles pass cuts

} // <-- namespace ISFTesting

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest( &argc, argv );

  // gets stuck forever while trying to finalize boost crap inside SGTools:
  //return RUN_ALL_TESTS();
  // skips proper finalization:
  std::quick_exit( RUN_ALL_TESTS() );
}

// // retrieve mockable GenParticleFilter tool and point InputConverter to the same instance
// m_svc->setProperty("GenParticleFilters", "['ISFTesting::MockFilterTool/DummyFilter']");
// ASSERT_TRUE( m_svc->initialize().isSuccess() );
