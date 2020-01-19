/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDDetectorPositioner_H
#define AGDDDetectorPositioner_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include "AGDDKernel/AGDDPositioner.h"

#include <string>

class AGDDVolume;

class AGDDDetector;

struct Identifiers {
	std::string detectorType;
	std::string detectorAddress;
	int phiIndex;
	int etaIndex;
	int radIndex;
	int sideIndex;
	int mlayerIndex;
	int spare1;
	int spare2;
};

struct DetectorPositionParameters {
    DetectorPositionParameters():Zposition(0),Radius(0),PhiStart(0),Phi(0) {;}
    double Zposition;
    double Radius;
    double PhiStart;
	double Phi;
};

class AGDDDetectorPositioner:public AGDDPositioner {
public:
	AGDDDetectorPositioner(std::string n,HepGeom::Transform3D t);
	AGDDDetectorPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v);

	Identifiers ID;

	DetectorPositionParameters position;
	
	AGDDDetector* theDetector;
	
	GeoFullPhysVol* theVolume;
	
	// accessors
	
	std::string GetDetectorType() {return ID.detectorType;}
	std::string GetDetectorAddress() {return ID.detectorAddress;}
	double GetZPosition()	{return position.Zposition;}
	double GetRadius() {return position.Radius;}
	double GetPhi()	{return position.Phi;}
	GeoFullPhysVol* GetFullPhysicalVolume() {return theVolume;}
	
};

#endif
