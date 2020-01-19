/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLHandlerStore_H
#define XMLHandlerStore_H

class XMLHandler;

#include <map>
#include <string>

#include <xercesc/dom/DOM.hpp>

//using namespace xercesc;

typedef std::map<std::string,XMLHandler*,std::less<std::string> > handlerStore;

class XMLHandlerStore:public handlerStore {
public:
	static XMLHandlerStore* GetHandlerStore();
	void RegisterHandler(XMLHandler*);
	XMLHandler* GetHandler(xercesc::DOMNode *);
	void Handle(xercesc::DOMNode *);
private:
	XMLHandlerStore();
	static XMLHandlerStore *s_theStore;
};

#endif
