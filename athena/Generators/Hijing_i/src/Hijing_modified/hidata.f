C
C
C***************************************************************

       BLOCK DATA HIDATA
       REAL*8 XL(10),XU(10),ACC
       COMMON/BVEG1/XL,XU,ACC,NDIM,NCALL,ITMX,NPRN
       SAVE  /BVEG1/
       COMMON/SEEDVAX/NUM1
       SAVE  /SEEDVAX/
       COMMON/HIPARNT/HIPR1(100),IHPR2(50),HINT1(100),IHNT2(50)
       SAVE  /HIPARNT/
       COMMON/RANSEED/NSEED
       SAVE  /RANSEED/
C Add an error entry (IERRSTAT) to HIMAIN1 common block
C       COMMON/HIMAIN1/ NATT,EATT,JATT,NT,NP,N0,N01,N10,N11
       COMMON/HIMAIN1/ NATT,EATT,JATT,NT,NP,N0,N01,N10,N11,IERRSTAT
       SAVE  /HIMAIN1/
C+++BAC
C       COMMON/HIMAIN2/KATT(130000,4),PATT(130000,4)
C       SAVE  /HIMAIN2/
C
       COMMON/HIMAIN2/KATT(130000,4),PATT(130000,4), VATT(130000,4)
       SAVE  /HIMAIN2/
C---BAC

       COMMON/HISTRNG/NFP(300,15),PP(300,15),NFT(300,15),PT(300,15)
       SAVE  /HISTRNG/
       COMMON/HIJCRDN/YP(3,300),YT(3,300)
       SAVE  /HIJCRDN/
       COMMON/HIJJET1/NPJ(300),KFPJ(300,500),PJPX(300,500),
     &               PJPY(300,500),PJPZ(300,500),PJPE(300,500),
     &               PJPM(300,500),NTJ(300),KFTJ(300,500),
     &               PJTX(300,500),PJTY(300,500),PJTZ(300,500),
     &               PJTE(300,500),PJTM(300,500)
       SAVE  /HIJJET1/
       COMMON/HIJJET2/NSG,NJSG(900),IASG(900,3),K1SG(900,100),
     &              K2SG(900,100),PXSG(900,100),PYSG(900,100),
     &              PZSG(900,100),PESG(900,100),PMSG(900,100)
       SAVE  /HIJJET2/
       COMMON/HIJDAT/HIDAT0(10,10),HIDAT(10)
       SAVE  /HIJDAT/
       COMMON/HIPYINT/MINT4,MINT5,ATCO(200,20),ATXS(0:200)
       SAVE  /HIPYINT/
       DATA NUM1/30123984/,XL/10*0.D0/,XU/10*1.D0/
       DATA NCALL/1000/ITMX/100/ACC/0.01/NPRN/0/
C...give all the switchs and parameters the default values
c       DATA NSEED/74769375/
c seed changed to zero, otherwise ran(seed) in gfortran will not generate random numbers 
        DATA NSEED/74769375/
       DATA HIPR1/
     &       1.5,  0.35, 0.5,  0.9,  2.0,  0.1,  1.5,  2.0, -1.0, -2.25,
     &       2.0,  0.5,  1.0,  2.0,  0.2,  2.0,  2.5,  0.3,  0.1,  1.4,
     &       1.6,  1.0,  2.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.4,  57.0,
     &       28.5, 3.9,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  
     &       3.141592654,
     &       0.0,  0.4,  0.1,  1.5,  0.1, 0.25, 0.0,  0.5,  0.0,  0.0,
     &       50*0.0/

       DATA IHPR2/
     &       1,    3,    0,    1,    1,    1,    1,    10,    0,    0,
     &       1,    1,    1,    1,    0,    0,    1,     0,    0,    1,
     &       30*0/

       DATA HINT1/100*0/
       DATA IHNT2/50*0/

C...initialize all the data common blocks
       DATA NATT/0/EATT/0.0/JATT/0/NT/0/NP/0/N0/0/N01/0/N10/0/N11/0/
C+++BAC
C
C       DATA KATT/520000*0/PATT/520000*0.0/
C
       DATA KATT/520000*0/PATT/520000*0.0/VATT/520000*0.0/
C---BAC

       DATA NFP/4500*0/PP/4500*0.0/NFT/4500*0/PT/4500*0.0/

       DATA YP/900*0.0/YT/900*0.0/

       DATA NPJ/300*0/KFPJ/150000*0/PJPX/150000*0.0/PJPY/150000*0.0/
     &       PJPZ/150000*0.0/PJPE/150000*0.0/PJPM/150000*0.0/
       DATA NTJ/300*0/KFTJ/150000*0/PJTX/150000*0.0/PJTY/150000*0.0/
     &       PJTZ/150000*0.0/PJTE/150000*0.0/PJTM/150000*0.0/

       DATA NSG/0/NJSG/900*0/IASG/2700*0/K1SG/90000*0/K2SG/90000*0/
     &       PXSG/90000*0.0/PYSG/90000*0.0/PZSG/90000*0.0/PESG/90000*0.0/
     &       PMSG/90000*0.0/
       DATA MINT4/0/MINT5/0/ATCO/4000*0.0/ATXS/201*0.0/
       DATA (HIDAT0(1,I),I=1,10)/0.0,0.0,0.0,0.0,0.0,0.0,2.25,
     &          2.5,4.0,4.1/
       DATA (HIDAT0(2,I),I=1,10)/2.0,3.0,5.0,6.0,7.0,8.0,8.0,10.0,
     &              10.0,10.0/
       DATA (HIDAT0(3,I),I=1,10)/1.0,0.8,0.8,0.7,0.45,0.215,
     &          0.21,0.19,0.19,0.19/
       DATA (HIDAT0(4,I),I=1,10)/0.35,0.35,0.3,0.3,0.3,0.3,
     &          0.5,0.6,0.6,0.6/
       DATA (HIDAT0(5,I),I=1,10)/23.8,24.0,26.0,26.2,27.0,28.5,28.5,
     &              28.5,28.5,28.5/
       DATA ((HIDAT0(J,I),I=1,10),J=6,9)/40*0.0/
       DATA (HIDAT0(10,I),I=1,10)/5.0,20.0,53.0,62.0,100.0,200.0,
     &          546.0,900.0,1800.0,4000.0/
       DATA HIDAT/10*0.0/
       END
