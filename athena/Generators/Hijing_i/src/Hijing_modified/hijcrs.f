C
C
C
C
       SUBROUTINE HIJCRS
C       THIS IS TO CALCULATE THE CROSS SECTIONS OF JET PRODUCTION AND
C       THE TOTAL INELASTIC CROSS SECTIONS.
       COMMON/HIPARNT/HIPR1(100),IHPR2(50),HINT1(100),IHNT2(50)
       SAVE  /HIPARNT/
        COMMON/NJET/N,IP_CRS
       SAVE  /NJET/
       EXTERNAL FHIN,FTOT,FNJET,FTOTJET,FTOTRIG
       IF(HINT1(1).GE.10.0) CALL CRSJET
C                     ********calculate jet cross section(in mb)
C
       APHX1=HIPR1(6)*(IHNT2(1)**0.3333333-1.0)
       APHX2=HIPR1(6)*(IHNT2(3)**0.3333333-1.0)
       HINT1(11)=HINT1(14)-APHX1*HINT1(15)
     &                     -APHX2*HINT1(16)+APHX1*APHX2*HINT1(17)
       HINT1(10)=GAUSS1(FTOTJET,0.0,20.0,0.01)
       HINT1(12)=GAUSS1(FHIN,0.0,20.0,0.01)
       HINT1(13)=GAUSS1(FTOT,0.0,20.0,0.01)
       HINT1(60)=HINT1(61)-APHX1*HINT1(62)
     &                     -APHX2*HINT1(63)+APHX1*APHX2*HINT1(64)
       HINT1(59)=GAUSS1(FTOTRIG,0.0,20.0,0.01)
       IF(HINT1(59).EQ.0.0) HINT1(59)=HINT1(60)
       IF(HINT1(1).GE.10.0) Then
          DO 20 I=0,20
             N=I
             HINT1(80+I)=GAUSS1(FNJET,0.0,20.0,0.01)/HINT1(12)
 20          CONTINUE
       ENDIF
       HINT1(10)=HINT1(10)*HIPR1(31)
       HINT1(12)=HINT1(12)*HIPR1(31)
       HINT1(13)=HINT1(13)*HIPR1(31)
       HINT1(59)=HINT1(59)*HIPR1(31)
C              ********Total and Inel cross section are calculated
C                     by Gaussian integration.
       IF(IHPR2(13).NE.0) THEN
       HIPR1(33)=1.36*(1.0+36.0/HINT1(1)**2)
     &             *ALOG(0.6+0.1*HINT1(1)**2)
       HIPR1(33)=HIPR1(33)/HINT1(12)
       ENDIF
C              ********Parametrized cross section for single
C                     diffractive reaction(Goulianos)
       RETURN
       END
