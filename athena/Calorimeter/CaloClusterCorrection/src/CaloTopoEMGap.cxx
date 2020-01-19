/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * AUTHORS:  N. Kerschen
 * CREATED:  Nov 2006
 * 
 * PURPOSE:  Correction for the barrel-endcap cracks for topoEM cluster
 *           (based on CaloSwGap_v2)
 */

#include "CaloTopoEMGap.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "StoreGate/ReadHandle.h"
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
CaloTopoEMGap::CaloTopoEMGap (const std::string& type,
                              const std::string& name,
                              const IInterface* parent)
  : CaloClusterCorrectionCommon(type, name, parent),
    m_cells ("AllCalo")
{
  declareProperty ("Cells", m_cells);

  declareConstant ("etamin_crack", m_etamin_crack);
  declareConstant ("etamax_crack", m_etamax_crack);
  declareConstant ("degree",       m_degree);
  declareConstant ("correction",   m_correction);
  declareConstant ("use_raw_eta",  m_use_raw_eta);
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloTopoEMGap::initialize()
{
  ATH_CHECK( CaloClusterCorrectionCommon::initialize() );
  ATH_CHECK( m_cells.initialize() );
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
void CaloTopoEMGap::makeTheCorrection (const EventContext& ctx,
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

  SG::ReadHandle<CaloCellContainer> cc (m_cells, ctx);

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
  float par0 = interpolate (m_correction, the_aeta, m_degree, 1);
  float par1 = interpolate (m_correction, the_aeta, m_degree, 2);
  float par2 = interpolate (m_correction, the_aeta, m_degree, 3);
  float par3 = interpolate (m_correction, the_aeta, m_degree, 4);

  // The correction is a weighted sum of calorimeter and scintillator energies.
  float ec = cluster->e();

  float a = ec/par0;
  //float b = 0;
 
  //if (a > 1.0) b = par0*log(a) + par0;

  // Sampling energies don't include scintillator contribution.
  setenergy (cluster, par1*(ec + a + par3));
  cluster->setE (par1*(ec + a + par2*eh_scint + par3));
}
