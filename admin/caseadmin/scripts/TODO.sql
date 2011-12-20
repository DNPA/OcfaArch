select parent.metadataid,child.metadataid,'maandag' from metadatainfo as parent,metadatainfo as child where rtrim(rtrim(child.evidence,'je0123456789'),'.') = parent.evidence and parent.itemid=child.itemid and parent.itemid in (select itemid from item where evidencesource='maandag');

select table_name,data_type from information_schema.columns where table_catalog='testcase' and column_name='meta' and substr(table_name,0,4)='row';



delete from metaname;
insert into metaname (name,scalartype) select  substr(table_name,4),1  from information_schema.columns where table_catalog='testcase' and column_name='meta' and substr(table_name,0,4)='row' and data_type='character varying';
insert into metaname (name,scalartype) select  substr(table_name,4),2  from information_schema.columns where table_catalog='testcase' and column_name='meta' and substr(table_name,0,4)='row' and data_type='integer';
insert into metaname (name,scalartype) select  substr(table_name,4),3  from information_schema.columns where table_catalog='testcase' and column_name='meta' and substr(table_name,0,4)='row' and data_type='real';
insert into metaname (name,scalartype) select  substr(table_name,4),4  from information_schema.columns where table_catalog='testcase' and column_name='meta' and substr(table_name,0,4)='row' and data_type like 'timestamp%';


select 'delete from distinctstringmeta where item_id=3; insert into distinctstringmeta (metaname_id,item_id,digestsource,val,cnt) select distinct ' || metaname_id || ',3,rds.meta,rmt.meta,count(*) from rowdigestsource rds right outer join row' || name || ' rmt on rds.metadataid = rmt.metadataid join metadatainfo mdi on rmt.metadataid=mdi.metadataid where itemid=3 group by rds.meta,rmt.meta;'   from metaname where scalartype=1;

select 'delete from distinctyearmeta where item_id=3; insert into distinctyearmeta (metaname_id,item_id,digestsource,val,cnt) select distinct ' || metaname_id || ',3,rds.meta,rmt.meta,count(*) from rowdigestsource rds right outer join row' || name || ' rmt on rds.metadataid = rmt.metadataid join metadatainfo mdi on rmt.metadataid=mdi.metadataid where itemid=3 group by rds.meta,rmt.meta;'   from metaname where scalartype=4;


select metadataid,repname from metastoreentity,rowmimetop where metastoreentity.id=rowmimetop.metadataid and meta='image';

select distinct 43,3,rds.meta,rmt.meta,count(*) from rowdigestsource rds right outer join rowfileextension rmt on rds.metadataid = rmt.metadataid join metadatainfo mdi on rmt.metadataid=mdi.metadataid where itemid=3 group by rds.meta,rmt.meta;

select 3,rds.meta,rds.metadataid from rowdigestsource rds right outer join rowmimetop  rmt on rds.metadataid = rmt.metadataid join metadatainfo mdi on rmt.metadataid=mdi.metadataid where itemid=3 and rmt.meta='image' group by rds.meta,rmt.meta;


select 1,rds.meta,rds.metadataid,mse.repname from rowdigestsource rds right outer join rowmimetop  rmt on rds.metadataid = rmt.metadataid join metadatainfo mdi on rmt.metadataid=mdi.metadataid join metastoreentity mse on mse.id=rmt.metadataid where itemid=1 and rmt.meta='image' group by rds.meta,rds.metadataid,mse.repname;

select mse.repname,'insert into thumbnails (metadataid,digestsource,item_id,repname) VALUES('|| rds.metadataid || ',\''|| rds.meta || '\',1,\'' || mse.repname || '\')' from rowdigestsource rds right outer join rowmimetop  rmt on rds.metadataid = rmt.metadataid join metadatainfo mdi on rmt.metadataid=mdi.metadataid join metastoreentity mse on mse.id=rmt.metadataid where itemid=1 and rmt.meta='image' group by rds.meta,rds.metadataid,mse.repname;
