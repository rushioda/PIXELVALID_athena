/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSlaveBoardOut_hh
#define TGCSlaveBoardOut_hh
#include "TrigT1TGC/TGCHitPattern.hh"
#include <fstream>
#include <iostream>

namespace LVL1TGCTrigger {

class TGCSlaveBoard;
const int MaxNumberOfSBData = 8;

const int posSize[6] = {5,5,4,5,4,4};//type = 0,1,2,3 : WT,WD,ST,SD,WI,SI
const int devSize[6] = {0,3,0,3,0,0};
const int devSign[6] = {0,1,0,1,0,0};
const int TotalNumberOfOutputData[6] = {3,2,8,2,8,8};

class TGCSlaveBoardOut {

public:
  TGCSlaveBoardOut(const TGCSlaveBoard* sb, int bidIn);
  TGCSlaveBoardOut(const TGCSlaveBoardOut &);
  TGCSlaveBoardOut();
  ~TGCSlaveBoardOut();
  TGCSlaveBoardOut& operator=(const TGCSlaveBoardOut &);

  int getDev(int iData) const;
  int getPos(int iData) const;
  TGCHitPattern* getbPos(int iData);
  bool getHit(int iData) const;

  void setDev(int iData, int devIn);
  void setPos(int iData, int posIn);
  void setHit(int iData, bool hitIn);
  void setbDev(int block, int sign, int dr);
  void setbPos(int block, int pos);

  void setNumberOfData(int nData);
  int getNumberOfData() const;
  int getBid() const;
  int getOrgBoardType() const;

  void clear();
  void print() const;
  void print(int OutPutBlock) const ;
  void print(std::ofstream* ofs) const ;
  void print(std::ofstream* ofs, int OutPutBlock) const;
  void printpattern(std::ofstream* ofs) const ;
  void printpattern(std::ofstream* ofs, int OutPutBlock) const;

  const TGCSlaveBoard* getOrigin() const;

private:
  const TGCSlaveBoard* origin;
  int bid;
  int orgBoardType;
  int orgSBid;

  int numberOfData; // number of block   A board have OutPutBlock:a,b ,numberOfData is 2.
  int dev[MaxNumberOfSBData]; // a b c [d [a b c d]]
  int pos[MaxNumberOfSBData];
  bool hit[MaxNumberOfSBData];

  TGCHitPattern* bpos[MaxNumberOfSBData]; // a b c d  [StripTriplet]a b c d
  TGCHitPattern* bdev[2];// 2=TotalNumberOfOutputData[1]

};

inline 
void TGCSlaveBoardOut::setbPos(int block, int pos)
{
  if (block<TotalNumberOfOutputData[orgBoardType]){
    bpos[block]->dec2bin(pos);
  } else {
    std::cerr << "TGCSlaveBoardOut::setbPos  : illegal block "
	      << " Type=" << orgBoardType 
	      << " #block=" << TotalNumberOfOutputData[orgBoardType]
	      << " block=" << block << std::endl;
  }
}

inline
void TGCSlaveBoardOut::setHit(int iData, bool hitIn)
{
  if (MaxNumberOfSBData <= iData){
     std::cerr << "internal error TGCSlaveBoardOut::setHit()" << std::endl;
  } else {
    hit[iData] = hitIn;
  }
}

inline 
void TGCSlaveBoardOut::setDev(int iData, int devIn)
{
  if(MaxNumberOfSBData <= iData) {
    std::cerr << "Internal error TGCSlaveBoardOut::setDev()" << std::endl;
  } else {
    dev[iData] = devIn;
  }
}

inline
TGCHitPattern* TGCSlaveBoardOut::getbPos(int iData)
{
  return bpos[iData];
}

inline
const TGCSlaveBoard* TGCSlaveBoardOut::getOrigin() const
{
  return origin;
}

inline
int TGCSlaveBoardOut::getDev(int iData) const
{
  return dev[iData];
}

inline
int TGCSlaveBoardOut::getPos(int iData) const
{
  return pos[iData];
}

inline
bool TGCSlaveBoardOut::getHit(int iData) const
{
  return hit[iData];
}

inline
int TGCSlaveBoardOut::getBid() const
{
  return bid;
}

inline
int TGCSlaveBoardOut::getOrgBoardType() const
{
  return orgBoardType;
}

inline
void TGCSlaveBoardOut::setNumberOfData(int nData) 
{
  numberOfData = nData;
}

inline
int TGCSlaveBoardOut::getNumberOfData() const 
{
  return numberOfData;
}


} //end of namespace bracket

#endif
