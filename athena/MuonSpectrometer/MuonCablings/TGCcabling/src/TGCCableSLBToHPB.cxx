/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCableSLBToHPB.h"
#include "TGCcabling/TGCChannelSLBOut.h"
#include "TGCcabling/TGCChannelHPBIn.h"

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCableSLBToHPB::TGCCableSLBToHPB (std::string filename)
  : TGCCable(TGCCable::SLBToHPB)
{
  database[TGCIdBase::Endcap][TGCIdBase::WT] = 
    new TGCDatabasePPToSL(filename,"SB EWT");
  database[TGCIdBase::Endcap][TGCIdBase::WD] = 
    new TGCDatabasePPToSL(filename,"SB EWD");
  database[TGCIdBase::Endcap][TGCIdBase::ST] =
    new TGCDatabasePPToSL(filename,"SB EST");
  database[TGCIdBase::Endcap][TGCIdBase::SD] = 
    new TGCDatabasePPToSL(filename,"SB ESD");
  database[TGCIdBase::Forward][TGCIdBase::WT] = 
    new TGCDatabasePPToSL(filename,"SB FWT");
  database[TGCIdBase::Forward][TGCIdBase::WD] =
    new TGCDatabasePPToSL(filename,"SB FWD");
  database[TGCIdBase::Forward][TGCIdBase::ST] = 
    new TGCDatabasePPToSL(filename,"SB FST");
  database[TGCIdBase::Forward][TGCIdBase::SD] = 
    new TGCDatabasePPToSL(filename,"SB FSD");
}

TGCCableSLBToHPB::~TGCCableSLBToHPB (void)
{
  delete database[TGCIdBase::Endcap][TGCIdBase::WT];
  delete database[TGCIdBase::Endcap][TGCIdBase::WD];
  delete database[TGCIdBase::Endcap][TGCIdBase::ST];
  delete database[TGCIdBase::Endcap][TGCIdBase::SD];
  delete database[TGCIdBase::Forward][TGCIdBase::WT];
  delete database[TGCIdBase::Forward][TGCIdBase::WD];
  delete database[TGCIdBase::Forward][TGCIdBase::ST];
  delete database[TGCIdBase::Forward][TGCIdBase::SD];
}

TGCChannelId* TGCCableSLBToHPB::getChannel (const TGCChannelId* channelId,
					    bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCChannelId::SLBOut)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCChannelId::HPBIn)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}

TGCModuleMap* TGCCableSLBToHPB::getModule (const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::SLB)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::HPB)
      return getModuleIn(moduleId);
  }
  return 0;
}

TGCChannelId* TGCCableSLBToHPB::getChannelIn (const TGCChannelId* hpbin,
					      bool orChannel) const {
  if(orChannel) return 0;
  if(hpbin->isValid()==false) return 0;
  TGCChannelSLBOut* slbout = 0;

  // HPB module
  TGCModuleId* hpb = hpbin->getModule();
  if(!hpb) return 0;
  
  // HPB -> SLB module connection
  TGCModuleMap* mapId = getModule(hpb);
  delete hpb;
  if(!mapId) return 0;
  
  int numOfBlock = TGCChannelSLBOut::getNumberOfBlock(TGCIdBase::WD);//SD
  int chInBlock = TGCChannelSLBOut::getChannelInBlock(TGCIdBase::WD);//SD
  int slbInBlock = TGCChannelHPBIn::getSlbInBlock();

  // SLB module
  TGCModuleId* slb = 0;
  int blockInHPB = hpbin->getBlock();
  int nSlb       = hpbin->getChannel()/(numOfBlock*2); // half block
  int port       = blockInHPB*slbInBlock+nSlb;
  int size = mapId->size();
  for(int i=0;i<size;i++){
    if(mapId->connector(i)==port){
      slb = mapId->popModuleId(i);
      break;
    }
  }
  
  delete mapId;
  if(!slb) return 0;
  
  // HPB ->SLB channel connection
  int nInSlb     = hpbin->getChannel()/(numOfBlock*2); // half block
  int blockInSlb = nInSlb/2;
  int nInBlock   = nInSlb%2;
  int channel    = (chInBlock/2)*nInBlock;
  
  slbout = new TGCChannelSLBOut(slb->getSideType(),
				slb->getModuleType(),
				slb->getRegionType(),
				slb->getSector(),
				slb->getId(),
				blockInSlb,
				channel);
  
  delete slb;
  return slbout;
}

TGCChannelId* TGCCableSLBToHPB::getChannelInforHPB (const TGCChannelId* hpbin,
						    TGCIdBase::ModuleType moduleType,
						    bool orChannel) const {
  if(orChannel) return 0;
  if(hpbin->isValid()==false) return 0;
  TGCChannelSLBOut* slbout = 0;

  // HPB module
  TGCModuleId* hpb = hpbin->getModule();//This function is defined in TGCChannelHPBIn
                                        //set current SideType,SignalType... 
                                        //ChannelID(HPBIn) is set in HPBIn constructor
  if(!hpb) return 0;

  // HPB -> SLB module connection
  TGCModuleMap* mapId = getModuleInforHPB(hpb,moduleType);
  delete hpb;
  if(!mapId) return 0;
  
  // need to check
  int numOfBlock = TGCChannelSLBOut::getNumberOfBlock(moduleType);
  int chInBlock = TGCChannelSLBOut::getChannelInBlock(moduleType);
  int slbInBlock = 0;
    
  // SLB module
  TGCModuleId* slb = 0;
  int blockInHPB = 0;
  int nSlb = 0; // half block
  int port = 0; // input position for HPT

  // HPB ->SLB channel connection
  int blockInSlb = 0;
  int posInHpb = 0;
  int channel = 0;
  int size = 0;
  switch(moduleType){

  case TGCIdBase::WD:
  case TGCIdBase::SD:
    slbInBlock = 3;    
    blockInHPB = hpbin->getBlock();
    nSlb = hpbin->getChannel()/(numOfBlock*2);
    port = blockInHPB*slbInBlock+nSlb;
    size = mapId->size();
    for(int i=0;i<size;i++){
      if(mapId->connector(i)==port){
	slb = mapId->popModuleId(i);
	break;
      }
    }
    blockInSlb = (hpbin->getChannel() - nSlb*(numOfBlock*2))/2;
    posInHpb = (hpbin->getChannel() - (numOfBlock*2)*nSlb)%2;
    channel    = (chInBlock/2)*posInHpb;
    break;
  
  case TGCIdBase::WT:
    slbInBlock = 2;
    blockInHPB = hpbin->getBlock();
    nSlb = hpbin->getChannel()/(numOfBlock*2);
    port = blockInHPB*slbInBlock+nSlb;
    size = mapId->size();
    for(int i=0;i<size;i++){
      if(mapId->connector(i)== -1*port){//triplet: - sign
	slb = mapId->popModuleId(i);
	break;
      }
    }
    blockInSlb = (hpbin->getChannel() - nSlb*(numOfBlock*2))/2;
    posInHpb = (hpbin->getChannel() - (numOfBlock*2)*nSlb)%2;
    channel    = (chInBlock/2)*posInHpb;
    break;

  case TGCIdBase::ST:
    blockInHPB = hpbin->getBlock();
    if(blockInHPB==0 && hpbin->getChannel()<numOfBlock) port=1;
    else port=2;
    size = mapId->size();
    for(int i=0;i<size;i++){
      if(mapId->connector(i)== -1*port){//triplet: - sign
	slb = mapId->popModuleId(i);
	break;
      }
    }
    blockInSlb = (hpbin->getChannel())%numOfBlock;
    if(blockInHPB == 1 && hpbin->getChannel() >= numOfBlock) blockInSlb = blockInSlb + numOfBlock/2;
    channel = 0;
    break;

  default:
    break;

  }

  delete mapId; mapId = 0;
  if(!slb) return 0;

  const int offset =6; //offset to get center of block
  channel += offset;
  
  int SLBID = slb->getId();
  if((chInBlock*blockInSlb+channel)>chInBlock*numOfBlock){
    SLBID++;
    channel = channel % chInBlock;
    blockInSlb = 0;
  } else if( channel > chInBlock){
    channel = channel % chInBlock;
    blockInSlb++;
  }

  slbout = new TGCChannelSLBOut(slb->getSideType(),
				moduleType,
				slb->getRegionType(),
				slb->getSector(),
				SLBID,
				blockInSlb,
				channel);
 
  delete slb;
  return slbout;
}


TGCChannelId* TGCCableSLBToHPB::getChannelOut (const TGCChannelId* slbout,
					       bool orChannel) const {
  if(orChannel) return 0;
  if(slbout->isValid()==false) return 0;
  if(slbout->getMultipletType()==TGCIdBase::Triplet) return 0;
  TGCChannelHPBIn* hpbin = 0;
  
  // SLB module
  TGCModuleId* slb = slbout->getModule();
  if(!slb) return 0;
  
  // SLB -> HPB module connection
  TGCModuleMap* mapId = getModule(slb);
  delete slb;
  if(!mapId) return 0;
  
  // HPB module
  int port = mapId->connector(0);
  TGCModuleId* hpb = mapId->popModuleId(0);
  delete mapId;
  if(!hpb) return 0;

  int chInBlock = TGCChannelSLBOut::getChannelInBlock(slbout->getModuleType());
  int slbInBlock = TGCChannelHPBIn::getSlbInBlock();
 
  // SLB ->HPB channel connection
  int block   = port/slbInBlock;
  int hitId   = (port%slbInBlock)*2+slbout->getBlock();
  int pos     = slbout->getChannel()/chInBlock;
  int channel = hitId+pos;

  hpbin = new TGCChannelHPBIn(hpb->getSideType(),
                              hpb->getSignalType(),
                              hpb->getRegionType(),
                              hpb->getSector(),
                              hpb->getId(),
			      block,
                              channel);
  delete hpb;

  return hpbin;
}

TGCModuleMap* TGCCableSLBToHPB::getModuleIn (const TGCModuleId* hpb) const {
  if(hpb->isValid()==false) return 0;

  TGCIdBase::ModuleType doublet=TGCIdBase::NoModuleType;
  TGCIdBase::ModuleType triplet=TGCIdBase::NoModuleType;
  if(hpb->getSignalType()==TGCIdBase::Wire){
    doublet = TGCIdBase::WD;
    triplet = TGCIdBase::WT;
  }
  if(hpb->getSignalType()==TGCIdBase::Strip){
    doublet = TGCIdBase::SD;
    triplet = TGCIdBase::ST;
  }

  if(doublet==TGCIdBase::NoModuleType || triplet==TGCIdBase::NoModuleType) { 
    return 0; 
  } 
  
  TGCDatabase* doubletP = database[hpb->getRegionType()][doublet];
  TGCDatabase* tripletP = database[hpb->getRegionType()][triplet];
  
  TGCModuleMap* mapId = 0;
  int MaxEntry = doubletP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(doubletP->getEntry(i,1)==hpb->getId())
      {
	int id = doubletP->getEntry(i,0);
	int block = doubletP->getEntry(i,2);
	TGCModuleSLB* slb = new TGCModuleSLB(hpb->getSideType(),
					     doublet,
					     hpb->getRegionType(),
					     hpb->getSector(),
					     id);
	if(mapId==0) mapId = new TGCModuleMap();
	mapId->insert(block,slb);
      } 
  }
  MaxEntry = tripletP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(tripletP->getEntry(i,1)==hpb->getId())
      {
	int id = tripletP->getEntry(i,0);
	int block = -tripletP->getEntry(i,2);
	TGCModuleSLB* slb = new TGCModuleSLB(hpb->getSideType(),
					     triplet,
					     hpb->getRegionType(),
					     hpb->getSector(),
					     id);
	if(mapId==0) mapId = new TGCModuleMap();
	mapId->insert(block,slb);
      } 
  }
  
  return mapId;
}

TGCModuleMap* TGCCableSLBToHPB::getModuleInforHPB (const TGCModuleId* hpb, 
						   TGCIdBase::ModuleType moduleType) const {
  if(hpb->isValid()==false) return 0;

  TGCDatabase* databaseP = database[hpb->getRegionType()][moduleType];

  TGCModuleMap* mapId = 0;
  int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,1)==hpb->getId())
      {
	int id = databaseP->getEntry(i,0);
	int block = -100;
	switch(moduleType){
	case TGCIdBase::WD:
	case TGCIdBase::SD:
	block = databaseP->getEntry(i,2);
	break;

	case TGCIdBase::WT:
	case TGCIdBase::ST:
	block = -1*databaseP->getEntry(i,2);// check - sign
	break;

	default:
	break;
	}
	TGCModuleSLB* slb = new TGCModuleSLB(hpb->getSideType(),
					     moduleType,
					     hpb->getRegionType(),
					     hpb->getSector(),
					     id);
	if(mapId==0) mapId = new TGCModuleMap();
	mapId->insert(block,slb);
      } 
  }
  
  return mapId;
}

TGCModuleMap* TGCCableSLBToHPB::getModuleOut (const TGCModuleId* slb) const {
  if(slb->isValid()==false) return 0;

  TGCDatabase* databaseP =database[slb->getRegionType()][slb->getModuleType()];
  
  TGCModuleMap* mapId = 0;
  int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,0)==slb->getId())
      {
	int id = databaseP->getEntry(i,1);
	int block = databaseP->getEntry(i,2);
	if(slb->getMultipletType()==TGCIdBase::Triplet) 
	  block *=-1;
	
	TGCModuleHPB* hpb = new TGCModuleHPB(slb->getSideType(),
					     slb->getSignalType(),
					     slb->getRegionType(),
					     slb->getSector(),
					     id);
	
	mapId = new TGCModuleMap();
	mapId->insert(block,hpb);
	break;
      } 
  }
  
  return mapId;
}

} //end of namespace
