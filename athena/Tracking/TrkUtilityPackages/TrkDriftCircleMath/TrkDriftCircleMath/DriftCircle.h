/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_DRIFTCIRCLE_H
#define DCMATH_DRIFTCIRCLE_H

#include "TrkDriftCircleMath/LocPos.h"
#include "TrkDriftCircleMath/MdtId.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <vector>

namespace Muon {
  class MdtDriftCircleOnTrack;
}

namespace TrkDriftCircleMath
{
  
  /** This class represents a drift time measurement */
  class DriftCircle
  {

  public:
    /** state of the drift circle */
    enum DriftState { EarlyHit = -1 , /** drift time too small to be compatible with drift spectrum */
		      InTime = 0,     /** drift time compatible with drift spectrum */
		      LateHit = 1,    /** drift time too large to be compatible with drift spectrum */
		      EmptyTube = 2,  /** channel without drift measurement */
		      NotCrossed = 3  /** channel not crossed */ 
    };
  public:
    /** minimal constructor 
	@param pos local position of the hit 
	@param r   drift radius
	@param dr  error on drift radius
	@param state state of the DC  */
    DriftCircle (const LocPos& pos, double r, double dr,
                 DriftState state = InTime)
      : m_pos (pos), m_r (r), m_dr (dr), m_drPrecise(dr), m_state (state),m_index(-1),m_rot(0) {}
            
    /** full constructor 
	@param pos local position of the hit 
	@param r   drift radius
	@param dr  error on drift radius
	@param state state of the DC
	@param identifier of hit
	@param index of drift circle use to link it back to the original RIO_OnTrack */
    DriftCircle (const LocPos& pos, double r, double dr,
                 DriftState state, MdtId id, int index, const Muon::MdtDriftCircleOnTrack* rot = 0)
      : m_pos (pos), m_r (r), m_dr (dr), m_drPrecise(dr), m_state (state), m_id (id), m_index (index), m_rot(rot) {}

    /** full constructor 
	@param pos local position of the hit 
	@param r   drift radius
	@param dr  error on drift radius
	@param drPrecise  error on drift radius (precise)
	@param state state of the DC
	@param identifier of hit
	@param index of drift circle use to link it back to the original RIO_OnTrack */
    DriftCircle (const LocPos& pos, double r, double dr, double drPrecise,
                 DriftState state, MdtId id, int index, const Muon::MdtDriftCircleOnTrack* rot = 0)
      : m_pos (pos), m_r (r), m_dr (dr), m_drPrecise(drPrecise), m_state (state), m_id (id), m_index (index), m_rot(rot) {}

      
    /** destructor */
    virtual ~DriftCircle() {}
    
    /** access to local position */
    const LocPos& position() const { return m_pos; }

    /** access to identifier */
    const MdtId&  id() const { return m_id; }
    
    /** direct access to local x position */
    double x() const { return m_pos.x(); }

    /** direct access to local y position */
    double y() const { return m_pos.y(); }
    
    /** access to drift radius */
    double r() const { return m_r; }
    
    /** access to error drift radius */
    double dr() const { return m_dr; }

    /** access to precise error drift radius */
    double drPrecise() const { return m_drPrecise; }
    
    /** access to drift state */
    const DriftState& state() const { return m_state; }
    
    /** access to index */
    int index() const { return m_index; }

    /** access to index */
    const Muon::MdtDriftCircleOnTrack* rot() const { return m_rot; }
    
    /** set method to allow update from t0 refit */
    void updateRadius( double r, double dr ) { m_r = r; m_dr = dr; }

  protected:
    LocPos  m_pos;
    double  m_r;
    double  m_dr;
    double  m_drPrecise;
    DriftState m_state; // Masked by DCOnTrack::m_state which is a DCOnTrackState
    MdtId   m_id;
    int     m_index;
    const Muon::MdtDriftCircleOnTrack* m_rot;
  };

  typedef std::vector<DriftCircle>  DCVec;
  typedef DCVec::iterator           DCIt;
  typedef DCVec::const_iterator     DCCit;
}

std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::DriftCircle& dc );

MsgStream   & operator<< (MsgStream   & os, const TrkDriftCircleMath::DriftCircle& dc);

#endif
