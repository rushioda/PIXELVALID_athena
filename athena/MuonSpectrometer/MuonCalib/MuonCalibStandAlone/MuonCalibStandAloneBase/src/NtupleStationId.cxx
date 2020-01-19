/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibIdentifier/MdtRegion.h"
#include "Identifier/IdContext.h"

//geo model
#include "MuonIdHelpers/MdtIdHelper.h"

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"


namespace MuonCalib {

bool NtupleStationId :: Initialize(const std::string & station, const int & eta, const int & phi, const int & ml, const int & author)
	{
	MuonFixedId id;
	if(station=="ANY")
		{
		m_station=-1;
		}
	else
		{
		m_station=id.stationStringToFixedStationNumber(station);
		if(m_station<0) return false;
		}
	m_eta=eta;
	m_phi=phi;
	m_ml=ml;
	m_author=author;
	m_region_id_valid = false;
	geom_ok=false;
	return true;
	}

void NtupleStationId :: SetStation(const std::string & station)
	{
	MuonFixedId id;
	m_station=id.stationStringToFixedStationNumber(station);
	m_region_id_valid = false;
	}


bool NtupleStationId :: InitializeGeometry(const MdtIdHelper* mdtIdHelper, const MuonGM::MuonDetectorManager* detMgr)
	{
	MuonFixedId fid;
	if(m_station == -1 || m_phi < 0 || m_eta == -99)
		{
		std::cerr<<" NtupleStationId :: InitializeGeometry: Cannot initialize geometry for multi station id"<<std::endl;
		geom_ok=false;
		return false;
		}
//	std::cout<<m_phi<<" "<<m_eta<<std::endl;
	Identifier id(mdtIdHelper->elementID(fid.stationNumberToFixedStationString(m_station), m_eta, m_phi));
//	std::cout<<"Station id is: "<<mdtIdHelper->print_to_string(id)<<std::endl;
	n_ml = mdtIdHelper->numberOfMultilayers(id);
//	const MuonGM::MdtReadoutElement* detEl = detMgr->getMdtReadoutElement(mdtIdHelper->channelID(id,1,1,1));
//	n_ml=detEl->getMultilayer();
//loop on multilayers
	for(int i=0; i<n_ml; i++)
		{
		const MuonGM::MdtReadoutElement* detEl_ml = detMgr->getMdtReadoutElement(mdtIdHelper->channelID(id,1+i ,1,1));
		layer_min[i]=1;
		if (detEl_ml==NULL)
			{
			std::cout<<regionId()<<" ml "<<i<< "does not exist in current geometry"<<std::endl;
			return false;
			}
		layer_max[i]=detEl_ml->getNLayers();
		n_layer[i] = layer_max[i] - layer_min[i] +1;
		tube_min[i]= 1;
		tube_max[i]= detEl_ml->getNtubesperlayer();
		n_tubes[i] = tube_max[i] - tube_min[i] +1;
		}
	MdtBasicRegionHash hash;
	IdContext idCont = mdtIdHelper->module_context();
	mdtIdHelper->get_hash( id, hash, &idCont );
	region_hash = static_cast<int>(hash);
	geom_ok = true;
	return true;
	}


void NtupleStationId :: createRegionId() const
	{
	MuonFixedId id;
	std::ostringstream id_stream;
	if(m_station<0)
		{
		id_stream<<"ANY";
		}
	else
		{
		id_stream<<id.stationNumberToFixedStationString(m_station);
		}
	id_stream<<"_";
	if(m_phi >= 0)
		{
		id_stream<<m_phi;
		}
	else
		{
		id_stream<<"ANY";
		}
	id_stream<<"_";
	if(m_eta!=-99)
		{
		id_stream<<m_eta;
		}
	else
		{
		id_stream<<"ANY";
		}
	if(m_ml>0)
		{
		id_stream<<"_"<<m_ml;
		}
	m_region_id = id_stream.str();
	m_region_id_valid = true;
	}


int NtupleStationId ::  FixedId() const
	{
	if(m_station<0 || m_eta==-99 || m_phi<0) return -1;
	MuonFixedId id;
	id.set_mdt();
	if(!id.setStationName(m_station)) return -1;
	if(!id.setStationEta(m_eta)) return -1;
	if(!id.setStationPhi(m_phi)) return -1;
/*	id.setMdtTubeIndex(0);
	id.setMdtTubeLayerIndex(0);
	id.setMdtMultilayerIndex(0);*/
	return id.mdtChamberId().getIdInt();
	}

}
