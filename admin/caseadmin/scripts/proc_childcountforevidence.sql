-- Function: childcountforevidence(character varying)

-- DROP FUNCTION childcountforevidence(character varying);

CREATE OR REPLACE FUNCTION childcountforevidence(inevidence character varying)
  RETURNS bigint AS
$BODY$
DECLARE
   theCount bigint;

BEGIN
   select count(*) into theCount from metadatainfo where evidence like inevidence || '.%';
   return theCount;
END;
$BODY$
  LANGUAGE 'plpgsql' VOLATILE
  COST 100;
ALTER FUNCTION childcountformetadataid(integer) OWNER TO ocfa;

