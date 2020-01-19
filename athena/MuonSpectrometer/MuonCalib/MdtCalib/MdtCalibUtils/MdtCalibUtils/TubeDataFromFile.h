/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_TUBEDATAFROMFILE_H
#define MUONCALIB_TUBEDATAFROMFILE_H

#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include <vector>
#include <iostream>

namespace MuonCalib {
  /** 
   @class TubeDataFromFile
   Manage the I/O of MdtTubeFitContainer objects.
   @author domizia.orestano@cern.ch
   @date july 2005 
  */

  class TubeDataFromFile {
  public:
    typedef std::vector<MdtTubeFitContainer*>   TubeData;
  public:
    TubeDataFromFile() : m_regions(0) {}
    ~TubeDataFromFile();
    
    /** return number of regions */
    unsigned int nRegions() const { return m_regions; }

    /** retrieve MdtTubeFitContainer for a give regionId */
    MdtTubeFitContainer* getTubes( unsigned int regionId ) const { 
      if( regionId >= (unsigned int)m_regions ){
	std::cout << "TubeDataFromFile::getTubes ERROR <regionId out of range> "
		  << regionId << " size " << m_regions << std::endl;
	return 0;
      }
      return m_tubeData[regionId];
    }

    /** set total number of regions */
    void setNRegions( unsigned int n ) { m_regions = n; m_tubeData.resize(n); }

    /** TubeDataFromFile takes ownership of the MdtTubeFitContainer */
    bool addTubes( int regionId, MdtTubeFitContainer* tubes ){
      if( regionId < 0 || regionId >= (int)m_regions ){
	std::cout << "TubeDataFromFile::addTubes ERROR <regionId out of range> "
		  << regionId << " size " << m_regions << std::endl;
	return false;
      }
      if( m_tubeData[regionId] != 0 ){
	std::cout << "TubeDataFromFile::addTubes ERROR <tubes already set> " << std::endl;
	return false;
      }

      m_tubeData[regionId] = tubes;

      return true;
    }

    std::ostream& write( std::ostream& os) const;

    void write_forDB( FILE *ftube, int mdt_head, int lowrun, int uprun) const;

  private:
    /** total number of regions */
    unsigned int m_regions;
    
    /** data */
    TubeData m_tubeData;
  };

}

std::ostream& operator<< (std::ostream& os, const MuonCalib::TubeDataFromFile& data);


#endif
