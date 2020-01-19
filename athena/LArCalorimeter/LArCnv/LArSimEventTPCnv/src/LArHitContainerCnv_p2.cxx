/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "Identifier/Identifier.h"

#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "AthenaPoolCnvSvc/Compressor.h"

// LArHitContainerCnv_p2, used for T/P separation
// author Ilija Vukotic

#include "LArSimEventTPCnv/LArHitContainerCnv_p2.h"
#include "map"

void LArHitContainerCnv_p2::transToPers(const LArHitContainer* transCont, LArHitContainer_p2* persCont, MsgStream &log) 
{   //static int ev=0;

	size_t size = transCont->size();  
	log << MSG::DEBUG  << " ***  Writing LArHitContainer_p2 of size:"<<size<<endreq;
	
	persCont->m_channelHash.reserve(size);	
	std::vector<float> tempE;	tempE.reserve(size);
	std::vector<float> tempT;	tempT.reserve(size);
	
	const CaloCell_ID* cellIdHelper = (CaloIdManager::instance())->getCaloCell_ID();// getDM_ID 
	

	LArHitContainer::const_iterator it  = transCont->begin();
	
	std::multimap <unsigned int, unsigned int> map_hashPositions;// first hash ; second its position in container

	for (unsigned int w=0;w<size;++w){
                IdentifierHash hashId = cellIdHelper->calo_cell_hash((*it)->cellID());
		map_hashPositions.insert(std::pair<unsigned int, int>((unsigned int)hashId, w));
//		if (!ev) std::cout<<hashId<<"\t"<<((*it)->cellID())<<std::endl;
		++it;
		}

	std::multimap<unsigned int, unsigned int>::const_iterator iter;
	unsigned int old=0;
//	int count=0;
	for (iter=map_hashPositions.begin(); iter != map_hashPositions.end(); ++iter) {
		//cout <<"container position:"<< iter->second << " \t hash:" << iter->first << endl;
		unsigned int pHash=(iter->first)-old; // to store as a difference 
		old=iter->first;
		unsigned int pos=iter->second;
		persCont->m_channelHash.push_back(pHash);
		tempE.push_back( (float) (transCont->At(pos))->energy() );
		tempT.push_back( (float) (transCont->At(pos))->time()   );
//		if (!ev) std::cout<<"Writing Hash: "<<iter->first<<"\t E: "<< (float) (transCont->At(pos))->m_energy<<"\t T: "<< (float) (transCont->At(pos))->m_time<<std::endl;
//		count++;
		}			
//	std::cout<<"ILIJA : "<<count<<std::endl;
	
	Compressor A; A.setNrBits(18);	
	A.reduce(tempE,persCont->m_energy); // packs energy
	Compressor B;
	B.reduceToUS(tempT, persCont->m_time);	
	persCont->m_name = transCont->Name(); //stores name
	
//	ev++;
}



void LArHitContainerCnv_p2::persToTrans(const LArHitContainer_p2* persCont, LArHitContainer* transCont, MsgStream &log) 
{
//	static int dog=0;
	size_t cells=persCont->m_channelHash.size();
	log << MSG::DEBUG  << " ***  Reading LArHitContainer of size: "<<cells<<endreq;
	transCont->clear();
	transCont->reserve(cells);
	transCont->setName(persCont->name() );
	
	
	const CaloCell_ID* cellIdHelper = (CaloIdManager::instance())->getCaloCell_ID();// getDM_ID 
	
	
	Compressor A;
	std::vector<float> tempE;	tempE.reserve(cells);
	std::vector<float> tempT;	tempT.reserve(cells);
	
	A.expandFromUStoFloat(persCont->m_time,tempT);
	A.expandToFloat(persCont->m_energy,tempE);
	unsigned int sum=0;
	for (unsigned int i=0;i<cells;++i){
		sum+= persCont->m_channelHash[i];
		LArHit* trans=new LArHit
                  (cellIdHelper->cell_id(sum),
                   tempE[i],
                   tempE[i] != 0 ? (double)(tempT[i])/tempE[i] : 0);
//		if(!dog) std::cout<<"Reading hash: "<< sum <<"\t E: "<< (double)tempE[i]<<"\t T: "<<(tempT[i]) <<std::endl;
		transCont->push_back(trans);
		}
//	dog++;
}
 
