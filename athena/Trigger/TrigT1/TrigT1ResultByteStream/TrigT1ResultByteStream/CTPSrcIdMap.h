/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_CTPSRCIDMAP_H
#define TRIGT1RESULTBYTESTREAM_CTPSRCIDMAP_H

#include <cstdint>

/** This class provides conversion between Lower level Source ID to
    higher level source ID for CTP ByteStream fragments.
    This is to be used in assembling the fragments from ROD fragments

    * @author Tadashi Maeno
*/
class CTPSrcIdMap {

public:
  /** constrcutor
  */
  CTPSrcIdMap();

  /** get a ROD Source ID
  */
  uint32_t getRodID ();

  /** Make a ROB Source ID from a ROD source ID
  */
  uint32_t getRobID (uint32_t rod_id);

  /** Make a ROS Source ID from a ROB source ID
  */
  uint32_t getRosID (uint32_t rob_id);

  /** Make a SubDetector ID from ROS source ID
  */
  uint32_t getDetID (uint32_t ros_id);

};

#endif // TRIGT1RESULTBYTESTREAM_CTPSRCIDMAP_H
