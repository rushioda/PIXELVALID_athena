/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLEMMISCALIB_H
#define LARCELLREC_LARCELLEMMISCALIB_H

/*! \class LArCellEmMiscalib 
 *
 * @brief Apply miscalibration in EM calorimeter at cell level
 *
 * \ G.Unal (based on other similar tools rom K.Voss)
 *
 *  \date   October 25, 2006 
 */

#include <utility>
#include <vector>

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "CaloUtils/CaloCellCorrection.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

namespace CLHEP {
  class HepRandomEngine;
}

class LArCellEmMiscalib :  public CaloCellCorrection

{
 
 public:    
  
  LArCellEmMiscalib(const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent) ;

  virtual ~LArCellEmMiscalib() {};
  
  /*! Constructor */
  virtual StatusCode initialize() ; 
  
  void MakeCorrection(CaloCell* theCell);

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);
  
 private:

  int region(int barrelec, double eta, double phi);
  void smearingPerRegion();

  const LArEM_ID*   m_larem_id;
  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;

  IAtRndmGenSvc* m_AtRndmGenSvc;
  CLHEP::HepRandomEngine* m_engine;

  int m_seed;
  double m_sigmaPerRegion;
  double m_sigmaPerCell;
  bool m_undo;

  unsigned int m_ncellem;
  std::vector<float> m_spread1;
  std::vector<float> m_calib;

};

#endif // not LARCELLREC_LARCELLEMMISCALIB_H
