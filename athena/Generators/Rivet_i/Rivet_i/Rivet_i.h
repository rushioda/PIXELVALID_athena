/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RIVET_I_H
#define RIVET_I_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "Rivet/AnalysisHandler.hh"

#include <vector>
#include <string>

class ISvcLocator;
class StoreGateSvc;
class ITHistSvc;


/// Interface to the Rivet analysis package
/// @author James Monk <jmonk@cern.ch>
/// @author Andy Buckley <andy.buckley@cern.ch>
class Rivet_i : public AthAlgorithm {
public:

  /// Standard algorithm constructor
  Rivet_i(const std::string &name, ISvcLocator *pSvcLocator);

  /// @brief Initialise the Rivet interface and Athena services.
  ///
  /// Pass a list of analyses to the Rivet AnalysisHandler, which dynamically
  /// loads the necessary libraries (including user-written analyses).
  StatusCode initialize();

  /// Run the Rivet analyses on one event, which is retrieved from StoreGate.
  StatusCode execute();

  /// Finalise each analysis and commit the plots to an AIDA tree and the
  /// THistSvc ROOT tree.
  StatusCode finalize();


private:

  /// Book an AIDA::IDataPointSet into the THistSvc as a TH1D at path @param path
//  StatusCode regHist(const AIDA::IDataPointSet& dps, const std::string& path);

  /// Book an AIDA::IDataPointSet into the THistSvc as a TGraph at path @param path
//  StatusCode regGraph(const AIDA::IDataPointSet& dps, const std::string& path);

  // Check and potentially modify events for correct units, beam particles, ...
  const HepMC::GenEvent* checkEvent(const HepMC::GenEvent* event);

  /// A pointer to the THistSvc
  ServiceHandle<ITHistSvc> m_histSvc;

  /// The stream name for storing the output plots under (default "/Rivet")
  std::string m_stream;

  /// The base file name to write results to.
  std::string m_file;

  /// @brief The analysis plugin search path
  ///
  /// This will be used to internally set RIVET_ANALYSIS_PATH, if not already set in the environment.
  std::string m_anapath;

  /// @brief Whether to avoid the beam consistency checks
  ///
  /// Default is false: checks will be made to ensure that the supplied
  /// events have beams of the sort that the analysis was written to expect.
  bool m_ignorebeams;

  /// @brief Whether to avoid the finalize() Rivet step
  ///
  /// Default is false: analyses will finalize event loop histograms with normalizations,
  /// scalings, divisions, etc. This may be useful for run combining iy yodamerge is not working.
  bool m_skipfinalize;

  /// @brief Will we convert Rivet's internal histo format into a ROOT histo for streaming with THistSvc?
  ///
  /// The default is yes
  /// Currently (03.01.12) there is no conversion for 2D distributions, in which case you
  /// want to set this to False
  bool m_doRootHistos;

  /// @brief Will the a ROOT histo (see above) be as a TGraphAsymmErrs?
  ///
  /// The default is no: it'll be written as a TH1D (easier to merge, but a bit dangerous).
  bool m_doRootAsTGraph;

  /// The name of the run (prepended to plot paths).
  std::string m_runname;

  /// The GenEvent StoreGate key (default "GEN_EVENT")
  std::string m_genEventKey;

  /// A Rivet analysis handler
  Rivet::AnalysisHandler* m_analysisHandler;

  /// A list of names of the analyses to run (set from the job properties)
  std::vector<std::string> m_analysisNames;

  /// The cross section for this run of events, set from the job properties.
  double m_crossSection;

  /// The name of the weight to use in case the input HepMC contains multiple weights
  std::string m_weightName;
  
  /// Flag to determine whether Rivet init has already happened (in execute())
  bool m_init;

};

#endif
