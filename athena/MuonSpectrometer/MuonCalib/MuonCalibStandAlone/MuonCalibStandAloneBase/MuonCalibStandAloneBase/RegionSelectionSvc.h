/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RegionSelectionSvc_H
#define RegionSelectionSvc_H

//c - c++
#include "string"
#include "iostream"
#include "set"
#include "list"


//gaudi
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"

//MuonCalibITools
#include "MuonCalibITools/IIdToFixedIdTool.h"

#include "MuonCalibStandAloneBase/NtupleStationId.h"

namespace MuonCalib {

//this
class RegionSelectorBase;

//MuonCalibIdentifier
class MuonFixedId;

class IIdToFixedIdTool;
class RPCTowerId;
}

// interface to enable retrieving of a pointer to the singleton //
const InterfaceID IID_IRegionSelectionSvc("RegionSelectionSvc", 1, 0);

class Identifier; 
class StoreGateSvc; 
class MdtIdHelper;

/** @class RegionSelectionSvc
Seolect calibration region

@author rauscher@cern.ch
*/

class TDirectory;
class TChain;

class RegionSelectionSvc : public AthService
	{
	public:
	/**contructor */
		RegionSelectionSvc(const std::string & name, ISvcLocator *svc_locator);
	/**destructor */
		virtual ~RegionSelectionSvc();
 		static const InterfaceID& interfaceID() { return IID_IRegionSelectionSvc; }
	/** just some crazy atheta function */	
		virtual StatusCode queryInterface(const InterfaceID& riid, 
							void** ppvUnknown);
	/**initialize */
		StatusCode initialize();	
	/**finalize function */
		inline 	StatusCode finalize() {
		return StatusCode::SUCCESS;}
	/** debug print */
		void Print(std::ostream &os) const;
	/** return true if id is in selected region */
		bool isInRegion(const MuonCalib::MuonFixedId &id) const;
	/** get region selection */
		inline const std::string & GetRegionSelection() const
			{
			return m_region_string;
			}
	/** get stations/multilayers in region */
		inline const std::vector<MuonCalib :: NtupleStationId> & GetStationsInRegions() const
			{
			return m_stations_in_region;
			}
	/** add region ntuples which are relevant for this region to TChain */
		inline int AddRegionNtuples(const char *infile, TChain *chain) const
			{
			std::list<std::string> dirnames;
			return AddRegionNtuples(infile, chain, dirnames);
			}
		int AddRegionNtuples(const char *infile, TChain *chain, std::list<std::string> &dirnames) const;
	private:
	//! region string - job option
		std::string m_region_string;
	//! print list of selected chambers if set to true - job option
		bool m_print_list_of_selected_chambers;
	//!master region
		MuonCalib :: RegionSelectorBase * m_master_region;
	//! chambers/mutlilayers in calibraition region
		std::vector<MuonCalib::NtupleStationId> m_stations_in_region;
		std::set<MuonCalib::NtupleStationId> m_unique_chambers;
		ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;
		StoreGateSvc* m_detStore;
	//!towers in selected region
		const MdtIdHelper* m_mdtIdHelper;
		const MuonGM::MuonDetectorManager* m_detMgr;	//! search for chambers and multilayers in selected region
		inline void search_chambers_in_region();
	/** process string */
		inline bool ProcessString(const std::string & input);
	//! print a list of selected chambers - debug function
		void print_list_of_selected_chambers() const;
	//!convert station, eta, phi in tower vector
		void convert_to_tower(int station, int phi, int eta, std::vector<int> & tower) const ;
	};

#endif
