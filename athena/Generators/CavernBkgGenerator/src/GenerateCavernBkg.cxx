/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CavernBkgGenerator/GenerateCavernBkg.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"

#include "HepPDT/ParticleData.hh"
#include "CLHEP/Random/RandFlat.h"

#include <cassert>
using namespace std;


// Random number generator stream name and static pointer to the RNG engine
namespace { const string CAVBKG_RANDOM_STREAM = "CAVERN_BKG"; }
CLHEP::HepRandomEngine* GenerateCavernBkg::CAVBKG_RANDOM_ENGINE = 0;
extern "C" float cav_rndm_( int* /*idummy*/ ) {
  return CLHEP::RandFlat::shoot(GenerateCavernBkg::CAVBKG_RANDOM_ENGINE);
}
extern "C" {
  void cavbkg_(const char*, int);
  void* getaddr_(void* arg) { return(arg); }
}


GenerateCavernBkg::GenerateCavernBkg(const string& name, ISvcLocator* pSvcLocator)
  : GenModule(name, pSvcLocator), m_mcEvtColl(0)
{
  declareProperty("InputFile",  m_infile="fvpn21n.dat");
}


StatusCode
GenerateCavernBkg::genInitialize() {
  CAVBKG_RANDOM_ENGINE = atRndmGenSvc().GetEngine(CAVBKG_RANDOM_STREAM);
  return StatusCode::SUCCESS;
}


StatusCode
GenerateCavernBkg::callGenerator() {
  ATH_MSG_INFO("GenerateCavernBkg GENERATING.");

  // Get the random number seeds
  assert(CAVBKG_RANDOM_ENGINE != 0);
  const long* s = CAVBKG_RANDOM_ENGINE->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);
  ATH_MSG_DEBUG("Random seeds: " << m_seeds[0] << ", " << m_seeds[1]);

  // Generate event
  cavbkg_(m_infile.c_str(), strlen(m_infile.c_str()));

  return StatusCode::SUCCESS;
}


StatusCode
GenerateCavernBkg::fillEvt(HepMC::GenEvent* evt) {
  ATH_MSG_INFO("GenerateCavernBkg FILLING.");

  // Set event properties
  evt->set_signal_process_id(-1);
  evt->set_event_number(m_bkgout.ievent());
  evt->set_random_states(m_seeds);

  for (int i = 1; i <= m_bkgout.npv(); ++i) {
    const CLHEP::HepLorentzVector position(m_bkgout.vx(i)*10., // CLHEP::cm to CLHEP::mm
                                           m_bkgout.vy(i)*10., // CLHEP::cm to CLHEP::mm
                                           m_bkgout.vz(i)*10., // CLHEP::cm to CLHEP::mm
                                           m_bkgout.ptof(i)*299.792458); // CLHEP::ns to CLHEP::mm
    HepMC::GenVertex* v = new HepMC::GenVertex(position, m_bkgout.irp(i));

    // Get the mass from the particle property service
    const HepPDT::ParticleData* particle = particleData(abs(m_bkgout.ityp(i)));
    const double mass = particle->mass().value();

    const double px = m_bkgout.px(i)*1000.; // CLHEP::GeV to CLHEP::MeV
    const double py = m_bkgout.py(i)*1000.; // CLHEP::GeV to CLHEP::MeV
    const double pz = m_bkgout.pz(i)*1000.; // CLHEP::GeV to CLHEP::MeV
    const double ener = sqrt(px*px + py*py + pz*pz + mass*mass);
    const CLHEP::HepLorentzVector momentum(px, py, pz, ener);
    HepMC::GenParticle* p = new HepMC::GenParticle(momentum, m_bkgout.ityp(i), 1);

    v->add_particle_out(p);
    evt->add_vertex(v);
  }
  ATH_MSG_DEBUG("IEVENT, NPV " << m_bkgout.ievent() << ", " <<  m_bkgout.npv());

  return StatusCode::SUCCESS;
}


StatusCode
GenerateCavernBkg::genFinalize() {

  ATH_MSG_INFO("GenerateCavernBkg ENDING.");
  return StatusCode::SUCCESS;

}
