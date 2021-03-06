*
* $Id: mninpu.F,v 1.1 2008/10/27 20:58:48 jfb Exp $
*
* $Log: mninpu.F,v $
* Revision 1.1  2008/10/27 20:58:48  jfb
* commit minuit
*
* Revision 1.1  2007/01/04 15:44:42  joe
* Populating...
*
* Revision 1.1.1.1  2000/06/08 11:19:20  andras
* import of MINUIT from CERNlib 2000
*
* Revision 1.1.1.1  1996/03/07 14:31:30  mclareni
* Minuit
*
*
#include "QatDataModeling/pilot.h"
      SUBROUTINE MNINPU(IUNIT,IERR)
#include "QatDataModeling/d506dp.inc"
CC      called by the user to SET INPUT to IUNIT,
CC      an alternative to MNSTIN where the user can specify just
CC      a logical unit number and he is not interrogated about
CC      open files and rewinding, all that is the responsibility
CC      of the user and cannot be fixed interactively.
#include "QatDataModeling/d506cm.inc"
C
      IERR = 0
C                              IUNIT = 0, revert to previous input file
      IF (IUNIT .EQ. 0) THEN
        IF (NSTKRD .EQ. 0)  THEN
           WRITE (ISYSWR, '(A)') ' CALL TO MNINPU(0) IGNORED'
           WRITE (ISYSWR, '(A)') ' ALREADY READING FROM PRIMARY INPUT'
        ELSE
          ISYSRD = ISTKRD(NSTKRD)
          NSTKRD = NSTKRD - 1
        ENDIF
C
C                               new input file
      ELSE
          IF (NSTKRD .GE. MAXSTK)  THEN
          WRITE (ISYSWR, '(A)') ' INPUT FILE STACK SIZE EXCEEDED.'
          GO TO 800
          ENDIF
        NSTKRD = NSTKRD + 1
        ISTKRD(NSTKRD) = ISYSRD
        ISYSRD = IUNIT
      ENDIF
C
      RETURN
  800 IERR = 1
      RETURN
      END
