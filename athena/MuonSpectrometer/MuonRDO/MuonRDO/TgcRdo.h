/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_TGCRDO_H
#define MUONRDO_TGCRDO_H 1

#include <inttypes.h>
#include "MuonRDO/TgcRawData.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

// This cast is needed to remove warning: dereferencing type-punned pointer will break strict-aliasing rules 
// bug #67256: compiler warning in MuonEventAthenaPool/src/TgcRdoCnv_p2.cxx
// This cast breaks ATLAS C++ Coding Standard : CC4 Do not use reinterpret_cast. (REQUIRED)  
template<typename DEST, typename SRC> const DEST* TgcRdo_const_pointer_cast(const SRC* src) {
  const void* ptr = src;
  return reinterpret_cast<const DEST*>(ptr);
}

/*
  TGC collection class used for bare RDO ByteStream Conversion.
  This class holds one ROD information

  @author Tadashi Maeno
      based on the previous one by H.Kurashige
*/
class TgcRdo : public DataVector<TgcRawData>
{
public:
    // typedef required by container
    typedef uint16_t ID;
    typedef TgcRawData DIGIT;
/*
    enum
    {
        MAX_N_SIDE = 2, // A and C side
        MAX_N_ROD  = 12, // max number of ROD in each side
        MAX_ID = MAX_N_SIDE*MAX_N_ROD
    };
*/
    struct Errors
    {
        unsigned badBcID:1;
        unsigned badL1Id:1;
        unsigned timedout:1;
        unsigned badData:1;
        unsigned overflow:1;
    };

    struct RodStatus
    {
        unsigned EC_RXsend:1; // Error in request to send an event via RXlink
        unsigned EC_FELdown:1; // A Front End link has gone down - abandoned
        unsigned EC_frame:1; // Invalid FE link framing words
        unsigned EC_Glnk:1; // G-link error
        unsigned EC_xor:1; // Invalid XOR event checksum
        unsigned EC_ovfl:1; // Input FE event is too long or FE FIFO overflow
        unsigned EC_timeout:1; // Timeout expired for at least one FE link
        unsigned EC_xormezz:1; // Bad XOR checksum from mezz board
        unsigned EC_wc0:1; // Event has WC=0
        unsigned EC_L1ID:1; // L1ID mismatch (TTC EVID FIFO vs local).
        unsigned EC_nohdr:1; // First word is not header
        unsigned EC_rectype:1; // Unrecognized record type
        unsigned EC_null:1; // Unexpected nulls in FE input
        unsigned EC_order:1; // Word is out of order
        unsigned EC_LDB:1; // Invalid or unexpected Local Data Block ID
        unsigned EC_RXovfl:1; // RXfifo has overflowed
        unsigned EC_SSWerr:1; // SSW reports T1C, NRC, T2C, or GlinkNoLock error
        unsigned EC_sbid:1; // Illegal SB ID
        unsigned EC_unxsbid:1; // Unexpected SB ID received
        unsigned EC_dupsb:1; // SB ID is duplicated in the event
        unsigned EC_ec4:1; // Unexpected SB L1 Event ID(lo 4)
        unsigned EC_bc:1; // Unexpected SB BCID
        unsigned EC_celladr:1; // Invalid cell address
        unsigned EC_hitovfl:1; // Too many hits in event
        unsigned EC_trgbit:1; // Unexpected trigger bits
        unsigned EC_badEoE:1; // Bad End-of-event marker received, not 0xFCA
        unsigned EC_endWCnot0:1; // WC not 0 after EoE marker
        unsigned EC_noEoE:1; // No End-of-event marker received
    };

    struct LocalStatus
    {
        unsigned mergedHitBCs:1;
        unsigned mergedTrackletBCs:1;
        unsigned sortedHits:1;
        unsigned sortedTracklets:1;
        unsigned hasRoI:1;
        unsigned fakeSsw:1;
        unsigned fill1:10;
    };

    /* Constructors
     */
    /**Default constructor*/
    TgcRdo();

    /** Creates an empty container ready for writing */
    TgcRdo(uint16_t id, IdentifierHash idHash);

    // P1
    TgcRdo(uint16_t subDetectorId,
           uint16_t rodId,
           uint16_t bcId,
           uint16_t l1Id);

    // Destructor
    virtual ~TgcRdo()
    {
    }

    // Identifier
    uint16_t identify() const
    {
        return m_id;
    }

        // Identifier
    IdentifierHash identifyHash() const
    {
        return m_idHash;
    }
    
    // set methods
    void setL1Id(uint32_t v)
    {
        m_l1Id = v;
    }
    void setBcId(uint16_t v)
    {
        m_bcId = v;
    }
    void setTriggerType(uint16_t v)
    {
        m_triggerType = v;
    }
    void setOnlineId (uint16_t subDetectorId, uint16_t rodId);

    // get methods
    uint16_t subDetectorId() const
    {
        return m_subDetectorId;
    }
    uint16_t rodId() const
    {
        return m_rodId;
    }
    uint16_t triggerType() const
    {
        return m_triggerType;
    }
    uint16_t bcId() const
    {
        return m_bcId;
    }
    uint16_t l1Id() const
    {
        return m_l1Id;
    }

    // class method for RawData identification
    static uint16_t identifyRawData (const TgcRawData &rawData);

    const Errors& errors() const
    {
        return m_errors;
    }
    void setErrors(uint16_t data)
    {
        m_errors = *TgcRdo_const_pointer_cast<Errors>(&data);
    }

    const RodStatus& rodStatus() const
    {
        return m_rodStatus;
    }
    void setRodStatus(uint32_t data)
    {
        m_rodStatus = *TgcRdo_const_pointer_cast<RodStatus>(&data);
    }

    const LocalStatus& localStatus() const
    {
        return m_localStatus;
    }
    void setLocalStatus(uint32_t data)
    {
        m_localStatus = *TgcRdo_const_pointer_cast<LocalStatus>(&data);
    }

    uint32_t orbit() const
    {
        return m_orbit;
    }
    void setOrbit(uint32_t orbit)
    {
        m_orbit = orbit;
    }

    uint16_t version() const
    {
        return m_version;
    }

    void setVersion(uint16_t version)
    {
        m_version = version;
    }

    void clear();

    // online ID calculator
    static uint16_t calculateOnlineId (uint16_t subDetectorId, uint16_t rodId);

private:
    uint16_t m_version; // starting August 2006 version = 300. Before that, version = 0

    /** ID of this instance*/
    uint16_t m_id;

    /** OFFLINE hash of this collection*/
    IdentifierHash m_idHash; 
    
    // online IDs
    uint16_t m_subDetectorId;
    uint16_t m_rodId;

    // Trigger Type
    uint16_t m_triggerType;

    // BCID and L1ID on ROD
    uint16_t m_bcId;
    uint16_t m_l1Id;

    struct Errors      m_errors;
    struct RodStatus   m_rodStatus;
    struct LocalStatus m_localStatus;
    uint32_t m_orbit;
};

/**Overload of << operator for std::ostream for debug output*/
std::ostream& operator<<(std::ostream& sl, const TgcRdo& coll);

CLASS_DEF(TgcRdo,4185,0)

// Class needed only for persistency
typedef DataVector<TgcRdo> TGC_RDO_vector;
CLASS_DEF(TGC_RDO_vector, 4182, 1)

#endif
