/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "MuonAlignEvent/TgcAlignModule.h"

namespace Muon {
    //________________________________________________________________________
    TgcAlignModule::TgcAlignModule( MsgStream* log,
                                    const Amg::Transform3D& transform )
        : CombinedMuonAlignModule( log, transform)
        , m_rootfile( 0 )
        , m_stripPositionTree( 0 )
        , m_wirePositionTree( 0 )
        , m_log( new MsgStream( *log ) )
    { }

    //________________________________________________________________________
    TgcAlignModule::TgcAlignModule( const AlgTool* algtool,
                                    const Amg::Transform3D& transform )
        : CombinedMuonAlignModule( algtool, transform )
        , m_rootfile( 0 )
        , m_stripPositionTree( 0 )
        , m_wirePositionTree( 0 )
        , m_log( new MsgStream( algtool->msgSvc(), "TgcAlignModule" ) )
    {
        const AthAlgTool* athAlgTool = dynamic_cast <const AthAlgTool*>( algtool );
        if ( athAlgTool ) {
            m_log->setLevel( athAlgTool->msg().level() );
        }
    }

    //________________________________________________________________________
    TgcAlignModule::~TgcAlignModule()
    {
        delete m_log;
    }

  //________________________________________________________________________
  /*void TgcAlignModule::shiftSurface(Trk::TrkDetElementBase* det, Identifier id) const { 
    std::cout<<"in TgcAlignModule::shiftSurface()"<<std::endl;
    (dynamic_cast<MuonGM::TgcReadoutElement*>(det))->shiftSurface(id); 
  }
  
  //________________________________________________________________________
  void TgcAlignModule::restoreSurfaces(Trk::TrkDetElementBase* det) const { 
    (dynamic_cast<MuonGM::TgcReadoutElement*>(det))->restoreSurfaces(); 
  }
  */
}
