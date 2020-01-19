/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @purpose: test the TruthParticle class
/// @author : S.Binet<binet@cern.ch>

// STL includes
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>

// Framework includes
#ifndef NOGAUDI
#include "TestTools/initGaudi.h"
#endif //NOGAUDI

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "GeneratorObjects/HepMcParticleLink.h"

// McParticleEvent includes
#define private public
#define protected public
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthEtIsolations.h"
#undef private
#undef protected

class tp_test_err
  : public std::exception
{
public:
  explicit tp_test_err (const char* file,
                        int line,
                        const char* what);
  virtual ~tp_test_err() throw() {}
  virtual const char* what() const throw() { return m_what.c_str(); }
private:
  std::string m_what;
};


tp_test_err::tp_test_err (const char* file,
                          int line,
                          const char* what)
{
  std::ostringstream os;
  os << file << ":" << line << " myassertion failure: " << what;
  m_what = os.str();
}


void throw_tp_test_err (const char* file, int line, const char* what)
{
  throw (tp_test_err (file, line, what));
}

#define myassert(X) do {     \
    if (!(X)) { \
      throw_tp_test_err (__FILE__, __LINE__, #X); \
    } \
  } while (0)

#define TP_ASSERT(X) myassert(X)

typedef CLHEP::HepLorentzVector HLV_t;
typedef TruthParticleContainer::Map_t Map_t;
typedef TruthEtIsolations::EtIsol_t EtIsol_t;

// helper to fill the TruthParticleContainer internal map[extended-bc -> truthpart]
//  note that it is only working for the case where there is only 1 GenEvent...
Map_t::value_type 
make_map_t_pair(const HepMC::GenParticle &p,
                const TruthParticle &tp)
{
  const std::size_t genEventIdx = 0;
  HepMcParticleLink link(p.barcode(), genEventIdx);
  return Map_t::value_type(link.compress(), &tp);
}

class TruthParticleTest
{
public:

  HepMC::GenEvent * m_evt;
  HepMC::GenVertex * m_vtx;
  HepMC::GenParticle * m_top;
  HepMC::GenParticle * m_w;
  HepMC::GenParticle * m_b;
  HepMC::GenParticle * m_g1;
  HepMC::GenParticle * m_g2;

  unsigned int m_nPartsIn;
  unsigned int m_nPartsOut;

  unsigned int m_nCones;
  EtIsol_t m_etIsols;

  double            m_epsilon;

  TruthParticleContainer * m_mc;
};

TruthParticleTest* makeTestData()
{
  TruthParticleTest * test = new TruthParticleTest;

  const int signalProcessId = 1000082;
  const int evtNbr = 1;
  HepMC::GenEvent * evt = new HepMC::GenEvent( signalProcessId, evtNbr );
  test->m_evt = evt;

  evt->set_event_scale( -1 );
  evt->set_alphaQCD( -1 );
  evt->set_alphaQED( -1 );

  std::vector<double> weights(3);
  weights[0] = 1;
  weights[1] = 1;
  weights[2] = 1;
  std::vector<long> rdmStates(2);
  rdmStates[0] = 85909879;
  rdmStates[1] = 9707499;
  evt->weights() = weights;
  evt->set_random_states( rdmStates );
    
  // Add a t->W+bgg
  HepMC::GenVertex * vtx = new HepMC::GenVertex;

  evt->add_vertex( vtx );
  // top
  HepMC::GenParticle * top = 0;
  top = new HepMC::GenParticle( HLV_t(-2.35e+05,
				      +7.34e+04,
				      +3.60e+04,
				      +3.04e+05),
				6, 3 );
  vtx->add_particle_in( top );
  

  // Wbgg
  HepMC::GenParticle * w = 0;
  w = new HepMC::GenParticle( HLV_t(-1.09e+05,
				    +6.99e+04,
				    -3.86e+04,
				    +1.57e+05),
			      24, 2 ) ;
  vtx->add_particle_out(w);

  HepMC::GenParticle * b = 0;
  b = new HepMC::GenParticle( HLV_t(-9.23e+04,
				    +2.54e+03,
				    +5.32e+04,
				    +1.07e+05),
			      5, 2 );
  vtx->add_particle_out(b);

  HepMC::GenParticle * g1 = 0;
  g1 = new HepMC::GenParticle( HLV_t(-4.76e+03,
				     +6.72e+02,
				     +2.90e+03,
				     +5.62e+03),
			       21, 2 );
  vtx->add_particle_out( g1 );

  HepMC::GenParticle * g2 = 0;
  g2 = new HepMC::GenParticle( HLV_t(-2.93e+04,
				     +2.13e+02,
				     +1.85e+04,
				     +3.46e+04),
			       21, 2 );
  vtx->add_particle_out( g2 );

  McEventCollection * genEvt = new McEventCollection;
  genEvt->push_back( evt );

  // filling Data test members
  test->m_evt        = evt;
  test->m_vtx        = vtx;
  test->m_top        = top;
  test->m_w          = w;
  test->m_b          = b;
  test->m_g1         = g1;
  test->m_g2         = g2;
  // we subtract one because we don't account for the top
  test->m_nPartsIn   = vtx->particles_in_size() - 1; 
  test->m_nPartsOut  = vtx->particles_out_size();

  test->m_nCones = TruthParticleParameters::NbrOfCones;
  for ( std::size_t i = 0; 
	i != static_cast<std::size_t>(test->m_nCones); 
	++i ) {
    test->m_etIsols[i] = 0.*CLHEP::GeV;
  }

  TruthParticleContainer * mc = new TruthParticleContainer;
  ElementLink<McEventCollection> genEvtLink( *genEvt, 0 );
  mc->setGenEvent( genEvtLink );
  
  test->m_mc = mc;
  test->m_epsilon = 1e-10;

  return test;
}

void testConstructor( TruthParticleTest* tp )
{
  {
    TruthParticle mc;

    // FourMomentum
    TP_ASSERT( mc.px () == 0 );
    TP_ASSERT( mc.py () == 0 );
    TP_ASSERT( mc.pz () == 0 );
    TP_ASSERT( mc.e  () == 0 );
    
    // TruthParticle-part
    TP_ASSERT( mc.genParticle() == 0   );
    TP_ASSERT( mc.m_container == 0   );
    TP_ASSERT( mc.m_mothers.empty()  );
    TP_ASSERT( mc.m_children.empty() );
  }

  {
    TruthParticle mc( tp->m_top, tp->m_mc );
    // FourMomentum
    TP_ASSERT( mc.px () == tp->m_top->momentum().px() );
    TP_ASSERT( mc.py () == tp->m_top->momentum().py() );
    TP_ASSERT( mc.pz () == tp->m_top->momentum().pz() );
    TP_ASSERT( mc.e  () == tp->m_top->momentum().e () );
    
    // TruthParticle-part
    TP_ASSERT( mc.genParticle() == tp->m_top );
    TP_ASSERT( mc.m_container == tp->m_mc  );
    TP_ASSERT( mc.m_mothers.size()  == tp->m_nPartsIn  );
    TP_ASSERT( mc.m_children.size() == tp->m_nPartsOut );
  }

  return;
}

void testCopyConstructor( TruthParticleTest* tp )
{
  TruthParticle top( tp->m_top, tp->m_mc );
  {
    TruthParticle mc(top);
    // FourMomentum
    TP_ASSERT( std::abs( mc.px () - top.px() ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.py () - top.py() ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.pz () - top.pz() ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.e  () - top.e()  ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.m()   - top.m()  ) < tp->m_epsilon );
    TP_ASSERT( mc.hlv().isNear(top.hlv(), tp->m_epsilon) );

    // TruthParticle-part
    TP_ASSERT( mc.genParticle() == top.genParticle() );
    TP_ASSERT( mc.m_container == top.m_container  );
    TP_ASSERT( mc.m_mothers.size()  == top.m_mothers.size()  );
    TP_ASSERT( mc.m_children.size() == top.m_children.size() );
  }
  return;
}

void testSettersAndGetters( TruthParticleTest* tp )
{
  const std::size_t refWIdx = 0;
  const std::size_t refBIdx = 1;
  const std::size_t refGluon1Idx = 2;
  const std::size_t refGluon2Idx = 3;
  const std::size_t refTopIdx = 0;

  TruthParticle top( tp->m_top, tp->m_mc );
  TruthParticle w  ( tp->m_w,   tp->m_mc );
  TruthParticle b  ( tp->m_b,   tp->m_mc );
  TruthParticle g1 ( tp->m_g1,  tp->m_mc );
  TruthParticle g2 ( tp->m_g2,  tp->m_mc );
  {
    TruthParticle mc( tp->m_w, tp->m_mc );
    Map_t parts;
    parts.insert( make_map_t_pair( *tp->m_w,   mc  ) );
    parts.insert( make_map_t_pair( *tp->m_top, top ) );
    tp->m_mc->setParticles( parts );

    TP_ASSERT( mc.genMother()          == tp->m_top );
    TP_ASSERT( mc.genMother(refTopIdx) == tp->m_top );
    TP_ASSERT( mc.mother()             == &top );
    TP_ASSERT( mc.mother(refTopIdx)    == &top );

    bool caught = false;
    try {
      mc.genChild(0);
    } catch ( std::out_of_range& err ) {
      caught = true;
    }
    TP_ASSERT( caught );
    TP_ASSERT( mc.child(0) == 0 );

    TP_ASSERT( mc.hasMother( PDG::t ) );
    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasMother( PDG::t, indices ) );
      TP_ASSERT( indices.size() == 1 );
      TP_ASSERT( indices[0]     == 0 );
    }
  }
  {
    TruthParticle mc( tp->m_top, tp->m_mc );
    Map_t parts;
    parts.insert( make_map_t_pair( *tp->m_top, mc ) );
    parts.insert( make_map_t_pair( *tp->m_w,   w  ) );
    parts.insert( make_map_t_pair( *tp->m_b,   b  ) );
    parts.insert( make_map_t_pair( *tp->m_g1,  g1 ) );
    parts.insert( make_map_t_pair( *tp->m_g2,  g2 ) );
    tp->m_mc->setParticles( parts );

    bool caught = false;
    try {
      mc.genMother(0);
    } catch ( std::out_of_range& err ) {
      caught = true;
    }
    TP_ASSERT( caught );
    TP_ASSERT( mc.mother( ) == 0 );
    TP_ASSERT( mc.mother(0) == 0 );

    TP_ASSERT( *mc.genParticle() == *tp->m_top );

    {
      // testing automatic cast to GenParticle
      const HepMC::GenParticle& hepMc = mc;
      TP_ASSERT( hepMc == *tp->m_top );
    }

    for ( unsigned int i = 0; i != mc.m_children.size(); ++i ) {
      caught = false;
      try {
	TP_ASSERT( mc.genChild(i) != 0 );
      } catch ( std::out_of_range& err ) {
	caught = true;
      }
      TP_ASSERT( !caught );
    }

    for ( unsigned int i = 0; i != mc.m_children.size(); ++i ) {
      caught = false;
      try {
	TP_ASSERT( mc.child(i) != 0 );
      } catch ( std::out_of_range& err ) {
	caught = true;
      }
      TP_ASSERT( !caught );
    }

    TP_ASSERT( mc.status() == tp->m_top->status() );
    TP_ASSERT( mc.hasChild( PDG::b ) );
    TP_ASSERT( mc.hasChild( PDG::W_plus ) );
    TP_ASSERT( mc.hasChild( PDG::g ) );

    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasChild( PDG::W_plus, indices ) );
      TP_ASSERT( indices.size() == 1 );
      TP_ASSERT( indices[0]     == refWIdx );
      for ( unsigned int i = 0; i != indices.size(); ++i ) {
	std::cout << "indices[" << i << "]= " << indices[i] << std::endl;
      }
      for ( unsigned int i = 0; i != mc.m_children.size(); ++i ) {
	TP_ASSERT( mc.genChild(i) != 0 );
	TP_ASSERT( mc.child(i)    != 0 );
	std::cout << "child[" << i << "]= " 
		  << mc.genChild(i)->pdg_id()
		  << "\t"
		  << mc.child(i)->pdgId() 
		  << std::endl;
	TP_ASSERT( mc.child(i)->pdgId() == mc.genChild(i)->pdg_id() );
      }
    }
    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasChild( PDG::b, indices ) );
      TP_ASSERT( indices.size() == 1 );
      TP_ASSERT( indices[0]     == refBIdx );
    }
    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasChild( PDG::g, indices ) );
      TP_ASSERT( indices.size() == 2 );
      TP_ASSERT( indices[0]     == refGluon1Idx );
      TP_ASSERT( indices[1]     == refGluon2Idx );
    }
      
    caught = false;
    try {
      double etIsol = mc.etIsol(TruthParticleParameters::NbrOfCones);
      if ( etIsol > 0. ) {
	// dummy
      }
    } catch ( std::out_of_range& err ) {
      caught = true;
    }
    TP_ASSERT( caught );
    CLHEP::HepLorentzVector mw_momentum_lv(tp->m_w->momentum().px(),
				 tp->m_w->momentum().py(),
				 tp->m_w->momentum().pz(),
				 tp->m_w->momentum().e());

    TP_ASSERT( mc.status()       == tp->m_top->status() );
    TP_ASSERT( mc.flow()         == tp->m_top->flow() );
    TP_ASSERT( mc.polarization() == tp->m_top->polarization() );
    TP_ASSERT( mc.barcode()      == tp->m_top->barcode() );

    TP_ASSERT( mc.nParents()        == tp->m_nPartsIn  );
    TP_ASSERT( mc.nDecay()          == tp->m_nPartsOut );
    TP_ASSERT( mc.pDecay(refWIdx)   == mw_momentum_lv );
    TP_ASSERT( mc.pdgDecay(refWIdx) == tp->m_w->pdg_id() );

    std::vector<double> etIsols;
    for ( unsigned int i = 0; 
	  i != static_cast<unsigned int>(TruthParticleParameters::NbrOfCones);
	  ++i ) {
      etIsols.push_back( i*2.*CLHEP::GeV );
    }
//     TP_ASSERT( mc.etIsol() == tp->m_etIsols );

//     mc.setEtIsol( etIsols );
//     TP_ASSERT( mc.etIsol() == etIsols );

//     mc.setEtIsol( TruthParticleParameters::etcone, -300.*CLHEP::GeV );
//     TP_ASSERT( mc.etIsol(TruthParticleParameters::etcone) == -300.*CLHEP::GeV );
  }
  
  return;
}

void test( TruthParticleTest* tp )
{
  testConstructor(tp);
  testCopyConstructor(tp);
  testSettersAndGetters(tp);
  return;
}

int main()
{
  // First instantiates a stripped version of Gaudi
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("TruthParticle_test.txt", pSvcLoc)) {
    std::cerr << " This test cannot be run without init Gaudi" << std::endl;
  }
  assert(pSvcLoc);

  // Load an instance of StoreGateSvc because we'll need it to store objects
  IService * pStoreGate = 0;
  static const bool CREATE(true);
  bool sc = ( pSvcLoc->service( "StoreGateSvc", 
				 pStoreGate, CREATE) ).isSuccess();
  assert( sc );
  assert( 0 != pStoreGate );

  std::cout << " *** TruthParticle test in progress: " << std::endl;
  
  std::cout << "Build test data..." << std::endl;

  TruthParticleTest * truthParticleTest = makeTestData();
  std::cout << "Launching tests..." << std::endl;
  test(truthParticleTest);

  std::cout << "Done [OK]" << std::endl;
  return EXIT_SUCCESS;
}
