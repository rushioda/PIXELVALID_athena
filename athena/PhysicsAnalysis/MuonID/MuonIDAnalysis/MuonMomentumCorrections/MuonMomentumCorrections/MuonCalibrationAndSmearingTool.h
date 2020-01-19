/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MUONCALIBRATIONANDMEARINGTOOL_H
#define MCAST_MUONCALIBRATIONANDMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

// ROOT include(s)
#include "TRandom3.h"

// C++ include(s)
#include <boost/unordered_map.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// Local include(s):
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"

#define EPSILON 1.0E-6
#define DEFAULT_INIT_VAL -999
#define MCAST_MAX_PT 100000000


namespace CP {

namespace MCAST {

  namespace DataType { enum { Data10 = 1, Data11 = 2, Data12 = 3, Data15 = 4, Data16=5}; }
  namespace AlgoType { enum { Muid = 1, Staco = 2, Muons = 3 }; }
  namespace Release { enum { Rel16_6 = 1, Rel17 = 2, Rel17_2 = 3, Rel17_2_Repro = 4, Rel17_2_Sum13 = 5, PreRec = 6, PreRec_2015_06_22  = 7, PreRec_2015_08_06  = 8, Rec_2015_11_15 = 9, Rec_2016_01_13 = 10, Rec_2016_01_19 = 11, PreRec_2016_05_23 = 12 , Recs2016_08_07=13 , Recs2016_15_07}; }
  namespace SmearingType { enum { Pt = 1, QoverPt = 2 }; }
  namespace DetectorType { enum { MS = 1, ID = 2, CB = 3 }; }
  namespace SystVariation { enum { Default = 0, Down = -1, Up = 1 }; }

}

class MuonCalibrationAndSmearingTool : public virtual IMuonCalibrationAndSmearingTool, public virtual ISystematicsTool, public asg::AsgTool {

  //::: Create a proper constructor for Athena
  ASG_TOOL_CLASS2( MuonCalibrationAndSmearingTool, CP::IMuonCalibrationAndSmearingTool, CP::ISystematicsTool )


public:

  //::: Constructor
  MuonCalibrationAndSmearingTool( const std::string& name );

  //::: Copy constructor
  MuonCalibrationAndSmearingTool( const MuonCalibrationAndSmearingTool& tool );

  virtual StatusCode initialize();

  //::: Apply the correction on a modifyable object
  virtual CorrectionCode applyCorrection( xAOD::Muon& mu );
  //::: Create a corrected copy from a constant muon
  virtual CorrectionCode correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output );

  //::: Is the tool affected by a specific systematic?
  virtual bool isAffectedBySystematic( const SystematicVariation& systematic ) const;
  //::: Which systematics have an effect on the tool's behaviour?
  virtual SystematicSet affectingSystematics() const;
  //::: Systematics to be used for physics analysis
  virtual SystematicSet recommendedSystematics() const;
  //::: Use specific systematic
  virtual SystematicCode applySystematicVariation ( const SystematicSet& systConfig );
  // Set seed for the random number generator
  void setRandomSeed( unsigned seed = 0 ) { m_random3.SetSeed( seed ); m_useExternalSeed = true;}
  double ExpectedResolution( const std::string& DetType, xAOD::Muon& mu, const bool mc = false ) const; //!< Expected resolution in data (or unsmeard MC if second argument is true)
  double ExpectedResolution( const int DetType, xAOD::Muon& mu, const bool mc = false ) const; //!< Expected resolution in data (or unsmeard MC if second argument is true)

protected:
  //::: Regions helpers 
  StatusCode   Regions( std::string inRegionFile, int doMacroRegionsFlag = 0 );
  void         PrintRegions() const;
  unsigned int GetNRegions() const;
  int          GetRegion( const double eta, const double phi ) const;
  float        GetRegionInnerEta( const int r_i ) const; //Return Eta closer to the origin
  std::string  GetRegionName( const int r_i ) const;
  std::string  GetRegionName( const double eta, const double phi ) const;
  double GetSmearing( int, xAOD::Muon& );
  double GetSystVariation( int DetType, double var );
  int GetScaleRegion( xAOD::Muon& );
  void CalcCBWeights( xAOD::Muon& );
  double CalculatePt( const int DetType, const double inSmearID = DEFAULT_INIT_VAL, const double inSmearMS = DEFAULT_INIT_VAL, const double scaleVar = 0. ) const;
  StatusCode FillValues();
  void Clean();
  double ScaleApply( const double pt, double S = 1.0, const double S_EnLoss = 0. ) const;
  //double ScaleApply( const double pt, const double S1, const double S2, double S = 1.0, const double S_EnLoss = 0. ) const;
  void CleanScales();
  void CollectMacroRegionsSL();//Small and large regions are collected together
  void CollectMacroRegionsSL_UpDn();//Small,Large,Up,Down regions are collected together
  void CollectMacroRegionsSL_SplitBAR();//Large,Small sectors split plus Feet(12+14) and 11+15 sector split in Barrel
  void CollectSectors();

  StatusCode SetData( std::string );
  StatusCode SetAlgorithm( std::string );
  StatusCode SetRelease( std::string );
  StatusCode SetType( std::string );

//private:
  //::: fake assignment operator missing actual implementation
  MuonCalibrationAndSmearingTool& operator=(const MuonCalibrationAndSmearingTool& );
  struct ParameterSet { 
    double SmearTypeID; 
    double SmearTypeMS; 
    double Scale; 
  };
  mutable TRandom3   m_random3;
  bool               m_useExternalSeed;
  double m_smearDeltaMS, m_smearDeltaID, m_smearDeltaCB;
  std::string m_year, m_algo, m_type, m_release;
  std::string m_FilesPath;
  bool m_toroidOff;
  int m_Tsmear;
  int m_Tdet;
  int m_Tdata;
  int m_Trel;
  int m_Talgo;
  int m_detRegion;
  double m_useNsigmaForICombine;
  double m_ptms, m_ptid, m_ptcb, m_eta, m_phi;
  std::vector<double> m_scale_ID, m_enLoss_MS, m_scale_MS, m_scale_CB;
  //sys variations (stat error added in quadrature), one if it's simmetrized, 2 if Up != Dw.
  std::vector<double> m_scaleSyst_ID, m_enLossSyst_MS, m_scaleSyst_MS, m_scaleSyst_CB;
  std::vector<double> m_scaleSystUp_ID, m_enLossSystUp_MS, m_scaleSystUp_MS;
  std::vector<double> m_scaleSystDw_ID, m_enLossSystDw_MS, m_scaleSystDw_MS;

  std::vector<double> m_scaleBins;
  std::vector<double> m_p1_ID, m_p2_ID, m_p2_ID_TAN, m_p0_MS, m_p1_MS, m_p2_MS;
  std::vector<double> m_E_p1_ID, m_E_p2_ID, m_E_p2_ID_TAN, m_E_p0_MS, m_E_p1_MS, m_E_p2_MS;
  // syst. errors on resolution parameters corrections:
  //one if it's simmetrized, then Stat and Sys err are separate in cfg file.
  std::vector<double> m_S_p1_ID, m_S_p2_ID, m_S_p2_ID_TAN, m_S_p0_MS, m_S_p1_MS, m_S_p2_MS;
  //Two if Up != Dw, Stat and Sys err added in quadrature in cfg file.
  std::vector<double> m_SUp_p1_ID, m_SUp_p2_ID, m_SUp_p2_ID_TAN, m_SUp_p0_MS, m_SUp_p1_MS, m_SUp_p2_MS;
  std::vector<double> m_SDw_p1_ID, m_SDw_p2_ID, m_SDw_p2_ID_TAN, m_SDw_p0_MS, m_SDw_p1_MS, m_SDw_p2_MS;
  std::vector<double> m_MC_p1_ID, m_MC_p2_ID, m_MC_p2_ID_TAN, m_MC_p0_MS, m_MC_p1_MS, m_MC_p2_MS;
  double m_weightMS, m_weightID;
  double m_g0, m_g1, m_g2, m_g3, m_g4, m_charge;

  std::vector<std::string> m_names;
  bool m_loadNames;
  int m_nb_regions;
  std::vector<float> m_eta_min, m_eta_max, m_phi_min, m_phi_max;
  
  bool m_doMacroRegions;
  std::map< int, int > m_MacroRegionIdxMap;
  std::vector< std::string > m_MacroRegionName;
  std::vector< double > m_MacroRegionInnerEta;

  int m_scaleRegion;

  boost::unordered_map< SystematicSet, ParameterSet > m_Parameters;
  ParameterSet *m_currentParameters;

}; //::: class MuonCalibrationAndSmearingTool

} //::: namespace CP

#endif
