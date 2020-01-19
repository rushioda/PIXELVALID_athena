/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RodDecoder.h
//   Header file for class BCM_RodDecoder
///////////////////////////////////////////////////////////////////
// (c) ATLAS BCM Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////
//  Version 00-00-01 12/05/2008 Daniel Dobos
//  Version 00-00-11 05/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RODDECODER_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RODDECODER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetBCM_RawData/BCM_RDO_Collection.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetBCM_RawData/BCM_RawData.h"

#include "ByteStreamData/RawEvent.h" 
#include "eformat/SourceIdentifier.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

class BCM_RodDecoder : public AthAlgTool
{

public: 

  // Collection type
  typedef BCM_RawData RDO;
  typedef BCM_RDO_Collection BCM_RawCollection; 

  // constructor
  BCM_RodDecoder(const std::string& type, const std::string& name, const IInterface* parent);

  static const InterfaceID& interfaceID();

  // destructor 
  virtual ~BCM_RodDecoder(); 

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode fillCollection(const ROBFragment *robFrag, BCM_RDO_Container* rdoCont, std::vector<unsigned int>* vecHash = NULL);

  inline void setDet(const eformat::SubDetector det);

  unsigned int getChannelID(int ROD_source_ID, unsigned int dataword_position);
  BCM_RDO_Collection* getCollection(unsigned int chan, BCM_RDO_Container* cont);
  
private:

  eformat::SubDetector        m_det;

  unsigned int                m_fragment_number;
  unsigned int                m_LVL1A_number;
  unsigned int                m_hit_number;
  
};

inline void BCM_RodDecoder::setDet(const eformat::SubDetector det)
{
  m_det = det;
}

#endif //BCM_RAWDATABYTESTREAMCNV_BCM_RODDECODER_H
