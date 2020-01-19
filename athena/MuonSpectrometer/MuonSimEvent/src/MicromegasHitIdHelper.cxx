/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include <iostream>

MicromegasHitIdHelper* MicromegasHitIdHelper::m_help = 0;

//private constructor
MicromegasHitIdHelper::MicromegasHitIdHelper() : HitIdHelper(){
	InitializeStationName();
  	Initialize();
}

MicromegasHitIdHelper* MicromegasHitIdHelper::GetHelper(){
	if (m_help==0) m_help = new MicromegasHitIdHelper();
	return m_help;
}

static char v1[] = {'M'};
static char v2[] = {'0','1','2','3','4'};
static char v3[] = {'S','L'};
static char v4[] = {'1','2'};

	
void MicromegasHitIdHelper::Initialize(){
 
 	InitializeField("PhiSector",1,16);
 	InitializeField("ZSector",0,4);
 	InitializeField("MultiLayer",1,2);
 	InitializeField("Layer",1,4);
	InitializeField("Side",-1,1);
	
}
void MicromegasHitIdHelper::InitializeStationName()
{
 	InitializeField("Station[1]",0,sizeof(v1));
 	InitializeField("Station[2]",0,sizeof(v2));
 	InitializeField("Station[3]",0,sizeof(v3));
	InitializeField("Station[4]",0,sizeof(v4));
}
void MicromegasHitIdHelper::SetStationName(std::string name)
{   
	for (unsigned int i=0;i<sizeof(v1);i++)
	    if (v1[i]==name[0]) SetFieldValue("Station[1]",i);
	for (unsigned int i=0;i<sizeof(v2);i++)
	    if (v2[i]==name[1]) SetFieldValue("Station[2]",i);
	for (unsigned int i=0;i<sizeof(v3);i++)
	    if (v3[i]==name[2]) SetFieldValue("Station[3]",i);
	for (unsigned int i=0;i<sizeof(v4);i++)
	    if (v4[i]==name[3]) SetFieldValue("Station[4]",i);	    				
}
std::string MicromegasHitIdHelper::GetStationName(const int& hid)
{
	this->SetID(hid);
	char v[5];
	v[0]=v1[this->GetFieldValue("Station[1]")];
	v[1]=v2[this->GetFieldValue("Station[2]")];
	v[2]=v3[this->GetFieldValue("Station[3]")];
	v[3]=v4[this->GetFieldValue("Station[4]")];
	v[4]='\0';
	std::string temp=v;
	return temp;
}

int MicromegasHitIdHelper::GetPhiSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("PhiSector");
}

int MicromegasHitIdHelper::GetZSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("ZSector");
}

//----Micromegas
int MicromegasHitIdHelper::GetMultiLayer(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("MultiLayer");
}
int MicromegasHitIdHelper::GetLayer(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("Layer");
}
int MicromegasHitIdHelper::GetSide(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("Side");
}


//packing method
int MicromegasHitIdHelper::BuildMicromegasHitId(const std::string statName, const int phiSect,
	const int zSect, const int multiLayer, const int layer, const int side){
	
	this->SetID(0);
	this->SetStationName(statName);
	this->SetFieldValue("PhiSector", phiSect);
	this->SetFieldValue("ZSector", zSect);
	this->SetFieldValue("MultiLayer", multiLayer);
	this->SetFieldValue("Layer", layer);
	this->SetFieldValue("Side", side);
	return this->GetID();
}

