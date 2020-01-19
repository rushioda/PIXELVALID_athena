/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwGap_v2.cxx,v 1.5 2009-04-25 17:57:01 ssnyder Exp $
/**
 * @file  CaloSwGap_v2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2004
 * @brief EM calorimeter gap corrections.
 *
 * Derived from previous versions:
 * AUTHORS:  L. Carminati
 * CREATED:  Feb 2003
 * 
 * PURPOSE:  Correction for the barrel-endcap cracks
 *           Tuned using 100 GeV photons
 * 
 * UPDATED:  10/04/03   MW
 *           bug fix to take only the cells in near cluster for gap
 *           correction
 * 
 * Updated:  May 5, 2004    (Sven Menke)
 * 	  base class changed from algo to tool
 * 
 * Updated:  June, 2004    (sss)
 *           Use ToolWithConstants to get correction constants.
 *
 * Updated:  May, 2006    (sss+SP)
 *           Introduce an offset and cluster size dependent corrections
 * 
 */


#include "CaloSwGap_v2.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <iostream>
#include <math.h>


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;
using CLHEP::pi;
using CLHEP::twopi;


namespace {


const double deta = 0.2;
const double dphi = twopi / 64. ;


} // anonymous namespace


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
CaloSwGap_v2::CaloSwGap_v2 (const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : CaloClusterCorrectionCommon(type, name, parent)
{
  declareConstant ("etamin_crack", m_etamin_crack);
  declareConstant ("etamax_crack", m_etamax_crack);
  declareConstant ("degree",       m_degree);
  declareConstant ("correction",   m_correction);
  declareConstant ("use_raw_eta",  m_use_raw_eta);

  declareProperty ("cells_name",   m_cells_name = "AllCalo");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloSwGap_v2::initialize()
{
  ATH_CHECK( CaloClusterCorrectionCommon::initialize() );
  ATH_CHECK( m_cells_name.initialize() );
  return StatusCode::SUCCESS;
}


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
void CaloSwGap_v2::makeTheCorrection (const EventContext& ctx,
                                      CaloCluster* cluster,
                                      const CaloDetDescrElement*/*elt*/,
                                      float eta,
                                      float adj_eta,
                                      float phi,
                                      float /*adj_phi*/,
                                      CaloSampling::CaloSample /*samp*/) const
{
  // ??? In principle, we should use adj_eta for the interpolation
  //     and range checks.  However, the v2 corrections were derived
  //     using regular eta instead.
  float the_aeta;
  if (m_use_raw_eta)
    the_aeta = std::abs (adj_eta);
  else
    the_aeta = std::abs (eta);

  if (the_aeta < m_etamin_crack || the_aeta > m_etamax_crack) 
    return; // no correction required

  SG::ReadHandle<CaloCellContainer> cc (m_cells_name, ctx);

  // Add up the tile scintillator energy in the region around the cluster.
  double eh_scint = 0;
  if(cc.isValid())
  {
    CaloCellContainer::const_iterator f_cell =
      cc->beginConstCalo(CaloCell_ID::TILE);
    CaloCellContainer::const_iterator l_cell =
      cc->endConstCalo(CaloCell_ID::TILE);

    for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ; 

      if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) {
        double phic = cell->phi();
        double etac = cell->eta(); 
        if (phic-phi < -pi) phic = phic + twopi;
        if (phic-phi > pi)  phic = phic - twopi;    
        if (etac >= eta-deta && etac <= eta+deta) {
          if (phic >= phi-dphi && phic <= phi+dphi) {
            eh_scint += cell->e();
          }
        }
      }
    }
  }

  // Find the correction weights.
  float a = interpolate (m_correction, the_aeta, m_degree, 1);
  float alpha = interpolate (m_correction, the_aeta, m_degree, 2);
  float offset = interpolate (m_correction, the_aeta, m_degree, 3);

  // The correction is a weighted sum of calorimeter and scintillator energies.
  float ec = cluster->e();

  // Sampling energies don't include scintillator contribution.
  setenergy (cluster, a*(ec + offset));
  cluster->setE (a*(ec + alpha*eh_scint + offset));
}


/**
 * @brief Change the name of the CaloCellContainer used by this tool.
 * @param name The new container name.
 */
StatusCode
CaloSwGap_v2::setCaloCellContainerName
  (const std::string& name)
{
  return this->setProperty (StringProperty ("cells_name", name));
}

