/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfBetheHeitlerEffects.cxx  -  description
      ------------------------------------------
begin                : Sunday 20th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for the Bethe-Heitler material effects
                       in the GSF package
*********************************************************************************/

#include "TrkGaussianSumFilter/GsfBetheHeitlerEffects.h"
#include "TrkGeometry/MaterialProperties.h"
#include "PathResolver/PathResolver.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/Layer.h"

#include <fstream>
#include <cmath>

namespace {
	template<class T>
	bool inRange(const T& var,const T& lo, const T& hi){
		return ((var <= hi) and (var>=lo));
	}
}

Trk::GsfBetheHeitlerEffects::GsfBetheHeitlerEffects(const std::string& type, const std::string& name, const IInterface* parent)
  : 
  MultiStateMaterialEffects(type, name, parent),
  m_parameterisationFileName("GeantSim_LT01_cdf_nC6_O5.par"),
  m_numberOfComponents(0),
  m_correctionFlag(0),
  m_parameterisationFileNameHighX0("GeantSim_GT01_cdf_nC6_O5.par"),
  m_numberOfComponentsHighX0(0)
{

  declareInterface<IMultiStateMaterialEffects>(this);
  declareProperty("BetheHeitlerParameterisationFileName",       m_parameterisationFileName);
  declareProperty("BetheHeitlerParameterisationFileNameHighX0", m_parameterisationFileNameHighX0);
  declareProperty("BetheHeitlerParameterisationCorrectionFlag", m_correctionFlag);
  declareProperty("SingleGaussianRange", m_singleGaussianRange = 0.0001);
  declareProperty("LowerRange", m_lowerRange = 0.002);
  declareProperty("XoverRange", m_xOverRange = 0.10);
  declareProperty("UpperRange", m_upperRange = 0.20);
  declareProperty("UseHighX0",  m_useHighX0 = true );
  declareProperty("ComponentMeanCut",  m_componentMeanCut = 0.0 );
  
}

StatusCode Trk::GsfBetheHeitlerEffects::initialize(){

  if (m_correctionFlag == 1)
    ATH_MSG_INFO( "1st moment of mixture will be corrected" );
  else if (m_correctionFlag == 2)
    ATH_MSG_INFO( "1st and 2nd moment of mixture will be corrected" );
  else if (m_correctionFlag == 0) 
    ATH_MSG_INFO( "Moments of mixture will not be corrected" );
  else {
    ATH_MSG_INFO(  "Inappropriate setting for Bethe-Heitler mixture correction! ...Exiting!" );
    return StatusCode::FAILURE;
  }

  if ( this->readParameters() ){
    ATH_MSG_INFO( "Parameters successfully imported from file" );

    ATH_MSG_INFO( "Initialisation of " << name() <<  " was successful" );

    return StatusCode::SUCCESS;

  }
  
  else {

    msg(MSG::ERROR) << "Parameters could NOT be successfully imported from file" << endmsg;

    return StatusCode::FAILURE;

  }

}

StatusCode Trk::GsfBetheHeitlerEffects::finalize(){

  ATH_MSG_INFO( "Finalisation of " << name() <<  " was successful" );

  return StatusCode::SUCCESS;

}

bool Trk::GsfBetheHeitlerEffects::readParameters()
{

  std::string resolvedFileName = PathResolver::find_file (m_parameterisationFileName, "DATAPATH");
  
  if (resolvedFileName != "")
    ATH_MSG_INFO( "Parameterisation file found: " << resolvedFileName );
  else {
    msg(MSG::ERROR) << "Parameterisation file not found" << endmsg;
    return false;
  }

  const char* filename = resolvedFileName.c_str();

  std::ifstream fin(filename);

  if ( fin.bad() ){
    msg(MSG::ERROR) << "Error opening file: " << resolvedFileName << endmsg;
    return false;
  }

  int orderPolynomial;

  fin >> m_numberOfComponents;
  fin >> orderPolynomial;
  fin >> m_transformationCode;
  //
  if (not inRange(m_numberOfComponents, 0, 100)){
      msg(MSG::ERROR) << "numberOfComponents Parameter out of range 0-100: " << m_numberOfComponents << endmsg;
      return false;
    }
    if (not inRange(orderPolynomial, 0, 10)){
      msg(MSG::ERROR) << "orderPolynomial Parameter out of range 0-10: " << orderPolynomial << endmsg;
      return false;
    }
    if (not inRange(m_transformationCode, 0, 10)){
      msg(MSG::ERROR) << "transformationCode Parameter out of range 0-10: " << m_transformationCode << endmsg;
      return false;
    }
  
  
  if (!fin) {
    msg(MSG::ERROR) << "Error while reading file : " << resolvedFileName << endmsg;
    return false;
  }

  int componentIndex = 0;
  
  for ( ; componentIndex < m_numberOfComponents; ++componentIndex ){
    m_polynomialWeights.push_back( readPolynomial(fin, orderPolynomial) );
    m_polynomialMeans.push_back( readPolynomial(fin, orderPolynomial) );
    m_polynomialVariances.push_back( readPolynomial(fin, orderPolynomial) );
  }


  if ( m_useHighX0){
    resolvedFileName = PathResolver::find_file (m_parameterisationFileNameHighX0, "DATAPATH");
    if (resolvedFileName != "")
      ATH_MSG_INFO( "Parameterisation file found: " << resolvedFileName );
    else {
      msg(MSG::ERROR) << "Parameterisation file not found" << endmsg;
      return false;
    }
  
    const char* filename = resolvedFileName.c_str();
  
    std::ifstream fin(filename);
  
    if ( fin.bad() ){
      msg(MSG::ERROR) << "Error opening file: " << resolvedFileName << endmsg;
      return false;
    }
  
    fin >> m_numberOfComponentsHighX0;
    fin >> orderPolynomial;
    fin >> m_transformationCodeHighX0;
    //
    if (not inRange(m_numberOfComponentsHighX0, 0, 100)){
      msg(MSG::ERROR) << "numberOfComponents Parameter out of range 0-100: " << m_numberOfComponentsHighX0 << endmsg;
      return false;
    }
    if (not inRange(orderPolynomial, 0, 10)){
      msg(MSG::ERROR) << "orderPolynomial Parameter out of range 0-10: " << orderPolynomial << endmsg;
      return false;
    }
    if (not inRange(m_transformationCodeHighX0, 0, 10)){
      msg(MSG::ERROR) << "transformationCode Parameter out of range 0-10: " << m_transformationCodeHighX0 << endmsg;
      return false;
    }
    if ( fin.bad() ){
      msg(MSG::ERROR) << "Error reading file: " << resolvedFileName << endmsg;
      return false;
    }
  
    int componentIndex = 0;
    
    for ( ; componentIndex < m_numberOfComponentsHighX0; ++componentIndex ){
      m_polynomialWeightsHighX0.push_back( readPolynomial(fin, orderPolynomial) );
      m_polynomialMeansHighX0.push_back( readPolynomial(fin, orderPolynomial) );
      m_polynomialVariancesHighX0.push_back( readPolynomial(fin, orderPolynomial) );
    }
  }
  return true;

}

Trk::GsfBetheHeitlerEffects::Polynomial
Trk::GsfBetheHeitlerEffects::readPolynomial (std::ifstream& fin, const int order)
{

  std::vector<double> coefficients(order + 1);

  int orderIndex = 0;

  for ( ; orderIndex < (order + 1); ++orderIndex ) {
    if (!fin) {
      throw std::runtime_error("Reached end of stream but still expecting data.");
    }
    fin >> coefficients[orderIndex];
  }

  return Polynomial(coefficients);

}

void Trk::GsfBetheHeitlerEffects::compute ( const Trk::ComponentParameters& componentParameters,
              const Trk::MaterialProperties& materialProperties,
              double pathLength,
              Trk::PropDirection direction,
              Trk::ParticleHypothesis ) const
{

  ATH_MSG_VERBOSE( "Computing Bethe-Heitler energy loss effects" );

  
  // Clear cache
  this->reset();

  const Trk::TrackParameters* trackParameters = componentParameters.first;
  const Amg::Vector3D& globalMomentum = trackParameters->momentum();
  
  double radiationLength = materialProperties.x0();  
  double pathlengthInX0 = pathLength / radiationLength;
  double momentum = globalMomentum.mag();

  if (msgLvl(MSG::VERBOSE)){ 
  
    msg(MSG::VERBOSE) << "Bethe-Heitler parameters and weights calculated as a function of X/X0 = " << pathlengthInX0 << endmsg;
    msg(MSG::VERBOSE) << "Global momentum: " << momentum << endmsg;
    msg(MSG::VERBOSE) << "Component weighting: " << componentParameters.second << endmsg;
  
  }
  
  // Produce a multi-component State
  if ( pathlengthInX0 > m_singleGaussianRange ){

    // If the amount of material is between 0.0001 and 0.01 return the gaussian approximation to the Bethe-Heitler distribution
    if ( pathlengthInX0 < m_lowerRange ){

      ATH_MSG_DEBUG( "Amount of material less than" << m_lowerRange <<"... Parameterising Bethe-Heitler as Gaussian" );

      double meanZ = exp( -1. * pathlengthInX0 );

      double deltaP(0.);
      double varQoverP(0.);
      double sign = ( direction == Trk::oppositeMomentum ) ? 1. : -1.;

      if ( direction == Trk::alongMomentum )
        deltaP = sign * momentum * (1. - meanZ);

      else
        deltaP = sign * momentum * (1. / meanZ - 1.);

      double varZ  = exp( -1. * pathlengthInX0 * log(3.) / log(2.) ) - exp(-2. * pathlengthInX0);

      if ( direction == Trk::alongMomentum )
        varQoverP = 1. / (meanZ * meanZ * momentum * momentum) * varZ;

      else
        varQoverP = varZ / (momentum * momentum);

      AmgSymMatrix(5)* newCovarianceMatrix = new AmgSymMatrix(5);
      newCovarianceMatrix->setZero();
      (*newCovarianceMatrix)(Trk::qOverP,Trk::qOverP) = varQoverP;

      m_deltaPs.push_back( deltaP );
      m_weights.push_back(1.);
      m_deltaCovariances.push_back(newCovarianceMatrix);

      ATH_MSG_VERBOSE( "Weight / deltaP / var (delta q/p) " << 1. << "\t" << deltaP << "\t" << varQoverP );

      return;

    }

    if ( pathlengthInX0 > m_upperRange ) pathlengthInX0 = m_upperRange;

    MixtureParameters mixture;
    
    if ( m_useHighX0 && pathlengthInX0 > m_xOverRange )
      this->getMixtureParametersHighX0(pathlengthInX0, mixture);
    else 
      this->getMixtureParameters(pathlengthInX0, mixture);
    
    this->correctWeights(mixture);
    
    if (m_correctionFlag == 1){
      mixture[0].mean = correctedFirstMean(pathlengthInX0, mixture);
    }

    if (m_correctionFlag == 2){
      mixture[0].mean     = correctedFirstMean(pathlengthInX0, mixture);
      mixture[0].variance = correctedFirstVariance(pathlengthInX0, mixture);
      // msg(MSG::VERBOSE) << "Corrected mean/variance: " << mixture[0].mean << " / " << mixture[0].variance << endmsg;
    }

    int componentIndex = 0;
	  double weightToBeRemoved(0.);
	  int componentWithHighestMean(0);
	  
	  for ( ; componentIndex < m_numberOfComponents; ++componentIndex ){
  	  if (mixture[componentIndex].mean > mixture[componentWithHighestMean].mean)
  	  	componentWithHighestMean = componentIndex;
  	  if ( mixture[componentIndex].mean >= m_componentMeanCut ) continue;
    	weightToBeRemoved += mixture[componentIndex].weight;
  	}

		componentIndex = 0;
    for ( ; componentIndex < m_numberOfComponents; ++componentIndex ){
      
      double varianceInverseMomentum;
      
      //This is not mathematically correct but it does stabilize the GSF
      if ( mixture[componentIndex].mean < m_componentMeanCut ) continue;
      
      if (componentIndex == componentWithHighestMean) 
      	m_weights.push_back(mixture[componentIndex].weight + weightToBeRemoved);
      else 
  			m_weights.push_back(mixture[componentIndex].weight);
    
      double deltaP(0.);

      if (direction == alongMomentum) {

        // For forward propagation
        deltaP = momentum  * ( mixture[componentIndex].mean - 1.);
      
        // msg(MSG::VERBOSE) << "Component mean: " << mixture[componentIndex].mean << endmsg;
        // msg(MSG::VERBOSE) << "Forwards propagation delta P: " << deltaP << endmsg;
      
        m_deltaPs.push_back( deltaP );
        double f = 1./ (momentum * mixture[componentIndex].mean);
        varianceInverseMomentum = f * f * mixture[componentIndex].variance;

      } // end forward propagation if clause
      
      else {
  
        // For backwards propagation
        deltaP = momentum * (1. / mixture[componentIndex].mean - 1.);
      
        // msg(MSG::VERBOSE) <<  "Component mean: " << mixture[componentIndex].mean << endmsg;
        // msg(MSG::VERBOSE) <<  "Backwards propagation delta P: " << deltaP << endmsg;
      
        m_deltaPs.push_back( deltaP );
        varianceInverseMomentum = mixture[componentIndex].variance / (momentum * momentum);
  
      } // end backwards propagation if clause

      AmgSymMatrix(5)* newCovarianceMatrix = new AmgSymMatrix(5);
      newCovarianceMatrix->setZero();
      (*newCovarianceMatrix)(Trk::qOverP,Trk::qOverP) = varianceInverseMomentum;

      m_deltaCovariances.push_back(newCovarianceMatrix);

      ATH_MSG_VERBOSE(  "Weight / deltaP / var (delta q/p) " << mixture[componentIndex].weight << "\t" << deltaP << "\t" << varianceInverseMomentum );

    } // end for loop over all components

  } // end material limiting if clause

  else {

    ATH_MSG_DEBUG(  "Trying to apply energy loss to " << pathlengthInX0 << " x/x0. No Bethe-Heitler effects applied" );

    m_weights.push_back(1.);
    m_deltaPs.push_back(0.);
    AmgSymMatrix(5)* newCovarianceMatrix = new AmgSymMatrix(5);
    newCovarianceMatrix->setZero();
    m_deltaCovariances.push_back( newCovarianceMatrix );
  }

}

void Trk::GsfBetheHeitlerEffects::getMixtureParameters(const double pathlengthInX0, Trk::GsfBetheHeitlerEffects::MixtureParameters& mixture) const
{

  // msg(MSG::VERBOSE) <<  "Retrieving mixture parameters" << endmsg;

  mixture.clear();
  mixture.reserve(m_numberOfComponents);

  int componentIndex = 0;

  for ( ; componentIndex < m_numberOfComponents; ++componentIndex )
  {

    double updatedWeight = m_polynomialWeights[componentIndex](pathlengthInX0);
    
    if ( m_transformationCode )
      updatedWeight = logisticFunction(updatedWeight);

    double updatedMean = m_polynomialMeans[componentIndex](pathlengthInX0);
    
    // msg(MSG::VERBOSE) <<  "Updated mean: " << updatedMean << endmsg;

    if ( m_transformationCode )
      updatedMean = logisticFunction(updatedMean);

    // msg(MSG::VERBOSE) <<  "Updated mean after update: " << updatedMean << endmsg;

    double updatedVariance = m_polynomialVariances[componentIndex](pathlengthInX0);

    if ( m_transformationCode )
      updatedVariance = exp(updatedVariance);
    else
      updatedVariance = updatedVariance * updatedVariance;

    ComponentValues newValues;

    newValues.weight   = updatedWeight;
    newValues.mean     = updatedMean;
    newValues.variance = updatedVariance;

    mixture.push_back(newValues);
  }
}


void Trk::GsfBetheHeitlerEffects::getMixtureParametersHighX0(const double pathlengthInX0, Trk::GsfBetheHeitlerEffects::MixtureParameters& mixture) const
{

  // msg(MSG::VERBOSE) <<  "Retrieving mixture parameters" << endmsg;

  mixture.clear();
  mixture.reserve(m_numberOfComponentsHighX0);

  int componentIndex = 0;

  for ( ; componentIndex < m_numberOfComponentsHighX0; ++componentIndex )
  {

    double updatedWeight = m_polynomialWeightsHighX0[componentIndex](pathlengthInX0);
    
    if ( m_transformationCodeHighX0 )
      updatedWeight = logisticFunction(updatedWeight);

    double updatedMean = m_polynomialMeansHighX0[componentIndex](pathlengthInX0);
    
    // msg(MSG::VERBOSE) <<  "Updated mean: " << updatedMean << endmsg;

    if ( m_transformationCodeHighX0 )
      updatedMean = logisticFunction(updatedMean);

    // msg(MSG::VERBOSE) <<  "Updated mean after update: " << updatedMean << endmsg;

    double updatedVariance = m_polynomialVariancesHighX0[componentIndex](pathlengthInX0);

    if ( m_transformationCodeHighX0 )
      updatedVariance = exp(updatedVariance);
    else
      updatedVariance = updatedVariance * updatedVariance;

    ComponentValues newValues;

    newValues.weight   = updatedWeight;
    newValues.mean     = updatedMean;
    newValues.variance = updatedVariance;

    mixture.push_back(newValues);
  }
}



void Trk::GsfBetheHeitlerEffects::correctWeights(Trk::GsfBetheHeitlerEffects::MixtureParameters& mixture) const
{

  if ( mixture.empty() )
    return;

  // Obtain the sum of weights
  double weightSum(0.);

  Trk::GsfBetheHeitlerEffects::MixtureParameters::const_iterator component = mixture.begin();

  for ( ; component != mixture.end(); ++component )
    weightSum += (*component).weight;

  // Rescale so that total weighting is 1
  Trk::GsfBetheHeitlerEffects::MixtureParameters::iterator modifiableComponent = mixture.begin();

  for ( ; modifiableComponent != mixture.end(); ++modifiableComponent )
    (*modifiableComponent).weight /= weightSum;

}

double Trk::GsfBetheHeitlerEffects::correctedFirstMean(const double pathlengthInX0, const Trk::GsfBetheHeitlerEffects::MixtureParameters& mixture) const
{

  if ( mixture.empty() )
    return 0.;

  // Obtain the difference between the true and weighted sum
  double meanBH = betheHeitlerMean(pathlengthInX0);
  
  Trk::GsfBetheHeitlerEffects::MixtureParameters::const_iterator component = mixture.begin() + 1;

  for ( ; component != mixture.end(); ++component )
    meanBH -= (*component).weight * (*component).mean;

  // return the corrected mean for the first component
  return std::max( std::min( meanBH / mixture[0].weight , 1. ), 0. );

}

double Trk::GsfBetheHeitlerEffects::correctedFirstVariance(const double pathlengthInX0, const Trk::GsfBetheHeitlerEffects::MixtureParameters& mixture) const
{

  if ( mixture.empty() )
    return 0.;

  // Obtain the difference between the true and weighed sum variances
  double varianceBH = betheHeitlerVariance(pathlengthInX0) + ( betheHeitlerMean(pathlengthInX0) * betheHeitlerMean(pathlengthInX0) );
  varianceBH -= mixture[0].weight * mixture[0].mean * mixture[0].mean;

  Trk::GsfBetheHeitlerEffects::MixtureParameters::const_iterator component = mixture.begin() + 1;

  for ( ; component != mixture.end(); ++component )
    varianceBH -= (*component).weight * ( (*component).mean * (*component).mean + (*component).variance );

  return std::max( varianceBH / mixture[0].weight, 0. );

}
