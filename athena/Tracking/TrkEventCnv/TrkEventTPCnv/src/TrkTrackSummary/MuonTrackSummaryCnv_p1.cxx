/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummaryCnv_p1.h"

void MuonTrackSummaryCnv_p1::persToTrans( const Trk::MuonTrackSummary_p1 *persObj,
                                            Trk::MuonTrackSummary    *transObj, MsgStream &/*log*/ )
{
    transObj->m_nscatterers       = persObj->m_nscatterers;
    transObj->m_npseudoMeasurements      = persObj->m_npseudoMeasurements;
    
    unsigned int size = persObj->m_chId.size();
    transObj->m_chamberHitSummary.resize(size); // fill vector with default instances of ChamberHitSummary
    unsigned int i=0;
    for ( ; i<size ; ++i ){
        transObj->m_chamberHitSummary[i].m_chId           = Identifier(persObj->m_chId[i]);
        transObj->m_chamberHitSummary[i].m_isMdt          = persObj->m_isMdt[i];

        transObj->m_chamberHitSummary[i].m_first.nhits      = persObj->nPhihits[i];
        transObj->m_chamberHitSummary[i].m_first.nholes     = persObj->nPhiholes[i];
        transObj->m_chamberHitSummary[i].m_first.noutliers  = persObj->nPhioutliers[i];
        transObj->m_chamberHitSummary[i].m_first.ndeltas    = persObj->nPhideltas[i];
        transObj->m_chamberHitSummary[i].m_first.ncloseHits = persObj->nPhicloseHits[i];

        transObj->m_chamberHitSummary[i].m_second.nhits      = persObj->nEtahits[i];
        transObj->m_chamberHitSummary[i].m_second.nholes     = persObj->nEtaholes[i];
        transObj->m_chamberHitSummary[i].m_second.noutliers  = persObj->nEtaoutliers[i];
        transObj->m_chamberHitSummary[i].m_second.ndeltas    = persObj->nEtadeltas[i];
        transObj->m_chamberHitSummary[i].m_second.ncloseHits = persObj->nEtacloseHits[i];
    }
}


void MuonTrackSummaryCnv_p1::transToPers( const Trk::MuonTrackSummary    *transObj,
                                            Trk::MuonTrackSummary_p1 *persObj, MsgStream &/*log*/ )
{
    persObj->m_nscatterers       = transObj->m_nscatterers;
    persObj->m_npseudoMeasurements      = transObj->m_npseudoMeasurements;

    unsigned int size = transObj->m_chamberHitSummary.size();
    
    //resize vectors
    persObj->m_chId.resize(size); 
    persObj->m_isMdt.resize(size,0); 
                                          
    persObj->nEtahits.resize(size); 
    persObj->nEtaholes.resize(size);  
    persObj->nEtaoutliers.resize(size); 
    persObj->nEtadeltas.resize(size); 
    persObj->nEtacloseHits.resize(size);
             
    persObj->nPhihits.resize(size); 
    persObj->nPhiholes.resize(size);  
    persObj->nPhioutliers.resize(size); 
    persObj->nPhideltas.resize(size); 
    persObj->nPhicloseHits.resize(size);
    
    unsigned int i=0;
    for (const Trk::MuonTrackSummary::ChamberHitSummary& s :
           transObj->chamberHitSummary())
    {
        persObj->m_chId[i]        = s.chamberId().get_identifier32().get_compact();   
        persObj->m_isMdt[i]       = s.isMdt()  ;
        
        persObj->nPhihits[i]      = s.mdtMl1().nhits      ;
        persObj->nPhiholes[i]     = s.mdtMl1().nholes     ;
        persObj->nPhioutliers[i]  = s.mdtMl1().noutliers  ;
        persObj->nPhideltas[i]    = s.mdtMl1().ndeltas    ;
        persObj->nPhicloseHits[i] = s.mdtMl1().ncloseHits ;

        persObj->nEtahits[i]      = s.mdtMl2().nhits      ;
        persObj->nEtaholes[i]     = s.mdtMl2().nholes     ;
        persObj->nEtaoutliers[i]  = s.mdtMl2().noutliers  ;
        persObj->nEtadeltas[i]    = s.mdtMl2().ndeltas    ;
        persObj->nEtacloseHits[i] = s.mdtMl2().ncloseHits ;        

        ++i;
    }
}
