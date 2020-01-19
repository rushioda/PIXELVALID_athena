// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwGap_v2.h,v 1.6 2009-04-25 17:57:00 ssnyder Exp $
/**
 * @file  CaloSwGap_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date December, 2004
 * @brief EM calorimeter gap corrections.
 *
 * This attempts to correct for energy lost in dead material
 * in the region between the cryostats.
 * If available, scintillator energy is used to improve the
 * energy measurement.
 *
 * Derived from previous versions:
 * AUTHORS:  L. Carminati
 * CREATED:  Feb 2003
 *
 * PURPOSE:  Correction for the barrel-endcap cracks
 *           Tuned using 100 GeV photons
 *
 * Updated:  May 5, 2004    (Sven Menke)
 *           base class changed from algo to tool
 *
 * Updated:  June, 2004    (sss)
 *           Use ToolWithConstants to get correction constants.
 */


#ifndef CALOCLUSTERCORRECTION_CALOSWGAP_V2_H
#define CALOCLUSTERCORRECTION_CALOSWGAP_V2_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
#include "StoreGate/ReadHandleKey.h"


class CaloCellContainer;


/**
 * @class CaloSwGap_v2
 * @brief EM calorimeter gap corrections.
 *
 * This tool attempts to correct for the energy lost in dead material
 * in the gap between the barrel and end cryostats.  In addition to
 * the energy in the EM calorimeter, we also use energy deposited in
 * scintillators between the cryostats.  The correction is of the
 * form @f[
 *
 *    E' = A(E_c + \alpha E_s),
 *
 * @f] where @f$E_c@f$ is the measured energy in the EM calorimeter,
 * and @f$E_s@f$ is the measured energy in the scintillator.  The
 * coefficients @f$A@f$ and @f$\alpha@f$ are @f$\eta@f$-dependent.
 * The total scale @f$A@f$ is determined by fitting a polynomial
 * to the @f$E_{\rm meas}/E_{\rm true}@f$ curve outside the gap
 * region, and interpolating over the gap.  The weight @f$\alpha@f$ is
 * found by requiring that the RMS of @f$E'@f$ be a minimum.
 *
 * The resulting weights do not vary a lot with energy in the range
 * studied (50 to 200 GeV).  Thus, we combine together all the energies
 * when determining the correction; the resulting correction is then
 * energy-independent.  The residual energy dependence will be folded
 * into subsequent corrections.
 *
 */
class CaloSwGap_v2
  : public CaloClusterCorrectionCommon,
    public ISetCaloCellContainerName
{
public:
 
  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  CaloSwGap_v2(const std::string& type,
               const std::string& name,
               const IInterface* parent);


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Virtual function for the correction-specific code.
   * @param ctx     The event context.
   * @param cluster The cluster to correct.
   *                It is updated in place.
   * @param elt     The detector description element corresponding
   *                to the cluster location.
   * @param eta     The @f$\eta@f$ coordinate of the cluster, in this sampling.
   * @param adj_eta The @f$\eta@f$ adjusted for
   *                any shift between the actual and nominal coordinates.
   *                (This is shifted back to the nominal coordinate system.)
   * @param phi     The @f$\phi@f$ coordinate of the cluster, in this sampling.
   * @param adj_phi The @f$\phi@f$ adjusted for
   *                any shift between the actual and nominal coordinates.
   *                (This is shifted back to the nominal coordinate system.)
   * @param samp    The calorimeter sampling we're examining.
   *                This is a sampling code as defined by
   *                @c CaloSampling::CaloSample; i.e., it has both
   *                the calorimeter region and sampling encoded.
   */
  virtual void makeTheCorrection (const EventContext& ctx,
                                  xAOD::CaloCluster* cluster,
                                  const CaloDetDescrElement* elt,
                                  float eta,
                                  float adj_eta,
                                  float phi,
                                  float adj_phi,
                                  CaloSampling::CaloSample samp) const override;



  /**
   * @brief Change the name of the CaloCellContainer used by this tool.
   * @param name The new container name.
   */
  virtual StatusCode setCaloCellContainerName (const std::string& name) override;


private:
  /// Calibration constants: The range over which this correction is defined.
  float             m_etamin_crack;
  float             m_etamax_crack;

  /// Calibration constant: The interpolation degree.
  int               m_degree;

  /// Calibration constant: The tabulated array of correction weights,
  /// A and alpha.
  CaloRec::Array<2> m_correction;

  /// Calibration constant: If true, tabulated values are in terms of
  /// raw (local) eta.
  bool m_use_raw_eta;

  /// Property: The name of the container in which to look to find tile cells.
  SG::ReadHandleKey<CaloCellContainer> m_cells_name;
};


#endif // not CALOCLUSTERCORRECTION_CALOSWGAP_V2_H
