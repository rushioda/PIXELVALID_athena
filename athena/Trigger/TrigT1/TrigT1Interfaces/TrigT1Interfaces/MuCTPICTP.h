// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_MUCTPICTP_H
#define TRIGT1INTERFACES_MUCTPICTP_H

namespace LVL1 {

   /**
    *  @short MuCTPI input class to the CTP simulation
    *
    *         This class is used to send the LVL1 muon multiplicity information
    *         from the MuCTPI simulation to the CTP simulation.
    *
    * $author Thomas SChoerner-Sadenius <thomas.schoerner@cern.ch>
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class MuCTPICTP {

   public:
      /* constructor and destructor */
      MuCTPICTP( unsigned int word = 0 );
      ~MuCTPICTP();

      /**
       * Return the muon data i.e. <code>(xxxxx|thr6|thr5|thr4|thr3|thr2|thr1)</code>
       */
      unsigned int muCTPIWord() const;

   private:
      /// The only data member
      const unsigned int m_MuCTPICTPWord;

   }; // class MuCTPICTP

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( LVL1::MuCTPICTP, 6070, 0 )

#endif // TRIGT1INTERFACES_MUCTPICTP_H
