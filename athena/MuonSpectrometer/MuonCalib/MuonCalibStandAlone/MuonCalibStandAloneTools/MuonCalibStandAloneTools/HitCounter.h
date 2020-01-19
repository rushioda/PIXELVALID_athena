/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HitCounter_H
#define HitCounter_H

//c - c++
#include "map"
#include "string"

//root
class TH1F;
class TDirectory;
#include "MuonCalibStandAloneBase/NtupleStationId.h"

namespace MuonCalib {


class MuonCalibSegment;


/** @class HitCounter
Counts hits per tube, mezzanine, multilayer and layer, and decides how the chambers are to be fitted
@author rauscher@cern.ch
*/


class HitCounter
	{
	public:
	/** id classes*/
		class TubeId
			{
			public:
				inline TubeId():ml(0),ly(0),tb(0) {}
				int ml, ly, tb;
				inline bool operator < (const TubeId &other) const
					{
					if(ml != other.ml) return (ml < other.ml);
					if(ly != other.ly) return (ly < other.ly);
					return (tb < other.tb);
					}
			};
		class MezzId
			{
			public:
				inline MezzId():ml(0),mezz(0) {}
				int ml, mezz;
				inline bool operator < (const MezzId & other) const 
					{
					if(ml != other.ml) return (ml < other.ml);
					return mezz < other.mezz;					
					}
			};
		class DQInfo
			{
			public:
				inline DQInfo(): severity(0), noisy(false), dead(false)
					{}
				inline void SetNoisy()
					{
					if(severity<2)
						severity=2;
					noisy=true;
					}
				inline void SetDead()
					{
					if(severity<1)
						severity=1;
					dead=true;	
					}
				inline const int & GetSeverity() const
					{
					return severity;
					}
				inline const bool & GetNoisy() const
					{
					return noisy;
					}
				inline const bool & GetDead() const
					{
					return dead;
					}
			private:
				int severity;
				bool noisy;
				bool dead;
			};
//==============================================================================
	/** default constructor */
		inline HitCounter() : m_is_initialized(false), m_per_chamber(0), n_dead_tubes(0),
			n_dead_mezz(0), n_dead_ml(0), m_fit_by_int(0), p_hits_per_segments(NULL),
			p_wd(NULL)
			{
			}
	/** initialize class */
		bool Initialize(const NtupleStationId &id);
	/** process segment */
		void ProcessSegment(const MuonCalibSegment & segment);
	/** decide on how to fit */
		const std::string & FittingBy(int min_hits, double m_bad_fit_rate);
	/** return true if class is initialized */
		inline bool IsInitialized()
			{
			return m_is_initialized;
			}
		inline int FitByInt() const
			{
			return m_fit_by_int;
			}
		inline int NHits() const
			{
			return m_per_chamber;
			}
	/** return number of dead tubes - mezzanines - multilayers */
		inline int NDeadTubes() const
			{
			return n_dead_tubes;
			}
		inline int NDeadMezzanines() const
			{
			return n_dead_mezz;
			}
		inline int NDeadMultilayers() const
			{
			return n_dead_ml;
			}
		std::map<HitCounter::TubeId, HitCounter::DQInfo> InitialDQ(int noisy_tube_factor) const;
	private:
//==============================================================================
	//! is true if class is initialized
		bool m_is_initialized;
	//! name string
		std::string m_name;	
	//! hit counter
		std::map<TubeId, int> m_per_tube;
		std::map<MezzId, int> m_per_mezz;
		std::map<int, int> m_per_ml;
		int m_per_chamber;
	//! number of dead tubes - dead mezzanines - dead tubs
		int n_dead_tubes;
		int n_dead_mezz;
		int n_dead_ml;
	//! number of tubes per layer and mezzanine
		std::map<int,int> m_mezz_width;
	//! fitt by
		std::string m_fit_by;
		int m_fit_by_int;
	//!root histograms
		TH1F *p_hits_per_segments;
		TDirectory *p_wd;
	//! station id to which this counter applies
		NtupleStationId m_id;
	};

}

#endif
