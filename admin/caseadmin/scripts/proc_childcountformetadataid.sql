-- Function: childcountformetadataid(integer)

-- DROP FUNCTION childcountformetadataid(integer);

CREATE OR REPLACE FUNCTION childcountformetadataid(inmetadataid integer)
  RETURNS bigint AS
$BODY$
DECLARE
   theEvidence character varying;
   theCount bigint;

BEGIN
   select evidence into theEvidence from metadatainfo where metadataid = inmetadataid;
   select count(*) into theCount from metadatainfo where evidence like theEvidence || '.%';
   return theCount;
END;
$BODY$
  LANGUAGE 'plpgsql' VOLATILE
  COST 100;
ALTER FUNCTION childcountformetadataid(integer) OWNER TO ocfa;

