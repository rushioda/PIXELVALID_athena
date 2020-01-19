/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_TIMINGTOOL_H
#define RPC_TIMINGTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"

/** @class RPC_TimingTool
    
    RPC_TimingTool calculates the time shift for a set of RPC hits WRT the current bunch

    @author MCP projects
*/


namespace Muon{

  class MuonClusterOnTrack;
  class MuonIdHelperTool;
  
  class RPC_TimingTool : virtual public Muon::IMuonHitTimingTool, public AthAlgTool{
  public:
    RPC_TimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor **/
    virtual ~RPC_TimingTool();

    /** standard initialization method **/
    virtual StatusCode initialize();
    
    /** standard finalization method **/
    virtual StatusCode finalize();

    /** Calculate the time offset of a given set of hits wrt to the current bunch */
    TimingResult calculateTimingResult( const std::vector<const MuonClusterOnTrack*>& hits ) const;

    /** return a set of technologies accepted by the tool */
    std::set<MuonStationIndex::TechnologyIndex> acceptedTechnologies() const { return std::set<MuonStationIndex::TechnologyIndex>({MuonStationIndex::RPC}); }

  private:
    /** calculate error on the RPC time */
    double getError(const Muon::MuonClusterOnTrack&) const ; 

    ToolHandle<MuonIdHelperTool> m_idHelper;

  };
}

#endif
