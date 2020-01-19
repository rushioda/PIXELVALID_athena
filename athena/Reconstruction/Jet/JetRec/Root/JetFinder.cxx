/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetFinder.cxx

#include "JetRec/JetFinder.h"
#include <iomanip>
#include <sstream>
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/config.h"
#include "fastjet/AreaDefinition.hh"
#ifndef NO_JET_VARIABLER
#include "fastjet/contrib/VariableRPlugin.hh"
#endif
#include "xAODEventInfo/EventInfo.h"
#include "JetEDM/FastJetUtils.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/ClusterSequence.h"
#include "JetEDM/IConstituentUserInfo.h"
#include "JetEDM/JetConstituentFiller.h"
#include "xAODJet/JetContainerInfo.h"

#ifdef USE_BOOST_AUTO
#include <boost/typeof/typeof.hpp>
#endif
#ifdef USE_BOOST_FOREACH
#include <boost/foreach.hpp>
#endif

using std::string;
using std::setw;
#ifndef NO_JET_VARIABLER
using fastjet::contrib::VariableRPlugin;
#endif
using xAOD::JetContainer;
using jet::IConstituentUserInfo;
using jet::JetConstituentFiller;

typedef IJetFinder::NameList NameList;

//**********************************************************************

JetFinder::JetFinder(string name)
: AsgTool(name),
  m_bld("JetFromPseudojet"),
  m_fjalg(fastjet::undefined_jet_algorithm),
  m_isVariableR(false) {
  declareProperty("JetAlgorithm", m_jetalg);
  declareProperty("JetRadius", m_jetrad =0.0);
  declareProperty("VariableRMinRadius", m_minrad =-1.0);
  declareProperty("VariableRMassScale", m_massscale =-1.0);
  declareProperty("PtMin", m_ptmin =0.0);
  declareProperty("GhostArea", m_ghostarea =0.0);
  declareProperty("RandomOption", m_ranopt =0);
  declareProperty("JetBuilder", m_bld);
}

//**********************************************************************

StatusCode JetFinder::initialize() {
  ATH_MSG_DEBUG("Initializing...");
  xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_jetalg);
  m_fjalg = xAOD::JetAlgorithmType::fastJetDef(ialg);
  if ( m_fjalg == fastjet::undefined_jet_algorithm ) {
    ATH_MSG_ERROR("Invalid jet algorithm name: " << m_jetalg);
    ATH_MSG_ERROR("Allowed values are Kt, CamKt, AntiKt, etc.");
    return StatusCode::FAILURE;
  }
  if ( m_jetrad <=0 ) {
    ATH_MSG_ERROR("Invalid jet size parameter: " << m_jetrad);
    return StatusCode::FAILURE;
  }
  if ( m_bld.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet builder.");
  }
  fastjet::JetDefinition jetdef(m_fjalg, m_jetrad);
  PseudoJetVector empty;
  fastjet::ClusterSequence cs(empty, jetdef);
  cs.inclusive_jets(m_ptmin);
  m_isVariableR = m_minrad>=0.0 && m_massscale>=0.0;
#ifdef NO_JET_VARIABLER
  if ( isVariableR() ) {
    ATH_MSG_ERROR("Variable-R jet findng is not supported in theis build.");
  }
#endif
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetFinder::find(const PseudoJetVector& inps, xAOD::JetContainer& jets,
                    xAOD::JetInput::Type inputtype,
                    const NameList& ghostlabs) const {
  if ( m_fjalg == fastjet::undefined_jet_algorithm ) {
    ATH_MSG_ERROR("Jet finder is not properly initiialized.");
    return 1;
  }
  ATH_MSG_DEBUG("Jet finding input count: " << inps.size());
  fastjet::JetDefinition jetdef(m_fjalg, m_jetrad);
#ifndef NO_JET_VARIABLER
  const VariableRPlugin* pvrp = 0;
  if ( isVariableR() ) {
    VariableRPlugin::ClusterType vct = VariableRPlugin::AKTLIKE;
    switch(m_fjalg) {
    case fastjet::kt_algorithm:        vct = VariableRPlugin::KTLIKE;  break;
    case fastjet::antikt_algorithm:    vct = VariableRPlugin::AKTLIKE; break;
    case fastjet::cambridge_algorithm: vct = VariableRPlugin::CALIKE;  break;
    default:
      ATH_MSG_ERROR("Invalid algorithm type for variable-R finding.");
    }
    pvrp = new VariableRPlugin(m_massscale, m_minrad, m_jetrad, vct, false);
    jetdef = fastjet::JetDefinition(pvrp);
  }
#else
  if ( isVariableR() ) {
    ATH_MSG_ERROR("Variable-R jet findng is not supported in theis build.");
  }
#endif
  fastjet::ClusterSequence* pcs = 0;
  if ( m_ghostarea <= 0 ) {
    ATH_MSG_DEBUG("Creating input cluster sequence");
    pcs = new fastjet::ClusterSequence(inps, jetdef);
  } else {
    fastjet::GhostedAreaSpec gspec(5.0, 1, m_ghostarea);
    if ( m_ranopt == 1 ) {
      // Use run/event number as random number seeds.
      std::vector<int> inseeds;
      const xAOD::EventInfo* pevinfo =
        evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
      if ( pevinfo != 0 ) {
#ifdef USE_BOOST_AUTO
        BOOST_AUTO(ievt, pevinfo->eventNumber());
        BOOST_AUTO(irun, pevinfo->runNumber());
#else
        auto ievt = pevinfo->eventNumber();
        auto irun = pevinfo->runNumber();
#endif
        if ( pevinfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
          // For MC, use the channel and MC event number
          ievt = pevinfo->mcEventNumber();
          irun = pevinfo->mcChannelNumber();
        }
        inseeds.push_back(ievt);
        inseeds.push_back(irun);
      } else {
        ATH_MSG_ERROR("Unable to retrieve event info");
      }
      if ( inseeds.size() ) gspec.set_random_status(inseeds);
    } // if (m_ranopt==1)
    ATH_MSG_DEBUG("Active area specs:");
    ATH_MSG_DEBUG("  Requested ghost area: " << m_ghostarea);
    ATH_MSG_DEBUG("     Actual ghost area: " << gspec.actual_ghost_area());
    ATH_MSG_DEBUG("               Max eta: " << gspec.ghost_etamax());
    ATH_MSG_DEBUG("              # ghosts: " << gspec.n_ghosts());
    ATH_MSG_DEBUG("       # rapidity bins: " << gspec.nrap());
    ATH_MSG_DEBUG("            # phi bins: " << gspec.nphi());
    std::vector<int> seeds;
    gspec.get_random_status(seeds);
    if ( seeds.size() == 2 ) {
      ATH_MSG_DEBUG("          Random seeds: " << seeds[0] << ", " << seeds[1]);
    } else {
      ATH_MSG_WARNING("Random generator size is not 2: " << seeds.size());
      ATH_MSG_DEBUG("          Random seeds: ");
#ifdef USE_BOOST_FOREACH
      BOOST_FOREACH(unsigned int seed, seeds) ATH_MSG_DEBUG("                 " << seed);
#else
      for ( auto seed : seeds ) ATH_MSG_DEBUG("                 " << seed);
#endif
    }
    fastjet::AreaDefinition adef(fastjet::active_area_explicit_ghosts, gspec);
    //fastjet::AreaDefinition adef(fastjet::active_area, gspec);
    ATH_MSG_DEBUG("Creating input area cluster sequence");
    pcs = new fastjet::ClusterSequenceArea(inps, jetdef, adef);
  }

  ATH_MSG_DEBUG("Calling fastjet");
  PseudoJetVector outs = sorted_by_pt(pcs->inclusive_jets(m_ptmin));
  ATH_MSG_DEBUG("Found jet count: " << outs.size());
  for ( PseudoJetVector::const_iterator ijet=outs.begin(); ijet!=outs.end(); ++ijet ) {
    xAOD::Jet* pjet = m_bld->add(*ijet, jets, inputtype, ghostlabs);
    xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_fjalg);
    pjet->setAlgorithmType(ialg);
    pjet->setSizeParameter(m_jetrad);
    if ( isVariableR() ) {
      pjet->setAttribute("VariableRMinRadius", m_minrad);
      pjet->setAttribute("VariableRMassScale", m_massscale);
    }
    pjet->setAttribute("JetGhostArea", m_ghostarea);
  }
  ATH_MSG_DEBUG("Reconstructed jet count: " << jets.size() <<  "  clusterseq="<<pcs);
  if ( outs.size() ) save(pcs);
  else delete pcs;
  return 0;
}

//**********************************************************************

void JetFinder::save(fastjet::ClusterSequence* pcs) const {
  // Create name for event store.
  string sdrop = "ToolSvc.";
  string myname = name();
  string::size_type ipos = myname.find(sdrop);
  if ( ipos != string::npos ) myname.replace(ipos, sdrop.size(), "");
  std::string cname = "ClusterSequence_JetFinder_" + myname;
  // Append version number if name already exists.
  if ( evtStore()->contains<fastjet::ClusterSequence>(cname) ) {
    bool found = false;
    for ( int iver=1; iver<100; ++iver ) {
      std::ostringstream ssnewname;
      ssnewname << cname << "_";
      if ( iver < 10 ) ssnewname << "00";
      if ( iver < 100 ) ssnewname << "0";
      ssnewname << iver;
      string newname = ssnewname.str();
      if ( ! evtStore()->contains<fastjet::ClusterSequence>(newname) ) {
        found = true;
        cname = newname;
        break;
      }
    }
    if ( ! found ) {
      ATH_MSG_WARNING("Event store already contains " << cname);
    }
  }
  if ( ! evtStore()->record(pcs, cname).isSuccess() ) {
    ATH_MSG_WARNING("Unable to record " << cname);
  } else {
    ATH_MSG_DEBUG("Recorded " << cname << "   " << pcs );
  }
  bool present = evtStore()->contains<fastjet::ClusterSequence>(cname);
  ATH_MSG_DEBUG("Check presence: " << present);
  ATH_MSG_DEBUG("Will delete self: " << pcs->will_delete_self_when_unused());
  const fastjet::SharedPtr<fastjet::PseudoJetStructureBase>& shrptr = pcs->structure_shared_ptr();
  ATH_MSG_DEBUG("  Pointer: " << shrptr.get());
  ATH_MSG_DEBUG("  Use count: " << shrptr.use_count());
}

//**********************************************************************

bool JetFinder::isVariableR() const {
  return m_isVariableR;
}

//**********************************************************************

void JetFinder::print() const {
  ATH_MSG_INFO("      Jet algorithm: " << m_jetalg);
  if ( isVariableR() ) {
    ATH_MSG_INFO("         Variable-R: true");
    ATH_MSG_INFO("         min radius: " << m_minrad);
    ATH_MSG_INFO("         max radius: " << m_jetrad);
    ATH_MSG_INFO("         mass scale: " << m_massscale);
  } else {
    ATH_MSG_INFO(" Jet size parameter: " << m_jetrad);
  }
  ATH_MSG_INFO("   Jet min pT [MeV]: " << m_ptmin);
  ATH_MSG_INFO("         Ghost area: " << m_ghostarea);
  ATH_MSG_INFO("       Output level: " << MSG::name(msg().level()));
}

//**********************************************************************
