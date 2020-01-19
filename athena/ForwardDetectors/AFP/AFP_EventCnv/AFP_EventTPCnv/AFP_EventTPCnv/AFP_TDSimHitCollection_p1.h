/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHitCollection_p1_h
#define AFP_TDSimHitCollection_p1_h

//#include "AthenaPoolUtilities/TPObjRef.h"
#include "AFP_EventTPCnv/AFP_TDSimHit_p1.h"

#include <vector>
#include <string>

class AFP_TDSimHitCollection_p1
{
public:
	typedef std::vector<AFP_TDSimHit_p1> HitVector;
	typedef HitVector::const_iterator const_iterator;
	typedef HitVector::iterator       iterator;

	AFP_TDSimHitCollection_p1 () {}

	// Accessors
	const std::string&  name() const { return m_name; }
	const HitVector&    getVector() const {return m_cont;}

private:
	std::string m_name;
	std::vector<AFP_TDSimHit_p1> m_cont;
};

#endif // AFP_TDSimHitCollection_p1_h
