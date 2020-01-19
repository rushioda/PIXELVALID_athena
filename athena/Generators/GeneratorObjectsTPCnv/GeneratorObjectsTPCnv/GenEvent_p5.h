///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenEvent_p5.h 
// Header file for class GenEvent_p5
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_GENEVENT_P5_H 
#define GENERATOROBJECTSTPCNV_GENEVENT_P5_H 

// STL includes
#include <vector>
#include <utility> //> for std::pair

// forward declarations
class McEventCollectionCnv_p5;

class GenEvent_p5
{ 
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class McEventCollectionCnv_p5;

  /////////////////////////////////////////////////////////////////// 
  // Public methods 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor
   */
  GenEvent_p5();
  
  /** Constructor with parameters
   */
  GenEvent_p5( int signalProcessId, 
	       int eventNbr,
	       int mpi,
	       double eventScale,
	       double alphaQCD,
	       double alphaQED,
	       int signalProcessVtx,
	       int beamParticle1,
	       int beamParticle2,
	       const std::vector<double>& weights,
	       const std::vector<long int>& randomStates,
	       const std::vector<double>& crossSection,
	       const std::vector<float>& heavyIon,
	       const std::vector<double>& pdfinfo,
	       int momentumUnit,
	       int lengthUnit,
	       unsigned int verticesBegin,
	       unsigned int verticesEnd,
	       unsigned int particlesBegin,
	       unsigned int particlesEnd );

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Id of the processus being generated
   */
  int m_signalProcessId;

  /** Event number
   */
  int m_eventNbr;

  /** Number of multi particle interactions
  */
  int m_mpi;

  /** Energy scale. see hep-ph/0109068
   */
  double m_eventScale;

  /** value of the QCD coupling. see hep-ph/0109068
   */
  double m_alphaQCD;

  /** value of the QED coupling. see hep-ph/0109068
   */
  double m_alphaQED;

  /** Barcode of the GenVertex holding the signal process.
   *  0 means that no signal process vertex has been written out.
   *  This may come from upstream limitations (like HEPEVT)
   */
  int m_signalProcessVtx;

  /** Barcode of the beam particle 1
  */
  int m_beamParticle1;
  
  /** Barcode of the beam particle 2
  */
  int m_beamParticle2;

  /** Weights for this event.
   *  First weight is used by default for hit and miss.
   */
  std::vector<double> m_weights;

  /** Container of random numbers for the generator states
   */
  std::vector<long int> m_randomStates;

  /** Container of HepMC::GenCrossSection object translated to vector<double>
  */
  std::vector<double> m_crossSection;

  /** Container of HepMC::HeavyIon object translated to vector<double>
  */
  std::vector<float> m_heavyIon;

  /** Container of HepMC::PdfInfo object translated to 
   * vector<double> for simplicity
   */
  std::vector<double> m_pdfinfo;

  /** HepMC::Units::MomentumUnit casted to int 
  */
  int m_momentumUnit;

  /** HepMC::Units::LengthUnit casted to int 
  */
  int m_lengthUnit;

  /** Begin position in the vector of vertices composing this event.
   */
  unsigned int m_verticesBegin;

  /** End position in the vector of vertices composing this event.
   */
  unsigned int m_verticesEnd;

  /** Begin position in the vector of particles composing this event.
   */
  unsigned int m_particlesBegin;

  /** End position in the vector of particles composing this event.
   */
  unsigned int m_particlesEnd;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline GenEvent_p5::GenEvent_p5():
  m_signalProcessId  ( -1 ),
  m_eventNbr         ( -1 ),
  m_mpi              ( -1 ),
  m_eventScale       ( -1 ),
  m_alphaQCD         ( -1 ),
  m_alphaQED         ( -1 ),
  m_signalProcessVtx (  0 ),
  m_beamParticle1    (  0 ),
  m_beamParticle2    (  0 ),
  m_weights          (    ),
  m_randomStates     (    ),
  m_crossSection     (    ),
  m_heavyIon         (    ),
  m_pdfinfo          (    ),
  m_momentumUnit     (  0 ),
  m_lengthUnit       (  0 ),
  m_verticesBegin    (  0 ),
  m_verticesEnd      (  0 ),
  m_particlesBegin   (  0 ),
  m_particlesEnd     (  0 )
{}

inline GenEvent_p5::GenEvent_p5( int signalProcessId, 
				 int eventNbr,
         int mpi,
				 double eventScale,
				 double alphaQCD,
				 double alphaQED,
				 int signalProcessVtx,
				 int beamParticle1,
				 int beamParticle2,
				 const std::vector<double>& weights,
				 const std::vector<long int>& randomStates,
				 const std::vector<double>& crossSection,
				 const std::vector<float>& heavyIon,
				 const std::vector<double>& pdfinfo,
				 int momentumUnit,
				 int lengthUnit,
				 unsigned int verticesBegin,
				 unsigned int verticesEnd,
				 unsigned int particlesBegin,
				 unsigned int particlesEnd ) :
  m_signalProcessId  ( signalProcessId ),
  m_eventNbr         ( eventNbr ),
  m_mpi              ( mpi ),
  m_eventScale       ( eventScale ),
  m_alphaQCD         ( alphaQCD ),
  m_alphaQED         ( alphaQED ),
  m_signalProcessVtx ( signalProcessVtx ),
  m_beamParticle1    ( beamParticle1 ),
  m_beamParticle2    ( beamParticle2 ),
  m_weights          ( weights ),
  m_randomStates     ( randomStates ),
  m_crossSection     ( crossSection ),
  m_heavyIon         ( heavyIon ),
  m_pdfinfo          ( pdfinfo ),
  m_momentumUnit     ( momentumUnit ),
  m_lengthUnit       ( lengthUnit ),
  m_verticesBegin    ( verticesBegin ),
  m_verticesEnd      ( verticesEnd ),
  m_particlesBegin   ( particlesBegin ),
  m_particlesEnd     ( particlesEnd )
{}

#endif //> GENERATOROBJECTSTPCNV_GENEVENT_P5_H
