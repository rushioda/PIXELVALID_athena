C            
C
C
       SUBROUTINE HIJINI
       COMMON/HIPARNT/HIPR1(100),IHPR2(50),HINT1(100),IHNT2(50)
       SAVE  /HIPARNT/
       COMMON/HISTRNG/NFP(300,15),PP(300,15),NFT(300,15),PT(300,15)
       SAVE  /HISTRNG/
       COMMON/HIJJET1/NPJ(300),KFPJ(300,500),PJPX(300,500),
     &                PJPY(300,500),PJPZ(300,500),PJPE(300,500),
     &                PJPM(300,500),NTJ(300),KFTJ(300,500),
     &                PJTX(300,500),PJTY(300,500),PJTZ(300,500),
     &                PJTE(300,500),PJTM(300,500)
       SAVE  /HIJJET1/
       COMMON/HIJJET2/NSG,NJSG(900),IASG(900,3),K1SG(900,100),
     &              K2SG(900,100),PXSG(900,100),PYSG(900,100),
     &              PZSG(900,100),PESG(900,100),PMSG(900,100)
       SAVE  /HIJJET2/
       COMMON/HIJJET4/NDR,IADR(900,2),KFDR(900),PDR(900,5),VDR(900,5)
       SAVE  /HIJJET4/
       COMMON/RANSEED/NSEED
       SAVE  /RANSEED/
C****************Reset the momentum of initial particles************
C             and assign flavors to the proj and targ string       *
C*******************************************************************
       NSG=0
       NDR=0
       IPP=2212
       IPT=2212
       IF(IHNT2(5).NE.0) IPP=IHNT2(5)
       IF(IHNT2(6).NE.0) IPT=IHNT2(6)
C              ********in case the proj or targ is a hadron.
C
       DO 100 I=1,IHNT2(1)
       PP(I,1)=0.0
       PP(I,2)=0.0
       PP(I,3)=SQRT(HINT1(1)**2/4.0-HINT1(8)**2)
       PP(I,4)=HINT1(1)/2
       PP(I,5)=HINT1(8)
       PP(I,6)=0.0
       PP(I,7)=0.0
       PP(I,8)=0.0
       PP(I,9)=0.0
       PP(I,10)=0.0
       NFP(I,3)=IPP
       NFP(I,4)=IPP
       NFP(I,5)=0
       NFP(I,6)=0
       NFP(I,7)=0
       NFP(I,8)=0
       NFP(I,9)=0
       NFP(I,10)=0
       NFP(I,11)=0
       NPJ(I)=0
       IF(I.GT.ABS(IHNT2(2))) NFP(I,3)=2112
       CALL ATTFLV(NFP(I,3),IDQ,IDQQ)
       NFP(I,1)=IDQ
       NFP(I,2)=IDQQ
       NFP(I,15)=-1
       IF(ABS(IDQ).GT.1000.OR.(ABS(IDQ*IDQQ).LT.100.AND.
     &              ATL_RAN(NSEED).LT.0.5)) NFP(I,15)=1
       PP(I,14)=ULMASS(IDQ)
       PP(I,15)=ULMASS(IDQQ)
100       CONTINUE
C
       DO 200 I=1,IHNT2(3)
       PT(I,1)=0.0
       PT(I,2)=0.0
       PT(I,3)=-SQRT(HINT1(1)**2/4.0-HINT1(9)**2)
       PT(I,4)=HINT1(1)/2.0
       PT(I,5)=HINT1(9)
       PT(I,6)=0.0
       PT(I,7)=0.0
       PT(I,8)=0.0
       PT(I,9)=0.0
       PT(I,10)=0.0
       NFT(I,3)=IPT
       NFT(I,4)=IPT
       NFT(I,5)=0
       NFT(I,6)=0
       NFT(I,7)=0
       NFT(I,8)=0
       NFT(I,9)=0
       NFT(I,10)=0
       NFT(I,11)=0
       NTJ(I)=0
       IF(I.GT.ABS(IHNT2(4))) NFT(I,3)=2112
       CALL ATTFLV(NFT(I,3),IDQ,IDQQ)
       NFT(I,1)=IDQ
       NFT(I,2)=IDQQ
       NFT(I,15)=1
       IF(ABS(IDQ).GT.1000.OR.(ABS(IDQ*IDQQ).LT.100.AND.
     &                     ATL_RAN(NSEED).LT.0.5)) NFT(I,15)=-1
       PT(I,14)=ULMASS(IDQ)
       PT(I,15)=ULMASS(IDQQ)
200       CONTINUE
       RETURN
       END
