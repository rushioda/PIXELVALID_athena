/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_MUONPRINTINGTOOL_H
#define REC_MUONPRINTINGTOOL_H

// Include files
#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"

#include <vector>
#include <string>
#include <fstream>  

// namespace Analysis {class Muon;}

namespace Muon {
  class MuonEDMPrinterTool;
}


namespace Rec {

  /** @class MuonPrintingTool
      @brief Tool to visualise muon structure, authors and content in logfile
             or extra dump text file, combining code from Staco/Mutag and the muon builder.
      @author Jean-Francois Laporte, Wolfgang Liebig, Niels van Eldik
  */
  class MuonPrintingTool : virtual public IMuonPrintingTool, public AthAlgTool {
  public:
  
    /** Standard Constructor */
    MuonPrintingTool(const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

    /** Standard destructor */
    virtual ~MuonPrintingTool( );

    /** Overriding initialize and finalize */
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** print muon object to a string */
    std::string print( const xAOD::Muon& muon ) const;

    /** print a muon container to a string */
    std::string print(const xAOD::MuonContainer& muons) const;

  private:
    std::string print( const xAOD::TrackParticle& tp ) const;
  
    ToolHandle<Muon::MuonEDMPrinterTool>	m_edmPrinter;
  
  };

}
#endif // REC_MUONPRINTINGTOOL_H
