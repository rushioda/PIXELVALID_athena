//$Id: CommandLine.cpp 662167 2015-04-22 04:14:30Z ssnyder $
/* ****************************************************************************
*
*  CommandLine.cpp
*  Created by John F. Hubbard, on Sat Jul 29 2000, 19:01:05 PST
*
*  Copyright (c) 2000, ATD Azad Technology Development Corporation
*
*            The Reliable Software Outsource Resource
*        You hire us, we do it for you, and we do it right.
*
*                       www.azadtech.com
*
*  Permission is granted to use this code without restriction,
*  as long as this copyright notice appears in all source files.
*
*  File Contents: Implementation of the CommandLine class.
*  Please see CommandLine.h for documentation of this class.
*
* %version: 2.30 %
* %date_modified: Thu Nov 02 16:10:59 2000 %
* %created_by: jhubbard %
*
*************************************************************************** */

#include <iostream>
#include <fstream>
#include <set>
#include "FileCatalog/CommandLine.h"

using namespace std;

pool::CommandLine::CommandLine(int argc, char* argv[])
: mnParameterCount(0)
{
  ParseCommandLine(argc, argv);

  if (Exists("settings_file"))
  {
    std::string strInput = LoadSettingsFile(GetByName("settings_file"));
    ParseSettings(strInput);
  }
}

pool::CommandLine::CommandLine(int argc, char* argv[], const std::string& strFileName)
: mnParameterCount(0)
{
  ParseCommandLine(argc, argv);

  std::string strInput = LoadSettingsFile(strFileName);
  ParseSettings(strInput);
}

pool::CommandLine::CommandLine(const std::string& strFileName)
: mnParameterCount(0)
{
  std::string strInput = LoadSettingsFile(strFileName);
  ParseSettings(strInput);
}

pool::CommandLine::~CommandLine()
{
}

void
pool::CommandLine::ParseCommandLine(int argc, char* argv[])
{
  mstrProgramName = argv[0];

  std::string strInput;
  std::string strWord;
  for (int i = 1; i < argc; ++i)
  {
    // It seems awkward to add quotes back in, after the shell has helpfully
    // stripped them out, but this allows me to treat an input file (which
    // may have quotes) and the command line input in the same way: as a
    // space-delimited string.
    strWord = argv[i];
    if (strWord.find(' ') != strWord.npos)
    {
      strWord = std::string("\"") + strWord + '"';
    }
    strInput += strWord + ' ';
  }
  ParseSettings(strInput);
}

void
pool::CommandLine::ParseSettings(const std::string& strInput)
{
  // Parse the file into words:
  std::istringstream ist(strInput.data());
  std::string strWord;
  std::string strNextWord;
  std::pair<ARGMAPTYPE::const_iterator, bool> pResult;

  while ((strNextWord.length()) > 0 || (ist >> strWord))
  {
    if (strNextWord.length() > 0) {
       strWord = strNextWord; // get word from last iteration
    }
    strNextWord.erase(strNextWord.begin(), strNextWord.end());
    if (IsParamName(strWord)) {
       if( ((ist >> strNextWord) && IsParamName(strNextWord))
            || strNextWord.length() == 0 )
       {
          pResult = mArgMap.insert(std::make_pair(GetParamName(strWord),std::string("")));
          if(!pResult.second) {
             throw(std::string("Detected a duplicate command line parameter. (1)"));
          }
          ++mnParameterCount;
       }
       else if ( strNextWord.length() > 0 )
       {
          TranslateQuotes(ist, strNextWord);
          pResult = mArgMap.insert(std::make_pair(GetParamName(strWord),strNextWord));
          if(!pResult.second) {
             throw(std::string("Detected a duplicate command line parameter. (2)"));
          }
          ++mnParameterCount;
          if(! (ist >> strNextWord))
          { break;
          }
       } // end if( ((ist >> strNextWord
    }
    else {
       throw(std::string("Improperly formed command line arguments."));
    } // end if(IsParamName...
  } // end while ((strNextWord....
}


void
pool::CommandLine::CheckOptions(const char* opts[])
{
   std::set<std::string>       acceptedOptions;
   for(size_t i=0; opts[i]; i++) {
      acceptedOptions.insert( opts[i] );
   }
   for( ARGMAPTYPE::const_iterator i=mArgMap.begin(); i!=mArgMap.end(); ++i ) {
      if( acceptedOptions.find(i->first) == acceptedOptions.end() ) {
         throw string("Unknown option: -") + i->first;
      }
   }
}

void
pool::CommandLine::TranslateQuotes(std::istringstream& ist, std::string& strWord)
{
  if (strWord.length() > 1 && strWord[0] == '"')
  {
    std::string strNextWord;
    std::string strFullWord = GetParamName(strWord.substr(1, strWord.length() - 1));

    while ( (ist >> strNextWord) &&
      (strNextWord.length() > 0) &&
      (strNextWord[strNextWord.length() - 1] != '"') )
    {
      strFullWord += ' ';
      strFullWord += strNextWord;
    }

    if (strNextWord[strNextWord.length() - 1] == '"')
    {
      strFullWord += ' ';
      strFullWord += strNextWord.substr(0, strNextWord.length() - 1);
    }
    else
    {
      throw (std::string("Mismatched quotes in CommandLine arguments."));
    }
    strWord = strFullWord;
  }
  // else, strWord is unchanged.
}

std::string
pool::CommandLine::LoadSettingsFile(const std::string& strFileName)
{
  std::ifstream settingsFile(strFileName.c_str());
  std::string strInput;
  char  ch;

  if(!settingsFile)
  {
    throw (std::string("Failed to find settings file: " + strFileName));
  }

  while (settingsFile.get(ch))
  {
    if (ch == 0x0A || ch == 0x0D) // Get rid of CR-LF characters
    {	strInput += ' ';
    }
    else
    {	strInput += ch;
    }
  }
  return strInput;
}

std::string
pool::CommandLine::GetByName( const std::string& strArgName ) const
{
  ARGMAPTYPE::const_iterator it = mArgMap.find(strArgName);
  if( it != mArgMap.end() )
  {
    return it->second;
  }
  throw (strArgName + " is required, but was not found.");
}

std::string
pool::CommandLine::Usage()
{
  // This base class does not know anything specific about usage.
  std::string strUsage ("The CommandLine base class has a built-in parameter: "
    "-settings_file <filename>.\n"
    "This is used to specify a settings file, which can "
    "contain exactly\n"
    "the same arguments that are used on the command line.");
  return strUsage;
}

void
pool::CommandLine::DumpDiagnostics() const
{
  using std::cout;
  using std::endl;

  cout << endl << "Number of parameters (parameter values are not counted): "
    << Count();
  cout << endl << "Parameter name" << "\t\t\tValue" << endl;
  cout << "-------------------------------------" << endl;
  for(ARGMAPTYPE::const_iterator it = mArgMap.begin();
    it != mArgMap.end(); it++)
  {
    cout << it->first << ":\t\t\t\t" << it->second << endl;
  }
  cout << endl;
}
