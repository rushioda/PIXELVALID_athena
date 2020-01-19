/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EtaPhiBinsCnv_p1.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for EtaPhiBins class
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EtaPhiBinsEvent includes
#define private public
#define protected public
#include "HIGlobal/EtaPhiBins.h"
#undef private
#undef protected

// RecTPCnv includes
#include "HIRecTPCnv/EtaPhiBinsCnv_p1.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,1)
#include <TBufferFile.h>
#include <TROOT.h>
#endif

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void EtaPhiBinsCnv_p1::persToTrans(  const EtaPhiBins_p1* pers,
				    EtaPhiBins* trans, 
				    MsgStream& msg ) 
{
   msg << MSG::DEBUG << "Loading EtaPhiBins from persistent state..."
       << endreq;

  trans->m_EtaMin      = pers->m_EtaMin;
  trans->m_EtaMax      = pers->m_EtaMax;
  trans->m_NEtaBins    = pers->m_NEtaBins;
  trans->m_NPhiBins    = pers->m_NPhiBins;
#if ROOT_VERSION_CODE < ROOT_VERSION(5,15,1)
  TBuffer b(TBuffer::kRead);
#else
  TBufferFile b(TBuffer::kRead);
#endif
  b.SetBuffer((void*)&(pers->m_h2vec[0]),pers->m_h2vec.size(),kFALSE);
  TH2F* h2 = (TH2F*) b.ReadObject(gROOT->GetClass("TH2F"));
  trans->m_H2          = *h2;
  //trans->m_H2.Print();
  msg << MSG::DEBUG << "H2 reading didn't crash!" << endreq;
  return;
}

void EtaPhiBinsCnv_p1::transToPers(  const EtaPhiBins* trans, 
				    EtaPhiBins_p1* pers, 
				    MsgStream& msg ) 
{
   msg << MSG::DEBUG << "Creating persistent state of EtaPhiBins..."
       << endreq;

  pers->m_EtaMin    = trans->m_EtaMin;	
  pers->m_EtaMax    = trans->m_EtaMax;	
  pers->m_NEtaBins  = trans->m_NEtaBins;
  pers->m_NPhiBins  = trans->m_NPhiBins;


  msg << MSG::DEBUG << "Printing H2" << endreq;
  //trans->m_H2.Print();

  msg << MSG::DEBUG << "Make TBuffer" << endreq;
#if ROOT_VERSION_CODE < ROOT_VERSION(5,15,1)
  TBuffer b(TBuffer::kWrite);
#else
  TBufferFile b(TBuffer::kWrite);
#endif
  msg << MSG::DEBUG << "Writing H2 into TBuffer" << endreq;
  b.WriteObject(&trans->m_H2);
  char* buf = b.Buffer(); 
  int len = b.Length();
  msg << MSG::DEBUG << "Clearing vector" << endreq;
  pers->m_h2vec.clear();
  msg << MSG::DEBUG << "Reserving m_h2vec to length " << len << endreq;
  pers->m_h2vec.reserve(len);
  msg << MSG::DEBUG << "m_h2vec size = " << pers->m_h2vec.size() <<  endreq; 
  
  msg << MSG::DEBUG << "Pushing back " << b.Length() << " bytes to m_h2vec" <<  endreq;

  for (int i = 0;i<len;i++)
    {
      unsigned char ib = buf[i];

      msg << MSG::DEBUG << "push back  i = " << i <<  "cont = " << ib << endreq; 
  
      pers->m_h2vec.push_back(ib);
    }

  msg << MSG::DEBUG << "H2 writing didn't crash!" << endreq;
  
  return;
}
