/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __IIOVCONDDBSVC_H__
#define __IIOVCONDDBSVC_H__

/*****************************************************************************
 *
 *  IIOVCondDbSvc.h
 *  AthenaKernel
 *
 *  Author: RD Schaffer, Antoine Perus
 *  $Id: IIOVCondDbSvc.h,v 1.8 2009-01-29 16:53:51 hawkings Exp $
 *
 *  Abstract interface to IOVDbSvc to access ConditionsDB
 *
 *****************************************************************************/

#include "GaudiKernel/IInterface.h"
#ifndef _CPP_STRING
#include <string>
#endif
#ifndef _CPP_VECTOR
#include <vector>
#endif
#ifndef KERNEL_STATUSCODES_H
#include "GaudiKernel/StatusCode.h"
#endif
#ifndef GAUDIKERNEL_CLASSID_H
#include "GaudiKernel/ClassID.h"
#endif

#include "CoolKernel/types.h"
#include "CoolKernel/IDatabase.h"

// Declaration of the interface ID. 
static const InterfaceID IID_IOVCondDbSvc("IOVCondDbSvc", 1 , 0);   

class IOVTime;

class IIOVCondDbSvc  : virtual public IInterface  {
public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID();

    /// COOL access
    virtual cool::IDatabasePtr getDatabase(bool readOnly = false ) = 0;

};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline
const InterfaceID& 
IIOVCondDbSvc::interfaceID() {
    static const InterfaceID IID("IOVCondDbSvc", 1, 0);
    return IID;
}

#endif
