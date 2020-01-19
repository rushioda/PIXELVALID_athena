/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// OutwardsCombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in 
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP using the ITrackFitter interface.
// 
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>

#include "MuidTrackBuilder/OutwardsCombinedMuonTrackBuilder.h"

#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/Track.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "VxVertex/RecVertex.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "AthenaKernel/Units.h"

namespace Units = Athena::Units;

namespace Rec
{
    
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

OutwardsCombinedMuonTrackBuilder::OutwardsCombinedMuonTrackBuilder (const std::string&	type,
								    const std::string&	name, 
								    const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_cleaner		(""),
	m_fitter		(""),
	m_trackSummary		(""),
        m_muonHoleRecovery      (""),
        m_muonErrorOptimizer    (""),
        m_trackingVolumesSvc            ("TrackingVolumesSvc/TrackingVolumesSvc",name),
        m_calorimeterVolume       (0),
        m_indetVolume             (0),
	m_allowCleanerVeto	(true),
	m_cleanCombined		(true),
	m_recoverCombined	(false),
        m_IDMS_xySigma          (1.*Gaudi::Units::mm),
        m_IDMS_rzSigma          (1.*Gaudi::Units::mm),
        m_addIDMSerrors          (true)


{
    declareInterface<ICombinedMuonTrackBuilder>(this);
    declareInterface<Trk::ITrackFitter>(this);
    declareProperty("Cleaner",			m_cleaner);
    declareProperty("Fitter",			m_fitter);
    declareProperty("TrackSummaryTool",		m_trackSummary);
    declareProperty("MuonHoleRecovery",         m_muonHoleRecovery);
    declareProperty("AllowCleanerVeto",		m_allowCleanerVeto);
    declareProperty("CleanCombined",		m_cleanCombined);
    declareProperty("RecoverCombined",		m_recoverCombined);
    declareProperty("MuonErrorOptimizer",       m_muonErrorOptimizer);
    declareProperty("IDMS_xySigma" ,            m_IDMS_xySigma);
    declareProperty("IDMS_rzSigma" ,            m_IDMS_rzSigma);
    declareProperty("AddIDMSerrors",            m_addIDMSerrors);


}

OutwardsCombinedMuonTrackBuilder::~OutwardsCombinedMuonTrackBuilder (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
OutwardsCombinedMuonTrackBuilder::initialize()
{
    ATH_MSG_INFO( "Initializing OutwardsCombinedMuonTrackBuilder"
		   << " - package version " << PACKAGE_VERSION );
    msg(MSG::INFO) << " with options: ";
    if (m_allowCleanerVeto)	msg(MSG::INFO) << " AllowCleanerVeto";
    if (m_cleanCombined)	msg(MSG::INFO) << " CleanCombined";
    msg(MSG::INFO) << endmsg;

    if (!m_cleaner.empty()){
    if (m_cleaner.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_cleaner );
        return StatusCode::FAILURE;
    }
    else
    {
        ATH_MSG_INFO( "Retrieved tool " << m_cleaner );
    }
    }
    if (! m_muonHoleRecovery.empty())
    {
        if (m_muonHoleRecovery.retrieve().isFailure())
        {
            ATH_MSG_FATAL( "Failed to retrieve tool " << m_muonHoleRecovery );
            return StatusCode::FAILURE;
        }
        else
        {
            ATH_MSG_INFO( "Retrieved tool " << m_muonHoleRecovery );
        }
    }

    if (! m_muonErrorOptimizer.empty())
    {
        if (m_muonErrorOptimizer.retrieve().isFailure())
        {
            ATH_MSG_FATAL( "Failed to retrieve tool " << m_muonErrorOptimizer );
            return StatusCode::FAILURE;
        }
        else
        {
            ATH_MSG_INFO( "Retrieved tool " << m_muonErrorOptimizer );
        }
    }


    if (!m_fitter.empty()){
    if (m_fitter.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_fitter );
        return StatusCode::FAILURE;
    }
    else
    {
        ATH_MSG_INFO( "Retrieved tool " << m_fitter );
    }
    }
    if (!m_trackSummary.empty()){
    if (m_trackSummary.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackSummary );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackSummary );
    }
    }

    if (m_trackingVolumesSvc.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve Svc " << m_trackingVolumesSvc );
        return StatusCode::FAILURE;
    }
    else
    {
        ATH_MSG_DEBUG( "Retrieved Svc " << m_trackingVolumesSvc );
        m_calorimeterVolume     = new Trk::Volume(
            m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));

        m_indetVolume           = new Trk::Volume( m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
    }
         

    return StatusCode::SUCCESS;
}

StatusCode
OutwardsCombinedMuonTrackBuilder::finalize()
{
 
    
    return StatusCode::SUCCESS;
}

/**ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::combinedFit (const Trk::Track& indetTrack,
					       const Trk::Track& /*extrapolatedTrack*/,
				       const Trk::Track& spectrometerTrack) const
{
    ATH_MSG_VERBOSE( "combinedFit:: " );


    
    Trk::Track* combinedTrack	= fit(indetTrack, spectrometerTrack, m_cleanCombined, Trk::muon);

 
    // add the track summary
   
    if (combinedTrack) m_trackSummary->updateTrack(*combinedTrack);
    //if (combinedTrack) std::cout << "in builder, track: " << *combinedTrack << " summary: " << *combinedTrack->trackSummary() << std::endl;
    return combinedTrack;
}

/**ICombinedMuonTrackBuilder interface:
   build and fit indet track extended to include MS Measurement set.
   Adds material effects as appropriate plus calo energy-loss treatment */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::indetExtension (const Trk::Track&	      indetTrack,
						  const Trk::MeasurementSet&  spectrometerMeas,
						  const Trk::TrackParameters* /*innerParameters*/,
						  const Trk::TrackParameters* /*middleParameters*/,
						  const Trk::TrackParameters* /*outerParameters*/) const
{
    ATH_MSG_VERBOSE( "indetExtension fit::" );
    DataVector<const Trk::TrackStateOnSurface> *trajectory=new DataVector<const Trk::TrackStateOnSurface>; 
    for (int i=0;i<(int)spectrometerMeas.size();i++) {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typeM;
      typeM.set(Trk::TrackStateOnSurface::Measurement);
//  Only measurements so not needed     if(spectrometerMeas[i]->alignmentEffectsOnTrack()) continue;
      Trk::TrackStateOnSurface *tsos=new Trk::TrackStateOnSurface(spectrometerMeas[i]->clone(),0,0,0,typeM);
      trajectory->push_back(tsos);
    }
    Trk::TrackInfo trackInfo(Trk::TrackInfo::Unknown,Trk::muon);
    Trk::Track inputtrack2(trackInfo,trajectory,0);
    Trk::Track *track=fit(indetTrack,inputtrack2,m_cleanCombined, Trk::muon);    
    return track;
}

/**ICombinedMuonTrackBuilder interface:
   propagate to perigee adding calo energy-loss and material to MS track */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::standaloneFit	(const Trk::Track&	/*spectrometerTrack*/,
						 const Trk::Vertex*	/*vertex*/,
                                                 float /*bs_x*/, float /*bs_y*/, float /*bs_z*/ ) const
{
  return 0;
}

/**ICombinedMuonTrackBuilder interface:
   refit a track removing any indet measurements with optional addition of pseudoMeasurements
       according to original extrapolation */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::standaloneRefit (const Trk::Track& combinedTrack,
                                                   float bs_x, float bs_y, float bs_z ) const
{

    ATH_MSG_DEBUG( " start OutwardsCombinedMuonTrackBuilder standaloneRefit" );

    ATH_MSG_DEBUG(" beam position bs_x " << bs_x << " bs_y " << bs_y << " bs_z " << bs_z );

    double vertex3DSigmaRPhi = 6.0;
    double vertex3DSigmaZ    = 60.0;
    
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =
	new DataVector<const Trk::TrackStateOnSurface>;
 
    bool addVertexRegion = true;  
    Amg::Vector3D origin(bs_x,bs_y,bs_z);
    AmgSymMatrix(3)  vertexRegionCovariance;
    vertexRegionCovariance.setZero();
    vertexRegionCovariance(0,0)		= vertex3DSigmaRPhi*vertex3DSigmaRPhi;
    vertexRegionCovariance(1,1)		= vertex3DSigmaRPhi*vertex3DSigmaRPhi;
    vertexRegionCovariance(2,2)		= vertex3DSigmaZ*vertex3DSigmaZ;
    Trk::RecVertex vertex(origin, vertexRegionCovariance);
    
//    if(!combinedTrack.perigeeParameters()) return 0;
    int itsos = 0;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator t = combinedTrack.trackStateOnSurfaces()->begin();
    // create perigee TSOS
    for ( ; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
      itsos++;
      if((**t).alignmentEffectsOnTrack()) continue;
      if((**t).type(Trk::TrackStateOnSurface::Perigee)) {
        const Trk::TrackParameters* pars = (**t).trackParameters();
        if(pars) {
          const Trk::TrackStateOnSurface* TSOS = const_cast<const Trk::TrackStateOnSurface*>((**t).clone());
          trackStateOnSurfaces->push_back(TSOS);
           
    // including vertex region pseudoMeas 
          if (addVertexRegion) {
  	    const Trk::PseudoMeasurementOnTrack* vertexInFit = vertexOnTrack(pars,vertex);
	    if (vertexInFit) {
	      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
	      type.set(Trk::TrackStateOnSurface::Measurement);
	      trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface(vertexInFit,
									       0,
									       0,
									       0,
									       type) );
              ATH_MSG_DEBUG( " found Perigee and added vertex " << itsos );
	    }
          }
          break;	    
        }
      }
    }
//
// add ID Eloss
//
      double Eloss = 0.;
      for ( ; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
        itsos++;
        if((**t).alignmentEffectsOnTrack()) continue;
        if(!(**t).trackParameters()) continue;
        if((**t).materialEffectsOnTrack()) {
          if(!m_indetVolume->inside((**t).trackParameters()->position())) break;
          double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();
          const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**t).materialEffectsOnTrack());
          if(meot) {
            const Trk::EnergyLoss* energyLoss = meot->energyLoss();
            if (energyLoss) {
              Eloss += fabs(energyLoss->deltaE());
              ATH_MSG_DEBUG("OutwardsCombinedMuonFit ID Eloss found r " << ((**t).trackParameters())->position().perp() << " z " << ((**t).trackParameters())->position().z() << " value " << energyLoss->deltaE() << " Eloss " << Eloss);
	      const Trk::ScatteringAngles* scat = meot->scatteringAngles();
	      if(scat) {
		double sigmaDeltaPhi = scat->sigmaDeltaPhi();     
		double sigmaDeltaTheta = scat->sigmaDeltaTheta();     
		const Trk::EnergyLoss* energyLossNew = new Trk::EnergyLoss(energyLoss->deltaE(),energyLoss->sigmaDeltaE(),energyLoss->sigmaDeltaE(),energyLoss->sigmaDeltaE());
		const Trk::ScatteringAngles* scatNew = new Trk::ScatteringAngles(0.,0.,sigmaDeltaPhi,sigmaDeltaTheta);
		const Trk::Surface& surfNew = (**t).trackParameters()->associatedSurface();
		std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
		meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
		meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);
		const Trk::MaterialEffectsOnTrack*  meotNew = new Trk::MaterialEffectsOnTrack(X0, scatNew, energyLossNew, surfNew, meotPattern);
		const Trk::TrackParameters* parsNew = ((**t).trackParameters())->clone();
		std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
		typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);
		const Trk::TrackStateOnSurface* newTSOS  = new Trk::TrackStateOnSurface(0, parsNew, 0, meotNew, typePatternScat);
		trackStateOnSurfaces->push_back(newTSOS); 
	      }
	    }
          }
        }
      }
      ATH_MSG_DEBUG("OutwardsCombinedMuonFit Total ID Eloss " << Eloss << " nr of states " << itsos);

// add Calo and MS TSOSs 

      for ( ; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
        itsos++;
        if((**t).alignmentEffectsOnTrack()) continue;
        const Trk::TrackStateOnSurface* TSOS = const_cast<const Trk::TrackStateOnSurface*>((**t).clone());
        trackStateOnSurfaces->push_back(TSOS);
       }   
       ATH_MSG_DEBUG( " trackStateOnSurfaces found " << trackStateOnSurfaces->size() <<  " from total " << itsos );

    Trk::Track* standaloneTrack = new Trk::Track(combinedTrack.info(), trackStateOnSurfaces, 0);
    standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandaloneRefit);
    Trk::Track* refittedTrack   = fit(*standaloneTrack,false,Trk::muon);
    delete standaloneTrack;

    if (! refittedTrack) {
      ATH_MSG_DEBUG( " OutwardsCombinedMuonTrackBuilder standaloneRefit FAILED " );
      return 0;
    }
    ATH_MSG_DEBUG( " OutwardsCombinedMuonTrackBuilder standaloneRefit OK " );
    m_trackSummary->updateTrack(*refittedTrack);
    return refittedTrack;
}

/**ITrackFitter interface: refit a track */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::fit (const Trk::Track&		track,
			       const Trk::RunOutlierRemoval	runOutlier,
			       const Trk::ParticleHypothesis	particleHypothesis) const
{
    // check valid particleHypothesis
    if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting)
    {
	ATH_MSG_WARNING( " invalid particle hypothesis " << particleHypothesis
			 << " requested. Must be 0 or 2 (nonInteracting or muon) " );
	return 0;
    }

   
    ToolHandle<Trk::ITrackFitter> fitter = m_fitter;
    
   
   
    
    // fit
    Trk::Track* fittedTrack = fitter->fit(track, false, particleHypothesis);
    if (! fittedTrack)	return 0;

    // track cleaning
    if (runOutlier)
    {

    // fit with optimized spectrometer errors
        if (! m_muonErrorOptimizer.empty() && ! fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) {
	  ATH_MSG_VERBOSE( " perform spectrometer error optimization before cleaning " );
  	  Trk::Track* optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack);
	  if (optimizedTrack) {
	    delete fittedTrack;
	    fittedTrack = optimizedTrack;
	  }
        }
 

	// muon cleaner
	ATH_MSG_VERBOSE( " perform track cleaning... " );
        Trk::Track* cleanTrack = m_cleaner->clean(*fittedTrack);
	if (! cleanTrack)
	{
	    ATH_MSG_DEBUG( " cleaner veto " );
	    
	    if (m_allowCleanerVeto)
	    {
		delete fittedTrack;
		fittedTrack = 0;
	    }
	}
        else if (cleanTrack != fittedTrack)
        {
	    ATH_MSG_VERBOSE( " found and removed spectrometer outlier(s) " );

            delete fittedTrack;
            fittedTrack = cleanTrack;
        }

	//FIXME: provide indet cleaner
	ATH_MSG_VERBOSE( " finished cleaning" );
    }
    
    return fittedTrack;
}

/**ITrackFitter interface:
   fit a set of MeasurementBase objects with starting value for perigeeParameters */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::fit (const Trk::MeasurementSet&	/*measurementSet*/,
				       const Trk::TrackParameters&	/*perigeeStartValue*/,
				       const Trk::RunOutlierRemoval	/*runOutlier*/,
				       const Trk::ParticleHypothesis	/*particleHypothesis*/) const
{
    return 0;
}


/**ITrackFitter interface:  combined muon fit */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::fit (const Trk::Track&		indetTrack,
				       const Trk::Track&		extrapolatedTrack,
				       const Trk::RunOutlierRemoval	runOutlier,
				       const Trk::ParticleHypothesis	particleHypothesis) const
{
    // check valid particleHypothesis
    if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting)
    {
	ATH_MSG_WARNING( " invalid particle hypothesis " << particleHypothesis
			 << " requested. Must be 0 or 2 (nonInteracting or muon) " );
	return 0;
    }

    ToolHandle<Trk::ITrackFitter> fitter = m_fitter;
    
    // fit
    Trk::Track* fittedTrack = fitter->fit(indetTrack, extrapolatedTrack, false, particleHypothesis);
    if (! fittedTrack)	return 0;
    if( !fittedTrack->perigeeParameters() ) {
      ATH_MSG_WARNING( " Fitter returned a track without perigee, failing fit");
      delete fittedTrack;
      return 0;
    }
    // track cleaning
    if (runOutlier)
    {

    // fit with optimized spectrometer errors
        if (! m_muonErrorOptimizer.empty() && ! fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) {
	  ATH_MSG_VERBOSE( " perform spectrometer error optimization before cleaning " );
  	  Trk::Track* optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack);
	  if (optimizedTrack) {
	    delete fittedTrack;
	    fittedTrack = optimizedTrack;
	  }
        }
	// muon cleaner
	ATH_MSG_VERBOSE( " perform track cleaning... " );
        Trk::Track* cleanTrack = m_cleaner->clean(*fittedTrack);
	if (! cleanTrack)
	{
	    ATH_MSG_DEBUG( " cleaner veto " );
	    
	    if (m_allowCleanerVeto)
	    {
		delete fittedTrack;
		fittedTrack = 0;
	    }
	}
        else if (cleanTrack != fittedTrack)
        {
	    ATH_MSG_VERBOSE( "  found and removed spectrometer outlier(s) " );

            delete fittedTrack;
            fittedTrack = cleanTrack;
        }

	ATH_MSG_VERBOSE( " finished cleaning" );
    }
    if(fittedTrack){ 
       Trk::Track* newTrack = addIDMSerrors(fittedTrack);  
       if(newTrack) {
         Trk::Track* refittedTrack   = fit(*newTrack,false,Trk::muon);
	 if(newTrack != fittedTrack)
	   delete newTrack;
         if(refittedTrack) {
           if(refittedTrack->fitQuality()) {
             delete fittedTrack;
             fittedTrack = refittedTrack;
           } else {
             delete refittedTrack;
           } 
         }
       }
    }
    if (m_recoverCombined && fittedTrack){
      Trk::Track* recoveredTrack = m_muonHoleRecovery->recover(*fittedTrack);
      double oldfitqual=fittedTrack->fitQuality()->chiSquared()/fittedTrack->fitQuality()->numberDoF();
      if (recoveredTrack && recoveredTrack!=fittedTrack){
        double newfitqual=recoveredTrack->fitQuality()->chiSquared()/recoveredTrack->fitQuality()->numberDoF();
        if (newfitqual<oldfitqual || newfitqual<1.5 || (newfitqual<2 && newfitqual<oldfitqual+.5)){
          delete fittedTrack;
          fittedTrack=recoveredTrack;
          /*if (runOutlier) { 
          Trk::Track* cleanTrack = m_cleaner->clean(*fittedTrack);
          if (cleanTrack && cleanTrack!=fittedTrack){
            delete fittedTrack;
            fittedTrack=cleanTrack;
          }
          }*/
        }
        else delete recoveredTrack;
      }   
    }
    if( fittedTrack && !fittedTrack->perigeeParameters() ) {
      ATH_MSG_WARNING( " Fitter returned a track without perigee, failing fit");
      delete fittedTrack;
      return 0;
    }
    if (runOutlier && fittedTrack) { 
      double fitqual=fittedTrack->fitQuality()->chiSquared()/fittedTrack->fitQuality()->numberDoF();
      if (fitqual>5 || (fittedTrack->perigeeParameters()->pT()<20000 && fitqual>2.5)) {
        delete fittedTrack;
        fittedTrack=0;
      }
      else {
	  //        double prevp=0;
        DataVector<const Trk::TrackStateOnSurface>::const_iterator itStates = fittedTrack->trackStateOnSurfaces()->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator endStates = fittedTrack->trackStateOnSurfaces()->end();
        for (;itStates!=endStates;itStates++) {
          if ((*itStates)->materialEffectsOnTrack()) {
            const Trk::MaterialEffectsOnTrack *meot=dynamic_cast<const Trk::MaterialEffectsOnTrack *>((*itStates)->materialEffectsOnTrack());
            if(!meot) continue;
            if (meot->scatteringAngles() && !meot->energyLoss()) {
              double pullphi=std::abs(meot->scatteringAngles()->deltaPhi()/meot->scatteringAngles()->sigmaDeltaPhi());
              double pulltheta=std::abs(meot->scatteringAngles()->deltaTheta()/meot->scatteringAngles()->sigmaDeltaTheta());
              if (pullphi>7 || pulltheta>7) {
                delete fittedTrack;
                fittedTrack=0;
                break;
              }
            }
            /* if (!meot->scatteringAngles() && meot->energyLoss()) {
              double eloss=std::abs(meot->energyLoss()->deltaE())-prevp+std::abs(1/(*itStates)->trackParameters()->parameters()[Trk::qOverP]);
              double sigmae= (eloss<0) ? meot->energyLoss()->sigmaPlusDeltaE() : meot->energyLoss()->sigmaDeltaE();
              //std::cout << "eloss: " << eloss << " sigma: " << sigmae << " pull: " << std::abs(eloss)/sigmae << std::endl; 
              if (std::abs(eloss)/sigmae>3) {
                delete fittedTrack;
                fittedTrack=0;
                break;
              }
            } */
          }
          // if ((*itStates)->trackParameters()) prevp=std::abs(1/(*itStates)->trackParameters()->parameters()[Trk::qOverP]);
        }
      }
    }
    // final fit with optimized spectrometer errors 
    if (! m_muonErrorOptimizer.empty() && fittedTrack && ! fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) { 
      ATH_MSG_VERBOSE( " perform spectrometer error optimization... " ); 
      Trk::Track* optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack); 
      if (optimizedTrack) { 
        delete fittedTrack; 
        fittedTrack = optimizedTrack; 
      } 
    } 
    return fittedTrack;
}
    

Trk::Track*  OutwardsCombinedMuonTrackBuilder::addIDMSerrors(Trk::Track* track) const
{
//
// take track and correct the two scattering planes in the Calorimeter 
// to take into account m_IDMS_rzSigma and m_IDMS_xySigma 
//
// returns a new Track
//   
    if(!m_addIDMSerrors) return track;

    ATH_MSG_VERBOSE( " OutwardsCombinedMuonTrackBuilder addIDMSerrors to track ");
    Amg::Vector3D positionMS(0,0,0);
    Amg::Vector3D positionCaloFirst(0,0,0);
    Amg::Vector3D positionCaloLast(0,0,0);
    int itsos = 0;
    int itsosCaloFirst = -1;
    int itsosCaloLast = -1;
    double p = -1.;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator t = track->trackStateOnSurfaces()->begin();
    for ( ; t != track->trackStateOnSurfaces()->end(); ++t) {
      itsos++;
      if((**t).trackParameters()) {
        if(p==-1.) p = (**t).trackParameters()->momentum().mag()/Units::GeV;
        if(m_indetVolume->inside((**t).trackParameters()->position())) {
          continue;
        }

        if((**t).trackParameters()->position().mag()<1000) continue;
 
        if(m_calorimeterVolume->inside((**t).trackParameters()->position())) {
// first scattering plane in Calorimeter 
          if((**t).type(Trk::TrackStateOnSurface::Scatterer)&&(**t).materialEffectsOnTrack()) {
            double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();
            if(X0<10) continue;
            if(itsosCaloFirst!=-1) { 
              itsosCaloLast = itsos;
              positionCaloLast = (**t).trackParameters()->position(); 
            }
            if(itsosCaloFirst==-1) {
              itsosCaloFirst = itsos;
              positionCaloFirst = (**t).trackParameters()->position(); 
            }
          }
        }
        if(!m_calorimeterVolume->inside((**t).trackParameters()->position())) {
          if((**t).measurementOnTrack()){
// inside muon system 
            positionMS = (**t).trackParameters()->position(); 
            break;
          }
        }
      }
    }

// it can happen that no Calorimeter Scatterers are found.

    ATH_MSG_DEBUG( " OutwardsCombinedMuonTrackBuilder addIDMSerrors p muon GeV " << p << " First Calorimeter Scatterer radius " <<  positionCaloFirst.perp() << " z " << positionCaloFirst.z() << " Second Scatterer r " << positionCaloLast.perp() << " z " << positionCaloLast.z() << " First Muon hit r " << positionMS.perp() << " z " << positionMS.z() );

   if(itsosCaloFirst<0||itsosCaloLast<0) { 
     ATH_MSG_DEBUG( " addIDMSerrors keep original track ");
     return track;
   }
// If no Calorimeter no IDMS uncertainties have to be propagated 
   positionCaloFirst = positionCaloFirst - positionMS;
   positionCaloLast  = positionCaloLast  - positionMS;

   double sigmaDeltaPhiIDMS2   = m_IDMS_xySigma / (positionCaloFirst.perp()+positionCaloLast.perp());
   double sigmaDeltaThetaIDMS2 = m_IDMS_rzSigma / (positionCaloFirst.mag()+positionCaloLast.mag()) ;
   sigmaDeltaPhiIDMS2   *= sigmaDeltaPhiIDMS2;
   sigmaDeltaThetaIDMS2 *= sigmaDeltaThetaIDMS2;
   DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;
   trackStateOnSurfaces->reserve(track->trackStateOnSurfaces()->size());

    t = track->trackStateOnSurfaces()->begin();
    itsos = 0;
    for ( ; t != track->trackStateOnSurfaces()->end(); ++t) {
      itsos++;
      if((**t).alignmentEffectsOnTrack()) continue;
      if(itsos==itsosCaloFirst||itsos==itsosCaloLast){
        if((**t).materialEffectsOnTrack()) {
          double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();
          const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**t).materialEffectsOnTrack());
          if(meot) {
            const Trk::ScatteringAngles* scat = meot->scatteringAngles();
            if(scat) {
              double sigmaDeltaPhi = sqrt((scat->sigmaDeltaPhi())*(scat->sigmaDeltaPhi()) + sigmaDeltaPhiIDMS2);     
              double sigmaDeltaTheta = sqrt((scat->sigmaDeltaTheta())*(scat->sigmaDeltaTheta()) + sigmaDeltaThetaIDMS2);     
              const Trk::EnergyLoss* energyLossNew = new Trk::EnergyLoss(0.,0.,0.,0.);
              const Trk::ScatteringAngles* scatNew = new Trk::ScatteringAngles(0.,0.,sigmaDeltaPhi,sigmaDeltaTheta);
              const Trk::Surface& surfNew = (**t).trackParameters()->associatedSurface();
              std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
              meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
              meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);
              const Trk::MaterialEffectsOnTrack*  meotNew = new Trk::MaterialEffectsOnTrack(X0, scatNew, energyLossNew, surfNew, meotPattern);
              const Trk::TrackParameters* parsNew = ((**t).trackParameters())->clone();
              std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
              typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);
              const Trk::TrackStateOnSurface* newTSOS  = new Trk::TrackStateOnSurface(0, parsNew, 0, meotNew, typePatternScat);
              trackStateOnSurfaces->push_back(newTSOS); 
              ATH_MSG_DEBUG( " old Calo scatterer had sigmaDeltaPhi mrad      " << scat->sigmaDeltaPhi()*1000 << " sigmaDeltaTheta mrad " << scat->sigmaDeltaTheta()*1000 << " X0 " << X0 );
              ATH_MSG_DEBUG( " new Calo scatterer made with sigmaDeltaPhi mrad " << sigmaDeltaPhi*1000 << " sigmaDeltaTheta mrad " << sigmaDeltaTheta*1000 );
            } else {
              ATH_MSG_WARNING( " This should not happen: no Scattering Angles for scatterer ");
            }
          } else {
            ATH_MSG_WARNING( " This should not happen: no MaterialEffectsOnTrack for scatterer ");
          }
        } 
      } else { 
        const Trk::TrackStateOnSurface* TSOS = const_cast<const Trk::TrackStateOnSurface*>((**t).clone());
        trackStateOnSurfaces->push_back(TSOS);
      }
    }   
    ATH_MSG_DEBUG( " trackStateOnSurfaces on input track " << track->trackStateOnSurfaces()->size() << " trackStateOnSurfaces found " << trackStateOnSurfaces->size());

    Trk::Track* newTrack = new Trk::Track(track->info(), trackStateOnSurfaces, 0);
    return newTrack;
}

Trk::PseudoMeasurementOnTrack*
OutwardsCombinedMuonTrackBuilder::vertexOnTrack(const Trk::TrackParameters*	parameters,
      					        const Trk::RecVertex&		vertex) const
{
    // create the corresponding PerigeeSurface, localParameters and covarianceMatrix
    const Trk::PerigeeSurface surface(vertex.position());
    Trk::LocalParameters localParameters;
    Amg::MatrixX covarianceMatrix;
    covarianceMatrix.setZero();
    // transform Cartesian (x,y,z) to beam axis or perigee
    Amg::Vector2D localPosition(0,0);
    double ptInv			=  1./parameters->momentum().perp();
    localParameters			=  Trk::LocalParameters(localPosition);
    Amg::MatrixX jacobian(2,3);
    jacobian.setZero();
    jacobian(0,0)			= -ptInv*parameters->momentum().y();
    jacobian(0,1)			=  ptInv*parameters->momentum().x();
    jacobian(1,2)			=  1.0;
    const Amg::MatrixX& cov         =  vertex.covariancePosition();
    covarianceMatrix		=  cov.similarity(jacobian);
    
    
    return new Trk::PseudoMeasurementOnTrack(localParameters,
					     covarianceMatrix,
					     surface);
}


}	// end of namespace
