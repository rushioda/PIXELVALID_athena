C
C       
C***************************************
       SUBROUTINE HIJFLV(ID)
       COMMON/RANSEED/NSEED
       SAVE  /RANSEED/
       ID=1
       RNID=ATL_RAN(NSEED)
       IF(RNID.GT.0.43478) THEN
              ID=2
              IF(RNID.GT.0.86956) ID=3
       ENDIF
       RETURN
       END
