/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BcmCollisionTime.h
//

#ifndef BcmCollisionTimeAlg_H
#define BcmCollisionTimeAlg_H

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"

class BcmCollisionTimeAlg : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    BcmCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~BcmCollisionTimeAlg();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------



  //int m_nevt;
  //ToolHandle<ICaloNoiseTool>  m_noiseTool;
  //bool m_isMC;
  //bool m_iterCut;
  float m_timeCut;
  //int m_minCells;
  //std::string m_cellsContName;

  //const DataHandle<CaloIdManager> m_caloIdMgr;
  //const CaloCell_ID*       m_calo_id;

};

class deltat_data{
 public: 
  deltat_data();
  deltat_data( unsigned int channel, unsigned int bcid, unsigned int position );

// private:

  unsigned int m_channel;
  unsigned int m_bcid;
  unsigned int m_position;

  //bool operator<(const deltat_data &data);
  // deltat_data min(const deltat_data &data);
      
};

inline deltat_data::deltat_data()
  : m_channel(99), m_bcid(99), m_position(99) {}


inline deltat_data::deltat_data( unsigned int channel, unsigned int bcid, unsigned int position )
  : m_channel(channel), m_bcid(bcid), m_position(position) {}

#endif
