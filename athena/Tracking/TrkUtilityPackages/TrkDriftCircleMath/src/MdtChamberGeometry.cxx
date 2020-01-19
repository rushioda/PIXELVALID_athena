/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/MdtChamberGeometry.h"
#include "TrkDriftCircleMath/SortDcsByY.h"

#include <iostream>
#include <algorithm>

namespace TrkDriftCircleMath {

  MdtChamberGeometry::MdtChamberGeometry() : 
		m_id{},
		m_nml{},
		m_nlay{},
		m_tubeStage{},
		m_stationTheta{},
		m_allTubes{},
		m_crossedTubes{},
		m_cachedPos(0.,0.),
		m_validGeometry{},
		m_isSecondMultiLayer{},
		m_resLine{}
  {
    init();
  }

  MdtChamberGeometry::MdtChamberGeometry( MdtStationId id, unsigned int nml, unsigned int nlay,
					  unsigned int ntubesml0, unsigned int ntubesml1, 
					  LocPos tube0ml0, LocPos tube0ml1, 
					  double tubeDist, double tubeStage, double layDist, double stationTheta ) 
    : m_id(id),m_cachedPos(0.,0.), m_isSecondMultiLayer(false)
  {
    init();
    setGeometry( nml,nlay,ntubesml0,ntubesml1,tube0ml0,tube0ml1,tubeDist,tubeStage,layDist, stationTheta );
  }
  
    void MdtChamberGeometry::init()
    {
      m_validGeometry = true;
      if( m_id.stName() == 53 || m_id.stName() == 54 ) { // BME and BMG chambers are small tubes
        m_tubeDist = 15.10;
        m_tubeRad  = 7.1;
        m_layDist  = 13.085;
      } else {
        m_tubeDist = 30.035;
        m_tubeRad  = 14.6;
        m_layDist  = 26.011;
      }
      // initialize first tubes to zero
      m_ntubesml.push_back(0);
      m_ntubesml.push_back(0);
      m_firstTube.push_back( LocPos(0.,0.) );
      m_firstTube.push_back( LocPos(0.,0.) );
      m_wasInit.push_back(1);
      m_wasInit.push_back(1);
      m_crossedTubes.reserve(50);
    }
  
    MdtChamberGeometry::~MdtChamberGeometry() {}

    void MdtChamberGeometry::setGeometry(unsigned int nml, unsigned int nlay,
					 unsigned int ntubesml0, unsigned int ntubesml1, 
					 LocPos tube0ml0, LocPos tube0ml1, 
					 double tubeDist, double tubeStage, double layDist, double stationTheta )
    {
      m_nml  = nml;
      m_nlay = nlay; 
      m_ntubesml.clear();
      m_ntubesml.push_back(ntubesml0);
      m_ntubesml.push_back(ntubesml1);
      m_tubeDist = tubeDist;
      m_tubeStage = tubeStage;
      m_layDist = layDist;

      m_firstTube[0] = tube0ml0;
      m_firstTube[1] = tube0ml1;

      m_allTubes.clear();
      if( m_nml > 2 ){
	std::cout << " invalid geometry: nml " << m_nml << std::endl;
	m_validGeometry = false;
      }
      if( m_nlay > 4 ){ 
	std::cout << " invalid geometry: nlay " << m_nlay << std::endl;
	m_validGeometry = false;
      }
      if( ntubesml0 > 78 || ntubesml1 > 78 ) {
	std::cout << " invalid geometry: ntubes ml0 " << ntubesml0
		  << " ntubes ml1 " << ntubesml1 << std::endl;
	m_validGeometry = false;
      }
      m_stationTheta = stationTheta;
    }




  bool MdtChamberGeometry::validId(  unsigned int ml, unsigned int lay,unsigned int tube) const
  {
    if( !m_validGeometry) return false;

    if( ml > 1 ) {
      std::cout << " ERROR wrong index: ml " << ml << " max " << m_nml << std::endl;
      print();
      return false;
    }else if( lay > m_nlay ){
      std::cout << " ERROR wrong index: lay " << lay << " max " << m_nlay << std::endl;
      print();
      return false;
    }else if ( tube > m_ntubesml[ml] ){
      std::cout << " ERROR wrong index: tube " << tube << " max " << m_ntubesml[ml] << std::endl;
      print();
      return false;
    }
    return true;
  }


  const std::vector<LocPos>& MdtChamberGeometry::allTubes() const
  {
    if( !m_validGeometry) return m_allTubes;
    if( !m_allTubes.empty() ) return m_allTubes;
    
    // create vector with all tubes in chamber
    for( unsigned int ml=0;ml<m_nml;++ml){
      if( !m_wasInit[ml] ) continue;
      for( unsigned int lay=0;lay<m_nlay;++lay){
	for( unsigned int tube=0;tube<m_ntubesml[ml];++tube ){
	  const LocPos& lp = tubePosition(ml,lay,tube);
	  m_allTubes.push_back( lp );
	}
      }
    }
    return m_allTubes;
  }
  
  void MdtChamberGeometry::tubesPassedByLine( const Line& line, int inMultilayer, DCVec& crossedTubes ) const {
  
    m_resLine.set(line);
    
//     if( inMultilayer != -1 )
//       std::cout << " ----- tubesPassedByLine for ml " << inMultilayer << std::endl;
    const LocPos& linepos = line.position();
    const LocDir& linedir = line.direction();
    double dxdy = fabs(linedir.y())> 0.0001 ? linedir.x()/linedir.y() :  linedir.x()/0.0001;
    for( unsigned int ml=0;ml<m_nml;++ml){

      // check whether geometry was initialized for given multilayer
      if( !m_wasInit[ml] ) continue;

      // if indicated only scan single multilayer
      if( inMultilayer != -1 && inMultilayer != (int)ml ){
 	//std::cout << "  skippin ml " << ml << std::endl;
	continue;
      }
      for( unsigned int lay=0;lay<m_nlay;++lay){
	double ylay = yPosTube( ml, lay );
	double xfirsttube = xPosTube( ml,lay,0 );
	double xintersect = dxdy*(ylay-linepos.y()) + linepos.x();
	double relpos = (xintersect-xfirsttube)/m_tubeDist;
	int ctube = (int)relpos;
	if( ctube < 0 ) ctube = 0;
	if( ctube >= (int)m_ntubesml[ml] ) ctube = m_ntubesml[ml]-1;

 	//if( inMultilayer != -1 && ( m_id.stName() == 53 || m_id.stName() == 54 ))
	//std::cout << " y position layer " << ylay << " xpos intersect " << xintersect << " relpos " 
	//<< relpos << " tube " << ctube << " in ml " << ml << " lay " << lay << std::endl;
	

	for( int i = ctube-1;i>=0;--i ){
	  const LocPos& lp = tubePosition(ml,lay,i);
	  double res = m_resLine.residual(lp);
	  if( std::abs(res) > m_tubeRad ) {

	    if( m_tubeDist > 0 ){
	      if( res > m_tubeRad ) break;
	    }else{
	      if( res < -m_tubeRad ) break;
	    }

	  }else{
	    // if this is a chamber with only the second ml, set the ml index accordingly
	    unsigned int actualMl = m_isSecondMultiLayer ? 1 : ml;
	    crossedTubes.push_back( DriftCircle(lp,m_tubeRad,res,DriftCircle::EmptyTube,MdtId(m_id.isBarrel(),actualMl,lay,i),0) );
	  }
	}
	for( int i =ctube;i < (int)m_ntubesml[ml];++i ){
	  const LocPos& lp = tubePosition(ml,lay,i);
	  double res = m_resLine.residual(lp);
	  if( std::abs(res) > m_tubeRad ) {
	    if( m_tubeDist > 0 ){
	      if( res < - m_tubeRad ) break;
	    }else{
	      if( res > m_tubeRad ) break;
	    }
	  }else{
	    unsigned int actualMl = m_isSecondMultiLayer ? 1 : ml;
	    crossedTubes.push_back( DriftCircle(lp,m_tubeRad,res,DriftCircle::EmptyTube,MdtId(m_id.isBarrel(),actualMl,lay,i),0) );
	  }
	}	
      }

    }

  }  

  const DCVec& MdtChamberGeometry::tubesPassedByLine( const Line& line, int inMultilayer ) const
  {
    m_crossedTubes.clear();

    if( !m_validGeometry){ 
      std::cout << " >>>>> invalid geometry <<<<< " << std::endl;
      return m_crossedTubes;
    }

    tubesPassedByLine(line,inMultilayer,m_crossedTubes);

    std::stable_sort( m_crossedTubes.begin(),m_crossedTubes.end(),SortDcsByY() );
    return m_crossedTubes;
  }

  void MdtChamberGeometry::print() const {
    std::cout << " MdtChamberGeometry " << m_id << std::endl;
    std::cout << "  nml " << m_nml << " nlay " << m_nlay << " ntube1 " << m_ntubesml[0]
	      << " ntube2 " << m_ntubesml[1] << std::endl;
    std::cout << " pos ml1 " << m_firstTube[0] << "  ml2 " << m_firstTube[1] << std::endl;
    std::cout << " tubeDist " << m_tubeDist << " tubeStage " << m_tubeStage
              << " layDist " << m_layDist << " tubeRad " << m_tubeRad << std::endl;
  }

}
