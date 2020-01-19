/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonStationNtupleHelperTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMUONSTATIONNTUPLEHELPERTOOL_H
#define MUONMUONSTATIONNTUPLEHELPERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h" // this tool is not correctly interfaced
#include "GaudiKernel/ToolHandle.h"

// class MdtIdHelper;
// class TgcIdHelper;
// class CscIdHelper;
// class MdtIdHelper;

namespace Muon 
{

  /** @class MuonStationNtupleHelperTool 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Wolfgang Liebig <wolfgang.liebig -at- cern.ch>
  */  

  class MuonStationNtupleHelperTool : virtual public Trk::IValidationNtupleHelperTool, public AthAlgTool
    {
    public:
      MuonStationNtupleHelperTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~MuonStationNtupleHelperTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      //! fill hit position data
      virtual StatusCode fillMeasurementData (
          const Trk::MeasurementBase*,
          const Trk::TrackParameters*,
          const int& detectorType,
          const bool& isOutlier ) const;

      //! fill special data about holes on track (here: do nothing)
      virtual StatusCode fillHoleData (
          const Trk::TrackStateOnSurface&,
          const int&) const;


      /** add items to the ntuple and configure the helper tool:
      should be called once (per detector type) by the steering tool (Trk::IValidationNtupleTool) */
      virtual StatusCode addNtupleItems (
          TTree* tree,
          const int& detectorType ) const;

      //! reset ntuple variables
      virtual StatusCode resetVariables (
          const int& detectorType ) const;

    private:

      ToolHandle<MuonIdHelperTool> m_muonIdHelperTool;
 
      mutable std::vector<int>* m_mdtSectorIx;  //!< sector number 1-16, odd=large, even=small
      mutable std::vector<int>* m_mdtStationIx;  //!< describe 
      mutable std::vector<int>* m_rpcSectorIx;  //!< sector number 1-16, odd=large, even=small
      mutable std::vector<int>* m_rpcStationIx;  //!< describe 
      mutable std::vector<int>* m_rpcMeasuresPhi; //!< describe
      mutable std::vector<int>* m_tgcSectorIx;  //!< sector number 1-16, odd=large, even=small
      mutable std::vector<int>* m_tgcStationIx;  //!< describe 
      mutable std::vector<int>* m_tgcMeasuresPhi; //!< describe
    }; 
} // end of namespace

#endif 
