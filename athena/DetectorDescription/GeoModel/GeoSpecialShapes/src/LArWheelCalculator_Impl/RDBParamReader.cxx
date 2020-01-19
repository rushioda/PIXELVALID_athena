/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RDBParamReader.h"
#include "RDBParamRecords.h"

// Helper class to simplify parameter reading
//#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <iostream>


RDBParamReader::RDBParamReader(IRDBAccessSvc* rdbAccess) :
	m_rdbAccess(rdbAccess)
{
	acquire();
}


RDBParamReader::~RDBParamReader() {
	release();
}

RDBParamRecords  RDBParamReader::data(const std::string& node, const std::string& tag, const std::string& tag2node) {
	IRDBRecordset_ptr recset = _getRecordsetPtr(node, tag, tag2node);
	return RDBParamRecords(this, recset);
}

IRDBRecordset_ptr  RDBParamReader::_getRecordsetPtr(const std::string& node, const std::string& tag, const std::string& tag2node) {
	return m_rdbAccess->getRecordsetPtr(node, tag, tag2node);
}

void RDBParamReader::acquire() {
  if (!m_rdbAccess->connect())
    std::cout << "ERROR: RDBParamReader: cannot connect.\n";
}

void RDBParamReader::release() {
	m_rdbAccess->disconnect();
}

