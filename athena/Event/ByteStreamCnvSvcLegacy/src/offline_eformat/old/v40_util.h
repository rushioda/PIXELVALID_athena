/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_OFFLINE_EFORMAT_OLD_V40_UTIL_H
#define BYTESTREAMCNVSVC_OFFLINE_EFORMAT_OLD_V40_UTIL_H 1

/**
 * @file util.h
 *
 * @brief Declarations of methods for working with old eformat versions.
 *
 * @author Andy Salnikov
 */

#include <stdint.h>
#include "eformat/checksum.h"

namespace offline_eformat {
namespace v40 {

uint32_t convert_to_40(const uint32_t* src, uint32_t* dest, uint32_t max,
                       eformat::CheckSum event_checksum=eformat::NO_CHECKSUM,
                       eformat::CheckSum rob_checksum=eformat::NO_CHECKSUM);

}} // namespace offline_eformat::V40

#endif // BYTESTREAMCNVSVC_OFFLINE_EFORMAT_OLD_V40_UTIL_H
