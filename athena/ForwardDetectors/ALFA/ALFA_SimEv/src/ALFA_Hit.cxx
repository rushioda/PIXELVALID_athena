/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ALFA_SimEv/ALFA_Hit.h"


  // Called by ALFA_SensitiveDetector::ProcessHits

ALFA_Hit::ALFA_Hit(int hit, 
                        int /*track*/, 
			int particle,
			float kinEnergy, 
			float eneDeposit, 
			float preX, float preY, float preZ, 
			float postX, float postY, float postZ, 
			float time,
			int sign, int plate, int fiber, int station
			):
  m_hitID(hit),
  //trackID(track), 
//  m_partLink(track), 
  m_particleEncoding(particle),
  m_kineticEnergy(kinEnergy), 
  m_energyDeposit(eneDeposit), 
  m_preStepX(preX), m_preStepY(preY), m_preStepZ(preZ), 
  m_postStepX(postX), m_postStepY(postY), m_postStepZ(postZ), 
  m_globalTime(time),
  m_sign_fiber(sign), m_n_plate(plate), m_n_fiber(fiber), m_n_station(station) 
  {} 

// Default constructor needed by athenaroot/athenapool
//
ALFA_Hit::ALFA_Hit( ) :
  m_hitID(0xffff),
  //trackID(0),  
  m_particleEncoding(0),
  m_kineticEnergy(0.), 
  m_energyDeposit(0.), 
  m_preStepX(0.), m_preStepY(0.), m_preStepZ(0.), 
  m_postStepX(0.), m_postStepY(0.), m_postStepZ(0.), 
  m_globalTime(0.),
  m_sign_fiber(0), m_n_plate(0), m_n_fiber(0), m_n_station(0)  
{}

// destructor
ALFA_Hit::~ALFA_Hit() {}

// barcode
int ALFA_Hit::GetTrackID() const 
{
  return 22;  
// return m_partLink.barcode();
}
