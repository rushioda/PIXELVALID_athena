///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthSequencer.cxx
// Implementation file for class AthSequencer
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
//$Id: AthSequencer.cxx,v 1.7 2009-05-06 12:01:57 binet Exp $

// AthSequencer class
// Implements:
// 1) Common functionality of IInterface
// 2) Default behaviour for the IAlgorithm

#include "AthSequencer.h"

#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/GaudiException.h"
#include "CxxUtils/excepts.h"

#include <errno.h>
#include <fenv.h>
#ifdef __APPLE__
#include <execinfo.h>
#endif

#include "valgrind/valgrind.h"

/**
 ** static variables to handle signals and save the FPE state
 **/
sigjmp_buf AthSequencer::s_fpe_landing_zone;                /* jmp point for SIGFPE handler */
siginfo_t AthSequencer::s_fpe_info;                     /* state storage for SIGFPE handler */
void *AthSequencer::s_array[];
size_t AthSequencer::s_curArraySize;
struct sigaction AthSequencer::s_oldHandler;
int AthSequencer::m_FPEexcepts;
bool AthSequencer::m_installedSignalHandler;

/**
 ** Constructor(s)
 **/
AthSequencer::AthSequencer( const std::string& name, 
                            ISvcLocator* pSvcLocator ):
  AthAlgorithm (name, pSvcLocator),
  m_timeoutMilliseconds(0),
  m_abortTimer(0, NULL, Athena::AlgorithmTimer::DEFAULT ),
  m_continueEventloopOnFPE(false)
{
  
  // Declare AthSequencer properties with their defaults
  declareProperty( "Members", m_names,
                   "Algorithm names (of the form '<cppType>/<instanceName>')" );
  m_names.declareUpdateHandler( &AthSequencer::membershipHandler, this );

  declareProperty( "StopOverride", m_stopOverride=false,
                   "Stop on filter failure Override flag" );
  
  declareProperty( "TimeOut",      m_timeout=0,
                   "Abort job after one algorithm or sequence reaches the time out. Timeout given in Nanoseconds (official ATLAS units), despite its millisecond resolution" );
  
  declareProperty("ContinueEventloopOnFPE", m_continueEventloopOnFPE,
                  "Flag to configure, if job crashes or skips event in case of FPE. "
                  "Default: crash job. It prints an FATAL to mark job unsuccessful "
                  "for production system if an FPE happened. USE WITH CARE !" );
  m_installedSignalHandler=false;
}

/**
 ** Destructor
 **/
AthSequencer::~AthSequencer()
{}

StatusCode
AthSequencer::initialize()
{
  m_timeoutMilliseconds = int(m_timeout * 1e-6);
  
  if ( RUNNING_ON_VALGRIND )
    {
      ATH_MSG_WARNING ("### detected running inside Valgrind, disabling algorithm timeout ###");
      m_timeoutMilliseconds = 0;
    }
  
  std::vector<Algorithm*>* theAlgs;
  std::vector<Algorithm*>::iterator it;
  std::vector<Algorithm*>::iterator itend;
  
  if (!decodeMemberNames().isSuccess()) {
    ATH_MSG_ERROR ("Unable to configure one or more sequencer members ");
    return StatusCode::FAILURE;
  }
  
  // Loop over all sub-algorithms
  theAlgs = subAlgorithms( );
  itend   = theAlgs->end( );
  for (it = theAlgs->begin(); it != itend; it++) {
    Algorithm* theAlgorithm = (*it);
    if (!theAlgorithm->sysInitialize( ).isSuccess()) {
      ATH_MSG_ERROR ("Unable to initialize Algorithm " << theAlgorithm->name());
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
AthSequencer::reinitialize()
{
  // Bypass the loop if this sequencer is disabled
  if ( isEnabled( ) ) {
    
    // Loop over all members calling their reinitialize functions
    // if they are not disabled.
    std::vector<Algorithm*>* theAlgms = subAlgorithms( );
    std::vector<Algorithm*>::iterator it;
    std::vector<Algorithm*>::iterator itend = theAlgms->end( );
    for (it = theAlgms->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      if ( ! theAlgorithm->isEnabled( ) ) {
        theAlgorithm->reinitialize( ).ignore();
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
AthSequencer::execute()
{
  volatile bool all_good = true;
  volatile bool caughtfpe= false;
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("Executing " << name() << "...");
  
  // Bypass the loop if this sequencer is disabled or has already been executed
  if ( isEnabled( ) && ! isExecuted( ) ) {

    // Prevent multiple executions of this sequencer for the current event
    setExecuted( true );

    // Loop over all algorithms calling their execute functions if they
    // are (a) not disabled, and (b) aren't already executed. Note that
    // in the latter case the filter state is still examined. Terminate
    // the loop if an algorithm indicates that it's filter didn't pass.
    std::vector<Algorithm*>* subAlgms = subAlgorithms( );
    //volatile std::vector<Algorithm*>::iterator it;
    //std::vector<Algorithm*>::iterator itend = subAlgms->end( );
    //for (it = subAlgms->begin(); it != itend; it++) {
    //volatile Algorithm* theAlgorithm = (*it);
    for (Algorithm* theAlgorithm : *subAlgms) {
      if ( theAlgorithm->isEnabled( ) ) {
        if ( ! theAlgorithm->isExecuted( ) ) {
          sc = executeAlgorithm (theAlgorithm, all_good, caughtfpe);
        }
	
        if ( all_good ) {
	  
          // Take the filter passed status of this algorithm as my own status
          bool passed = theAlgorithm->filterPassed( );
          setFilterPassed( passed );
	  
          // The behaviour when the filter fails depends on the 
          // StopOverride property.
          // The default action is to stop processing, but this default can be
          // overridden by setting the "StopOverride" property to false.
          if ( ! m_stopOverride ) {
            if ( ! passed ) break;
          }
        } else {
          ATH_MSG_INFO ("execute of [" << theAlgorithm->name() << "] did NOT succeed");
          break;
        }
      }
    }
  }
  return caughtfpe ? StatusCode::RECOVERABLE : sc;
}


StatusCode AthSequencer::executeAlgorithm (Algorithm* theAlgorithm,
                                           volatile bool& all_good,
                                           volatile bool& caughtfpe)
{
  StatusCode sc = StatusCode::SUCCESS;

  // short-circuit evaluation to run the same code 
  // with and w/o catching FPEs
  if ( ! m_continueEventloopOnFPE || 
       prepareCatchAndEnableFPE() ||
       !sigsetjmp(s_fpe_landing_zone, 1) )
  {
    // Call the sysExecute() of the method the algorithm
    m_abortTimer.start(m_timeoutMilliseconds);
    sc = theAlgorithm->sysExecute( );
    all_good = sc.isSuccess();
    // I think this should be done by the algorithm itself, 
    // but just in case...
    theAlgorithm->setExecuted( true );
    int tmp=m_abortTimer.stop();
    // but printout only if non-zero timeout was used
    if (m_timeoutMilliseconds) {
      ATH_MSG_DEBUG ("Time left before interrupting <" 
                     << theAlgorithm->name() << "> : " << tmp);
    }
  }
  else
  {
    // longjmp was called, "catch" the failure
    caughtfpe = true;
    // and cleanup for rest of events
    cleanupAfterFPE( &s_fpe_info );
  }
  // if needed, uninstall the signal handler
  if ( m_continueEventloopOnFPE )
    uninstallFPESignalHandler();

  return sc;
}

StatusCode
AthSequencer::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode
AthSequencer::start()
{
#ifdef GAUDIKERNEL_STATEMACHINE_H_

  std::vector<Algorithm*>* theAlgs;
  std::vector<Algorithm*>::iterator it;
  std::vector<Algorithm*>::iterator itend;
  
  // Loop over all sub-algorithms
  theAlgs = subAlgorithms( );
  itend   = theAlgs->end( );
  for (it = theAlgs->begin(); it != itend; it++) {
    Algorithm* theAlgorithm = (*it);
    if (!theAlgorithm->sysStart( ).isSuccess()) {
      ATH_MSG_ERROR ("Unable to start Algorithm " << theAlgorithm->name());
      return StatusCode::FAILURE;
    }
  }

#endif // !GAUDIKERNEL_STATEMACHINE_H_

  return StatusCode::SUCCESS;
}

StatusCode
AthSequencer::stop()
{
#ifdef GAUDIKERNEL_STATEMACHINE_H_
  // Loop over all sub-algorithms if they are not disabled.
  std::vector<Algorithm*>* theAlgs;
  std::vector<Algorithm*>::iterator it;
  std::vector<Algorithm*>::iterator itend;
  
  theAlgs = subAlgorithms( );
  itend   = theAlgs->end( );
  for (it = theAlgs->begin(); it != itend; it++) {
    Algorithm* theAlgorithm = (*it);
    if (theAlgorithm->sysStop( ).isFailure()) {
      ATH_MSG_ERROR ("Unable to stop Algorithm " << theAlgorithm->name());
    }
  }
  
#endif // !GAUDIKERNEL_STATEMACHINE_H_
  return StatusCode::SUCCESS;
}

StatusCode
AthSequencer::beginRun()
{
  // Bypass the loop if this sequencer is disabled
  if ( isEnabled( ) ) {
    
    // Loop over all members calling their sysInitialize functions
    // if they are not disabled. Note that the Algoriithm::sysInitialize
    // function protects this from affecting Algorithms that have already
    // been initialized.
    std::vector<Algorithm*>* theAlgs = subAlgorithms( );
    std::vector<Algorithm*>::iterator it;
    std::vector<Algorithm*>::iterator itend = theAlgs->end( );
    for (it = theAlgs->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      if (!theAlgorithm->sysInitialize( ).isSuccess()) {
        ATH_MSG_ERROR 
          ("Unable to initialize Algorithm " << theAlgorithm->name());
        return StatusCode::FAILURE;
      }
    }
    
    // Loop over all members calling their beginRun functions
    // if they are not disabled.
    for (it = theAlgs->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      if ( ! theAlgorithm->isEnabled( ) ) {
        theAlgorithm->beginRun( ).ignore();
      }
    }
    
  }

  return StatusCode::SUCCESS;
}

StatusCode
AthSequencer::endRun()
{
  // Bypass the loop if this sequencer is disabled
  if ( isEnabled( ) ) {
    
    // Loop over all members calling their endRun functions
    // if they are not disabled.
    std::vector<Algorithm*>* theAlgms = subAlgorithms( );
    std::vector<Algorithm*>::iterator it;
    std::vector<Algorithm*>::iterator itend = theAlgms->end( );
    for (it = theAlgms->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      if ( ! theAlgorithm->isEnabled( ) ) {
        theAlgorithm->endRun( ).ignore();
      }
    }
  }
  return StatusCode::SUCCESS;
}

void
AthSequencer::resetExecuted( )
{
  Algorithm::resetExecuted( );
  
  // Loop over all members calling their resetExecuted functions
  // if they are not disabled.
  std::vector<Algorithm*>* subAlgms = subAlgorithms( );
  std::vector<Algorithm*>::iterator it;
  std::vector<Algorithm*>::iterator itend = subAlgms->end( );
  for (it = subAlgms->begin(); it != itend; it++) {
    Algorithm* theAlgorithm = (*it);
    theAlgorithm->resetExecuted( );
  }
}

bool
AthSequencer::isStopOverride( ) const
{
  return m_stopOverride.value( );
}

StatusCode
AthSequencer::append( Algorithm* pAlgorithm )
{
  return append( pAlgorithm, subAlgorithms( ) );
}

StatusCode
AthSequencer::createAndAppend( const std::string& type,
                               const std::string& name,
                               Algorithm*& pAlgorithm )
{
  return createAndAppend( type, name, pAlgorithm, subAlgorithms( ) );
}

StatusCode
AthSequencer::remove( Algorithm* pAlgorithm )
{
  return remove (pAlgorithm->name());
}

StatusCode
AthSequencer::remove( const std::string& algname )
{
  return remove( algname, subAlgorithms( ) );
}

StatusCode
AthSequencer::decodeMemberNames( )
{
  // Decode the membership list
  return decodeNames( m_names, subAlgorithms() );
}

void
AthSequencer::membershipHandler( Property& /* theProp */ )
{
  if ( isInitialized() ) decodeMemberNames();
}

/**
 ** Protected Member Functions
 **/

StatusCode
AthSequencer::append( Algorithm* pAlgorithm,
                      std::vector<Algorithm*>* theAlgs )
{
  bool all_good = true;
  // Check that the specified algorithm doesn't already exist 
  // in the membership list
  std::vector<Algorithm*>::iterator it;
  std::vector<Algorithm*>::iterator itend = theAlgs->end( );
  for (it = theAlgs->begin(); it != itend; it++) {
    Algorithm* theAlgorithm = (*it);
    if ( theAlgorithm == pAlgorithm ) {
      all_good = false;
      break;
    }
  }
  if ( all_good ) {
    theAlgs->push_back( pAlgorithm );
    pAlgorithm->addRef();
  }
  return all_good ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode 
AthSequencer::createAndAppend( const std::string& type,
                               const std::string& algName,
                               Algorithm*& pAlgorithm,
                               std::vector<Algorithm*>* theAlgs )
{
  StatusCode result = StatusCode::FAILURE;
  IAlgManager* theAlgMgr;
  //result = service( "ApplicationMgr", theAlgMgr );
  result = serviceLocator()
    ->getService( "ApplicationMgr",
                  IAlgManager::interfaceID(),
                  *pp_cast<IInterface>(&theAlgMgr) );
  if ( result.isSuccess( ) ) {
    IAlgorithm* tmp;
    result = theAlgMgr->createAlgorithm( type, algName, tmp );
    if ( result.isSuccess( ) ) {
      try{
        pAlgorithm = dynamic_cast<Algorithm*>(tmp);
        theAlgs->push_back( pAlgorithm );
      } catch(...){
        ATH_MSG_ERROR ("Unable to create Algorithm " << algName);
        result = StatusCode::FAILURE;
      }
    }
  }
  theAlgMgr->release();
  return result;
}

StatusCode
AthSequencer::decodeNames( StringArrayProperty& theNames,
                           std::vector<Algorithm*>* theAlgs )
{
  StatusCode result;
  IAlgManager* theAlgMgr;
  //result = service( "ApplicationMgr", theAlgMgr );
  result = serviceLocator()->getService( "ApplicationMgr",
                                         IAlgManager::interfaceID(),
                                         *pp_cast<IInterface>(&theAlgMgr) );

  if ( result.isSuccess( ) ) {
    
    // Clear the existing list of algorithms
    theAlgs->clear( );

    // Build the list of member algorithms from the contents of the
    // theNames list.
    const std::vector<std::string>& theNameVector = theNames.value( );
    std::vector<std::string>::const_iterator it;
    std::vector<std::string>::const_iterator itend = theNameVector.end( );
    for (it = theNameVector.begin(); it != itend; it++) {

      // Parse the name for a syntax of the form:
      //
      // <type>/<name>
      //
      // Where <name> is the algorithm instance name, and <type> is the
      // algorithm class type (being a subclass of Algorithm).
      std::string theName = (*it);
      std::string theType = (*it);
      int slash = (*it).find_first_of( "/" );
      if ( slash > 0 ) {
        theType = (*it).substr( 0, slash );
        theName = (*it).substr( slash+1 );
      }

      // Check whether the suppied name corresponds to an existing
      // Algorithm object.
      IAlgorithm* theIAlg;
      Algorithm*  theAlgorithm;
      StatusCode status = theAlgMgr->getAlgorithm( theName, theIAlg );
      if ( status.isSuccess( ) ) {
        theAlgorithm = dynamic_cast<Algorithm*>(theIAlg);
        if (!theAlgorithm) {
          ATH_MSG_WARNING 
            (theName << " is not an Algorithm - Failed dynamic cast");
          status = StatusCode::FAILURE;
        }
      }
      if ( status.isSuccess( ) ) {
        
        // The specified Algorithm already exists - 
        // just append it to the membership list.
        status = append( theAlgorithm, theAlgs );
        if ( status.isSuccess( ) ) {
          ATH_MSG_DEBUG 
            (theName << " already exists - appended to member list");
        } else {
          ATH_MSG_WARNING 
            (theName << " already exists - append failed!!!");
          result = StatusCode::FAILURE;
        }
      } else {
        
        // The specified name doesn't exist -
        // create a new object of the specified type and append it to 
        // the membership list.
        status = createAndAppend( theType, theName, theAlgorithm, theAlgs );
        if ( status.isSuccess( ) ) {
          ATH_MSG_DEBUG 
            (theName << " doesn't exist - created and appended to member list");
        } else {
          ATH_MSG_WARNING
            (theName << " doesn't exist - creation failed!!!");
          result = StatusCode::FAILURE;
        }
      }
    } //> loop over names
    
  }
  // Print membership list
  if ( result.isSuccess() && !theAlgs->empty() ) {
    msg(MSG::INFO) << "Member list: ";
    bool first = true;
    for (Algorithm* alg : *theAlgs) {
      if (first)
        first = false;
      else
        msg() << ", ";
      if ( alg->name() == System::typeinfoName(typeid(*alg)))
        msg() << alg->name();
      else
        msg() << System::typeinfoName(typeid(*alg)) << "/" << alg->name();
    }
    msg(MSG::INFO) << endreq;
  }
  theAlgMgr->release();
  return result;
}

StatusCode
AthSequencer::remove( const std::string& algname, 
                      std::vector<Algorithm*>* theAlgs )
{
  StatusCode result = StatusCode::FAILURE;
  
  // Test that the algorithm exists in the member list
  std::vector<Algorithm*>::iterator it;
  std::vector<Algorithm*>::iterator itend = theAlgs->end( );
  for (it = theAlgs->begin(); it != itend; it++) {
    Algorithm* theAlgorithm = (*it);
    if ( theAlgorithm->name( ) == algname ) {
      
      // Algorithm with specified name exists in the algorithm list - remove it
      // THIS ISN'T IMPLEMENTED YET!!!!
      ATH_MSG_INFO ("AthSequencer::remove( ) isn't implemented yet!!!!!");
      result = StatusCode::SUCCESS;
      break;
    }
  }
  return result;
}

bool AthSequencer::prepareCatchAndEnableFPE()
{
  // protect against several nested AthSequencer installing signal handlers
  if ( m_installedSignalHandler )
    return true;
  if (sigsetjmp(s_fpe_landing_zone, 1))
    {
      /* this should never happen, but allows us to catch a non-catch jump */
      fprintf(stderr, "%s:%d %s error: "
              "longjmp called without valid target\n",
              __FILE__, __LINE__, __FUNCTION__);
      return true;
    }
  
  /* setup a signal handler for SIGFPE */
  struct sigaction action;
  
  memset(&action, 0, sizeof(action));
  
  action.sa_sigaction = fpe_callback;          /* which callback function */
  sigemptyset(&action.sa_mask);                 /* other signals to block */
  action.sa_flags = SA_SIGINFO;               /* give details to callback */
  
  if (sigaction(SIGFPE, &action, &s_oldHandler))
    {
      fprintf(stderr, "%s:%d %s error: "
              "failed to register signal handler %d (%s)\n",
              __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
      return true;
    }
  
  // enable exceptions for overflow, division by zero and invalid operation
  m_FPEexcepts=feenableexcept( FE_OVERFLOW | FE_DIVBYZERO | FE_INVALID);
  m_installedSignalHandler = true;
  
  return false;
}

// in case we catch a FPE, get stack trace, save the siginfo information then call longjmp
void AthSequencer::fpe_callback( int /* sig_number */, siginfo_t *info, void* /* data */ )
{
  s_curArraySize = backtrace (s_array, s_maxArraySize);
  
  memcpy(&s_fpe_info, info, sizeof(s_fpe_info)); /* copy to global for refernce */
  // ARGH: re-enable the trap
  feenableexcept( FE_OVERFLOW | FE_DIVBYZERO | FE_INVALID);
  
  siglongjmp(s_fpe_landing_zone, 1);                /* jump out to catch area */
}

void
AthSequencer::cleanupAfterFPE(siginfo_t *info)
{
  if (info->si_signo != SIGFPE)                      /* should never happen */
    {
      ATH_MSG_ERROR( "somehow got a wrong signo = " << info->si_signo );
    }
  else
    {
      std::string str =
        info->si_code == FPE_INTDIV ? "integer divide by zero"           :
        info->si_code == FPE_INTOVF ? "integer overflow"                 :
        info->si_code == FPE_FLTDIV ? "FP divide by zero"                :
        info->si_code == FPE_FLTOVF ? "FP overflow"                      :
        info->si_code == FPE_FLTUND ? "FP underflow"                     :
        info->si_code == FPE_FLTRES ? "FP inexact result"                :
        info->si_code == FPE_FLTINV ? "FP invalid operation"             :
        info->si_code == FPE_FLTSUB ? "subscript out of range"           :
        "unknown";
      
      ATH_MSG_FATAL ( "FPE reason " << info->si_code << " = \"" <<
                      str << "\", from address " << info->si_addr );
      
      // now have to unwind gaudi stack
      // Check if the AlgContextSvc is running
      IAlgContextSvc* algContextSvc(0);
      if (service("AlgContextSvc", algContextSvc, /*createIf=*/ false).isSuccess() && algContextSvc)
        {
          IAlgorithm* alg = algContextSvc->currentAlg();
          this->msg() << MSG::ERROR << "Stack from AlgContextSvc:\n   Current algorithm: " << (alg ? alg->name() : "<NONE>") << "\n";
          this->msg() << "     Algorithm stack: ";
          if ( algContextSvc->algorithms().size()==0 ) 
            {
              this->msg() << "<EMPTY>" << endreq;
            }
          else
            {
              this->msg() << "\n";
              IAlgContextSvc::Algorithms::const_iterator iter;
              for ( iter=algContextSvc->algorithms().begin();
                    iter!=algContextSvc->algorithms().end();
                    ++iter) {
                if (*iter)
                  {
                    this->msg() << "        " << (*iter)->name() << "\n";
                  }
              }
              while ( algContextSvc->algorithms().size() && algContextSvc->currentAlg()->name() != this->name() )
                if ( algContextSvc->unSetCurrentAlg(algContextSvc->currentAlg()).isFailure() )
                  this->msg() << "cannot unwind: " << algContextSvc->currentAlg();
              this->msg() << endreq;
            }
          algContextSvc->release();
        }
      
      char **strings;
      strings = backtrace_symbols (s_array, s_curArraySize);
      this->msg() << MSG::ERROR << "Printing stacktrace:\n  Obtained " << s_curArraySize << " stack frames.\n";
      for (size_t i(0); i < s_curArraySize; i++)
        this->msg() << std::setw(10) << i << " : " << strings[i] << "\n";
      
      this->msg() << endreq;
      
      free (strings);
    }
}

void
AthSequencer::uninstallFPESignalHandler()
{
  if (sigaction(SIGFPE, &s_oldHandler, 0))
    {
      fprintf(stderr, "%s:%d %s error: "
              "failed to register signal handler %d (%s)\n",
              __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
      return;
    }
  fedisableexcept( FE_ALL_EXCEPT );
  feenableexcept(m_FPEexcepts);

  m_installedSignalHandler = false;
}
