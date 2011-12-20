create language plpgsql;

CREATE OR REPLACE FUNCTION deleteitemdata(inItem integer) returns integer AS '
DECLARE
	BEGIN
		SET CONSTRAINTS ALL DEFERRED;
		DELETE FROM EvidenceStoreEntity where id in 
			(SELECT dataid FROM Metadatainfo where itemid = inItem);
		DELETE FROM MetaStoreEntity where id in 
			(SELECT metadataid FROM Metadatainfo where itemid = inItem);
		DELETE FROM MetadataInfo where itemid = inItem;
		DELETE FROM item where itemid = inItem;
		RETURN 0;
END;
' LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION preparetodeleteitem() returns integer as $PROC$
DECLARE
   theItemId integer;
   metatablecursor refcursor;
   metatableRec RECORD;
   BEGIN

	FOR metatableRec in select tablename from pg_tables where tablename like 'meta%'  
				and tablename != 'metadatainfo' 
	                        and tablename != 'metastoreentity'
				and schemaname = 'public' 
	LOOP
	raise notice 'alter table %s  add column willdelete boolean default false',  metatableRec.tablename;
	execute 'alter table ' || metatableRec.tablename || ' add column willdelete boolean default false';
	execute 'create index ' || metatableRec.tablename || 'xmlidx on ' || metatableRec.tablename || '(xml)';
	
	END LOOP;
			
   alter table evidencestoreentity add column willdelete boolean default false;
   alter table metastoreentity add column willdelete boolean default false;	
   return '1';
   END ;
$PROC$  LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION cleanupafterdelete() returns integer as $PROC$
DECLARE
   metatableRec RECORD;


   BEGIN

	FOR metatableRec in select tablename from pg_tables where tablename like 'meta%'  
				and tablename != 'metadatainfo' 
				and tablename != 'metastoreentity'
				and schemaname = 'public' 
	LOOP
		raise notice 'alter table %s  delete column',  metatableRec.tablename;
		 execute 'alter table ' || metatableRec.tablename || ' drop column willdelete';
		 execute 'drop index ' || metatableRec.tablename || 'xmlidx';
	
	END LOOP;
	alter table evidencestoreentity drop column willdelete;
	alter table metastoreentity drop column willdelete;
	

	return 0;
   END;
$PROC$ LANGUAGE plpgsql;



CREATE OR REPLACE FUNCTION setForDelete(inItem integer) returns integer as $PROC$
DECLARE
	metadatarecord RECORD;
	BEGIN
	FOR metadatarecord in select metadataid, dataid from metadatainfo where itemid = inItem
	LOOP
		perform	setMetaForDelete(metadatarecord.metadataid);
	        perform setDataForDelete(metadatarecord.dataid);
		
	end LOOP; 
	return 1;
	END;
$PROC$  LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION setMetaForDelete(inMetaId integer) returns void as $PROC$
DECLARE
	metatableRec record;
	BEGIN

	FOR metatableRec in select tablename from pg_tables where tablename like 'meta%'  
				and tablename != 'metadatainfo' 
				and tablename != 'metastoreentity'
				and schemaname = 'public' 
	LOOP
		raise notice 'update table %s  delete set willdelete = true where xml = %s',  metatableRec.tablename, inMetaId;
		execute 'update ' || metatableRec.tablename || ' set willdelete = true where xml = ' || inMetaId;
	
	END LOOP;
	update metastoreentity set willdelete = 'true' where id = inMetaId;
	END;
$PROC$  LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION setDataForDelete(inDataId integer) returns integer as $PROC$
DECLARE 
BEGIN
	update evidencestoreentity set willdelete = 'true' where id = inDataId;
	return 0;
END;
$PROC$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION purgeDatabase(inItemId Integer) returns integer as $PROC$
DECLARE
	metatableRec RECORD;
BEGIN

   FOR metatableRec in select tablename from pg_tables where tablename like 'meta%'  
				and tablename != 'metadatainfo' 
				and tablename != 'metastoreentity'
				and schemaname = 'public' 
	LOOP
		raise notice 'delete from %s', metatableRec.tableName;
		execute 'delete from ' || metatableRec.tablename  || ' where willdelete = ''true''';
   END LOOP;
   raise notice 'delete from metadatainfo';
   DELETE FROM metadatainfo where itemId = inItemId;
   raise notice 'delete from evidencestoreentity';
   DELETE FROM evidencestoreentity where willdelete = 'true';
   raise notice 'delete from metastoreentity';
   DELETE FROM metastoreentity where willdelete = 'true';
   return 0;
END;
$PROC$ LANGUAGE plpgsql;
		


CREATE OR REPLACE FUNCTION hideItem(inCase varchar, insource varchar, inItem varchar) returns void as $PROC$ 

DECLARE 
	theItemId Integer;
BEGIN
	select itemid into theItemId from item  where casename = inCase and evidencesource = inSource and item = inItem; 
	update item set item = 'hidden' || item || substring(now(), 1, 19), hidden = true where itemid = theItemId; 
END;
$PROC$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION deleteHidden() returns void as $PROC$ 
DECLARE 
	theItemRec RECORD;
BEGIN
	for theItemRec in select itemid from item where hidden = true
	LOOP

--		perform setForDelete(theItemRec.itemid);
		perform purgeDatabase(theItemRec.itemid);
	END LOOP;
	delete from item where hidden = true;
END;
$PROC$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION setHiddenForDelete() returns void as $PROC$ 
DECLARE 
	theItemRec RECORD;
BEGIN
	for theItemRec in select itemid from item where hidden = true
	LOOP

		perform setForDelete(theItemRec.itemid);
--		perform purgeDatabase(theItemRec.itemid);
	END LOOP;
--	delete from item where hidden = true;
END;
$PROC$ LANGUAGE plpgsql;