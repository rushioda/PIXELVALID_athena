-- PROJECT: ATLAS MDT Calibration DB
-- AUTHOR: Elisabetta.Vilucchi@lnf.infn.it, celio@roma3.infn.it
-- DATE: May 08
-- VERSION v1r1

--USAGE:
--sqlplus <USER_NAME>/<USER_PASSWORD> @mdt_rt_map_s_trigger.sql <USER_NAME>

--DROP TRIGGER &1..MDT_RT_MAP_S_TRIGGER;
CREATE TRIGGER &1..MDT_RT_MAP_S_TRIGGER BEFORE
INSERT ON &1.."MDT_RT_MAP_S" REFERENCING OLD AS OLD NEW AS NEW FOR EACH ROW
BEGIN

SELECT &1..MDT_RT_MAP_S_SEQUENCE.NEXTVAL INTO :NEW.MDT_RT_MAP_S_ID FROM DUAL;
SELECT TO_CHAR(SYSTIMESTAMP,'YYYY-MM-DD HH:MI AM') INTO :NEW.INSERT_TIME FROM DUAL;

EXCEPTION
WHEN OTHERS THEN
       RAISE;
END MDT_RT_MAP_S_TRIGGER;
/
