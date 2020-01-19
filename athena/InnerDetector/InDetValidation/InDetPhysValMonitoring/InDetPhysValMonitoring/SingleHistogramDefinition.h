/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  SingleHistogramDefinition.h
//  HDef
//
//  Created by sroe on 07/07/2015.
//

#ifndef HDef_SingleHistogramDefinition_h
#define HDef_SingleHistogramDefinition_h
#include "InDetPhysValMonitoring/IHistogramDefinitionSvc.h"
#include <string>
///Almost-a-struct for holding the single histogram definition
struct SingleHistogramDefinition{
    typedef const std::string & Titles_t;
    typedef const unsigned int NBins_t;
    typedef const float Var_t;
    SingleHistogramDefinition();
    
    ///C'tor for 1D histograms (no Y bins or limits)
    SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, NBins_t nbinsX,Var_t xLo, Var_t xHi,Titles_t xName,Titles_t yName,Titles_t theFolder="");
    
    ///C'tor for 2D histograms (Y bins and limits)
    SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, NBins_t nbinsX,  NBins_t nbinsY,Var_t xLo, Var_t xHi, Var_t yLo, Var_t yHi, Titles_t xName, Titles_t yName, Titles_t theFolder="");
    
    std::string name;
    std::string histoType;
    std::string title;
    unsigned int nBinsX;
    unsigned int nBinsY;
    IHistogramDefinitionSvc::axesLimits_t xAxis;
    IHistogramDefinitionSvc::axesLimits_t yAxis;
    std::string xTitle;
    std::string yTitle;
    std::string allTitles;
    std::string folder;
    bool empty() const;
    std::string str() const;
    bool validType() const;
    bool isValid() const;
    std::string stringIndex() const;
    static std::string stringIndex(const std::string & thisname, const std::string & thisfolder);
    std::string titleDigest() const;

    bool m_empty;
};

#endif
