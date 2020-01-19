/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSums_v1.cxx 687949 2015-08-06 15:48:49Z amazurov $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSums_v1.h"

namespace xAOD{

  CMXEtSums_v1::CMXEtSums_v1() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXEtSums_v1::initialize(const uint8_t crate,const uint8_t source)
  {
    setCrate( crate );
    setSourceComponent( source );
  }

  /// initialize
  void CMXEtSums_v1::initialize(const uint8_t crate,const uint8_t source,
                                const std::vector<uint16_t>& etVec,
                                const std::vector<uint16_t>& exVec,
                                const std::vector<uint16_t>& eyVec,
                                const std::vector<uint32_t>& etErrorVec,
                                const std::vector<uint32_t>& exErrorVec,
                                const std::vector<uint32_t>& eyErrorVec,
                                const uint8_t peak)
  {
    setCrate( crate );
    setSourceComponent( source );
    setEtVec( etVec );
    setExVec( exVec );
    setEyVec( eyVec );
    setEtErrorVec( etErrorVec );
    setExErrorVec( exErrorVec );
    setEyErrorVec( eyErrorVec );
    setPeak( peak );
  }

  
  /// add data to existing object
  /// add Et
  void CMXEtSums_v1::addEt(const std::vector<uint16_t>& etVec,const std::vector<uint32_t>& etErrorVec)
  {
    setEtVec( etVec );
    setEtErrorVec( etErrorVec );
  }
  
  /// add Ex
  void CMXEtSums_v1::addEx(const std::vector<uint16_t>& exVec,const std::vector<uint32_t>& exErrorVec)
  {
    setExVec( exVec );
    setExErrorVec( exErrorVec );    
  }
  
  /// add Ey
  void CMXEtSums_v1::addEy(const std::vector<uint16_t>& eyVec,const std::vector<uint32_t>& eyErrorVec)
  {
    setEyVec( eyVec );
    setEyErrorVec( eyErrorVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v1 , uint8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v1 , uint8_t , sourceComponent , setSourceComponent )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v1 , uint8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v1 , std::vector<uint16_t> , etVec , setEtVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v1 , std::vector<uint16_t> , exVec , setExVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v1 , std::vector<uint16_t> , eyVec , setEyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v1 , std::vector<uint32_t> , etErrorVec , setEtErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v1 , std::vector<uint32_t> , exErrorVec , setExErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v1 , std::vector<uint32_t> , eyErrorVec , setEyErrorVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get etVec at peak bunch crossing
  uint16_t CMXEtSums_v1::et() const
  {
    return etVec()[ peak() ];
  }

  /// get exVec at peak bunch crossing
  uint16_t CMXEtSums_v1::ex() const
  {
    return exVec()[ peak() ];
  }

  /// get eyVec at peak bunch crossing
  uint16_t CMXEtSums_v1::ey() const
  {
    return eyVec()[ peak() ];
  }

  /// get etErrorVec at peak bunch crossing
  uint32_t CMXEtSums_v1::etError() const
  {
    return etErrorVec()[ peak() ];
  }

  /// get exErrorVec at peak bunch crossing
  uint32_t CMXEtSums_v1::exError() const
  {
    return exErrorVec()[ peak() ];
  }

  /// get eyErrorVec at peak bunch crossing
  uint32_t CMXEtSums_v1::eyError() const
  {
    return eyErrorVec()[ peak() ];
  }

} // namespace xAOD

