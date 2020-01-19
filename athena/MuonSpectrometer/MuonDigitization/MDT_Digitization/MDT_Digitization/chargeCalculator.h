/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_CHARGECALCULATOR_H
#define MDT_DIGITIZATION_CHARGECALCULATOR_H
/*-----------------------------------------------
  
Created 20-8-2011 by Oleg.Bulekov@cern.ch
Function chargeCalculator returns the value of electric charge for Qball particle.
The information about charge coded in the pdgid of Qball particle is used.
It is supposed that the first digit of decimal number of charge corresponds 
to the third digit of decimal number of the  pdgid.
(e.g. Qball pdgid=10000200 corresponds to Q=2 )

-----------------------------------------------*/

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

#include "GaudiKernel/AlgFactory.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonSimEvent/MdtHitIdHelper.h"

#include "StoreGate/StoreGateSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "PathResolver/PathResolver.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/MdtTubeCalibContainer.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"

#include "TrkDetDescrUtils/GeometryStatics.h"

#include <vector>
#include <sstream>

#include <iostream>
// SB
#include "HepMC/GenParticle.h" 
//
double chargeCalculator(const MDTSimHit& hit){

    const HepMcParticleLink& trkParticle = hit.particleLink();
    const HepMC::GenParticle* genParticle = trkParticle.cptr(); 
	double qcharge=1.;
    if (genParticle){
		int particleEncoding = genParticle->pdg_id();
//      std::cout << "SB: pdgId=" << particleEncoding <<std::endl;
		if (((int)(abs(particleEncoding)/10000000) == 1) && ((int)(abs(particleEncoding)/100000)==100)) {
			qcharge = ((abs(particleEncoding) / 100000.0) - 100.0) * 1000.0;
			if (particleEncoding < 0.0) qcharge = -qcharge;
//			std::cout << "SB: BINGO! Qball: qcharge=" << qcharge <<std::endl;	
		}
    } else {
//      std::cout << "SB: genParticle=0 " <<std::endl;
    }

return qcharge;
}

#endif
