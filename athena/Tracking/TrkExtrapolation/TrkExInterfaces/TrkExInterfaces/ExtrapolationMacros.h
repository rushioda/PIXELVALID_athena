/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationMacros.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// output stream macros to facilitate screen output and consistent checking

#ifndef EXSCREENOUTPUTDEFS
#define EXSCREENOUTPUTDEFS 1
#define EX_MSG_INFO(navstep, step, idx, x)    ATH_MSG_INFO(     m_sopPrefix << std::setw(4) << navstep << m_sopPostfix << std::setw(12) << step << m_sopPostfix << std::setw(4) << idx << m_sopPostfix << x)
#define EX_MSG_DEBUG(navstep, step, idx, x)   ATH_MSG_DEBUG(    m_sopPrefix << std::setw(4) << navstep << m_sopPostfix << std::setw(12) << step << m_sopPostfix << std::setw(4) << idx << m_sopPostfix << x)
#define EX_MSG_VERBOSE(navstep, step, idx, x) ATH_MSG_VERBOSE(  m_sopPrefix << std::setw(4) << navstep << m_sopPostfix << std::setw(12) << step << m_sopPostfix << std::setw(4) << idx << m_sopPostfix << x)
#define EX_MSG_WARNING(navstep, step, idx, x) ATH_MSG_WARNING(  m_sopPrefix << std::setw(4) << navstep << m_sopPostfix << std::setw(12) << step << m_sopPostfix << std::setw(4) << idx << m_sopPostfix << x)
#define EX_MSG_FATAL(navstep, step, idx, x)   ATH_MSG_FATAL(    m_sopPrefix << std::setw(4) << navstep << m_sopPostfix << std::setw(12) << step << m_sopPostfix << std::setw(4) << idx << m_sopPostfix << x)
#endif

#ifndef TRKEXENINGE_OUTPUTHELPER 
#define TRKEXENINGE_OUTPUTHELPER 
#define OH_CHECKFOUND(object) ( object ? "found" : "not found")
#endif

#ifndef TRKEXENGINE_EXCODECHECKS
#define TRKEXENGINE_EXCODECHECKS
#define CHECK_ECODE_CONTINUE(ecell, ecode) if (!ecode.inProgress()) { EX_MSG_VERBOSE(ecell.navigationStep, "continue", "", ecode.toString() << " triggers cotinue."); return ecode; }
#define CHECK_ECODE_SUCCESS_NODEST(ecell, ecode) if (ecode.isSuccessBeforeDestination()) { EX_MSG_VERBOSE(ecell.navigationStep, "return", "", ecode.toString() << " stops extrapolation sequence."); return ecode; }
#define CHECK_ECODE_SUCCESS(ecell, ecode) if (ecode.isSuccess()) { EX_MSG_VERBOSE(ecell.navigationStep, "return", "", ecode.toString() << " stops extrapolation sequence."); return ecode; }

#endif

