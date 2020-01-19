/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYTHIA8_USERHOOKS_USERSETTING_H
#define PYTHIA8_USERHOOKS_USERSETTING_H

#include "Pythia8/Settings.h"
#include "Pythia8_i/UserHooksFactory.h"
#include <string>
#include <iostream>
#include <stdexcept>


namespace Pythia8_UserHooks{
 
  using std::string;
  
  template <class T>
  class UserSetting{
    
    public:
    
  UserSetting(string name, T defaultValue):
    m_paramName(name),
    m_param(defaultValue),
    m_settingsPtr(0),
    m_retrieved(false){
    
      
      typename std::map<std::string, T>::const_iterator test = UserHooksFactory::userSettings<T>().find(m_paramName);
      if(test != UserHooksFactory::userSettings<T>().end()) throw std::runtime_error("Duplicate user-defined setting already exists: " + m_paramName);
      UserHooksFactory::userSettings<T>()[m_paramName] = defaultValue;
      
      
    }
    
    T operator()(Pythia8::Settings *settingsPtr){
    
      if(m_settingsPtr == 0 && settingsPtr == 0) throw std::runtime_error("settingsPtr is not yet initialised!");
      
      if(m_retrieved && m_settingsPtr == settingsPtr) return m_param;
      
      m_settingsPtr = settingsPtr;
      m_param = uncachedRetrieve();
      m_retrieved = true;
      return m_param;
    };
    
    protected:
    
    string m_paramName;
    T m_param;
    Pythia8::Settings *m_settingsPtr;
    bool m_retrieved;
    
    private:
    
    T uncachedRetrieve();
    
  };
  
  template<>
  inline double UserSetting<double>::uncachedRetrieve(){
    if(m_settingsPtr->isParm(m_paramName)){
      return m_settingsPtr->parm(m_paramName);
    }
    
    throw std::runtime_error("UserSetting " + m_paramName + " does not exist!");
    return m_param;
  }
  
  template<>
  inline int UserSetting<int>::uncachedRetrieve(){
    if(m_settingsPtr->isMode(m_paramName)){
      return m_settingsPtr->mode(m_paramName);
    }
    
    throw std::runtime_error("UserSetting " + m_paramName + " does not exist!");
    return m_param;
  }
  
  template<>
  inline string UserSetting<string>::uncachedRetrieve(){
    if(m_settingsPtr->isWord(m_paramName)){
      return m_settingsPtr->word(m_paramName);
    }
    
    throw std::runtime_error("UserSetting " + m_paramName + " does not exist!");
    return m_param;
  }
  
}
#endif
