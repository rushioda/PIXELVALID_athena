/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameters.h
 * header file containing enumerated type of parameters which may be retrieved
 * by the ISCT_ConditionsParameterSvc, and their string interpretation
 * @author shaun.roe@cern.ch
**/
#ifndef SCT_ConditionsParameters_h
#define SCT_ConditionsParameters_h
#include <string>
namespace SCT_ConditionsServices{
  enum ParameterIndex{AVG_THRESHOLD,N_PARAMETERS,INVALID_PARAMETER};
  static const std::string parameterNames[]={"average threshold per module side","number of parameters"};
}
#endif

