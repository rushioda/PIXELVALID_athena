/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGGSectorLogic_hh
#define TGGSectorLogic_hh

#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TGCEvent.h"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include "TrigT1TGC/TGCInnerCoincidenceMap.hh"
#include "TrigT1TGC/TGCTileMuCoincidenceMap.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.hh"
#include "TrigT1TGC/TGCSSCController.hh"
#include "TrigT1TGC/TGCSSCControllerOut.hh"
#include "TrigT1TGC/TGCSLPreSelectorOut.hh"
#include "TrigT1TGC/TGCSLPreSelector.hh"
#include "TrigT1TGC/TGCSLSelector.hh"
#include "TrigT1TGC/TGCSLSelectorOut.hh"
#include "TrigT1TGC/TGCInnerTrackletSlotHolder.hh"

class ITGCTriggerDbTool;

namespace LVL1TGCTrigger {

const int MaxNumberOfWireHighPtBoard = 2;

class  TGCHighPtBoard;
class  TGCHighPtChipOut;
class  TGCTMDB;

class TGCSectorLogic {
public:
  TGCSectorLogic(const TGCSectorLogic& right);

  TGCSLSelectorOut* getSelectorOutput() const;
  int  getTileMuonWord() const;
  int  getInnerStationWord() const;

  void eraseSelectorOut(); 
  void clockIn(int bidIn);

  int getId() const;
  int getModuleID() const;
  int getOctantID() const;
  int getSideID() const;
  TGCRegionType getRegion() const { return region;};

  void setWireHighPtBoard(int port, TGCHighPtBoard* highPtBoard);
  void setStripHighPtBoard(TGCHighPtBoard* highPtBoard);
  int  getNumberOfWireHighPtBoard() const;
  void deleteHPBOut();

  TGCSSCController* getSSCController(){return &SSCController;};

  void setRPhiMap(const TGCRPhiCoincidenceMap* map, 
		  const TGCInnerCoincidenceMap* mapI=0);
  void setInnerMap(const TGCInnerCoincidenceMap* mapI);
  void setTileMuMap(const TGCTMDB* tmdb,
		    const TGCTileMuCoincidenceMap* mapTM);
  void showResult(TGCSLSelectorOut* out);
 
  TGCSectorLogic(TGCRegionType regionIn, int id);
  ~TGCSectorLogic();

  int getNumberOfSubSectorCluster() const; 
  int getNumberOfSubSector() const; 

  void dec2bin(int dec, char* binstr, int length);

  int getBid() const {return bid;};

  void setInnerTrackletSlots(const TGCInnerTrackletSlot* innerTrackletSlots[]);
  
protected: 
  enum {MaxNumberOfWireHighPtBoard =2};
  void collectInput();

  void doInnerCoincidence(int SSCId,  TGCRPhiCoincidenceOut* coincidenceOut);

private:
  TGCSectorLogic& operator=(const TGCSectorLogic& right);

private:

  int bid;

  int id;
  int sectorId, moduleId;
  int sideId, octantId;
  TGCRegionType region;
  int  NumberOfWireHighPtBoard;

  TGCSSCController SSCController;
  TGCRPhiCoincidenceMatrix matrix;
  const TGCInnerCoincidenceMap*  mapInner;
  const TGCTileMuCoincidenceMap*  mapTileMu;
  const TGCTMDB*            pTMDB;
  TGCSLPreSelector preSelector; 
  TGCSLSelector selector;
  TGCSLSelectorOut* selectorOut; 
  int wordTileMuon;
  int wordInnerStation;

  TGCHighPtBoard* wireHighPtBoard[MaxNumberOfWireHighPtBoard];
  TGCHighPtChipOut* wireHighPtChipOut[MaxNumberOfWireHighPtBoard];
  TGCHighPtBoard* stripHighPtBoard;
  TGCHighPtChipOut* stripHighPtChipOut;

  // for inner trigger
  const TGCInnerTrackletSlot* m_innerTrackletSlots[TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR];
  bool useInner;
  bool useTileMu;

  ToolHandle<ITGCTriggerDbTool> m_condDbTool; 
};

inline
 int TGCSectorLogic::getNumberOfSubSectorCluster() const
{
  if (region==FORWARD) return 8;
  else  return 19;
}

inline
 int TGCSectorLogic::getNumberOfSubSector() const
{
  if (region==FORWARD) return 8*8;
  else  return (18*8+4);
}

inline
 int  TGCSectorLogic::getNumberOfWireHighPtBoard() const
{
  return NumberOfWireHighPtBoard; 
}


inline 
 int TGCSectorLogic::getId() const
{
  return id;
}

inline
 int TGCSectorLogic::getModuleID() const
{
  return moduleId;
}


inline 
 int TGCSectorLogic::getOctantID() const
{
  return octantId;
}

inline 
 int TGCSectorLogic::getSideID() const
{
  return sideId;
}

inline   
 int  TGCSectorLogic::getTileMuonWord() const
{
  return wordTileMuon;
}

inline   
 int  TGCSectorLogic::getInnerStationWord() const
{
  return wordInnerStation;
}

#ifdef GENRPHIMAP
  extern std::ofstream rphimap;
#endif

} //end of namespace bracket

#endif
