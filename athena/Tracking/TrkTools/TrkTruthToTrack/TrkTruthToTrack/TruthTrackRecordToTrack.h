/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRECORDTOTRACK_H
#define TRUTHRECORDTOTRACK_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkParameters/TrackParameters.h"
#include <string>

namespace HepPDT { class ParticleDataTable; }
namespace HepMC  { class GenParticle;       }

namespace Trk {

  class IExtrapolator;

  /**
     @class TruthTrackRecordToTrack

     TruthTrackRecordToTrack is an AlgTool to create
     Trk::TrackParameters from a HepMC::GenParticle expressed
     at a "TrackRecordCollection" anywhere in ATLAS.  The purpose
     is to analyse non-collision particles (therefore uses TrackRecord,
     not GenParticle's prod vertex) in the reconstructed Track's
     reference frame. The code stems from old cosmics-testbeam analysis
     refurbished under the ITruthToTrack interface.

     @author Wolfgang.Liebig -at- cern.ch
   */

  class TruthTrackRecordToTrack : virtual public ITruthToTrack, public AthAlgTool {
  public:

    TruthTrackRecordToTrack(const std::string& type, const std::string& name, const IInterface* parent);
    virtual StatusCode initialize();

    // virtual ~TruthTrackRecordToTrack ()

    /** This function produces a Trk::TrackParameters object
     *  corresponding to the HepMC::GenParticle at the TrackRecord's
     *  layer.  The method is a TrackParameter factory (client obtains
     *  ownership!)
     */
    using ITruthToTrack::makeProdVertexParameters;
    virtual const Trk::TrackParameters* makeProdVertexParameters(const HepMC::GenParticle* part) const;
    virtual const Trk::TrackParameters* makeProdVertexParameters(const xAOD::TruthParticle* part) const;

    /** This function produces Trk::TrackParameters corresponding to
       MC truth and in addition extrapolates them to the perigee.
       The return object will be of flavour Trk::Perigee.
       Once more it employs factory design: clients need to take care
       of deleting the returned parameters object.
     */
    using ITruthToTrack::makePerigeeParameters;
    virtual const Trk::TrackParameters* makePerigeeParameters(const HepMC::GenParticle* part) const;
    virtual const Trk::TrackParameters* makePerigeeParameters(const xAOD::TruthParticle* part) const;

  private:
    const HepPDT::ParticleDataTable *m_particleDataTable;
    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    std::string   m_reccollkey;
    
    /** Forbid copying of Truth...ToTrack to not mess with the
        internal pointers. */ 
    TruthTrackRecordToTrack(const TruthTrackRecordToTrack&);
    TruthTrackRecordToTrack& operator=(const TruthTrackRecordToTrack&);
  };
}

#endif/*TRUTHRECORDTOTRACK_H*/
