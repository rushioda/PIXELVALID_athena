/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Table of connection between Patch Panel and Slave Board.
#ifndef TGCConnectionPPToSB_hh
#define TGCConnectionPPToSB_hh

#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {

enum TGCPatchPanelType { NOPP=-1, 
			 WTPP=0, WDPP, STPP, SDPP, WIPP, SIPP, 
			 TotalNumPatchPanelType };
enum{ NumberOfPatchPanelType = 6};
enum{ NumberOfPPPort = 2 };

class TGCConnectionPPToSB : public TGCBoardConnection {
public:
  int getSBIdToPP(int type, int port, int index) const;
  void setSBIdToPP(int type, int port, int index, int idIn);
  int getNumberOfPort() const { return NumberOfPPPort; };

  TGCConnectionPPToSB();
  ~TGCConnectionPPToSB();
  TGCConnectionPPToSB(const TGCConnectionPPToSB& right);
  TGCConnectionPPToSB& operator=(const TGCConnectionPPToSB& right);
private:
  int* SBIdToPP[NumberOfPPPort][NumberOfPatchPanelType];
};

inline
int TGCConnectionPPToSB::getSBIdToPP(int type, int port, int index) const
{
  return SBIdToPP[port][type][index];
}

inline
void TGCConnectionPPToSB::setSBIdToPP(int type, int port, int index, int idIn)
{
  if(SBIdToPP[port][type]==0)
    SBIdToPP[port][type] = new int [numberOfBoard[type]];
  SBIdToPP[port][type][index] = idIn;
}

inline TGCPatchPanelType operator++(TGCPatchPanelType &rs, int)
{
    return rs = (TGCPatchPanelType)(rs + 1);
}


} //end of namespace bracket

#endif // TGCConnectionPPToSB_hh
