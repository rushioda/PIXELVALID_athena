/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/CorrelationYX.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
//#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "ers/ers.h"


static dqi::CorrelationYX staticInstance;


namespace dqi {

  // *********************************************************************
  // Public Methods
  // *********************************************************************

  CorrelationYX::
  CorrelationYX()
    : name("CorrelationYX")
  {
    dqm_core::AlgorithmManager::instance().registerAlgorithm( name, this );
  }


  CorrelationYX::
  CorrelationYX( const CorrelationYX& other )
    : dqm_core::Algorithm(other)
    , name(other.name)
  {
  }


  CorrelationYX::
  ~CorrelationYX()
  {
  }


  dqm_core::Algorithm*
  CorrelationYX::
  clone()
  {
    return new CorrelationYX(*this);
  }


  dqm_core::Result*
  CorrelationYX::
  execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
  {
    TH1 * histogram;

    if( object.IsA()->InheritsFrom( "TH1" ) ) {
      histogram = (TH1*)&object;
      if (histogram->GetDimension() > 2 ){
	throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
      }
    } else {
      throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
    }

    int binStart, binEnd;
    int CountsTh;
    int method;
    double gmin;
    double rmin;
    try {
      binStart  = (int) GetFirstFromMap("BinStart", config.getParameters() );
      binEnd    = (int) GetFirstFromMap("BinEnd"  , config.getParameters() );
      rmin      = GetFromMap( "CorrMin", config.getRedThresholds());
      gmin      = GetFromMap( "CorrMin", config.getGreenThresholds() );
    }
    catch ( dqm_core::Exception & ex ) {
      throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
    }


    double count = 0;
    double avX   = 0;
    double avY   = 0;
    double avX2  = 0;
    double avY2  = 0;
    double avXY  = 0;
    double sdX   = 0;
    double sdY   = 0;
    double correlation=0;    
  
    int nbinx = histogram -> GetNbinsX(); 
    int nbiny = histogram -> GetNbinsY(); 
    if(binStart>binEnd) {
      binStart= 1;
      binEnd  = histogram -> GetNbinsX();
    }
    int binSize = binEnd-binStart+1;

    std::vector<double> contents;
    contents.resize(binSize);
    for(int i=0;i<binSize;i++) {
      int binx = binStart+i;
      contents[i]=0;
      for ( int j = 1; j <= nbiny; ++j ) {
	double content= histogram -> GetBinContent(binx,j);
	contents[i]+=content;
	for(int k=0; k!=content;k++) {
	  count ++           ;
	  avX   +=      binx ;
	  avY   +=	  j ;
	  avX2  += binx*binx ;
	  avY2  +=	j*j ;
	  avXY  +=    binx*j ; 
	}
      }
    }
  
    if(count>0) {
      avX   = avX  / count;
      avY   = avY  / count;
      avX2  = avX2 / count;
      avY2  = avY2 / count;
      avXY  = avXY / count;
    }

    sdX  = sqrt(avX2-avX*avX)   ;
    sdY  = sqrt(avY2-avY*avY)   ;
    if(sdX>0&&sdY>0)correlation=(avXY-avX*avY)/(sdX*sdY); 


    ERS_DEBUG(1,"Number of entries for bins is "<< count );
    ERS_DEBUG(1,"Correlation biny and binx is "<< correlation );
    ERS_DEBUG(1,"Green: "<< gmin << " entries;   Red: " << rmin << " entries ");

    dqm_core::Result* result = new dqm_core::Result();
  

  
    if ( correlation >= gmin ) {
      result->status_ = dqm_core::Result::Green;
    } else if ( correlation > rmin ) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
    result->tags_["Correlation"] = correlation;
   

    return result;
  }


  void
  CorrelationYX::
  printDescription()
  {
    std::string message;
    message += "\n";
    message += "Algorithm: \"" + name + "\"\n";
    message += "Description: Counts the number of listed bins with entries above CountsTh\n";
    message += "Parameters:  BinStart first bin to be checked (1=first bin)\n";
    message += "             BinEnd   last bin to be checked (nbin=last bin)\n";
    message += "             CorrMin  minimum correlation Red/Green\n";
    message += "\n";
  
    std::cout << message;
  }


  // *********************************************************************
  // Protected Methods
  // *********************************************************************

  double CorrelationYX::GetFirstFromMap(const std::string & pname, const std::map<std::string,std::vector<double> > & params)
  {
    std::map<std::string,std::vector<double> >::const_iterator it = params.find(pname);
    if ( it != params.end() && it->second.size() ){
      return it->second[0];
    }else {
      throw dqm_core::BadConfig( ERS_HERE, "None", pname );
    }
  }

  std::vector<double> CorrelationYX::GetVectorFromMap(const std::string & pname, const std::map<std::string,std::vector<double> > & params)
  {
    std::map<std::string,std::vector<double> >::const_iterator it = params.find(pname);
    if ( it != params.end() && it->second.size() ){
      return it->second;
    }else {
      throw dqm_core::BadConfig( ERS_HERE, "None", pname );
    }
  }

  double CorrelationYX::GetFromMap( const std::string & pname, const std::map<std::string,double> & params )
  {
    std::map<std::string,double>::const_iterator it = params.find( pname );
    if ( it != params.end() ){
      return it->second;
    }else {
      throw dqm_core::BadConfig( ERS_HERE, "None", pname );
    }
  }

} // namespace dqi
