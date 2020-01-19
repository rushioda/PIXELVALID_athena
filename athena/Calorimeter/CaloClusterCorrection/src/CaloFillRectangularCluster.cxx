/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloFillRectangularCluster.cxx,v 1.20 2009-04-25 17:57:01 ssnyder Exp $
/**
 * @file  CaloFillRectangularCluster.h
 * @author scott snyder <snyder@bnl.gov>, D. Lelas, H. Ma, S. Rajagopalan
 * @date March, 2006
 * @brief Calculates the per-layer position, size, etc. of a cluster.
 *        Optionally, fills the cluster with cells from StoreGate.
 */

#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/ReadHandle.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloUtils/CaloCellList.h"
#include "AthenaKernel/errorcheck.h"
#include <algorithm>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

using xAOD::CaloCluster;
using CLHEP::GeV;
using CLHEP::pi;
using CLHEP::twopi;

namespace {
const double deta = 0.2;
const double dphi = twopi / 64. + pi / 64.; // ~ 0.15 rad
} // anonymous namespace




namespace CaloClusterCorr {


//**************************************************************************


/**
 * @brief Return eta/phi ranges encompassing +- 1 cell.
 * @param eta Central eta value.
 * @param phi Central phi value.
 * @param sampling The sampling to use.
 * @param[out] deta Range in eta.
 * @param[out] dphi Range in phi.
 *
 * This can be a little tricky due to misalignments and the fact
 * that cells have different sizes in different regions.  Also,
 * CaloLayerCalculator takes only a symmetric eta range.
 * We try to find the neighboring cells by starting from the center
 * cell and looking a little bit more than half its width in either
 * direction, and finding the centers of those cells.  Then we use
 * the larger of these for the symmetric range.
 */
void etaphi_range (double eta,
                   double phi,
                   CaloCell_ID::CaloSample sampling,
                   double& deta,
                   double& dphi)
{
  deta = 0;
  dphi = 0;

  // Get the DD element for the central cell.
  const CaloDetDescrManager* dd_man = CaloDetDescrManager::instance();
  const CaloDetDescrElement* elt = dd_man->get_element_raw (sampling, eta, phi);
  if (!elt) return;

  // Should be smaller than the eta half-width of any cell.
  const double eps = 0.001;

  // Now look in the negative eta direction.
  const CaloDetDescrElement* elt_l = dd_man->get_element_raw
    (sampling,
     eta - elt->deta() - eps,
     phi);
  double deta_l = 0; // Eta difference on the low (left) side.
  if (elt_l)
    deta_l = std::abs (eta - elt_l->eta_raw()) + eps;

  // Now look in the positive eta direction.
  const CaloDetDescrElement* elt_r = dd_man->get_element_raw
    (sampling,
     eta + elt->deta() + eps,
     phi);
  double deta_r = 0; // Eta difference on the high (right) side.
  if (elt_r)
    deta_r = std::abs (eta - elt_r->eta_raw()) + eps;

  // Total deta is twice the maximum.
  deta = 2 * std::max (deta_r, deta_l);

  // Now for the phi variation.
  // The phi size can change as a function of eta, but not of phi.
  // Thus we have to look again at the adjacent eta cells, and
  // take the largest variation.

  // Now look in the negative eta direction.
  elt_l = dd_man->get_element_raw
    (sampling,
     eta  - elt->deta() - eps,
     CaloPhiRange::fix (phi - elt->dphi() - eps));
  double dphi_l = 0; // Phi difference on the low-eta () side.
  if (elt_l)
    dphi_l = std::abs (CaloPhiRange::fix (phi - elt_l->phi_raw())) + eps;

  // Now look in the positive eta direction.
  elt_r = dd_man->get_element_raw
    (sampling,
     eta + elt->deta() + eps,
     CaloPhiRange::fix (phi - elt->dphi() - eps));
  double dphi_r = 0; // Phi difference on the positive (down) side.
  if (elt_r)
    dphi_r = std::abs (CaloPhiRange::fix (phi - elt_r->phi_raw())) + eps;

  // Total dphi is twice the maximum.
  dphi = 2 * std::max (dphi_l, dphi_r);
}


//**************************************************************************


/**
 * @brief Sampling calculator helper class.
 *
 * This helper class is used to decouple the layer calculations
 * from the decision of the origin of the cells (a StoreGate container
 * or the existing cluster cells).
 *
 * This is an abstract base class; there are concrete implementations
 * of this for the two cases of cells coming from StoreGate
 * and cells coming from the clusters.
 */
class SamplingHelper
{
public:
  /**
   * @brief Constructor.
   * @param parent The parent correction class.
   * @param cluster The cluster being operated on.
   */
  SamplingHelper (const CaloClusterCorrection& parent,
                  CaloCluster* cluster);


  /// Destructor --- just to get a vtable.
  virtual ~SamplingHelper() {}


  /**
   * @brief Calculate layer variables --- abstract method.
   * @param eta Center of the cluster in @f$\eta$@f.
   * @param phi Center of the cluster in @f$\phi$@f.
   * @param deta Full width of the cluster in @f$\eta$@f.
   * @param dphi Full width of the cluster in @f$\phi$@f.
   * @param sampling The sampling for which to do the calculation.
   * @param dofill If true, add selected cells to the cluster (if possible).
   *
   * This virtual method should select the cells within the specified
   * window in the specified sampling from the list of candidate cells
   * and calculate the layer variables.  If @c dofill is true, then
   * the selected cells will also be added to the cluster, provided
   * that this makes sense (it will not be done if the cluster
   * is the source of the cells).
   *
   * The result of the calculation will be held in internal variables.
   */
  virtual void
  calculate (double eta,
             double phi,
             double deta,
             double dphi,
             CaloSampling::CaloSample sampling,
             bool dofill = false) = 0;


  /// Return the cell with the maximum energy --- abstract method.
  virtual const CaloCell* max_et_cell() const = 0;

  /// Test for empty candidate cell list --- abstract method.
  virtual bool empty() const = 0;


  /**
   * @brief Calculate layer variables for cells in the cluster.
   * @param eta Center of the cluster in @f$\eta$@f.
   * @param phi Center of the cluster in @f$\phi$@f.
   * @param deta Full width of the cluster in @f$\eta$@f.
   * @param dphi Full width of the cluster in @f$\phi$@f.
   * @param sampling The sampling for which to do the calculation.
   *
   * This method selects the cells within the specified
   * window in the specified sampling from the current list of cells
   * in the cluster and calculates the layer variables.
   *
   * The result of the calculation will be held in internal variables.
   */
  void
  calculate_cluster (double eta,
                     double phi,
                     double deta,
                     double dphi,
                     CaloSampling::CaloSample sampling);


  /**
   * @brief Calculate layer variables and update cluster.
   * @param eta Center of the cluster in @f$\eta$@f.
   * @param phi Center of the cluster in @f$\phi$@f.
   * @param deta Full width of the cluster in @f$\eta$@f.
   * @param dphi Full width of the cluster in @f$\phi$@f.
   * @param fallback_eta @f$\eta$@f result to use if there's an error.
   * @param fallback_phi @f$\phi$@f result to use if there's an error.
   * @param sampling The sampling for which to do the calculation.
   * @param allow_badpos Should error flags be allowed into the cluster?
   *
   * This method selects the cells within the specified
   * window in the specified sampling from the current list of cells
   * in the cluster and calculates the layer variables.
   *
   * The result of the calculation will be held in internal variables.
   * In addition, the cluster variables for this sampling will
   * be updated with the result.
   *
   * In some cases, the calculation of the cluster position may yield
   * an error (for example, if there are no selected cells).  In this case,
   * the values specified by @c fallback_eta and @c fallback_phi are used
   * instead of the calculated result.  If @c allow_badpos is true,
   * then the error flags are used to update the cluster variables;
   * otherwise, the fallback position is used when updating the cluster.
   */
  void
  calculate_and_set (double eta,
                     double phi,
                     double deta,
                     double dphi,
                     double fallback_eta,
                     double fallback_phi,
                     CaloSampling::CaloSample sampling,
                     bool allow_badpos = false);


  /// Return the cluster we're updating.
  CaloCluster* cluster() const;

  /// Return the @f$\eta$@f position from the last calculation.
  double etam() const;

  /// Return the @f$\phi$@f position from the last calculation.
  double phim() const;

  /// Return the @f$\eta$@f maximum position from the last calculation.
  double etamax() const;

  /// Return the @f$\phi$@f maximum position from the last calculation.
  double phimax() const;

  // return also the real value (now that the raw value are returned instead)
  double etareal() const { return m_calc.etam(); }


protected:
  /// The calculator object.
  CaloLayerCalculator m_calc;

  /// The correction object using us.
  const CaloClusterCorrection& m_parent;

  /// The cluster we're updating.
  CaloCluster* m_cluster;

  /// @f$\eta$@f position from last calculation.
  double m_etam;

  /// @f$\phi$@f position from last calculation.
  double m_phim;
};


/**
 * @brief Constructor.
 * @param parent The parent correction class.
 * @param cluster The cluster being operated on.
 */
SamplingHelper::SamplingHelper (const CaloClusterCorrection& parent,
                                CaloCluster* cluster)
  : m_parent (parent),
    m_cluster (cluster),
    m_etam(0),
    m_phim(0)
{
}


/**
 * @brief Calculate layer variables and update cluster.
 * @param eta Center of the cluster in @f$\eta$@f.
 * @param phi Center of the cluster in @f$\phi$@f.
 * @param deta Full width of the cluster in @f$\eta$@f.
 * @param dphi Full width of the cluster in @f$\phi$@f.
 * @param fallback_eta @f$\eta$@f result to use if there's an error.
 * @param fallback_phi @f$\phi$@f result to use if there's an error.
 * @param sampling The sampling for which to do the calculation.
 * @param allow_badpos Should error flags be allowed into the cluster?
 *
 * This method selects the cells within the specified
 * window in the specified sampling from the current list of cells
 * in the cluster and calculates the layer variables.
 *
 * The result of the calculation will be held in internal variables.
 * In addition, the cluster variables for this sampling will
 * be updated with the result.
 *
 * In some cases, the calculation of the cluster position may yield
 * an error (for example, if there are no selected cells).  In this case,
 * the values specified by @c fallback_eta and @c fallback_phi are used
 * instead of the calculated result.  If @c allow_badpos is true,
 * then the error flags are used to update the cluster variables;
 * otherwise, the fallback position is used when updating the cluster.
 */
void
SamplingHelper::calculate_and_set
  (double eta,
   double phi,
   double deta,
   double dphi,
   double fallback_eta,
   double fallback_phi,
   CaloSampling::CaloSample sampling,
   bool allow_badpos)
{
  calculate (eta, phi, deta, dphi, sampling, true);

  double seteta = m_calc.etam();
  double setphi = m_calc.phim();

  if (!allow_badpos) {
    //if (m_etam == -999) m_etam = fallback_eta;
    //if (m_phim == -999) m_phim = fallback_phi;
    if (seteta == -999) seteta = fallback_eta;
    if (setphi == -999) setphi = fallback_phi;
  }

  //FIXME: Sampling pattern not yet set! 
  m_parent.setsample (m_cluster,
                      sampling,
                      m_calc.em(),
		      seteta,
		      setphi,
                      m_calc.emax(),
                      m_calc.etamax(),
                      m_calc.phimax(),
                      m_calc.etas(),
                      m_calc.phis());
  if (allow_badpos) {
    if (m_etam == -999) m_etam = fallback_eta;
    if (m_phim == -999) m_phim = fallback_phi;
  }
}


/**
 * @brief Calculate layer variables for cells in the cluster.
 * @param eta Center of the cluster in @f$\eta$@f.
 * @param phi Center of the cluster in @f$\phi$@f.
 * @param deta Full width of the cluster in @f$\eta$@f.
 * @param dphi Full width of the cluster in @f$\phi$@f.
 * @param sampling The sampling for which to do the calculation.
 *
 * This method selects the cells within the specified
 * window in the specified sampling from the current list of cells
 * in the cluster and calculates the layer variables.
 *
 * The result of the calculation will be held in internal variables.
 */
void
SamplingHelper::calculate_cluster
  (double eta,
   double phi,
   double deta,
   double dphi,
   CaloSampling::CaloSample sampling)
{
  m_calc.fill (m_cluster->cell_begin(), m_cluster->cell_end(),
               eta, phi, deta, dphi, sampling);
  m_etam = m_calc.etamr();
  m_phim = m_calc.phimr();
}


/// Return the cluster we're updating.
inline
CaloCluster* SamplingHelper::cluster() const
{
  return m_cluster;
}


/// Return the @f$\eta$@f position from the last calculation.
inline
double SamplingHelper::etam() const
{
  return m_etam;
}


/// Return the @f$\phi$@f position from the last calculation.
inline
double SamplingHelper::phim() const
{
  return m_phim;
}


/// Return the @f$\eta$@f maximum position from the last calculation.
inline
double SamplingHelper::etamax() const
{
  return m_calc.etarmax();
}


/// Return the @f$\phi$@f maximum position from the last calculation.
inline
double SamplingHelper::phimax() const
{
  return m_calc.phirmax();
}


/**
 * @brief Helper to compare two cells by Et.
 *
 * When we find the maximum cell, we only want to find those
 * in LAr EM.  The list may contain other cells, though.
 * So, bias their energies down by some large value.
 */
struct et_compare_larem_only
{
  bool operator() (const CaloCell* a, const CaloCell* b);
  static double et (const CaloCell* cell);
};


bool et_compare_larem_only::operator() (const CaloCell* a,
                                        const CaloCell* b)
{
  return et(a) < et(b);
}


double et_compare_larem_only::et (const CaloCell* cell)
{
  double et = cell->et();
  if (cell->caloDDE()->getSubCalo() != CaloCell_ID::LAREM)
    et -= 1000*GeV;
  return et;
}


//**************************************************************************


/**
 * @brief Version of helper class for cells taken from StoreGate.
 */
class SamplingHelper_CaloCellList
  : public SamplingHelper
{
public:
  /**
   * @brief Constructor.
   * @param parent The parent correction class.
   * @param cluster The cluster being operated on.
   * @param list The cell list.
   * @param cell_container The container from which the cells came.
   */
  SamplingHelper_CaloCellList (const CaloClusterCorrection& parent,
                               CaloCluster* cluster,
                               const CaloCellList& list,
                               const CaloCellContainer* cell_container);


  /**
   * @brief Calculate layer variables.
   * @param eta Center of the cluster in @f$\eta$@f.
   * @param phi Center of the cluster in @f$\phi$@f.
   * @param deta Full width of the cluster in @f$\eta$@f.
   * @param dphi Full width of the cluster in @f$\phi$@f.
   * @param sampling The sampling for which to do the calculation.
   * @param dofill If true, add selected cells to the cluster.
   *
   * This method selects the cells within the specified
   * window in the specified sampling from the list of candidate cells
   * and calculates the layer variables.  If @c dofill is true, then
   * the selected cells will also be added to the cluster.
   *
   * The result of the calculation will be held in internal variables.
   */
  virtual void
  calculate (double eta,
             double phi,
             double deta,
             double dphi,
             CaloSampling::CaloSample sampling,
             bool dofill = false);


  /// Return the cell with the maximum energy.
  virtual const CaloCell* max_et_cell() const;

  /// Test for empty candidate cell list.
  virtual bool empty() const;


private:
  /// The cell list.
  const CaloCellList& m_list;
};


/**
 * @brief Constructor.
 * @param parent The parent correction class.
 * @param cluster The cluster being operated on.
 * @param list The cell list.
 * @param cell_container The container from which the cells came.
 */
SamplingHelper_CaloCellList::SamplingHelper_CaloCellList
   (const CaloClusterCorrection& parent,
    CaloCluster* cluster,
    const CaloCellList& list,
    const CaloCellContainer* /*cell_container*/)
     : SamplingHelper (parent, cluster),
       m_list (list)
{
}


/**
 * @brief Calculate layer variables.
 * @param eta Center of the cluster in @f$\eta$@f.
 * @param phi Center of the cluster in @f$\phi$@f.
 * @param deta Full width of the cluster in @f$\eta$@f.
 * @param dphi Full width of the cluster in @f$\phi$@f.
 * @param sampling The sampling for which to do the calculation.
 * @param dofill If true, add selected cells to the cluster.
 *
 * This method selects the cells within the specified
 * window in the specified sampling from the list of candidate cells
 * and calculates the layer variables.  If @c dofill is true, then
 * the selected cells will also be added to the cluster.
 *
 * The result of the calculation will be held in internal variables.
 */
void
SamplingHelper_CaloCellList::calculate
  (double eta,
   double phi,
   double deta,
   double dphi,
   CaloSampling::CaloSample sampling,
   bool dofill)
{
  m_calc.fill (m_list.begin(), m_list.end(),
               eta, phi, deta, dphi, sampling,
               dofill ? m_cluster : 0);
  // use the calo frame to gather the cells
  m_etam = m_calc.etamr();
  m_phim = m_calc.phimr();
}

  
/// Return the cell with the maximum energy.
const CaloCell* SamplingHelper_CaloCellList::max_et_cell() const
{
  return *std::max_element (m_list.begin(), m_list.end(),
                            et_compare_larem_only());
}


/// Test for empty candidate cell list.
bool SamplingHelper_CaloCellList::empty() const
{
  return m_list.begin() == m_list.end();
}


//**************************************************************************


/**
 * @brief Version of helper class for cells taken from the cluster itself.
 */
class SamplingHelper_Cluster
  : public SamplingHelper
{
public:
  /**
   * @brief Constructor.
   * @param parent The parent correction class.
   * @param cluster The cluster being operated on.
   */
  SamplingHelper_Cluster (const CaloClusterCorrection& parent,
                          CaloCluster* cluster);

  /**
   * @brief Calculate layer variables.
   * @param eta Center of the cluster in @f$\eta$@f.
   * @param phi Center of the cluster in @f$\phi$@f.
   * @param deta Full width of the cluster in @f$\eta$@f.
   * @param dphi Full width of the cluster in @f$\phi$@f.
   * @param sampling The sampling for which to do the calculation.
   * @param dofill (Ignored for this implementation.)
   *
   * This method selects the cells within the specified
   * window in the specified sampling from the list of candidate cells
   * and calculates the layer variables.
   *
   * The result of the calculation will be held in internal variables.
   */
  virtual void
  calculate (double eta,
             double phi,
             double deta,
             double dphi,
             CaloSampling::CaloSample sampling,
             bool dofill = false);

  /// Return the cell with the maximum energy.
  virtual const CaloCell* max_et_cell() const;

  /// Test for empty candidate cell list.
  virtual bool empty() const;
};


/**
 * @brief Constructor.
 * @param parent The parent correction class.
 * @param cluster The cluster being operated on.
 */
SamplingHelper_Cluster::SamplingHelper_Cluster (const CaloClusterCorrection& parent,
                                                CaloCluster* cluster)
  : SamplingHelper (parent, cluster)
{
}


/**
 * @brief Calculate layer variables.
 * @param eta Center of the cluster in @f$\eta$@f.
 * @param phi Center of the cluster in @f$\phi$@f.
 * @param deta Full width of the cluster in @f$\eta$@f.
 * @param dphi Full width of the cluster in @f$\phi$@f.
 * @param sampling The sampling for which to do the calculation.
 * @param dofill (Ignored for this implementation.)
 *
 * This method selects the cells within the specified
 * window in the specified sampling from the list of candidate cells
 * and calculates the layer variables.
 *
 * The result of the calculation will be held in internal variables.
 */
void
SamplingHelper_Cluster::calculate
  (double eta,
   double phi,
   double deta,
   double dphi,
   CaloSampling::CaloSample sampling,
   bool /*dofill*/)
{
  calculate_cluster (eta, phi, deta, dphi, sampling);
}

  
/// Return the cell with the maximum energy.
const CaloCell* SamplingHelper_Cluster::max_et_cell() const
{
  return *std::max_element(m_cluster->cell_begin(), m_cluster->cell_end(),
                           et_compare_larem_only());
}


/// Test for empty candidate cell list.
bool SamplingHelper_Cluster::empty() const
{
  return m_cluster->size()==0;
}


} // namespace CaloClusterCorr


//**************************************************************************


/**
 * @brief Standard Gaudi constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
CaloFillRectangularCluster::CaloFillRectangularCluster
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : CaloClusterCorrection(type, name, parent),
      m_detas2(0),
      m_dphis2(0),
      m_deta0(0),
      m_deta1(0),
      m_deta2(0),
      m_deta3(0),
      m_dphi0(0),
      m_dphi1(0),
      m_dphi2(0),
      m_dphi3(0)
{ 
  // properties 
  declareProperty("eta_size",     m_neta = 5);
  declareProperty("phi_size",     m_nphi = 5);
  declareProperty("fill_cluster", m_fill_cluster = true);
  declareProperty("cells_name",   m_cellsName = "AllCalo");
  declareProperty("set_raw_state",m_setRawState=true);
}


/**
 * @brief Standard Gaudi initialize method.
 *
 * Derived classes can extend this to change the sampling window sizes.
 */
StatusCode CaloFillRectangularCluster::initialize()
{
  // The method from the base class.
  CHECK( CaloClusterCorrection::initialize() );
  if (!m_cellsName.key().empty())
    CHECK( m_cellsName.initialize() );

  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    msg(MSG::ERROR) << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore()->regFcn(&IGeoModelSvc::geoInit,
			  geoModel,
			  &CaloFillRectangularCluster::geoInit,this);
    if(sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return sc;
}

StatusCode
CaloFillRectangularCluster::geoInit(IOVSVC_CALLBACK_ARGS)
{
  // Look up the middle layer cell segmentation.
  {
    const CaloDetDescrManager* dd_man = CaloDetDescrManager::instance();
    const CaloDetDescrElement* elt = dd_man->get_element (CaloCell_ID::EMB2,
                                                          0.001,
                                                          0.001);
    if (elt) {
      m_detas2 = elt->deta();
      m_dphis2 = elt->dphi();
    }
    else {
      // various TB configurations might have other eta/phi ranges or
      // no access at all to EMB2 but would need still the standard
      // EMB2 cell width as reference. Therefore the nominal eta and
      // phi width is assumed here
      m_detas2 = 0.025;
      m_dphis2 = M_PI/128.;
    }
  }

  // set up the sampling windows:
  m_deta0 = m_detas2*m_neta;
  m_dphi0 = m_dphis2*4;

  if (m_nphi >= 7)
    m_dphi0 = m_dphi0*2;
  else
    m_dphi0 = m_dphi0*1.5;

  m_deta1 = m_deta0;
  m_dphi1 = m_dphi0;

  m_deta2 = m_detas2*m_neta;
  m_dphi2 = m_dphis2*m_nphi;

  m_deta3 = (2*m_detas2)*(0.5 + (m_neta/2.));
  m_dphi3 = m_dphi2;

  return StatusCode::SUCCESS;
}


/*
 * @brief Actually make the correction for one region (barrel or endcap).
 * @param ctx     The event context.
 * @param helper Sampling calculation helper object.
 * @param eta The @f$\eta$@f seed of the cluster.
 * @param phi The @f$\phi$@f seed of the cluster.
 * @param samplings List of samplings for this region.
 */
void
CaloFillRectangularCluster::makeCorrection1(const EventContext& ctx,
                                            CaloClusterCorr::SamplingHelper&
                                             helper,
                                            double eta,
                                            double phi,
                                            const CaloSampling::CaloSample 
                                             samplings[4]) const
{
  // Do sampling 2.
  helper.calculate_and_set (eta, phi, m_deta2, m_dphi2, eta, phi,
                            samplings[2], true);
  // the etam and phim of the helper are now filled with etamr and phimr from the CaloLayerCalculator
  double eta2 = helper.etam();
  double phi2 = helper.phim();
  // Make sure that we have a seed. Assume the input cluster has a good (eta,phi)
  if (eta2 == -999.) eta2 = eta;
  if (phi2 == -999.) phi2 = phi;

  // But the fall back are in the calo frame
  double etaf = helper.cluster()->etaSample(samplings[2]);
  double phif = helper.cluster()->phiSample(samplings[2]);

  // Now do sampling 1; use the result from sampling 2 as the seed.
  helper.calculate_and_set (eta2, phi2, m_deta1, m_dphi1, etaf, phif,
                            samplings[1]);
  double eta1 = helper.etam();
  double phi1 = helper.phim();
  bool refine = true;
  if (eta1 == -999. || phi1 == -999.) {
    // Make sure that we have a seed. If eta,phi1 not OK, (e.g. deadOTX), take (eta2,phi2)
    if (eta1 == -999.) eta1 = eta2;
    if (phi1 == -999.) phi1 = phi2;
    refine = false;
  }

  // For some silly reason, we have TWO different sampling enums.
  // The clusters use one, the detector description uses the other.
  CaloCell_ID::CaloSample xsample;
  if (samplings[1] == CaloSampling::EMB1)
    xsample = CaloCell_ID::EMB1;
  else
    xsample = CaloCell_ID::EME1;

  // Now refine the eta position using +-1 strip around hot cell
  // This only makes sense if the previous step was OK 
  if (refine) {
    double detastr, dphistr;
    CaloClusterCorr::etaphi_range (helper.etamax(), helper.phimax(),
				   xsample,
				   detastr, dphistr);
    
    if (detastr > 0 && dphistr > 0) {
      helper.calculate_cluster (helper.etamax(), helper.phimax(),
				detastr, dphistr, samplings[1]);
      
      if (helper.etam()!=-999.) {
	eta1 = helper.etam();
	double eta1r = helper.etareal();
	helper.cluster()->setEta(samplings[1], eta1r);
      }
    }
  }

  // Now do sampling 0 using the eta1 point: 
  etaf = helper.cluster()->etaSample(samplings[1]);
  phif = helper.cluster()->phiSample(samplings[1]);
  helper.calculate_and_set (eta1, phi2, m_deta0, m_dphi0, etaf, phif,
                            samplings[0]);

  // Do for sampling 3 (using the sampling 2 seed).
  etaf = helper.cluster()->etaSample(samplings[2]);
  phif = helper.cluster()->phiSample(samplings[2]);
  helper.calculate_and_set (eta2, phi2, m_deta3, m_dphi3, etaf, phif,
                            samplings[3]);

  // Crack;
  // Check if the cluster has TileGap3 sampling and avoid to calculate the TileGap3 energy twice
  if ( helper.cluster()->hasSampling(CaloSampling::TileGap3) && samplings[0]==CaloSampling::PreSamplerE )
  {
    //By default, use the original cell container
    const CaloCellContainer* cc = helper.cluster()->getCellLinks()->getCellContainer();

    //Leave the option to use a different cell container
    if (!m_cellsName.key().empty()) {
      SG::ReadHandle<CaloCellContainer> cchand (m_cellsName, ctx);
      if (!cchand.isValid()) {
        REPORT_ERROR(StatusCode::FAILURE)
          << "Can't retrieve cell container " << m_cellsName.key();
        return;
      }
      cc = cchand.cptr();
    }
    
    if(!cc) //cover the case when the cluster does not give a cell container and the name is empty
    {
      REPORT_ERROR(StatusCode::FAILURE)
        << "Can't find cell container; cluster does not give a cell container";
      return;
    }

    // Add up the tile scintillator energy in the region around the cluster.
    double eh_scint = 0;
    CaloCellContainer::const_iterator f_cell =
      cc->beginConstCalo(CaloCell_ID::TILE);
    CaloCellContainer::const_iterator l_cell =
      cc->endConstCalo(CaloCell_ID::TILE);

    for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ;

      if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) {
        // consider only E4 cell
        if( fabs(cell->eta()) < 1.4 || fabs(cell->eta()) > 1.6 ) continue;
        double phic = cell->phi();
        double etac = cell->eta();

        float diffeta = etac-eta2;
        float diffphi = phic-phi2;
        if (diffphi < -pi) diffphi += twopi;
        if (diffphi > pi)  diffphi -= twopi;

        if(fabs(diffeta)<deta && fabs(diffphi)<dphi){
          eh_scint += cell->e();
        }
      }
    }
    //Set the TileGap3 sampling energy to the cluster; Needed for MVA calibration
    helper.cluster()->setEnergy(CaloSampling::TileGap3,eh_scint);

    helper.cluster()->setEta(CaloSampling::TileGap3, eta2);
    helper.cluster()->setPhi(CaloSampling::TileGap3, phi2);
  }
}


/*
 * @brief Execute the correction, given a helper object.
 * @param ctx     The event context.
 * @param helper Sampling calculation helper object.
 */
void
CaloFillRectangularCluster::makeCorrection2 (const EventContext& ctx,
                                             CaloClusterCorr::SamplingHelper&
                                             helper) const
{

  // Don't do anything if we don't have any cells.
  if (helper.empty())
    return;

  // Get the seed position of the cluster.
  CaloCluster* cluster = helper.cluster();
  double eta, phi;
  const CaloCell* cell_max = helper.max_et_cell();
  get_seed (cluster, cell_max, eta, phi);
  double aeta = fabs(eta);  

  // set the appropriate cluster size
  int neta = cluster->getClusterEtaSize();
  int nphi = cluster->getClusterPhiSize();

  if (m_neta != neta || m_nphi != nphi) {
     CaloCluster::ClusterSize oldSize = cluster->clusterSize();
     CaloCluster::ClusterSize newSize = oldSize;
     switch(oldSize) {
       case CaloCluster::SW_softe:
          break;
       case CaloCluster::SW_55ele:
       case CaloCluster::SW_35ele:
       case CaloCluster::SW_37ele:
          if (m_neta==5 && m_nphi==5) newSize=CaloCluster::SW_55ele;
          if (m_neta==3 && m_nphi==5) newSize=CaloCluster::SW_35ele;
          if (m_neta==3 && m_nphi==7) newSize=CaloCluster::SW_37ele;
          if (m_neta==7 && m_nphi==11) newSize=CaloCluster::SW_7_11;
          break;
       case CaloCluster::SW_55gam:
       case CaloCluster::SW_35gam:
       case CaloCluster::SW_37gam:
          if (m_neta==5 && m_nphi==5) newSize=CaloCluster::SW_55gam;
          if (m_neta==3 && m_nphi==5) newSize=CaloCluster::SW_35gam;
          if (m_neta==3 && m_nphi==7) newSize=CaloCluster::SW_37gam;
          if (m_neta==7 && m_nphi==11) newSize=CaloCluster::SW_7_11;
          break;
       case CaloCluster::SW_55Econv:
       case CaloCluster::SW_35Econv:
       case CaloCluster::SW_37Econv:
          if (m_neta==5 && m_nphi==5) newSize=CaloCluster::SW_55Econv;
          if (m_neta==3 && m_nphi==5) newSize=CaloCluster::SW_35Econv;
          if (m_neta==3 && m_nphi==7) newSize=CaloCluster::SW_37Econv;
          if (m_neta==7 && m_nphi==11) newSize=CaloCluster::SW_7_11;
          break;
       default:
          if (m_neta==5 && m_nphi==5) newSize=CaloCluster::SW_55ele;
          if (m_neta==3 && m_nphi==5) newSize=CaloCluster::SW_35ele;
          if (m_neta==3 && m_nphi==7) newSize=CaloCluster::SW_37ele;
          if (m_neta==7 && m_nphi==11) newSize=CaloCluster::SW_7_11;
          break;
     }
     cluster->setClusterSize(newSize);
  }

  // Lists of samplings in the barrel and endcap.
  static const CaloSampling::CaloSample samplings_b[4] = 
    { CaloSampling::PreSamplerB, CaloSampling::EMB1,
      CaloSampling::EMB2, CaloSampling::EMB3 };
  static const CaloSampling::CaloSample samplings_e[4] = 
    { CaloSampling::PreSamplerE, CaloSampling::EME1,
      CaloSampling::EME2, CaloSampling::EME3 };

  // We need to calculate sampling properties for barrel and endcap 
  // separately.
  // FIXME: the overlap with barrel should be checked!!

  //Now set the sampling pattern for this cluster
  //Can't set sampling variables w/o setting the sampling pattern before
  uint32_t samplingPattern_b=0xf;  //first four bits: The barrel sampling (PS to Back)
  uint32_t samplingPattern_e=0xf0; //bits 4-7: The EMEC samplings (PS to back)
  uint32_t samplingPattern=0;

  if (aeta < 1.6)
    samplingPattern |=samplingPattern_b;

  if (aeta > 1.3)
    samplingPattern |=samplingPattern_e;

  if (aeta > 1.37 && aeta < 1.63)
    samplingPattern |=(1<<(uint32_t)CaloSampling::TileGap3);

  cluster->setSamplingPattern(samplingPattern);

  // Barrel
  if (aeta < 1.6) {
    makeCorrection1 (ctx, helper, eta, phi, samplings_b);
  }

  // Endcap
  if (aeta > 1.3) {
    makeCorrection1 (ctx, helper, eta, phi, samplings_e);
  }

  // Set the total cluster energy to the sum over all samplings.
  double cl_ene = 0;
  for(int i=0; i<4; i++ ){
    cl_ene += cluster->eSample(samplings_b[i]);
    cl_ene += cluster->eSample(samplings_e[i]);
  }
  cluster->setE(cl_ene);
  
  if (m_setRawState) {
    cluster->setRawE(cl_ene);
    cluster->setRawEta(eta);
    cluster->setRawPhi(phi);
  }

}


/**
 * @brief CaloClusterCorrection virtual method
 * @param ctx     The event context.
 * @param cluster The cluster on which to operate.
 */
void CaloFillRectangularCluster::makeCorrection(const EventContext& ctx,
                                                CaloCluster* cluster) const
{

  ATH_MSG_DEBUG( "Executing CaloFillRectangularCluster" << endreq) ;

  if (m_fill_cluster) {

    //By default, use the original cell container
    const CaloCellContainer* cell_container = cluster->getCellLinks()->getCellContainer();
    // We're filling the cluster with cells from StoreGate.
    // First, remove existing cells.
    cluster->getCellLinks()->clear();
 
    //Leave the option to use a different cell container 
    if (!m_cellsName.key().empty()) {
      SG::ReadHandle<CaloCellContainer> cchand (m_cellsName, ctx);
      if (!cchand.isValid()) {
        REPORT_ERROR(StatusCode::FAILURE)
          << "Can't retrieve cell container " << m_cellsName.key();
        return;
      }
      cell_container = cchand.cptr();
    }


    // Define the center for building the list of candidate cells.
    double eta = cluster->eta0();
    double phi = cluster->phi0();
    phi = CaloPhiRange::fix (phi);

    // Build the candidate cell list.
    // This 5 is a safe margin for cell_list calculation
    // and should not be changed.
    CaloCellList cell_list(cell_container); 
    cell_list.select(eta,phi,m_detas2*(m_neta+5),m_dphis2*(m_nphi+5));

    // Do the calculation.
    CaloClusterCorr::SamplingHelper_CaloCellList helper (*this,
                                                         cluster,
                                                         cell_list,
                                                         cell_container);
    makeCorrection2 (ctx, helper);
  }
  else {
    // We're recalculating a cluster using the existing cells.
    CaloClusterCorr::SamplingHelper_Cluster helper (*this, cluster);
    makeCorrection2 (ctx, helper);
  }
}


/*
 * @brief Return the seed position of a cluster.
 * @param cluster The cluster on which to operate.
 * @param max_et_cell The cell with the largest energy
 *  (of those being considered for inclusion in the cluster).
 * @param[out] eta The @f$\eta@f$ location of the cluster seed.
 * @param[out] phi The @f$\phi@f$ location of the cluster seed.
 *
 * The cluster seed is the center of rectangular cluster windows.
 * This may be overridden by derived classes to change the seed definition.
 */
void CaloFillRectangularCluster::get_seed (const CaloCluster* cluster,
                                           const CaloCell* max_et_cell,
                                           double& eta,
                                           double& phi) const
{
  ///!!! NEW way of the endcap-shift treatment (same for barrel and endcap)
  // a.b.c 2004 : for barrel, correct for the alignment before 
  //               comparing the Tower direction and the cell's
  // ( for Atlas the difference is null, but it's not true for TB )
  const CaloDetDescrElement* elt = max_et_cell->caloDDE();
  double phi_shift = elt->phi()-elt->phi_raw();
  double eta_shift = elt->eta()-elt->eta_raw();
  eta = cluster->eta0()+eta_shift;
  phi = CaloPhiRange::fix(cluster->phi0()+phi_shift);

  // Special case to handle a pathology seen at the edge of the calorimeter
  // with clusters with an eta size of 3.  The cluster size used for the SW
  // clustering is 5x5.  The SW clustering will find the window that contains
  // the maximum amount of energy.  So, suppose that there's a cluster
  // near the edge of the calorimeter such that the most energetic cell
  // is right at the edge of the calorimeter.  In this case, the SW clustering
  // is likely to position the seed cell two cells from the edge
  // (the next-to-next-to-last cell), as in that case, all 5 eta cells
  // are contained within the calorimeter.  But in that case, if we then
  // build a cluster of size 3 around this seed, then we'll be missing
  // the cell with the highest energy!  This will severely bias the
  // energy and eta measurements.
  //
  // So, what I'll do is this.  If the maximum cell is at the outer
  // edge of the (outer) EC and it is not within our eta window, then I'll
  // use the maximum cell position as the seed instead of what
  // the SW clustering gives.  I restrict this to the outer edge
  // of the EC to avoid any chance of changing the clustering results
  // in the bulk of the calorimeter.
  // Also do this if the maximum cell is on the edge of the inner endcap --- 
  // we can get the same effect.
  if ((elt->is_lar_em_endcap_inner() &&
       std::abs(elt->eta_raw()) - elt->deta() <
           elt->descriptor()->calo_eta_min()) ||
      (elt->is_lar_em_endcap_outer() &&
       std::abs(elt->eta_raw()) + elt->deta() >
           elt->descriptor()->calo_eta_max()))
  {
    // Max cell is at the edge.  Is it outside the window?
    if (std::abs (eta - elt->eta()) > m_deta2/2) {
      // Yes --- change the seed.
      eta = elt->eta();
    }
  }

  // stay in the calo frame and do not cook for cluster on edge
  // (inputs are now 3x5 so there should not be problems anymore)
  eta = cluster->eta0();
  phi = CaloPhiRange::fix(cluster->phi0());

}


/**
 * @brief Change the name of the CaloCellContainer used by this tool.
 * @param name The new container name.
 */
StatusCode
CaloFillRectangularCluster::setCaloCellContainerName
  (const std::string& name)
{
  return this->setProperty (StringProperty ("cells_name", name));
}

