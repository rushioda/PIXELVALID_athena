/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

C...HEPEVT commonblock.
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      SAVE /HEPEVT/
