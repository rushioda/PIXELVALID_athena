/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __JETCLEANINGTOOL__
#define __JETCLEANINGTOOL__

/**
   @class JetCleaningTool
   @brief Class for selecting jets that pass cleaning cuts

   @author Zach Marshall
   @date   Feb 2014
*/

// Stdlib includes
#include <string>
#include <vector>
#include <unordered_map>

// Base classes
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetSelector.h"

// The xAOD jet type
#include "xAODJet/Jet.h"

// Return object
#include "PATCore/TAccept.h"

namespace JCT { class HotCell; }

class JetCleaningTool : public asg::AsgTool , virtual public IJetSelector
{

  ASG_TOOL_CLASS(JetCleaningTool,IJetSelector)

  public: 
    /** Levels of cut */
    enum CleaningLevel{ LooseBad , LooseBadLLP, LooseBadTrigger, TightBad , UnknownCut };

    /** Standard constructor */
    JetCleaningTool(const std::string& name="JetCleaningTool");

    /** Cut-based constructor */
    JetCleaningTool(const CleaningLevel alevel, const bool doUgly=false);

    /** Cut and string based constructor */
    JetCleaningTool(const std::string& name , const CleaningLevel alevel, const bool doUgly=false);

    /** Standard destructor */
    virtual ~JetCleaningTool();

    /** Initialize method */
    virtual StatusCode initialize();

    /** The main accept method: the actual cuts are applied here */
    const Root::TAccept& accept( const double emf,
                 const double hecf,
                 const double larq,
                 const double hecq,
                 //const double time,     //in ns
                 const double sumpttrk, //in MeV, same as sumpttrk
                 const double eta,      //emscale Eta  
                 const double pt,       //in MeV, same as sumpttrk
                 const double fmax,
                 const double negE ,     //in MeV
                 const double AverageLArQF,
                 const int    fMaxIndex     ) const;

    /** The D3PDReader accept method */
    const Root::TAccept& accept( const xAOD::Jet& jet) const;

    int keep(const xAOD::Jet& jet) const
#if __cplusplus >= 201100
                  final
#endif // GCC XML, you make me so sad, with your lack of C++11 support making my code ugly
                 { return accept(jet); }

    /** Hot cell checks */
    bool containsHotCells( const xAOD::Jet& jet, const unsigned int runNumber) const;

    /** Helpers for cut names */
    CleaningLevel getCutLevel( const std::string ) const;
    std::string   getCutName( const CleaningLevel ) const;

  private:
    /** Name of the cut */    
    std::string m_cutName; 
    CleaningLevel m_cutLevel;
    bool m_doUgly;

    /** Previous decision */
    mutable Root::TAccept m_accept;

    /** Hot cells caching */
    std::string m_hotCellsFile;
    std::unordered_map<unsigned int, std::vector<JCT::HotCell*>*>* m_hotCellsMap;
    StatusCode readHotCells();

}; // End: class definition



#endif
