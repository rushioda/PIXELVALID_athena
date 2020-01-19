///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4PtEtaPhiMCnv_p1.cxx 
// Implementation file for class P4PtEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#define private public
#define protected public
#include "FourMom/P4PtEtaPhiM.h"
#undef protected
#undef private

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void 
P4PtEtaPhiMCnv_p1::persToTrans( const P4PtEtaPhiM_p1* persObj, 
				P4PtEtaPhiM* transObj, 
				MsgStream &/*log*/ )
{
  transObj->m_pt  = persObj->m_pt;
  transObj->m_eta = persObj->m_eta;
  transObj->m_phi = persObj->m_phi;
  transObj->m_m   = persObj->m_mass;
  return;
}

void 
P4PtEtaPhiMCnv_p1::transToPers( const P4PtEtaPhiM* transObj, 
				P4PtEtaPhiM_p1* persObj, 
				MsgStream &/*log*/ )
{
  persObj->m_pt   = transObj->m_pt;
  persObj->m_eta  = transObj->m_eta;
  persObj->m_phi  = transObj->m_phi;
  persObj->m_mass = transObj->m_m;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
