/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_EVENTINFOREADER_H
# define SRC_EVENTINFOREADER_H

/**
 * @file EventInfoReader.h
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, checks tags on reading
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: EventInfoReader.h,v 1.3 2005-03-16 14:54:02 schaffer Exp $
 *
 */

/**
 * @class EventInfoReader
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo
 *
 */

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;
class IGeoModelSvc;

class EventInfoReader : public AthAlgorithm {
public:

    /// Algorithm constructor
    EventInfoReader(const std::string &name,ISvcLocator *pSvcLocator);

    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();
  
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:

    /// Avoid use of default constructor
    EventInfoReader();

    /// Avoid use of copy constructor
    EventInfoReader(const EventInfoReader&);

    /// Avoid use of copy operator
    EventInfoReader &operator=(const EventInfoReader&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    /// Access to GeoModelSvc for tags
    ServiceHandle<IGeoModelSvc> m_geoModel;
};


#endif // SRC_EVENTINFOREADER_H
