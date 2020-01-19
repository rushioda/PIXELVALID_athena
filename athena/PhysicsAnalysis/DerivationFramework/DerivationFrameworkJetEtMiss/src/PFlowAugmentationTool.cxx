/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PFlowAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Fabrice Balli (fabrice.balli@cern.ch)
//

#include "PFlowAugmentationTool.h"
#include "xAODTracking/VertexContainer.h"

namespace DerivationFramework {

const static SG::AuxElement::Decorator<char> dec_PVmatched("DFCommonPFlow_PVMatched");

  PFlowAugmentationTool::PFlowAugmentationTool(const std::string& t,
					       const std::string& n,
					       const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("Z0SinThetaCut", m_z0sinthcut = 2.0);
  }

  StatusCode PFlowAugmentationTool::initialize()
  {
		


    return StatusCode::SUCCESS;
  }

  StatusCode PFlowAugmentationTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode PFlowAugmentationTool::addBranches() const
  {

    // Get the vertex.
    const xAOD::VertexContainer* pvcont(0);
    const xAOD::Vertex* pv(0);
    ATH_CHECK(evtStore()->retrieve(pvcont, "PrimaryVertices"));
    if ( pvcont == 0 || pvcont->size()==0 ) {
      ATH_MSG_WARNING(" Failed to retrieve PrimaryVertices collection" );
      return StatusCode::FAILURE;
    }
    for (const auto& vx : *pvcont) {
      if (vx->vertexType() == xAOD::VxType::PriVtx) {
	pv = vx;
	break;
      }//If we have a vertex of type primary vertex
    }//iterate over the vertices and check their type

    // Use NoVtx as fall-back in case no PV is found, but the events should be rejected by the user
    // If there is no such then mark all CPFOs as unmatched
    if (pv == nullptr) {
      ATH_MSG_DEBUG("Could not find a primary vertex in this event" );
      for (auto theVertex : *pvcont) {
	if (xAOD::VxType::NoVtx == theVertex->vertexType() ) {
	  pv = theVertex;
	  break;
	}
      }
      if (nullptr == pv) {
	ATH_MSG_WARNING("Found neither PriVtx nor NoVtx in this event" );
      }
    }

    const xAOD::PFOContainer* cpfos = evtStore()->retrieve< const xAOD::PFOContainer >("JetETMissChargedParticleFlowObjects");
    for ( const xAOD::PFO* cpfo : *cpfos ) {
      if ( cpfo == 0 ) {
	ATH_MSG_WARNING("Have NULL pointer to charged PFO");
	continue;
      }
      const xAOD::TrackParticle* ptrk = cpfo->track(0);
      if ( ptrk == 0 ) {
	ATH_MSG_WARNING("Skipping charged PFO with null track pointer.");
	continue;
      }

      bool matchedToPrimaryVertex = false;
      //vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we correct the track z0 to be w.r.t z = 0
      float z0 = ptrk->z0() + ptrk->vz();
      if (pv) {z0 = z0 - pv->z();
	float theta = ptrk->theta();
	if ( fabs(z0*sin(theta)) < m_z0sinthcut ) {
	  matchedToPrimaryVertex = true;
	}
      }

      // generate static decorators to avoid multiple lookups	
      dec_PVmatched(*cpfo) = matchedToPrimaryVertex;
    }

    return StatusCode::SUCCESS;
  }
}
