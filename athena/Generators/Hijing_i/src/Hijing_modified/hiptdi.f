C
C
C
       SUBROUTINE HIPTDI(PT,PTMAX,IOPT)
       COMMON/HIPARNT/HIPR1(100),IHPR2(50),HINT1(100),IHNT2(50)
       SAVE  /HIPARNT/
       COMMON/RANSEED/NSEED
       SAVE  /RANSEED/
       IF(IOPT.EQ.2) THEN
              PT=HIRND2(7,0.0,PTMAX)
              IF(PT.GT.HIPR1(8)) 
     &              PT=HIPR1(2)*SQRT(-ALOG(EXP(-HIPR1(8)**2/HIPR1(2)**2)
     &                     -ATL_RAN(NSEED)*(EXP(-HIPR1(8)**2/HIPR1(2)**2)-
     &                     EXP(-PTMAX**2/HIPR1(2)**2))))

       ELSE
              PT=HIPR1(2)*SQRT(-ALOG(1.0-ATL_RAN(NSEED)*
     &                     (1.0-EXP(-PTMAX**2/HIPR1(2)**2))))
       ENDIF
       PTMAX0=MAX(PTMAX,0.01)
       PT=MIN(PTMAX0-0.01,PT)
       RETURN
       END
