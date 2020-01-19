/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonAlignEvent/MdtAlignModule.h"

namespace Muon {

  //________________________________________________________________________
  MdtAlignModule::MdtAlignModule(MsgStream* log, 
				 const Amg::Transform3D& transform)
    : CombinedMuonAlignModule(log,transform)
    , m_rootfile(0)
    , m_tubePositionTree(0)
    , m_log(new MsgStream(*log))
  {
  }

  //________________________________________________________________________
  MdtAlignModule::MdtAlignModule(const AlgTool* algtool,
				 const Amg::Transform3D& transform)
    : CombinedMuonAlignModule(algtool,transform)
    , m_rootfile(0)
    , m_tubePositionTree(0)
    , m_log(new MsgStream(algtool->msgSvc(),"MdtAlignModule"))
  {
    const AthAlgTool* athAlgTool=dynamic_cast<const AthAlgTool*>(algtool);
    if (athAlgTool) 
      m_log->setLevel(athAlgTool->msg().level());       
  }

  //________________________________________________________________________
  MdtAlignModule::~MdtAlignModule()
  {
    delete m_log;
  } 

  /*
  //________________________________________________________________________
  void MdtAlignModule::shiftSurface(Trk::TrkDetElementBase* det, Identifier tubeId) const { 
    (dynamic_cast<MuonGM::MdtReadoutElement*>(det))->shiftTube(tubeId); 
  }
  
  //________________________________________________________________________
  void MdtAlignModule::restoreSurfaces(Trk::TrkDetElementBase* det) const { 
    (dynamic_cast<MuonGM::MdtReadoutElement*>(det))->restoreTubes(); 
  }
  */

} // namespace
