/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_SELECTION_H
#define POOL_SELECTION_H

#include <vector>
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"

namespace pool    {

  typedef enum { EQ, NE, GT, GE, LT, LE } SelectionItemType;

  class Selection{
    public:

    static const int endOfRange = -1;
    static std::string indexVariable();
    static std::vector<std::string>& selectionTypes();
    static std::string variableNameFromUniqueString(const std::string& uniqueString);

    public:
    
    Selection();

    virtual ~Selection();

    void addIndexItem( int startIndex, int endIndex );
    
    template <typename Prim> void addDataItem(const std::string& dataMemberName, SelectionItemType stype, Prim selectionData);

    bool isEmpty() const;
    
    const std::vector<std::pair<std::string,std::string> >& items() const;
    
    const coral::AttributeList& data() const;

    private:

    std::string uniqueVariableName(const std::string& varName) const;
    
    private:

    std::vector<std::pair<std::string,std::string> > m_items;
    coral::AttributeList m_data;
  };
  
}

template <typename Prim> void pool::Selection::addDataItem(const std::string& dataMemberName, pool::SelectionItemType stype, Prim selectionData){
  std::string varName = uniqueVariableName( dataMemberName );
  m_items.push_back(std::make_pair(varName,selectionTypes()[stype]));
  m_data.extend<Prim>(varName);
  m_data[varName].data<Prim>()=selectionData;
}

#endif  // POOL_SELECTION_H

