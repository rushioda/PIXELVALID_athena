/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaTruthAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Select egtruth particles.
 */


#include "egammaTruthAlg.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"
#include "EventKernel/PdtPdg.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/SystemOfUnits.h"


using Gaudi::Units::GeV;
using Gaudi::Units::MeV;


namespace {


bool isGenStable (const xAOD::TruthParticle& tp)
{
  // get generator id 
  int p_id = tp.pdgId();

  const xAOD::TruthVertex* vertex = tp.hasDecayVtx() ? tp.decayVtx() : 0;
  // we want to keep primary particle with status==2 but without vertex in HepMC
  int vertex_barcode=-999999;
  if (vertex) vertex_barcode=vertex->barcode();

  return (
          ( ( tp.status()%1000 == 1) ||  
            (tp.status()==2 && vertex_barcode<-200000) || 
            (tp.status()%1000 == 2 && tp.status() > 1000) 
            ) && (tp.barcode()<200000) 
          && !(abs(p_id) == 21 && tp.e()==0)
          ) ? true:false;    
}


bool isGenInteracting (const xAOD::TruthParticle& tp)
{
  int status = tp.status();
  int barcode = tp.barcode();
  int pdg_id = abs(tp.pdgId());
  const xAOD::TruthVertex* vertex = tp.hasDecayVtx() ? tp.decayVtx() : 0;
  // we want to keep primary particle with status==2 but without vertex in HepMC
  int vertex_barcode=-999999;
  if (vertex) vertex_barcode=vertex->barcode();

  return
    (

     (((status%1000 == 1) ||
       (status%1000 == 2 && status > 1000) ||
       (status==2 && vertex_barcode<-200000)) && (barcode<200000)) &&

     !(pdg_id==12 || pdg_id==14 || pdg_id==16 ||
       (pdg_id==1000022 &&  status%1000==1 ) ||
       (pdg_id==5100022 &&  status%1000==1 ) ||
       (pdg_id==1000024 &&  status%1000==1 ) ||
       (pdg_id==39 &&  status%1000==1 ) ||
       (pdg_id==1000039 &&  status%1000==1 ) ||
       (pdg_id==5000039 &&  status%1000==1 ))

     )
    ? true:false;
}


}


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
egammaTruthAlg::egammaTruthAlg (const std::string& name,
                                ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc)
{
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");

  declareProperty ("InputKey", m_inputKey,
                   "SG key for the input container.");
  declareProperty ("OutputKey", m_outputKey,
                   "SG key for the output container.");
  
  declareProperty ("ElectronPtMin", m_electronPtMin = 2*GeV,
                   "Minimum pt for electrons.");
  declareProperty ("PhotonPtMin",   m_photonPtMin = 2*GeV,
                   "Minimum pt for photons.");
  declareProperty ("EtaMax",    m_etaMax = 2.5,
                   "Maximum eta.");
  declareProperty ("IsoCone",    m_isoCone = 0.2,
                   "Isolation cone width.");
  declareProperty ("PhotonEtIsoMax", m_photonEtIsoMax = 2*MeV,
                   "Maximum isolation cone energy allowed to keep a photon.");

  declareProperty("ParticleCaloExtensionTool", m_exten,
                  "Extrapolator to calorimeter.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaTruthAlg::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_exten.retrieve() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaTruthAlg::execute()
{
  const xAOD::TruthParticleContainer* pin = 0;
  CHECK( evtStore()->retrieve (pin, m_inputKey) );

  auto pout = CxxUtils::make_unique<xAOD::TruthParticleContainer>();
  auto pout_aux = CxxUtils::make_unique<xAOD::TruthParticleAuxContainer>();
  pout->setStore (pout_aux.get());

#define DECOR(TYPE,N) xAOD::TruthParticle::Decorator<TYPE> N (m_auxPrefix + #N)
  DECOR(float,        etaCalo);
  DECOR(float,        phiCalo);
  DECOR(float,        Etcone20);
#undef DECOR

  for (const xAOD::TruthParticle* tp : *pin) {
    float iso = -999;
    if (isAccepted (*tp, *pin, iso)) {
      pout->push_back (CxxUtils::make_unique<xAOD::TruthParticle>());
      *pout->back() = *tp;

      CHECK( findImpact (*tp, etaCalo(*pout->back()), phiCalo(*pout->back())) );
      Etcone20(*pout->back()) = iso;
    }
  }

  CHECK( evtStore()->record (std::move(pout), m_outputKey) );
  CHECK( evtStore()->record (std::move(pout_aux), m_outputKey + "Aux.") );

  return StatusCode::SUCCESS;
}


/**
 * @brief Test to see if we accept a particle.
 * @param tp The particle to test.
 * @param cont The container of particles.
 * @param iso[out] The isolation energy for this particle.
 */
bool egammaTruthAlg::isAccepted (const xAOD::TruthParticle& tp,
                                 const xAOD::TruthParticleContainer& cont,
                                 float& iso)
{
  iso = -999;

  int id = tp.pdgId();
  int aid = abs(id);
  int barcode = tp.barcode();

  if (aid == abs(PDG::e_minus)) {
    if (tp.pt() < m_electronPtMin) return false;
  }
  else if (aid == abs(PDG::gamma)) {
    if (tp.pt() < m_photonPtMin) return false;
  }
  else
    return false;

  if (fabs(tp.eta()) > m_etaMax) return false;

  if (!isGenStable (tp)) return false;
  if (!isGenInteracting (tp)) return false;

  // Remove electrons/gammas decaying into themselves
  if( tp.hasDecayVtx() ) {
    const xAOD::TruthVertex* v = tp.decayVtx();
    size_t sz = v->nOutgoingParticles();
    for (size_t i = 0; i < sz; i++) {
      const xAOD::TruthParticle* child = v->outgoingParticle(i);
      if( child->pdgId()==id && child->barcode()!=barcode
          && (child->barcode() <100000 ))
      {
        return false;
      }
    }
  } // end decays into themselves

  // Isolation selection for photons.
  iso = computeIso (tp, cont);
  if (aid == abs(PDG::gamma)) {
    if (iso > m_photonEtIsoMax)
      return false;
  }

  return true;
}


/**
 * @brief Compute isolation around a particle.
 * @param tp The particle for which we want isolation.
 * @param cont The container of particles.
 */
float egammaTruthAlg::computeIso (const xAOD::TruthParticle& tp,
                                  const xAOD::TruthParticleContainer& cont)
{
  TLorentzVector sum;
  for (const xAOD::TruthParticle* p : cont) {
    if (p == &tp || p->barcode() == tp.barcode()) continue;
    if (!isGenStable (*p)) continue;
    if (!isGenInteracting (*p)) continue;
    if (tp.p4().DeltaR (p->p4()) < m_isoCone)
      sum += p->p4();
  }

  return sum.Pt();
}


/**
 * @brief Find the impact of a particle in the calorimeter.
 * @param p The particle to analyze.
 * @param etaCalo[out] Eta of the particle's impact with the calorimeter.
 * @param phiCalo[out] Phi of the particle's impact with the calorimeter.
 */
StatusCode egammaTruthAlg::findImpact (const xAOD::TruthParticle& tp,
                                       float& etaCalo,
                                       float& phiCalo)
{
  etaCalo = -999;
  phiCalo = -999;

  const Trk::CaloExtension* extension = 0;
  if (!m_exten->caloExtension (tp, extension)) {
    REPORT_MESSAGE (MSG::ERROR) <<  "Extension to calorimeter failed";
    return StatusCode::FAILURE;
  }

  CaloExtensionHelpers::EtaPhiHashLookupVector posvec;
  CaloExtensionHelpers::entryEtaPhiHashLookupVector (*extension, posvec);
  if (std::get<0> (posvec[CaloSampling::EMB2])) {
    etaCalo = std::get<1> (posvec[CaloSampling::EMB2]);
    phiCalo = std::get<2> (posvec[CaloSampling::EMB2]);
  }
  else if (std::get<0> (posvec[CaloSampling::EME2])) {
    etaCalo = std::get<1> (posvec[CaloSampling::EME2]);
    phiCalo = std::get<2> (posvec[CaloSampling::EME2]);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
