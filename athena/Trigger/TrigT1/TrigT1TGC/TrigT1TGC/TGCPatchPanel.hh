/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCPatchPanel.hh,v 1.4 2008-08-10 11:36:44 isaya Exp $
#ifndef TGCPatchPanel_hh
#define TGCPatchPanel_hh

#include "TrigT1TGC/TGCConnectionPPToSB.hh"
#include "TrigT1TGC/TGCNumbering.hh"
#include <fstream>
#include <string>

namespace LVL1TGCTrigger {

class TGCPatchPanelOut;
class TGCASDOut;
class TGCBIDOut;
class TGCConnectionInPP;
class TGCDatabaseManager;

typedef class TGCPatchPanel TGCWireTripletPP;
typedef class TGCPatchPanel TGCWireDoubletPP;
typedef class TGCPatchPanel TGCStripTripletPP;
typedef class TGCPatchPanel TGCStripDoubletPP;
typedef class TGCPatchPanel TGCWireInnerPP;
typedef class TGCPatchPanel TGCStripInnerPP;


const int NumberOfPatchPanelOut = 2;
const int NumberOfASDOut = 4;
const int NumberOfConnectorPerPPOut = 2;
const int NumberOfPPOutputConnector = 4;
const int NumberOfASDChannel = 16;
const int NChOfPPOutputConnector = 2*NumberOfASDChannel;
const int MaxNumberOfConnector = 10;
const int NumberOfBunchKeptInPP = 1;

class TGCPatchPanel {
public:
  void clockIn(int bunch, TGCDatabaseManager* db=0);

  TGCPatchPanelOut* getOutput(int SBId);
  void eraseOutput(int SBId);

  int getIdSlaveBoard(int port) const;
  void setIdSlaveBoard(int port,int id);

  // pointer to adjacent Patch Panel board.
  TGCPatchPanel* getAdjacentPP(int side) const { return PPAdj[side]; };
  void setAdjacentPP(int side, TGCPatchPanel* PP);

  void setASDOut(int connector, int ch, TGCASDOut* asdOut);

  TGCBIDOut* getBIDOut(int ch, int connector, int bunch);

  void dumpPPOut();      // print output of Patch Panel
  void dumpPPOut(int i); // bit pattern is separated by underscore

  void showProperty();

  int getId() const;
  void setId(int idIn);
  int getType() const { return type; };
  void setType(int typeIn) { type=typeIn; };
  TGCRegionType getRegion() const { return region; };
  void setRegion(TGCRegionType regionIn) { region=regionIn; };

  void connect();
  std::string getTypeName(int typeIn) const;

  TGCPatchPanel();
  ~TGCPatchPanel();

  TGCPatchPanel(const TGCPatchPanel& right);
  TGCPatchPanel& operator=(const TGCPatchPanel& right);

private:
  void showResult() const;
  void doBID();
  int doOrLogic();
  int createOutput();
  int getInputConnectorIndex(const int connectorId) const;
  void clearASDOut();
  void deleteBIDOut();

  int id;
  int idSlaveBoard[NumberOfPatchPanelOut];
  int type;
  TGCRegionType region;
  int bunchCounter;
  bool hasASDOut;
  bool hasBIDOut;
  int  nHit; // 18-Jan-01 Added by KH

  TGCPatchPanelOut* PPOut[NumberOfPatchPanelOut];
  TGCASDOut* ASDOut[NChOfPPOutputConnector][MaxNumberOfConnector];
  TGCBIDOut* BIDOut[NChOfPPOutputConnector][MaxNumberOfConnector][NumberOfBunchKeptInPP];
  TGCPatchPanel* PPAdj[2]; // pointer to adjacent board.
  TGCConnectionInPP* connectionInPP;
};


} //end of namespace bracket

#endif // TGCPatchPanel_hh
