!
! AmdcCore/comama.inc
!
      INTEGER, PARAMETER :: MAMAMA=100
      INTEGER :: NBMAMA
      INTEGER :: IIMAMA
      REAL(8) :: X0MAMA,EAMAMA,EBMAMA,ZZMAMA
!
      COMMON/AMDCCOMAMA/X0MAMA(MAMAMA),EAMAMA(MAMAMA),EBMAMA(MAMAMA)      &
     &                 ,ZZMAMA(MAMAMA),IIMAMA(MAMAMA),NBMAMA
!