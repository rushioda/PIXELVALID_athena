/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SiSPSeededTrackFinder_H
#define SiSPSeededTrackFinder_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h" 
#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "TrkSpacePoint/SpacePointContainer.h" 

// For new strategy reconstruction
#include "TrkTrack/TrackCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/PerigeeSurface.h" 
#include "StoreGate/DataHandle.h"

//class SpacePointContainer;
namespace InDet {

// forward declare of these interfaces does not work in opt build
//   class ISiSpacePointsSeedMaker;
//   class ISiZvertexMaker;
//   class ISiTrackMaker; 


  // Class-algorithm for track finding in Pixels and SCT
  // initiated by space points seeds
  // 
  class SiSPSeededTrackFinder : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      SiSPSeededTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~SiSPSeededTrackFinder() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     
      bool                           m_useZvertexTool     ; 
      bool                           m_useMBTS            ;
      bool                           m_useNewStrategy     ;
      bool                           m_useZBoundaryFinding;
      bool                           m_ITKGeometry        ; // Is it ITK geometry
      int                            m_outputlevel        ; // Print level for debug
      int                            m_nprint             ; // Kind of  print    
      int                            m_nseeds             ; // Number seeds
      int                            m_ntracks            ; // Number found tracks
      int                            m_nseedsTotal        ; // Number seeds
      int                            m_ntracksTotal       ; // Number found tracks
      int                            m_neventsTotal       ; // Number events 
      int                            m_neventsTotalV      ; // Number events 
      int                            m_problemsTotal      ; // Numbe revents with number seeds > maxNumber
      int                            m_problemsTotalV     ; // Numbe revents with number seeds > maxNumber
      int                            m_maxNumberSeeds     ; // Max. number used seeds
      int                            m_maxPIXsp           ; // Max. number pixels space points
      int                            m_maxSCTsp           ; // Max. number sct    space points
      int                            m_nfreeCut           ; // Min number free clusters

      SG::ReadHandle<SpacePointContainer> m_SpacePointsSCT  ;
      SG::ReadHandle<SpacePointContainer> m_SpacePointsPixel;
      SG::WriteHandle<TrackCollection>    m_outputTracks    ;

      ToolHandle< ISiSpacePointsSeedMaker > m_seedsmaker    ;  // Space poins seed     maker
      ToolHandle< ISiZvertexMaker         > m_zvertexmaker  ;  // Space poins z-vertex maker
      ToolHandle< ISiTrackMaker           > m_trackmaker    ;  // Track                maker     
    
      // For new strategy reconstruction
      //
      int*                           m_nhistogram;
      int                            m_histsize  ;
      int                            m_nvertex   ;
      double                         m_pTcut     ;
      double                         m_imcut     ;
      double                         m_zstep     ;
      double                         m_zcut      ;
      double*                        m_zhistogram;
      double*                        m_phistogram;

      std::string                    m_beamconditions          ;
      std::string                    m_fieldmode               ; 
      IBeamCondSvc*                                 m_beam     ;
      ToolHandle<Trk::IPatternParametersPropagator> m_proptool ;
      Trk::MagneticFieldProperties                  m_fieldprop;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////
      
      bool isGoodEvent();
      double trackQuality(const Trk::Track*);
      void filterSharedTracks(std::multimap<double,Trk::Track*>&);
      void fillZHistogram(const Trk::Track*,Trk::PerigeeSurface&);
      void findZvertex(std::list<Trk::Vertex>&,double*); 
      StatusCode  oldStrategy();
      StatusCode  newStrategy();
      void magneticFieldInit();

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const SiSPSeededTrackFinder&);
  std::ostream& operator << (std::ostream&,const SiSPSeededTrackFinder&); 
}
#endif // SiSPSeededTrackFinder_H
