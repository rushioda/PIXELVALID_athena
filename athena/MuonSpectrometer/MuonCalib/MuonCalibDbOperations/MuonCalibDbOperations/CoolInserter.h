/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib__CoolInserter_h
#define MuonCalib__CoolInserter_h

//this
#include "MuonCalibDbOperations/IConditionsStorage.h"
#include "MuonCalibDbOperations/IMuonCalibConditionsSource.h"

//athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// COOL API: database service bootstrap
#include "CoolApplication/Application.h"
#include "CoolApplication/DatabaseSvcFactory.h"

// COOL API include files (CoolKernel)
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/StorageType.h"
#include "CoolKernel/types.h"
#include "CoolKernel/Record.h"


//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/NtupleStationId.h"

class RegionSelectionSvc;

namespace MuonGM {
  class MuonDetectorManager;
}

class MdtIdHelper;	
	
namespace MuonCalib {

class CalibDbConnection;
class CalibHeadOperations;

class CoolInserter : public AthAlgorithm {
 public:
//====================destructor - constructor=================================
  CoolInserter(const std::string& name, ISvcLocator* pSvcLocator);
  inline ~CoolInserter() {}
  /** Is called at the beginning of the analysis */
  StatusCode initialize();
  /** execute function NOTE: This will read all events at once*/
  StatusCode execute();	
  /** finalize functions */
  StatusCode finalize();
  bool CoolConnect(const std::string &connection_string, const std::string & tag_t0);
		
  bool StartT0Chamber(const NtupleStationId & sid);
  bool AppendT0(float t0, int validflag, float adc0);
  bool StoreT0Chamber(const NtupleStationId & id,  const std::string & file, unsigned int creation_flags=0);
  bool StoreRtChamber(const NtupleStationId & id, const std::map<int, SamplePoint> & points, const std::string & file, unsigned int creation_flags=0);
//====================interface functions======================================
 private:
//====================private data=============================================
  //version string - job option
  std::string m_version;
  //tags - job options
  std::vector<std::string> m_tagt0 ; // tag of the folder
  std::vector<std::string> m_tagrt; // tag of the folder
  //overwirte iov - job-option
  int m_iov_start, m_iov_end;
  long long m_iovt_start, m_iovt_end;
  //cool connection string - job option job option
  std::string m_cool_connection_string;
  //cool folders - job options
  std::vector<std::string> m_t0_folder;
  std::vector<std::string> m_rt_folder;
  //indicate if cool folders should be compressed if they are created 
  std::vector<bool> m_compressed_t0;
  std::vector<bool> m_compressed_rt;
  //calibration source tools - jo
  ToolHandleArray<IMuonCalibConditionsSource> m_calibration_sources;
  //region selection service - copy part of the calibration
  const RegionSelectionSvc *p_reg_sel_svc;
  //cool
  cool::IDatabasePtr m_db;
  std::vector<cool::IFolderPtr> m_mdtt0_fld;
  std::vector<cool::IFolderPtr> m_mdtrt_fld;
  cool::Application app;
  std::map<bool, coral::AttributeList> m_mdtt0_cool_row;
  std::map<bool, coral::AttributeList> m_mdtrt_cool_row;
  //flags for cool status
  bool m_cool_connect, m_t0_created, m_rt_created;	
  //already filled chambers - do not double fill
  std::set<NtupleStationId> m_t0_filled, m_rt_filled;
  const MdtIdHelper* m_mdtIdHelper;
  const MuonGM::MuonDetectorManager* m_detMgr;
  std::ostringstream m_data_string;
  int m_n_tubes_added;
  int m_n_tubes_chamber;
  float m_aver_t0, m_aver_adc;
  NtupleStationId m_sid;
  //create a new cool folder
  inline bool create_folder(const unsigned int & fldr_nr, const bool &is_rt);
};

} //namespace Muoncalib

#endif 
