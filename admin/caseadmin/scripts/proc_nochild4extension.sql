-- Function: childcountformetadataid(integer)

-- DROP FUNCTION childcountformetadataid(integer);

create or replace type childless as (size int, metadataid int, location varchar(2048));

CREATE OR REPLACE FUNCTION nochild4extension(inextension character varying, insize integer)
  RETURNS setof childless AS
$BODY$
DECLARE
   r childless%rowtype; 

BEGIN
   FOR r in
   	SELECT s.meta as size, mi.metadataid,mi.location
	from rowfileextension mt, metadatainfo mi, childcount c, rowsize s
	where c.count < 1  and c.metadataid=mi.metadataid and mi.metadataid=mt.metadataid and
        	s.metadataid=mi.metadataid and mt.meta=inextension and s.meta > insize order by location
   	loop 
   return next r;
   end loop;
   return;
END;
$BODY$
  LANGUAGE 'plpgsql' VOLATILE
  COST 100;
ALTER FUNCTION nochild4extension(inextension character varying, insize integer) OWNER TO ocfa;

