/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTruthAlgs/TrackTruthSelector.h"

TrackTruthSelector::TrackTruthSelector(const std::string &name,ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_detailed("DetailedTrackTruth"),
  m_out("TrackTruthNew"),
  m_subDetWeights(SubDetHitStatistics::NUM_SUBDETECTORS, 1.)
{
  declareProperty("DetailedTrackTruthName",  m_detailed);
  declareProperty("OutputName",  m_out);

  declareProperty("WeightPixel",  m_subDetWeights[SubDetHitStatistics::Pixel]);
  declareProperty("WeightSCT",    m_subDetWeights[SubDetHitStatistics::SCT]);
  declareProperty("WeightTRT",    m_subDetWeights[SubDetHitStatistics::TRT]);
  declareProperty("WeightMDT",    m_subDetWeights[SubDetHitStatistics::MDT]);
  declareProperty("WeightRPC",    m_subDetWeights[SubDetHitStatistics::RPC]);
  declareProperty("WeightTGC",    m_subDetWeights[SubDetHitStatistics::TGC]);
  declareProperty("WeightCSC",    m_subDetWeights[SubDetHitStatistics::CSC]);
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackTruthSelector::initialize()
{
  ATH_MSG_INFO ("TrackTruthSelector::initialize()");
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackTruthSelector::finalize() {
  ATH_MSG_INFO ("TrackTruthSelector finalized");
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackTruthSelector::execute() {
  ATH_MSG_DEBUG ("TrackTruthSelector::execute()");

  //----------------------------------------------------------------
  // Retrieve the input

  if (!m_detailed.isValid()){
    ATH_MSG_WARNING ("DetailedTrackTruthCollection "<<m_detailed.name()<<" NOT found");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ("Got DetailedTrackTruthCollection "<<m_detailed.name());
  }


  //----------------------------------------------------------------
  // Produce and store the output.

  m_out = std::unique_ptr<TrackTruthCollection>(new TrackTruthCollection(m_detailed->trackCollectionLink()));

  fillOutput(m_out.ptr(), m_detailed.cptr());


  ATH_MSG_DEBUG ("TrackTruthCollection '" << m_out.name() << "' is registered in StoreGate, size="<<m_out->size());
  
  
  return StatusCode::SUCCESS;

}
//================================================================

void TrackTruthSelector::fillOutput(TrackTruthCollection *out, 
				    const DetailedTrackTruthCollection *in)
{
  typedef DetailedTrackTruthCollection::const_iterator Iter;
  Iter itrackData=in->begin();
  while(itrackData!=in->end()) {
    std::pair<Iter,Iter> range = in->equal_range(itrackData->first);

    // We KNOW that the range is not empty - no need to check that.
    Iter selected = range.first;
    double bestProb = getProbability(selected->second);
    ATH_MSG_VERBOSE ("track=" << selected->first.index() << " prob=" << bestProb << " link: " << *(selected->second.trajectory().rbegin()));
    for(Iter imatch = ++range.first; imatch != range.second; imatch++) {
      double prob = getProbability(imatch->second);
      ATH_MSG_VERBOSE ("track=" << imatch->first.index() << " prob=" << prob << " link: " << *(imatch->second.trajectory().rbegin()));
      if(prob>bestProb) {
	bestProb = prob;
	selected = imatch;
      }
    }

    // trajectory[0] is the LAST particle on the trajectory. The first
    // is at trajectory.rbegin(), but different trajectories can have
    // the same first particle.
    //    const HepMcParticleLink& particleLink = selected->second.trajectory()[0];
    const HepMcParticleLink& particleLink = *(selected->second.trajectory().rbegin());

    ATH_MSG_DEBUG ("Truth selected for track=" << selected->first.index() << " prob=" << bestProb << " link: " << particleLink);
    out->insert(std::make_pair(selected->first, TrackTruth(particleLink, bestProb, 0) ));
    itrackData=range.second;
  }

}

//================================================================
double TrackTruthSelector::getProbability(const DetailedTrackTruth& dt) const 
{
  double prd_track=0, prd_common=0;
  for(unsigned i=0; i<SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
    prd_common += m_subDetWeights[i] * dt.statsCommon()[SubDetHitStatistics::SubDetType(i)];
    prd_track += m_subDetWeights[i] * dt.statsTrack()[SubDetHitStatistics::SubDetType(i)];
  }
  return (prd_track>0)? prd_common/prd_track : -1.;
}

//================================================================
