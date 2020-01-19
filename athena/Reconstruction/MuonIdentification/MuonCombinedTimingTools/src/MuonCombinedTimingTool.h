/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonCombinedTimingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef RECMUONCOMBINEDTIMINGTOOL_H
#define RECMUONCOMBINEDTIMINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTimingTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/Muon.h"

namespace Trk { class ITrackTimingTool; }
// namespace Muon { class ISegmentTimingTool; }

namespace Rec 
{
  class IMuonPropertiesTool;

  /** @class MuonCombinedTimingTool 

      Tool to coordinate time-of-flight calculation for various types
      of muons. It analyses the type of measurements contributing to
      the muon and calls the appropriate detector tools.
      
      @author  Wolfgang Liebig <->
  */  

  class MuonCombinedTimingTool : virtual public IMuonCombinedTimingTool, public AthAlgTool
    {
    public:
      MuonCombinedTimingTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~MuonCombinedTimingTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const xAOD::Muon& muon) const;
      
    private:
      
      //! time calculation in subdetector responsibility: inner detector
      ToolHandle<Trk::ITrackTimingTool> m_trtTimingTool;
      //! time calculation in subdetector responsibility: calorimeter
      ToolHandle<Trk::ITrackTimingTool> m_caloTimingTool;
      //! time calculation in subdetector responsibility: muon spectrometer (T0 fit) for MDT and CSC
      ToolHandle<Trk::ITrackTimingTool> m_mdtSegmentTimingTool;
      //! time calculation in subdetector responsibility: muon spectrometer (RPC timing)
      ToolHandle<Trk::ITrackTimingTool> m_rpcTimingTool;

      virtual const Trk::SegmentCollection* getSegmentContainer(const xAOD::Muon& muon) const;
    }; 
} // end of namespace

#endif 
