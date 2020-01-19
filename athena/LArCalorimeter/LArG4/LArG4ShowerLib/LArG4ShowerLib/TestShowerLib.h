/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SHOWER_LIB_TEST_SHOWER_LIB_H
#define SHOWER_LIB_TEST_SHOWER_LIB_H

// STL includes
#include <list>

// local includes
#include "LArG4ShowerLib/IShowerLib.h"

// G4 forward declarations
class G4Track;
// forward declarations in namespace
namespace HepMC {
	class FourVector;
}
// Namespace for the ShowerLib related classes
namespace ShowerLib {

  /**
   *
   *   @short Class for shower library shower lib
   *
   *          Store complete shower library (collection of energy bins)
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version \$Id: TestShowerLib.h 769594 2016-08-23 13:48:34Z ssnyder $
   *
   */

  class TestShowerLib : public IShowerLib {

  public:

    //! factory method. create a library from root file. returns NULL if file is invalid.
	static IShowerLib* readFromROOTFile(TFile* source);

	//! factory method. create empty library with the given structure. returns NULL if file is invalid.
	static IShowerLib* createEmptyLib(const std::string& inputFile);

    //! default destructor
    virtual ~TestShowerLib();

    //! get shower for given G4 track
    virtual std::vector<EnergySpot>* getShower(const G4Track* track, ShowerLibStatistics* stats, int randomShift) const;
    //! get average length of showers for the given energy
    virtual double getContainmentZ(const G4Track* track) const;
    //! get average lateral spread of the showers for the given energy
    virtual double getContainmentR(const G4Track* track) const;
    //! store shower in the library
    virtual bool storeShower(const HepMC::GenParticle* genParticle,const Shower* shower);
    //! write library to ROOT file
    virtual bool writeToROOT(TFile* dest);

    virtual ShowerLibStatistics* createStatistics() const;

    inline virtual const std::string getName() const { return "Test Energy ShowerLib"; }

  protected:

    virtual const std::string printParameters() const {return "";}

  private:

    TestShowerLib() {}

    struct genInfo {
		HepMC::FourVector * vertex;
		HepMC::FourVector * momentum;
	};

	typedef std::pair<genInfo,Shower> storedShower;
	typedef std::list<storedShower> library;

    //! read library from given TTree
    bool read(TTree* source);
    //! write library to given TTree
    bool write(TTree* dest) const;

    library m_libData;
  };

} // namespace ShowerLib

#endif // SHOWER_LIB_ETA_SHOWER_LIB_H
