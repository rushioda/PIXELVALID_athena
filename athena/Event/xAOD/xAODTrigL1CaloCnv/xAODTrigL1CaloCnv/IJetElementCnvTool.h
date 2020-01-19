// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IJetElementCnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_IJETELEMENTCNVTOOL_H
#define XAODTRIGL1CALOCNV_IJETELEMENTCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/JetElementContainer.h"

// Forward declaration(s):
namespace LVL1{
  class JetElement;
}

namespace xAODMaker {

   /// The interface provided by IJetElementCnvTool
   static const InterfaceID
   IID_IJetElementCnvTool( "xAODMaker::IJetElementCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating JetElementContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/JetElement objects from an existing ESD container into
    *         an xAOD::JetElementContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class IJetElementCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::JetElementContainer
      virtual StatusCode convert( const DataVector<LVL1::JetElement>* esd,
                                  xAOD::JetElementContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IJetElementCnvTool;
      }

   }; // class IJetElementCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_IJETELEMENTCNVTOOL_H
