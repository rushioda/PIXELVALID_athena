/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IChargeCollProbSvc_H
#define IChargeCollProbSvc_H
 
// Include Files
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <string>
 
// Forward declarations
//class IIncidentListener;
//class Incident;
class StatusCode; 

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IChargeCollProbSvc("IChargeCollProbSvc", 1 , 0);
 
 
class IChargeCollProbSvc : virtual public IInterface {
public:
 
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IChargeCollProbSvc; }
 
 
    virtual StatusCode readProbMap(std::string) = 0;
    virtual StatusCode printProbMap( std::string ) = 0;
    virtual double getProbMapEntry(std::string, int, int)const = 0;
 
    //virtual MsgStream GetMsgStream() const =0;
 
};
 
 
#endif
