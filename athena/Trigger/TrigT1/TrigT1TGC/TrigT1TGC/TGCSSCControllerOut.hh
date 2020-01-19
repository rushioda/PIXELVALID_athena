/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSSCControllerOut_hh
#define TGCSSCControllerOut_hh
#include "TrigT1TGC/TGCNumbering.hh"
#include <iostream>

namespace LVL1TGCTrigger {

enum {MaxNumberOfPhiInSSC = 2};
enum {MaxNumberOfChamberInR = 5};

class TGCSSCControllerOut {
public: 
  TGCSSCControllerOut(TGCRegionType region=ENDCAP);
  
  bool hasChamberBoundary(int ssc) const;
  bool hasUpperChamber(int ssc) const;
  int getOredChamberNumber(int ssc) const;
 
  int getR(int ssc) const;
  int getDR(int ssc) const;
  int getPtR(int ssc) const;

  int getPhi(int ssc, int phipos, bool ored=false) const;
  int getDPhi(int ssc, int phipos, bool ored=false) const;
  int getPtPhi(int ssc, int phipos, bool ored=false) const;

  void setR(int ssc, int rIn);
  void setDR(int ssc, int dRIn);
  void setPtR(int ssc, int ptLvl);
  void setHitR(int ssc, bool hit);

  void setPhi(int chamberNo, int phiPos, int phiIn);
  void setDPhi(int chamberNo, int phiPos, int dPhiIn);
  void setPtPhi(int chamberNo, int phiPos, int ptLvl);
  void setHitPhi(int chamberNo, int phiPos, bool hit);

  bool hasHit(int ssc, bool ored=false) const;
  bool hasHit(int ssc, int phiposInSSC, bool ored=false) const;
  bool hasHitPhi(int chamberNo, int phiPos) const;

  void clear();
  void print() const; 

protected:
  enum {MaxNumberOfSubSectorCluster = 19};
  int getChamberNumber(int ssc) const;
  int getNumberOfChamberInR() const;
  int getNumberOfSubSectorCluster() const;
private:
  static const int chamber[TotalNumTGCRegionType][MaxNumberOfSubSectorCluster];
  
  TGCRegionType regionType;

  int phi[MaxNumberOfChamberInR][MaxNumberOfPhiInSSC];
  int dPhi[MaxNumberOfChamberInR][MaxNumberOfPhiInSSC];
  int ptPhi[MaxNumberOfChamberInR][MaxNumberOfPhiInSSC];
  bool hitPhi[MaxNumberOfChamberInR][MaxNumberOfPhiInSSC];

  int r[MaxNumberOfSubSectorCluster];
  int dR[MaxNumberOfSubSectorCluster];
  int ptR[MaxNumberOfSubSectorCluster];
  bool hitR[MaxNumberOfSubSectorCluster];
};
  
inline
bool TGCSSCControllerOut::hasChamberBoundary(int ssc) const
{
  if(regionType==ENDCAP)
    if(ssc==2||ssc==4||ssc==6||ssc==12) return true;
  
  return false;
}

inline
bool TGCSSCControllerOut::hasUpperChamber(int ssc) const
{
  if(hasChamberBoundary(ssc)){
    if(ssc==4||ssc==6) return true;
    else return false;
  } else {
    std::cerr << "internal error: ssc " << ssc
              << " don't has chamber boundary" << std::endl;
    return false;
  }
}

inline
int TGCSSCControllerOut::getOredChamberNumber(int ssc) const
{
  if(hasChamberBoundary(ssc)){
    if(hasUpperChamber(ssc)) return getChamberNumber(ssc)-1;
    else return getChamberNumber(ssc)+1;
  } else {
    std::cerr << "internal error: ssc " << ssc
              << " don't has chamber boundary" << std::endl;
    return -1;
  }
}

inline
 int TGCSSCControllerOut::getChamberNumber(int ssc) const
{
  if (regionType==FORWARD) return chamber[0][ssc];
  else  return chamber[1][ssc];
}
inline
 int TGCSSCControllerOut::getNumberOfChamberInR() const
{
  if (regionType==FORWARD) return 1;
  else  return 5;
}
inline
 int TGCSSCControllerOut::getNumberOfSubSectorCluster() const
{
  if (regionType==FORWARD) return 8;
  else  return 19;
}
inline
 int TGCSSCControllerOut::getR(int ssc) const 
{ 
  return r[ssc]; 
}

inline
  int TGCSSCControllerOut::getDR(int ssc) const 
{ 
  return dR[ssc]; 
}

inline
  int TGCSSCControllerOut::getPtR(int ssc) const 
{ 
  return ptR[ssc]; 
}


inline 
  int TGCSSCControllerOut::getPhi(int ssc, int phipos, bool ored) const 
{ 
  if(!ored) {
    return phi[getChamberNumber(ssc)][phipos];
  } else {
    int idx =getOredChamberNumber(ssc);
    if (idx>=0 ) return phi[idx][phipos];
    else {
      std::cerr << "internal error in TGCSSCControllerOut::getPhi()" 
		<< std::endl;
      return 0;
    }
  }
  return 0;
}

inline
  int TGCSSCControllerOut::getDPhi(int ssc, int phipos, bool ored) const 
{ 
  if(!ored) {
    return dPhi[getChamberNumber(ssc)][phipos];
  } else {
    int idx =getOredChamberNumber(ssc);
    if (idx>=0 ) return dPhi[idx][phipos];
    else {
      std::cerr << "internal error in TGCSSCControllerOut::getDPhi()" 
		<< std::endl;
      return 0;
    }
  }
  return 0;
}


inline
  int TGCSSCControllerOut::getPtPhi(int ssc, int phipos, bool ored) const 
{ 
  if(!ored) {
    return ptPhi[getChamberNumber(ssc)][phipos];
  } else {
    int idx =getOredChamberNumber(ssc);
    if (idx>=0 ) return ptPhi[idx][phipos];
    else {
      std::cerr << "internal error in TGCSSCControllerOut::getPtPhi()" 
		<< std::endl;
      return 0;
    }
  }
  return 0;
}

inline
  void TGCSSCControllerOut::setR(int ssc, int rIn)
{
  if((0 <= ssc)&&(ssc < MaxNumberOfSubSectorCluster)){
    r[ssc]=rIn; 
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setR() sscid:" << ssc << " 0 < SSCId < " << MaxNumberOfSubSectorCluster << std::endl;
  }
}

inline
  void TGCSSCControllerOut::setDR(int ssc, int dRIn)
{
  if((0 <= ssc)&&(ssc < MaxNumberOfSubSectorCluster)){
    dR[ssc]=dRIn; 
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setDR() sscid:" << ssc << " 0 < SSCId < " << MaxNumberOfSubSectorCluster << std::endl;
  }
}

inline
  void TGCSSCControllerOut::setPtR(int ssc, int ptLvl)
{ 
  if((0 <= ssc)&&(ssc < MaxNumberOfSubSectorCluster)){
    ptR[ssc]=ptLvl; 
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setPtR() sscid:" << ssc << " 0 < SSCId < " << MaxNumberOfSubSectorCluster << std::endl;
  }
}

inline
  void TGCSSCControllerOut::setHitR(int ssc, bool hit)
{
  if((0 <= ssc)&&(ssc < MaxNumberOfSubSectorCluster)){
    hitR[ssc]=hit; 
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setHitR() sscid:" << ssc << " 0 < SSCId < " << MaxNumberOfSubSectorCluster << std::endl;
  }
}

inline
  void TGCSSCControllerOut::setPhi(int chamberNo, int phiPos, int phiIn)
{
  if((0 <= chamberNo)&&(chamberNo < MaxNumberOfChamberInR)&&(0 <= phiPos)&&(phiPos < MaxNumberOfPhiInSSC)){
    phi[chamberNo][phiPos]=phiIn;
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setPhi() chamberNo:" << chamberNo << " phiPos:" << phiPos << " phiIn:" << phiIn << std::endl;
  }
}

inline
  void TGCSSCControllerOut::setDPhi(int chamberNo, int phiPos, int dPhiIn)
{
  if((0 <= chamberNo)&&(chamberNo < MaxNumberOfChamberInR)&&(0 <= phiPos)&&(phiPos < MaxNumberOfPhiInSSC)){
    dPhi[chamberNo][phiPos]=dPhiIn; 
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setDPhi() chamberNo:" << chamberNo << " phiPos:" << phiPos << " dPhiIn:" << dPhiIn << std::endl;
  }
}

inline
  void TGCSSCControllerOut::setPtPhi(int chamberNo, int phiPos, int ptLvl)
{ 
  if((0 <= chamberNo)&&(chamberNo < MaxNumberOfChamberInR)&&(0 <= phiPos)&&(phiPos < MaxNumberOfPhiInSSC)){
    ptPhi[chamberNo][phiPos]=ptLvl; 
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setPtPhi() chamberNo:" << chamberNo << " phiPos:" << phiPos << " ptLvl:" << ptLvl << std::endl;
  }
}

inline
  void TGCSSCControllerOut::setHitPhi(int chamberNo, int phiPos, bool hit)
{
  if((0 <= chamberNo)&&(chamberNo < MaxNumberOfChamberInR)&&(0 <= phiPos)&&(phiPos < MaxNumberOfPhiInSSC)){
    hitPhi[chamberNo][phiPos]=hit; 
  }else{
    std::cerr << "internal error in TGCSSCControllerOut::setHitPhi() chamberNo:" << chamberNo << " phiPos:" << phiPos << " hit:" << hit << std::endl;
  }
}

inline
 bool TGCSSCControllerOut::hasHitPhi(int chamberNo, int phiPos) const
{
  if((0 <= chamberNo)&&(chamberNo < MaxNumberOfChamberInR)&&(0 <= phiPos)&&(phiPos < MaxNumberOfPhiInSSC)){
    return hitPhi[chamberNo][phiPos]; 
  }
  return false;
}


} //end of namespace bracket

#endif

