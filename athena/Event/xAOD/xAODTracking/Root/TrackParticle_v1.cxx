/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticle_v1.cxx 576255 2013-12-19 12:54:41Z emoyse $

// Misc includes
#include <bitset>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTracking/versions/TrackParticle_v1.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackSummaryAccessors_v1.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

// Amg include
//#include "EventPrimitives/EventPrimitives.h"

namespace xAOD {

  TrackParticle_v1::TrackParticle_v1()
  : IParticle(), m_p4(), m_p4Cached( false ), m_perigeeCached(false) {
    // std::cout<<"TrackParticle_v1 CTOR this="<<this<<", \tm_perigeeCached="<<m_perigeeCached<<", \tm_perigeeParameters="<<m_perigeeParameters<<std::endl;
#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    m_perigeeParameters=0;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE
  }
  
  TrackParticle_v1::TrackParticle_v1(const TrackParticle_v1& tp ) 
  : IParticle( tp ), m_p4(tp.m_p4), m_p4Cached( tp.m_p4Cached ), m_perigeeCached(tp.m_perigeeCached) {
    makePrivateStore( tp );
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    m_perigeeParameters = tp.m_perigeeParameters;
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
  }
  
  TrackParticle_v1& TrackParticle_v1::operator=(const TrackParticle_v1& tp ){
    if(this == &tp) return *this;
    
    if( ( ! hasStore() ) && ( ! container() ) ) {
       makePrivateStore();
    }
    this->IParticle::operator=( tp );
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    m_perigeeParameters = tp.m_perigeeParameters;
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
    return *this;
  }
  
  TrackParticle_v1::~TrackParticle_v1(){
    // std::cout<<"TrackParticle_v1 DTOR this="<<this<<", \tm_perigeeCached="<<m_perigeeCached<<", \tm_perigeeParameters="<<m_perigeeParameters<<std::endl;

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    delete m_perigeeParameters;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE
  }
  double TrackParticle_v1::pt() const {
    return p4().Pt();
  }

  double TrackParticle_v1::eta() const {
    return p4().Eta(); 
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1,float,double,phi)

  double TrackParticle_v1::m() const {
    return p4().M();
  }

  double TrackParticle_v1::e() const {
    return p4().E(); 
  }
  double TrackParticle_v1::rapidity() const {
    return p4().Rapidity();
  }

  const TrackParticle_v1::FourMom_t& TrackParticle_v1::p4() const {
    using namespace std;
  // Check if we need to reset the cached object:
    if( ! m_p4Cached ) {
      float p = 10.e6; // 10 TeV (default value for very high pt muons, with qOverP==0)
      if (fabs(qOverP())>0.) p = 1/fabs(qOverP());
      float thetaT = theta();
      float phiT = phi();
      float sinTheta= sin(thetaT);
      float px = p*sinTheta*cos(phiT);
      float py = p*sinTheta*sin(phiT);
      float pz = p*cos(thetaT);
      float e  =  pow (139.570,2) + /// @todo Get value from somewhere. Also, the TrackParticle took the Pion mass - do we really want to do this? We have ParticleHypo?
        pow( px,2) + pow( py,2) + pow( pz,2);
      m_p4.SetPxPyPzE( px, py, pz, sqrt(e) ); 
      m_p4Cached = true;
    }
  // Return the cached object:
    return m_p4;
  }

  Type::ObjectType TrackParticle_v1::type() const {
     return Type::TrackParticle;
  }

  float TrackParticle_v1::charge() const {
    // static Accessor< float > acc( "charge" );
    return (qOverP() > 0) ? 1 : ((qOverP() < 0) ? -1 : 0);
  }

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, d0)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, z0)

  float TrackParticle_v1::phi0() const {

     Accessor< float > acc( "phi" );
     return acc( *this );
  }

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, theta)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, qOverP)

  const DefiningParameters_t TrackParticle_v1::definingParameters() const{
    DefiningParameters_t tmp;
    tmp << d0() , z0() , phi0() , theta() , qOverP();
    return tmp;
  }

  void TrackParticle_v1::setDefiningParameters(float d0, float z0, float phi0, float theta, float qOverP) {
    m_perigeeCached=false;
#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    delete m_perigeeParameters;
    m_perigeeParameters=0;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE
    static Accessor< float > acc1( "d0" );
    acc1( *this ) = d0;

    static Accessor< float > acc2( "z0" );
    acc2( *this ) = z0;

    static Accessor< float > acc3( "phi" );
    acc3( *this ) = phi0;

    static Accessor< float > acc4( "theta" );
    acc4( *this ) = theta;

    static Accessor< float > acc5( "qOverP" );
    acc5( *this ) = qOverP;

    m_p4Cached = false;
    return;
  }

  void TrackParticle_v1::setDefiningParametersCovMatrix(const xAOD::ParametersCovMatrix_t& cov){
    m_perigeeCached=false;
#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    delete m_perigeeParameters;
    m_perigeeParameters=0;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

    static Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    Amg::compress(cov,acc(*this));
  }

  const xAOD::ParametersCovMatrix_t TrackParticle_v1::definingParametersCovMatrix() const {
    xAOD::ParametersCovMatrix_t cov; 
    const std::vector<float>& covVec = definingParametersCovMatrixVec();
    if( !covVec.empty() ) Amg::expand( covVec.begin(), covVec.end(),cov );
    else cov.setIdentity();
    return cov;
  }

  const std::vector<float>& TrackParticle_v1::definingParametersCovMatrixVec() const {
  // Can't use AUXSTORE_PRIMITIVE_SETTER_AND_GETTER since I have to add Vec to the end of setDefiningParametersCovMatrix to avoid clash.
    static Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    return acc(*this);
  }

  void TrackParticle_v1::setDefiningParametersCovMatrixVec(const std::vector<float>& cov){
  // Can't use AUXSTORE_PRIMITIVE_SETTER_AND_GETTER since I have to add Vec to the end of setDefiningParametersCovMatrix to avoid clash.
    static Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    acc(*this)=cov;
  }

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, vx)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, vy)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, vz)

  void TrackParticle_v1::setParametersOrigin(float x, float y, float z){
    static Accessor< float > acc1( "vx" );
    acc1( *this ) = x;

    static Accessor< float > acc2( "vy" );
    acc2( *this ) = y;

    static Accessor< float > acc3( "vz" );
    acc3( *this ) = z;
  }

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
  const Trk::Perigee& TrackParticle_v1::perigeeParameters() const {
    if (m_perigeeCached)
      return *m_perigeeParameters;
    m_perigeeCached=true;

    static Accessor< float > acc1( "d0" );
    static Accessor< float > acc2( "z0" );
    static Accessor< float > acc3( "phi" );
    static Accessor< float > acc4( "theta" );
    static Accessor< float > acc5( "qOverP" );
    static Accessor< std::vector<float> > acc6( "definingParametersCovMatrix" );
    ParametersCovMatrix_t* cov = new ParametersCovMatrix_t(definingParametersCovMatrix());
    // cov->setZero();
    // auto it= acc6(*this).begin();
    // for (size_t irow = 0; irow<5; ++irow)
    //   for (size_t icol =0; icol<=irow; ++icol)
    //       cov->fillSymmetric(irow,icol,*it++) ;
    static Accessor< float > acc7( "beamlineTiltX" );
    static Accessor< float > acc8( "beamlineTiltY" );
    
    if(!acc7.isAvailable( *this ) || !acc8.isAvailable( *this )){
      m_perigeeParameters = new Trk::Perigee(acc1(*this),acc2(*this),acc3(*this),acc4(*this),acc5(*this),Trk::PerigeeSurface(Amg::Vector3D(vx(),vy(),vz())),cov);
       return *m_perigeeParameters;
    }
    
    Amg::Transform3D * amgTransf = new Amg::Transform3D();	
    Amg::Translation3D amgtranslation(vx(),vy(),vz());
    *amgTransf = amgtranslation * Amg::RotationMatrix3D::Identity();
    *amgTransf *= Amg::AngleAxis3D(acc8(*this), Amg::Vector3D(0.,1.,0.));
    *amgTransf *= Amg::AngleAxis3D(acc7(*this), Amg::Vector3D(1.,0.,0.));
    m_perigeeParameters = new Trk::Perigee(acc1(*this),acc2(*this),acc3(*this),acc4(*this),acc5(*this),Trk::PerigeeSurface(amgTransf),cov);
    
    return *m_perigeeParameters;
  }
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, chiSquared)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, numberDoF)

  void TrackParticle_v1::setFitQuality(float chiSquared, float numberDoF){
    static Accessor< float > acc1( "chiSquared" );
    acc1( *this ) = chiSquared;  
    static Accessor< float > acc2( "numberDoF" );
    acc2( *this ) = numberDoF;   
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, float, radiusOfFirstHit, setRadiusOfFirstHit)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint64_t, identifierOfFirstHit, setIdentifierOfFirstHit)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, float, beamlineTiltX, setBeamlineTiltX)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, float, beamlineTiltY, setBeamlineTiltY)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint32_t, hitPattern, setHitPattern)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint8_t,numberOfUsedHitsdEdx ,setNumberOfUsedHitsdEdx )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint8_t,numberOfIBLOverflowsdEdx , setNumberOfIBLOverflowsdEdx)

  size_t TrackParticle_v1::numberOfParameters() const{
    ///@todo - Can we do this in a better way? Not great to force retrieval of one specific parameter - any would do.
    static Accessor< std::vector<float>  > acc( "parameterX" );
    if(! acc.isAvailable( *this )) return 0;
    return acc(*this).size();
  }

  const CurvilinearParameters_t TrackParticle_v1::trackParameters(unsigned int index) const{
    CurvilinearParameters_t tmp;
    tmp << parameterX(index),parameterY(index),parameterZ(index),
      parameterPX(index),parameterPY(index),parameterPZ(index);
    return tmp;
  }

  void TrackParticle_v1::setTrackParameters(std::vector<std::vector<float> >& parameters){
    static Accessor< std::vector < float > > acc1( "parameterX" );
    static Accessor< std::vector < float > > acc2( "parameterY" );
    static Accessor< std::vector < float > > acc3( "parameterZ" );
    static Accessor< std::vector < float > > acc4( "parameterPX" );
    static Accessor< std::vector < float > > acc5( "parameterPY" );
    static Accessor< std::vector < float > > acc6( "parameterPZ" );

    static Accessor< std::vector<uint8_t>  > acc7( "parameterPosition" );

    acc1(*this).resize(parameters.size());
    acc2(*this).resize(parameters.size());
    acc3(*this).resize(parameters.size());
    acc4(*this).resize(parameters.size());
    acc5(*this).resize(parameters.size());
    acc6(*this).resize(parameters.size());

    acc7(*this).resize(parameters.size());

    unsigned int index=0;
    // std::cout<<"Adding this many parameters: "<<parameters.size()<<std::endl;
    std::vector<std::vector<float> >::const_iterator it=parameters.begin(), itEnd=parameters.end();
    for (;it!=itEnd;++it,++index){
      assert((*it).size()==6);
      acc1(*this).at(index)=(*it).at(0);
      acc2(*this).at(index)=(*it).at(1);
      acc3(*this).at(index)=(*it).at(2);
      acc4(*this).at(index)=(*it).at(3);
      acc5(*this).at(index)=(*it).at(4);
      acc6(*this).at(index)=(*it).at(5);
      // std::cout<<"param=("<<(*it).at(0)<<", "<<(*it).at(0)<<", "<<(*it).at(1)<<", "<<(*it).at(2)<<", "<<(*it).at(3)<<", "<<(*it).at(4)<<", "<<(*it).at(5)<<")"<<std::endl;
    }
  }

  float TrackParticle_v1::parameterX(unsigned int index) const  {
    static Accessor< std::vector<float>  > acc( "parameterX" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterY(unsigned int index) const  {
    static Accessor< std::vector<float>  > acc( "parameterY" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterZ(unsigned int index) const  {
    static Accessor< std::vector<float>  > acc( "parameterZ" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterPX(unsigned int index) const {
    static Accessor< std::vector<float>  > acc( "parameterPX" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterPY(unsigned int index) const {
    static Accessor< std::vector<float>  > acc( "parameterPY" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterPZ(unsigned int index) const {
    static Accessor< std::vector<float>  > acc( "parameterPZ" );    
    return acc(*this).at(index);
  }

  xAOD::ParametersCovMatrix_t TrackParticle_v1::trackParameterCovarianceMatrix(unsigned int index) const
  {
    static Accessor< std::vector<float>  > acc( "trackParameterCovarianceMatrices" );
    unsigned int offset = index*15;
    // copy the correct values into the temp matrix
    xAOD::ParametersCovMatrix_t tmp;
    std::vector<float>::const_iterator it = acc(*this).begin()+offset;
    Amg::expand(it,it+15,tmp);
    return tmp;
  }

  void TrackParticle_v1::setTrackParameterCovarianceMatrix(unsigned int index, std::vector<float>& cov){
    assert(cov.size()==15);
    unsigned int offset = index*15;
    static Accessor< std::vector < float > > acc( "trackParameterCovarianceMatrices" );
    std::vector<float>& v = acc(*this);
    v.resize(offset+15);
    std::copy(cov.begin(),cov.end(),v.begin()+offset );
  }

  xAOD::ParameterPosition TrackParticle_v1::parameterPosition(unsigned int index) const
  {
    static Accessor< std::vector<uint8_t>  > acc( "parameterPosition" );
    return static_cast<xAOD::ParameterPosition>(acc(*this).at(index));
  }

  bool TrackParticle_v1::indexOfParameterAtPosition(unsigned int& index, ParameterPosition position) const 
  {
    size_t maxParameters = numberOfParameters();
    bool foundParameters=false;
    for (size_t i=0; i<maxParameters; ++i){
      if (parameterPosition(i)==position){
        foundParameters=true;
        index=i;
        break;
      }
    }
    return foundParameters;
  }

  void  TrackParticle_v1::setParameterPosition(unsigned int index, xAOD::ParameterPosition pos){
    static Accessor< std::vector<uint8_t>  > acc( "parameterPosition" );
    acc( *this ).at(index) = static_cast<uint8_t>(pos);
  }

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
  const Trk::CurvilinearParameters TrackParticle_v1::curvilinearParameters(unsigned int index) const {    

    static Accessor< std::vector<float>  > acc( "trackParameterCovarianceMatrices" );
    unsigned int offset = index*15;
    // copy the correct values into the temp matrix
    ParametersCovMatrix_t* cov = new ParametersCovMatrix_t(); 
    auto it = acc(*this).begin()+offset;
    Amg::expand(it,it+15,*cov);
    // retrieve the parameters to build the curvilinear frame
    Amg::Vector3D pos(parameterX(index),parameterY(index),parameterZ(index));
    Amg::Vector3D mom(parameterPX(index),parameterPY(index),parameterPZ(index));
    Trk::CurvilinearParameters param(pos,mom,charge(),cov);

    return param;
  }
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackProperties,trackProperties)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackProperties,trackProperties, setTrackProperties)

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackFitter,trackFitter)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackFitter,trackFitter, setTrackFitter)

  std::bitset<xAOD::NumberOfTrackRecoInfo>   TrackParticle_v1::patternRecoInfo() const {
    static Accessor< uint64_t > acc( "patternRecoInfo" );
    std::bitset<xAOD::NumberOfTrackRecoInfo> tmp(acc(*this));
    return tmp;
  }

  void TrackParticle_v1::setPatternRecognitionInfo(uint64_t patternReco)  {
    static Accessor< uint64_t > acc( "patternRecoInfo" );
    acc( *this ) = patternReco;
  }

  void TrackParticle_v1::setPatternRecognitionInfo(const std::bitset<xAOD::NumberOfTrackRecoInfo>& patternReco)  {
    static Accessor< uint64_t > acc( "patternRecoInfo" );
  #if __cplusplus < 201100
    uint64_t value = 0;
    unsigned int i = 0;
    unsigned int size=patternReco.size();
    for (;i<32;++i)       value   |= ((patternReco[i]) << i);
    for (i=32;i<size;++i) value   |= ((patternReco[i]) << (i-32));
    acc( *this ) = value;

  #else
    acc( *this ) = patternReco.to_ullong();
  #endif
  }

  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::ParticleHypothesis, particleHypothesis, setParticleHypothesis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::ParticleHypothesis, particleHypothesis)

  bool TrackParticle_v1::summaryValue(uint8_t& value, const SummaryType &information)  const {
    xAOD::TrackParticle_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information );
    if( ( ! acc ) || ( ! acc->isAvailable( *this ) ) ) return false;
  // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
  
  bool TrackParticle_v1::summaryValue(float& value, const SummaryType &information)  const {
    xAOD::TrackParticle_v1::Accessor< float >* acc = trackSummaryAccessorV1<float>( information );
    if( ( ! acc ) || ( ! acc->isAvailable( *this ) ) ) return false;
  // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
  
  void TrackParticle_v1::setSummaryValue(uint8_t& value, const SummaryType &information){
    xAOD::TrackParticle_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information );
  // Set the value:
    ( *acc )( *this ) = value;
  }

  void TrackParticle_v1::setSummaryValue(float& value, const SummaryType &information){
    xAOD::TrackParticle_v1::Accessor< float >* acc = trackSummaryAccessorV1<float>( information );
  // Set the value:
    ( *acc )( *this ) = value;
  }
 

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
   /// The function will return an invalid ElementLink in case nothing was set
   /// for it yet. This is to avoid users having to always check both for
   /// the decoration being available, and the link being valid.
   ///
   /// @returns An element link to the parent Trk::Track of this track particle
   ///
   const ElementLink< TrackCollection >& TrackParticle_v1::trackLink() const {

      // The accessor:
      static ConstAccessor< ElementLink< TrackCollection > > acc( "trackLink" );

      // Check if one of them is available:
      if( acc.isAvailable( *this ) ) {
         return acc( *this );
      }

      // If no Trk::Track link was not set (yet), return a dummy object:
      static const ElementLink< TrackCollection > dummy;
      return dummy;
   }

   void TrackParticle_v1::
   setTrackLink( const ElementLink< TrackCollection >& el ) {

      // The accessor:
      static Accessor< ElementLink< TrackCollection > > acc( "trackLink" );

      // Do the deed:
      acc( *this ) = el;
      return;
   }

   const Trk::Track* TrackParticle_v1::track() const{

      // The accessor:
      static ConstAccessor< ElementLink< TrackCollection > > acc( "trackLink" );

      if( ! acc.isAvailable( *this ) ) {
         return 0;
      }
      if( ! acc( *this ).isValid() ) {
         return 0;
      }

      return *( acc( *this ) );
   }
   
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackParticle_v1,
                                      ElementLink< VertexContainer >,
                                      vertexLink, setVertexLink )

   const Vertex* TrackParticle_v1::vertex() const {

      // The accessor:
      static SG::AuxElement::Accessor< ElementLink< VertexContainer > >
         acc( "vertexLink" );

      if( ! acc.isAvailable( *this ) ) {
         return 0;
      }
      if( ! acc( *this ).isValid() ) {
         return 0;
      }
      return *( acc( *this ) );
   }

} // namespace xAOD
