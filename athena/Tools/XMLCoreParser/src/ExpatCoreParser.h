/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ExpatCoreParser_h__
#define __ExpatCoreParser_h__

#include <expat.h>
#include <map>
#include "DOMNode.h"

class ExpatCoreParser
{
public:

  typedef std::map <std::string, std::string> ExternalEntityMap; 

  static CoreParser::DOMNode* parse (const std::string& file_name);

  static ExpatCoreParser& instance ();

  static void start (void* /*user_data*/, const char* el, const char** attr);
  static void end (void* /*user_data*/, const char* el);
  static void char_data (void* /*user_data*/, const XML_Char* s, int len);
  static void default_handler (void* /*user_data*/, const XML_Char* s, int len);
  static void comment (void* /*userData*/, const XML_Char* s);
  static int external_entity (XML_Parser parser,
			      const XML_Char* context,
			      const XML_Char* /*base*/,
			      const XML_Char* systemId,
			      const XML_Char* /*publicId*/);
  static void entity (void* userData,
		      const XML_Char* entityName,
		      int is_parameter_entity,
		      const XML_Char* value,
		      int value_length,
		      const XML_Char* base,
		      const XML_Char* systemId,
		      const XML_Char* publicId,
		      const XML_Char* /*notationName*/);
  CoreParser::DOMNode* get_document ();

  static void register_external_entity (const std::string& name, const std::string& file_name);  
  static void register_text_entity (const std::string& name, const std::string& text);  
 
private:

  ExpatCoreParser ();
  void do_start (const char* el, const char** attr);
  void do_end (const char* el);
  void do_char_data (const XML_Char* s, int len);
  void do_default_handler (const XML_Char* s, int len);
  void do_comment (const XML_Char* s);
  int generic_parse (XML_Parser p, const std::string& file_name);
  int generic_text_parse (XML_Parser p, const std::string& text);
  int do_external_entity (XML_Parser parser,
			  const XML_Char* context,
			  const XML_Char* systemId);

  void do_register_external_entity (const std::string& name, const std::string& file_name);  
  void do_register_text_entity (const std::string& name, const std::string& text);  
  const std::string& find_external_entity (const std::string& name) const;
  const std::string& find_text_entity (const std::string& name) const;

  void clean ();

  CoreParser::DOMNode* m_top;
  CoreParser::DOMNode* m_last;
  std::string m_prefix;
  ExternalEntityMap m_entities; 
  ExternalEntityMap m_text_entities; 
};

#endif

