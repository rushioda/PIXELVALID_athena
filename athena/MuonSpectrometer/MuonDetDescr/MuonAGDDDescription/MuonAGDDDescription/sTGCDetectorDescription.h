/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCDetectorDescription_H
#define sTGCDetectorDescription_H

#include "AGDDKernel/AGDDDetector.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

using MuonGM::sTGC_Technology;

struct sTGCReadoutParameters {
    double sPadWidth;
    double lPadWidth;
    std::vector<double> padH;
    std::vector<int> nPadX;
    double anglePadX;
    std::vector<double> firstPadPhi;
    std::vector<double> PadPhiShift;
    std::vector<double> nPadH;
    std::vector<double> firstPadH;
    std::vector<int> firstPadRow;
    std::vector<int> nWires;
    std::vector<double> firstWire;
    int wireGroupWidth;
    int nStrips;
    std::vector<int> firstTriggerBand;
    std::vector<int> nTriggerBands;
    std::vector<int> firstStripInTrigger;
    std::vector<double> firstStripWidth;
    std::vector<int> StripsInBandsLayer1;
    std::vector<int> StripsInBandsLayer2;
    std::vector<int> StripsInBandsLayer3;
    std::vector<int> StripsInBandsLayer4;
    std::vector<int> nWireGroups;
    std::vector<int> firstWireGroup;
    std::vector<int> leftmostPadColumn;
    std::vector<int> rightmostPadColumn;
	
};

class sTGCDetectorDescription: public AGDDDetector {
public:
    sTGCDetectorDescription(std::string s);
    void Register();
	
	void SetXYZ(std::vector<double> v) 
	{
		_small_x=v[0];
		_large_x=v[1];
		_y=v[2];
		_z=v[3];
		_yCutout=v[4];
	}
	
	double sWidth() {return _small_x;}
	double lWidth() {return _large_x;}
	double Length() {return _y;}
	double Tck()    {return _z;}

	void yCutout(double y) {_yCutout=y;}
	double yCutout() {return _yCutout;}
	
	void xFrame(double y) {_xFrame=y;}
	double xFrame() {return _xFrame;}
	
	void ysFrame(double y) {_ysFrame=y;}
	double ysFrame() {return _ysFrame;}
	
	void ylFrame(double y) {_ylFrame=y;}
	double ylFrame() {return _ylFrame;}

	sTGCReadoutParameters roParameters;
	
	sTGCReadoutParameters& GetReadoutParameters() {return roParameters;}

	static sTGCDetectorDescription* GetCurrent() {return current;}
	
	sTGC_Technology* GetTechnology();
protected:

	double _yCutout;
	
	double _xFrame;
	double _ysFrame;
	double _ylFrame;
	
	static sTGCDetectorDescription* current;
	void SetDetectorAddress(AGDDDetectorPositioner*);
};

#endif
