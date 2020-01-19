/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

#include "TrigCaloEventTPCnv/TrigRNNOutput_p1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_p1.h"

#include "DataModel/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

void TrigRNNOutputCnv_p1::transToPers(const TrigRNNOutput *trans, 
                                    TrigRNNOutput_p1 *pers,
				    MsgStream &log) {

  log << MSG::DEBUG << "TrigRNNOutputCnv_p1::tranToPers" << endreq;
  
  if ( (!trans) || (!pers) )
    return;

  pers->m_output.clear();
  for (unsigned int i = 0; i < trans->size(); ++i) { 
    pers->m_output.push_back(trans->at(i));
  }

  m_ELinkTrigEMClusterCnv.transToPers(&trans->clusterLink(), &pers->m_cluster, log);
}

void TrigRNNOutputCnv_p1::persToTrans(const TrigRNNOutput_p1 *pers, 
                                    TrigRNNOutput *trans,
				    MsgStream &log) {

  log << MSG::DEBUG << "TrigRNNOutputCnv_p1::persToTrans" << endreq;

  if ( (!pers) || (!trans) )
    return;

  trans->output().clear();
  for (unsigned int i = 0; i < pers->m_output.size(); ++i){   
    trans->output().push_back(pers->m_output[i]);
  }

  ElementLink<TrigEMClusterContainer> cluster;
  m_ELinkTrigEMClusterCnv.persToTrans(&pers->m_cluster, &cluster, log);
  trans->setCluster (cluster);
}

