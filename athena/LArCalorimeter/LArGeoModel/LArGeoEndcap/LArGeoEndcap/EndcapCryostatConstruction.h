/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EndcapCryostatConstruction.h
 *
 * @brief Declaration of EndcapCryostatConstruction class
 *
 * $Id: EndcapCryostatConstruction.h,v 1.14 2009-02-10 16:50:53 tsulaia Exp $
 */

#ifndef LARGEOENDCAP_ENDCAPCRYOSTATCONSTRUCTION_H
#define LARGEOENDCAP_ENDCAPCRYOSTATCONSTRUCTION_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "LArGeoHec/HEC2WheelConstruction.h"
#include "LArGeoFcal/FCALConstruction.h"
#include "LArGeoEndcap/EMECConstruction.h"
#include "StoreGate/DataHandle.h"

class IRDBAccessSvc;
class IRDBRecord;
class IGeoModelSvc;
class LArDetectorToolNV;
class StoredMaterialManager;

namespace LArGeo {

  /** @class LArGeo::EndcapCryostatConstruction
      @brief Description of the LAr End Cap cryostat, including MBTS description
   */
  class EndcapCryostatConstruction 
    {
    public:
    
    EndcapCryostatConstruction(bool fullGeo);
    virtual ~EndcapCryostatConstruction();
    
    // Get the envelope containing one endcap (pos/neg)
    GeoFullPhysVol*     createEnvelope(bool bPos);

    virtual GeoVPhysVol* GetEnvelope() {return 0;};
    

    // Set a vis limit for the FCAL
    void setFCALVisLimit(int limit) {m_fcalVisLimit=limit;}

  private:

    //    GeoPhysVol          *cryoEnvelopePhysical;

    int                 m_fcalVisLimit;

    IRDBAccessSvc       *m_pAccessSvc;
    IGeoModelSvc        *m_geoModelSvc;

    EMECConstruction          m_emec;
    HEC2WheelConstruction     m_hec2;
    FCALConstruction*         m_fcal;
    
    bool                      m_fullGeo;  // true->FULL, false->RECO

    friend class ::LArDetectorToolNV;

    GeoPhysVol* buildMbtsTrd(const IRDBRecord* rec
			     , const DataHandle<StoredMaterialManager>& matmanager
			     , GeoPhysVol* parent);
  };

} // namespace LArGeo

#endif // LARGEOENDCAP_ENDCAPCRYOSTATCONSTRUCTION_H
