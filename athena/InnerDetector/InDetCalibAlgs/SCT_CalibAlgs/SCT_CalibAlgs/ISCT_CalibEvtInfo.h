/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file ISCT_CalibEvtInfo.h
* interface for SCT_CalibEvtInfo
* 
* author Shaun Roe
*
**/

#ifndef ISCT_CalibEvtInfo_H
#define ISCT_CalibEvtInfo_H

//Gaudi includes
#include "GaudiKernel/IInterface.h"
#include <vector>
#include <string>
#include "GaudiKernel/IIncidentListener.h"

/**
*  @class ISCTCalib_EvtInfo
*  Interface to class to deal with run/event information for SCTCalib
**/

class ISCT_CalibEvtInfo: virtual public IInterface,public IIncidentListener{
  public:
    /// no-op c/tor
    ISCT_CalibEvtInfo(){}
    /// no-op destructor
    virtual ~ISCT_CalibEvtInfo(){}
    /// interfaceID re-implemented from IInterface
    static const InterfaceID & interfaceID();
    /// incidentListener method
    virtual void handle( const Incident & thisIncident)=0;
    /// specific methods for this class
    //virtual void setOptions(const bool useBytestream, const bool useTrkVal, const bool sortChain, const bool readHist)=0;
    virtual void setTimeStamp(const std::string & begin, const std::string & end)=0;
    virtual void setTimeStamp(const int begin, const int end)=0;
    virtual void setTimeStamp(const int ts)=0;
    
    virtual void setSource(const std::string source)=0;
    
    virtual void setLumiBlock(const int begin, const int end)=0;
    virtual void setLumiBlock(const int lb)=0;
    
    virtual void setRunNumber(const int rn)=0;
    virtual void setBunchCrossing(const int bc)=0;
    virtual void setCounter(const int counterVal)=0;
    virtual void incrementCounter()=0;

	virtual void getTimeStamps(int & begin, int & end) const=0;
	virtual void getTimeStamps(std::string & begin, std::string & end) const=0;
	
    virtual int timeStamp()const=0;
    virtual int lumiBlock() const=0;
    virtual int runNumber() const=0;
    virtual int counter() const =0;
    virtual int duration() const=0;
    virtual int numLumiBlocks() const=0;

};


inline const InterfaceID & ISCT_CalibEvtInfo::interfaceID(){
  static const InterfaceID IID("ISCT_CalibEvtInfo",1,0);
  return IID;
}












#endif
