/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibIdentifier/OfflineOnlineIdConversion.h"
#include "cstdlib"
namespace MuonCalib {

MuonFixedId OnlineToOffline(const std::string online_id)
	{
	std::cout<<online_id<<std::endl;
	MuonFixedId ret(0), invalid;
	ret.set_mdt();
//minimum size of idenifier is 7
	if (online_id.size()!=7 && online_id.size()!=9)
		{
		std::cout<<"WARNING: Wrong length"<<std::endl;
		return invalid;
		}
//check for A/C side
	bool c_side(false);
	if(online_id[4]!='A' && online_id[4]!='C' && online_id[4]!='B')
		{
		std::cout<<"WARNING: Invalid side"<<std::endl;
		return invalid;
		}
	if(online_id[4]=='C')
		{
		c_side=true;
		}
//set station name
	std::string station_nm_string(online_id, 0, 3);
	int station_mm = MuonFixedId::stationStringToFixedStationNumber(station_nm_string);
	if (station_mm<0)
		{
		std::cout<<"WARNING: Invalid station name"<<std::endl;
		return invalid;
		}
//set eta
	ret.setStationName(station_mm);
	char eta_str[2];
	char *endptr;
	eta_str[0]=online_id[3];
	eta_str[1]='\0';
	long int eta=strtol(eta_str, &endptr, 10);
	if(endptr!=&(eta_str[1]))
		{
		std::cout<<"WARNING: eta is not a number"<<std::endl;
		return invalid;
		}
//set phi
	char phi_str[3];
	phi_str[0]=online_id[5];
	phi_str[1]=online_id[6];
	phi_str[2]='\0';
	long int phi=strtol(phi_str, &endptr, 10);
	if(endptr!=&(phi_str[2]))
		{
		std::cout<<"WARNING: phi is not a number: "<<phi_str<<std::endl;
		return invalid;
		}
//treat special cases
	if(phi==13 && eta>3 && station_nm_string=="BML")
		{
		eta--;
		}
	if(station_nm_string=="BOF")
		eta=(eta+1)/2;
	else if(station_nm_string=="BOG")
		eta/=2;
	else 
		{
		if((phi==1 || phi==9) && station_nm_string=="EIL")
			{
			if(eta==5) eta=4;
			else if(eta==4) eta=5;
			}
		if(phi==5 && station_nm_string=="EEL") eta=1;
		}
//convert online to offline phi	
	if(station_nm_string[2]=='L' || station_nm_string[2]=='M' || station_nm_string[2]=='R')
		{
		phi++;
		}
	phi/=2;
	ret.setStationEta(c_side?(eta*-1):eta);
	ret.setStationPhi(phi);
//set multilayer id if present
	if(online_id.size()==9)
		{
		switch(online_id[8])
			{
			case '1':
				ret.setMdtMultilayer(1);
				break;
			case '2':
				ret.setMdtMultilayer(2);
				break;
			default:
				std::cout<<"WARNING: Multilayer is neither 1 or 2"<<std::endl;
				return invalid;
			}
		return ret.mdtMultilayerId();
		}	
	return ret.mdtChamberId();
	}


}
