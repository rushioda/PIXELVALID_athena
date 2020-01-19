// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    label.h        
//
//                   
//  
//
//   $Id: label.h, v0.0   Sun 26 Aug 2012 21:32:27 BST sutt $


#ifndef  LABEL_H
#define  LABEL_H

#include <cstdio>
#include <iostream>


inline std::string label( const std::string& format, int i ) { 
  char c[256];
  std::sprintf( c, format.c_str(), i );
  return c;
}

inline std::string label( const std::string& format, unsigned i ) { 
  char c[256];
  std::sprintf( c, format.c_str(), i );
  return c;
}

inline std::string label( const std::string& format, int i, int j ) { 
  char c[256];
  std::sprintf( c, format.c_str(), i, j );
  return c;
}

inline std::string label( const std::string& format, double d ) { 
  char c[256];
  std::sprintf( c, format.c_str(), d );
  return c;
}

inline std::string label( const std::string& format, double d, double e ) { 
  char c[256];
  std::sprintf( c, format.c_str(), d, e );
  return c;
}

inline std::string label( const std::string& format, double d, double e, int i ) { 
  char c[256];
  std::sprintf( c, format.c_str(), d, e, i );
  return c;
}



inline std::string label( const std::string& format, const std::string& s ) { 
  char c[256];
  std::sprintf( c, format.c_str(), s.c_str());
  return c;
}

#endif  // LABEL_H 










