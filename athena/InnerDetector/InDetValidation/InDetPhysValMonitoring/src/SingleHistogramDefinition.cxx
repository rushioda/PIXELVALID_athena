/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//
//  SingleHistogramDefinition.cpp
//  HDef
//
//  Created by sroe on 13/07/2015.
//

#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <utility>
#include <limits>
SingleHistogramDefinition::SingleHistogramDefinition() :
  name{},
  histoType{},
  title{},
  nBinsX{},
  nBinsY{},
  xAxis(std::make_pair(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN())),
  yAxis(std::make_pair(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN())),
  xTitle{},
  yTitle{},
  allTitles{},
  folder{},
  m_empty(true) {
}

SingleHistogramDefinition::SingleHistogramDefinition(Titles_t thename, Titles_t thehistoType,
                                                     Titles_t thetitle, NBins_t nbinsX, Var_t xLo, Var_t xHi,
                                                     Titles_t xName, Titles_t yName,
                                                     Titles_t thefolder) :
  name(thename), histoType(thehistoType), title(thetitle), nBinsX(nbinsX),
  nBinsY(0), xTitle(xName), yTitle(yName), folder(thefolder) {
  // should do sanity checks here
  xAxis = std::make_pair(xLo, xHi);
  yAxis = std::make_pair(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
  m_empty = false;
  allTitles = titleDigest();
}

SingleHistogramDefinition::SingleHistogramDefinition(Titles_t thename, Titles_t thehistoType,
                                                     Titles_t thetitle, NBins_t nbinsX, NBins_t nbinsY, Var_t xLo,
                                                     Var_t xHi, Var_t yLo,
                                                     Var_t yHi, Titles_t xName, Titles_t yName, Titles_t thefolder) :
  name(thename), histoType(thehistoType), title(thetitle), nBinsX(nbinsX), nBinsY(nbinsY),
  xTitle(xName), yTitle(yName), folder(thefolder) {
  // should do sanity checks here
  xAxis = std::make_pair(xLo, xHi);
  yAxis = std::make_pair(yLo, yHi);
  m_empty = false;
  allTitles = titleDigest();
}

bool
SingleHistogramDefinition::empty() const {
  return m_empty;
}

std::string
SingleHistogramDefinition::str() const {
  const std::string s(" ");

  return name + s + histoType + s + title + s + std::to_string(nBinsX) + s + std::to_string(nBinsY) +
         s + std::to_string(xAxis.first) + s + std::to_string(xAxis.second) + s +
         std::to_string(yAxis.first) + s + std::to_string(yAxis.second) + s + xTitle + s + yTitle;
}

bool
SingleHistogramDefinition::validType() const {
  const std::string signature((histoType.substr(0, 3)));

  return((signature == "TH1")or(signature == "TH2") or(signature == "TPr") or(signature == "TEf"));
}

bool
SingleHistogramDefinition::isValid() const {
  if (name.empty() or histoType.empty()) {
    return false;
  }
  bool sane(true);
  // note: if yaxis is left undefined, the limits should be NaN, but (NaN != NaN) is always true
  const bool sensibleLimits = (xAxis.first != xAxis.second)and(yAxis.first != yAxis.second);
  const bool sensibleXBins = (nBinsX != 0);
  const bool sensibleYBins = (nBinsY != 0);
  const bool sensibleTitles = not (title.empty() or xTitle.empty());
  sane = (sensibleLimits and sensibleXBins and sensibleTitles);
  if (histoType.substr(0, 3) == "TH2") {
    sane = (sane and sensibleYBins);
  }
  return sane;
}

std::string
SingleHistogramDefinition::stringIndex() const {
  return stringIndex(name, folder);
}

std::string
SingleHistogramDefinition::titleDigest() const {
  const std::string s(";");

  return title + s + xTitle + s + yTitle;
}

std::string
SingleHistogramDefinition::stringIndex(const std::string& thisname, const std::string& thisfolder) {
  if (thisfolder.empty()) {
    return thisname;
  }
  const std::string delimiter("/");
  std::string result(thisfolder);
  if ((thisfolder.substr(0, 1)) == delimiter) {
    result = thisfolder.substr(1, thisfolder.size() - 2);// reduce "/myfolder" to "myfolder"
  }
  size_t lastChar(result.size() - 1);
  if ((result.substr(lastChar, 1) != delimiter)) {
    result = result + delimiter; // add a slash: "myfolder" => "myfolder/"
  }
  return result + thisname;
}
