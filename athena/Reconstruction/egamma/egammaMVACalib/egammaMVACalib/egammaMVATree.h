/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAMVACALIB_EGAMMAMVATREE
#define EGAMMAMVACALIB_EGAMMAMVATREE

#include <functional>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <tuple>
#include <cmath>
#include <set>
#include <cstdint>
#include <numeric>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <TTree.h>
#include <TLorentzVector.h>

#include <AsgTools/AsgMessaging.h>

#include <egammaMVACalib/egammaMVALayerDepth.h>



/** Adaptor class: xAOD::Electron (or xAOD::Photon) to TTree for MVA calibration
 *
 *  This class is a TTree with 0 events that recreates the structure of the
 *  TTree used during the training of the MVA calibration. In this way the same
 *  input (in the same format and computed with the same code) is exposed to
 *  egammaMVACalib (via egammaMVATool).
 *
 *  The structure of the TTree is dynamically created, given the set of Branches
 *  needed. The name of the branches is the same used during the training, and
 *  follow the convenctions of PhotonD3PD. By the way this tool should not be
 *  intended as a xOAD->D3PD converter.
 *
 *  Developer note: the structure is dynamic, only the needed branches are
 *    created. In addition the class holds the internal variables linked to the
 *    branches. Since variables are of different types this means to have
 *    different containers. This is the simplest implementation, but not very
 *    maintainable. TODO: Consider the usage of type erasure, for example with
 *    boost::variant.
 **/

#ifndef __CINT__
namespace detail
{
  // best solution here should be to don't implement first template function
  // to raise compilation error. But maybe there are problems when
  // linking dictionaries
  template<typename T> inline std::string ROOT_type_name() {               //!
  //  static_assert((sizeof(T) == 0), "type is not int/float/bool");
    return ""; }                                                           //!
  template<> inline std::string ROOT_type_name<int>() { return "I"; }      //!
  template<> inline std::string ROOT_type_name<float>() { return "F"; }    //!
  template<> inline std::string ROOT_type_name<bool>() { return "O"; }     //!
}
#endif

/** Base class implementing variables defined for egamma clusters **/

class egammaMVATreeEgamma : public TTree, public asg::AsgMessaging
{
public:
  // egammaMVATreeEgamma() : TTree(), asg::AsgMessaging("egammaMVATreeEgamma") { };
  egammaMVATreeEgamma(const std::string& name,
                      const std::string& prefix,
                      const std::set<std::string>& variables,
                      bool use_layer_corrected=true);
  // Note: using pointer version, so that trigger can use the same
  // interface with egamma = nullptr
  // TODO: move to const reference, to do that
  // 1. MVA must know in advance the input variables (in the MVA weights)
  // 2. remove particleType (Rconv > 0 && Rconv < 800) from TMVACalib:
  //    particle type should be written in the MVA weights

  /** call update for every egamma object, this update the internal variables
   *  connected to the branches
   **/
  void update(const xAOD::Egamma* egamma);
  void update(const xAOD::Egamma* egamma, const xAOD::CaloCluster* cluster);
private:
  void init_variables(const std::set<std::string>& variables);
  std::string m_prefix;
  bool m_use_layer_corrected = true;
protected:
  template<typename T> const T* get_ptr(const std::string&) const
  {
    static_assert((sizeof(T) == 0), "Type must be int/float");
    return nullptr;
  }
  template<typename T, typename MAPSTRINGFUNCTION, typename CONTAINER>
  void search_var_and_add(const std::string& var_name,
                          const MAPSTRINGFUNCTION& map_string_function,
                          CONTAINER& container)
  {
    // WARNING: pay attention to iterator/pointer invalidation
    auto it_function = map_string_function.find(var_name);
    if (it_function != map_string_function.end()) {
      container.emplace_back(var_name, it_function->second, T());
    }
  }

  template<typename T, typename MAPSTRINGFUNCTION, typename CONTAINER>
  void create_structure(const std::set<std::string> vars,
                        const MAPSTRINGFUNCTION& map_string_function,
                        CONTAINER& container)
  {
    // WARNING: pay attention to iterator/pointer invalidation:
    // first populate container, then access to pointers. Do not mix!

    for (const auto var_name : vars) {
      search_var_and_add<float>(var_name, map_string_function, container);
    }

    for (auto& it: container) {
      ATH_MSG_DEBUG("creating branch " << std::get<0>(it) << " at " << &std::get<2>(it));
      const std::string root_type_name = detail::ROOT_type_name<T>();
      //if (not root_type_name.empty()) {
      //  Branch((std::get<0>(it)).c_str(), &std::get<2>(it), (std::get<0>(it) + "/" + root_type_name).c_str());
      //}
      //else {
      //  ATH_MSG_WARNING("type of " << std::get<0>(it) << " is not int/float/bool");
        Branch((std::get<0>(it)).c_str(), &std::get<2>(it));
      //}
    }
  }
protected:
  typedef std::tuple<std::string, std::function<float(const xAOD::CaloCluster&)>, float> FloatElement; //!
  std::vector<FloatElement> m_functions_float_from_calo_cluster; //!
  typedef std::tuple<std::string, std::function<float(const xAOD::Egamma&)>, float> FloatElementParticle; //!
  std::vector<FloatElementParticle> m_functions_float_from_particle; //!
public:
  template<typename T> const T& get_ref(const std::string& var_name) const
  {
    const T* ptr = get_ptr<T>(var_name);
    if (!ptr) { throw std::runtime_error("var " + var_name + " not present"); }
    return *ptr;
  }
//#ifdef ROOTCORE
  //ClassDef(egammaMVATreeEgamma, 0)
//#endif
};

template<> const float* egammaMVATreeEgamma::get_ptr<float>(const std::string&) const;
template<> const int* egammaMVATreeEgamma::get_ptr<int>(const std::string&) const;

namespace egammaMVATreeHelpers { struct ConversionHelper; }

class egammaMVATreePhoton : public egammaMVATreeEgamma
{
public:
  egammaMVATreePhoton(const std::string& name,
                      const std::set<std::string>& variables,
                      bool use_layer_corrected=true,
                      bool force_conversion_to_zero_when_null_photon=false);
  void update(const xAOD::Photon* photon);
  void update(const xAOD::Photon* photon, const xAOD::CaloCluster* cluster);

private:
  bool m_force_conversion_to_zero_when_null_photon;
  template<typename T> const T* get_ptr(const std::string& var_name) const {
     return egammaMVATreeEgamma::get_ptr<T>(var_name);
  }
  void init_variables(const std::set<std::string>& variables);
  typedef std::tuple<std::string, std::function<float(const xAOD::Photon&)>, float> FloatElement; //!
  typedef std::tuple<std::string, std::function<int(const xAOD::Photon&)>, int> IntElement; //!
  typedef std::tuple<std::string, std::function<float(const egammaMVATreeHelpers::ConversionHelper&)>, float> FloatElementConv; //!
  typedef std::tuple<std::string, std::function<int(const egammaMVATreeHelpers::ConversionHelper&)>, int> IntElementConv; //!
  std::vector<FloatElement> m_functions_float_from_photon; //!
  std::vector<IntElement> m_functions_int_from_photon; //!
  std::vector<FloatElementConv> m_functions_float_from_ConversionHelper; //!
  std::vector<IntElementConv> m_functions_int_from_ConversionHelper; //!
public:
  template<typename T> const T& get_ref(const std::string& var_name) const
  {
    const T* ptr = get_ptr<T>(var_name);
    if (!ptr) { throw std::runtime_error("var " + var_name + " not present"); }
    return *ptr;
  }
//#ifdef ROOTCORE
    //ClassDef(egammaMVATreePhoton, 0)
//#endif
};

template<> const float* egammaMVATreePhoton::get_ptr<float>(const std::string&) const;
template<> const int* egammaMVATreePhoton::get_ptr<int>(const std::string&) const;

class egammaMVATreeElectron : public egammaMVATreeEgamma
{
public:
  egammaMVATreeElectron(const std::string& name,
                        const std::set<std::string>& variables,
                        bool use_layer_corrected=true);
  void update(const xAOD::Electron* electron);
  void update(const xAOD::Electron* electron, const xAOD::CaloCluster* cluster);
private:
  template<typename T> const T* get_ptr(const std::string& var_name) const {
     return egammaMVATreeEgamma::get_ptr<T>(var_name);
  }
  void init_variables(const std::set<std::string>& variables);
  typedef std::tuple<std::string, std::function<float(const xAOD::Electron&)>, float> FloatElement; //!
  typedef std::tuple<std::string, std::function<int(const xAOD::Electron&)>, int> IntElement; //!
  std::vector<FloatElement> m_functions_float_from_electron; //!
  std::vector<IntElement> m_functions_int_from_electron; //!
public:
  template<typename T> const T& get_ref(const std::string& var_name) const
  {
    const T* ptr = get_ptr<T>(var_name);
    if (!ptr) { throw std::runtime_error("var " + var_name + " not present"); }
    return *ptr;
  }
//#ifdef ROOTCORE
  //ClassDef(egammaMVATreeElectron, 0)
//#endif
};

template<> const float* egammaMVATreePhoton::get_ptr<float>(const std::string&) const;
template<> const int* egammaMVATreePhoton::get_ptr<int>(const std::string&) const;

/** inline functions to compute quantities **/
namespace egammaMVATreeHelpers
{
  // inline functions to avoid duplicates problem during linking (and performance)
  // cluster functions
  // REMEMBER to add the functions using corrected layer energies
  inline float compute_cl_eta(const xAOD::CaloCluster& cluster) { return cluster.eta(); }
  inline float compute_cl_phi(const xAOD::CaloCluster& cluster) { return cluster.phi(); }
  inline float compute_cl_e(const xAOD::CaloCluster& cluster) { return cluster.e(); }
  inline float compute_cl_etaCalo(const xAOD::CaloCluster& cluster) {
    double tmp = 0.;
    if(! (cluster.retrieveMoment(xAOD::CaloCluster::ETACALOFRAME, tmp))) {
      throw std::runtime_error("etaCalo not found in CaloCluster object");
    }
    return tmp;
  }
  inline float compute_cl_phiCalo(const xAOD::CaloCluster& cluster) {
    double tmp = 0.;
    if(! (cluster.retrieveMoment(xAOD::CaloCluster::PHICALOFRAME, tmp))) {
      throw std::runtime_error("phiCalo not found in CaloCluster object");
    }
    return tmp;
  }
  inline float compute_cl_etas1(const xAOD::CaloCluster& cluster) { return cluster.etaBE(1); }
  inline float compute_cl_etas2(const xAOD::CaloCluster& cluster) { return cluster.etaBE(2); }
  inline float compute_rawcl_Es0(const xAOD::CaloCluster& cl) { return cl.energyBE(0); }
  /*inline std::function<float(const xAOD::CaloCluster&)> compute_rawcl_Es0_auto(bool use_corrected)
  {
      if (use_corrected) return [](const xAOD::CaloCluster& cl) { return cl.energyBE(0); };
      else return [](const xAOD::CaloCluster& cl) { return cl.energyBE(0); };
  }*/
  inline float compute_rawcl_Es1(const xAOD::CaloCluster& cl) { return cl.energyBE(1); }
  inline float compute_rawcl_Es2(const xAOD::CaloCluster& cl) { return cl.energyBE(2); }
  inline float compute_rawcl_Es3(const xAOD::CaloCluster& cl) { return cl.energyBE(3); }

  inline float compute_correctedcl_Es0(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es0") ? cl.auxdataConst<double>("correctedcl_Es0") : cl.energyBE(0); }
  inline float compute_correctedcl_Es1(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es1") ? cl.auxdataConst<double>("correctedcl_Es1") : cl.energyBE(1); }
  inline float compute_correctedcl_Es2(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es2") ? cl.auxdataConst<double>("correctedcl_Es2") : cl.energyBE(2); }
  inline float compute_correctedcl_Es3(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es3") ? cl.auxdataConst<double>("correctedcl_Es3") : cl.energyBE(3); }

  inline float compute_rawcl_Eacc(const xAOD::CaloCluster& cl) { return cl.energyBE(1) + cl.energyBE(2) + cl.energyBE(3); }
  inline float compute_rawcl_f0(const xAOD::CaloCluster& cl) { return cl.energyBE(0) / (cl.energyBE(1) + cl.energyBE(2) + cl.energyBE(3)); }

  inline float compute_correctedcl_Eacc(const xAOD::CaloCluster& cl) { return compute_correctedcl_Es1(cl) + compute_correctedcl_Es2(cl) + compute_correctedcl_Es3(cl); }
  inline float compute_correctedcl_f0(const xAOD::CaloCluster& cl) { return compute_correctedcl_Es0(cl) / (compute_correctedcl_Eacc(cl)); }


  inline float compute_calibHitsShowerDepth(const std::array<float, 4>& cl, float eta)
  {
    const std::array<float, 4> radius(get_MVAradius(eta));

    // loop unrolling
    const float denominator = cl[0] + cl[1] + cl[2] + cl[3];
    if (denominator == 0) return 0.;

    return (radius[0] * cl[0]
          + radius[1] * cl[1]
          + radius[2] * cl[2]
          + radius[3] * cl[3]) / denominator;
  }

  inline float compute_rawcl_calibHitsShowerDepth(const xAOD::CaloCluster& cl)
  {
      const std::array<float, 4> cluster_array { compute_rawcl_Es0(cl),
                                                 compute_rawcl_Es1(cl),
                                                 compute_rawcl_Es2(cl),
                                                 compute_rawcl_Es3(cl) };
      return compute_calibHitsShowerDepth(cluster_array, compute_cl_eta(cl));
  }

  inline float compute_correctedcl_calibHitsShowerDepth(const xAOD::CaloCluster& cl) {
    const std::array<float, 4> cluster_array { compute_correctedcl_Es0(cl),
                                               compute_correctedcl_Es1(cl),
                                               compute_correctedcl_Es2(cl),
                                               compute_correctedcl_Es3(cl) };
    return compute_calibHitsShowerDepth(cluster_array, compute_cl_eta(cl));
  }




  // electron functions
  inline float compute_el_charge(const xAOD::Electron& el) { return el.charge(); }
  inline float compute_el_tracketa(const xAOD::Electron& el) { return el.trackParticle()->eta(); }
  inline float compute_el_trackpt(const xAOD::Electron& el) { return el.trackParticle()->pt(); }
  inline float compute_el_trackz0(const xAOD::Electron& el) { return el.trackParticle()->z0(); }
  inline float compute_el_refittedTrack_qoverp(const xAOD::Electron& el) { return el.trackParticle()->qOverP(); }
  inline int compute_el_author(const xAOD::Electron& el) { return el.auxdata<unsigned short int>("author"); }


  // photon functions
  inline int compute_ph_convFlag(const xAOD::Photon& ph) {
    const auto original = xAOD::EgammaHelpers::conversionType(&ph);
    if (original == 3) return 2;
    else if (original != 0) return 1;
    else return original;
  }

  struct ConversionHelper
  {
    ConversionHelper(const xAOD::Photon* ph)
      : m_vertex(ph ? ph->vertex() : nullptr),
        m_tp0(m_vertex ? m_vertex->trackParticle(0) : nullptr),
        m_tp1(m_vertex ? m_vertex->trackParticle(1) : nullptr),
        m_pt1conv(0.), m_pt2conv(0.)
    {
      static asg::AsgMessaging static_msg("ConversionHelper");
      static_msg.msg(MSG::DEBUG) << "init conversion helper";
      if (!m_vertex) return;

      static SG::AuxElement::Accessor<float> accPt1("pt1");
      static SG::AuxElement::Accessor<float> accPt2("pt2");
      if (accPt1.isAvailable(*m_vertex) && accPt2.isAvailable(*m_vertex))
      {
        m_pt1conv = accPt1(*m_vertex);
        m_pt2conv = accPt2(*m_vertex);
      }
      else
      {
        static_msg.msg(MSG::WARNING) << "pt1/pt2 not available, will approximate from first measurements";
        m_pt1conv = getPtAtFirstMeasurement(m_tp0);
        m_pt2conv = getPtAtFirstMeasurement(m_tp1);
      }
    }

    ConversionHelper(const xAOD::Photon& ph)
      : ConversionHelper(&ph) { }  // delegating constr

    inline float ph_Rconv() const { return m_vertex ? hypot(m_vertex->position().x(), m_vertex->position().y()) : 0; }
    inline float ph_zconv() const { return m_vertex ? m_vertex->position().z() : 0.; }
    inline int ph_convtrk1nPixHits() const {
      if (!m_tp0) { return 0; }
      uint8_t hits = 0;
      if (m_tp0->summaryValue(hits, xAOD::numberOfPixelHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfPixelHits";
        return 0;
      }
    }
    inline int ph_convtrk2nPixHits() const {
      if (!m_tp1) return 0;
      uint8_t hits;
      if (m_tp1->summaryValue(hits, xAOD::numberOfPixelHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfPixelHits";
        return 0;
      }
    }
    inline int ph_convtrk1nSCTHits() const {
      if (!m_tp0) { return 0; }
      uint8_t hits;
      if (m_tp0->summaryValue(hits, xAOD::numberOfSCTHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfSCTHits";
        return 0;
      }
    }
    inline int ph_convtrk2nSCTHits() const {
      if (!m_tp1) { return 0; }
      uint8_t hits;
      if (m_tp1->summaryValue(hits, xAOD::numberOfSCTHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfSCTHits";
        return 0;
      }
    }
    inline float ph_pt1conv() const { return m_pt1conv; }
    inline float ph_pt2conv() const { return m_pt2conv; }
    inline float ph_ptconv() const {
      // TODO: evaluate if move to this new definition, by now keep the previous one
      // to be consistent with the training
      // return m_vertex ? xAOD::EgammaHelpers::momentumAtVertex(*m_vertex).perp() : 0.;
      TLorentzVector sum;
      if (m_tp0) sum += m_tp0->p4();
      if (m_tp1) sum += m_tp1->p4();
      return sum.Perp();
    }
  private:
    float getPtAtFirstMeasurement(const xAOD::TrackParticle* tp) const
    {
      if (!tp) return 0;
      for (unsigned int i = 0; i < tp->numberOfParameters(); ++i)
        if (tp->parameterPosition(i) == xAOD::FirstMeasurement)
          return hypot(tp->parameterPX(i), tp->parameterPY(i));
      static asg::AsgMessaging static_msg("ConversionHelper");
      static_msg.msg(MSG::WARNING) << "Could not find first parameter, return pt at perigee";
      return tp->pt();
    }
    const xAOD::Vertex* m_vertex;
    const xAOD::TrackParticle* m_tp0;
    const xAOD::TrackParticle* m_tp1;
    float m_pt1conv, m_pt2conv;
  };
}

#endif
