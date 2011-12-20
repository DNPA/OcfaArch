

create table fuse_multimedia_node (
   	metadataid integer,
	itemid integer,
	mimetype varchar(255),
	filename varchar(255),
	location varchar(4096),
	fileext varchar(255),
	repname varchar(255)	
);

CREATE table fuse_multimedia_distinct_fileext (
	meta varchar(255)
);

CREATE table fuse_multimedia_distinct_mimetype (
        meta varchar(255),
	count integer
);

CREATE table fuse_multimedia_distinct_digestsource (
        meta varchar(255)
);

INSERT INTO fuse_multimedia_distinct_fileext
SELECT distinct fe.meta
FROM
	rowfileextension fe,
        rowmimetype mt
WHERE
        mt.metadataid = fe.metadataid AND
        (mt.meta like 'video%' OR mt.meta like 'image%') ;


INSERT INTO fuse_multimedia_distinct_mimetype
SELECT distinct mt.meta, count(*)
FROM
	rowmimetype mt
WHERE
        (mt.meta like 'video%' OR mt.meta like 'image%')
GROUP BY mt.meta ;



INSERT INTO fuse_multimedia_distinct_digestsource
SELECT distinct ds.meta
FROM
        rowdigestsource ds,
        rowmimetype mt
WHERE
        mt.metadataid = ds.metadataid AND
        (mt.meta like 'video%' OR mt.meta like 'image%') ;



INSERT INTO fuse_multimedia_node (metadataid, itemid, mimetype, filename, location, fileext, repname)
SELECT
        mi.metadataid,
        mi.itemid,
        mt.meta,
        n.meta,
        mi.location,
        fe.meta,
        e.repname
FROM
        metadatainfo mi,
        rowmimetype mt,
        rowname n,
        rowfileextension fe,
        rowdigestsource ds,
        evidencestoreentity e
WHERE
        mi.metadataid = ds.metadataid AND
        mi.metadataid = mt.metadataid AND
        mi.metadataid = n.metadataid AND
        mi.metadataid = fe.metadataid AND
        mi.dataid = e.id AND
        ds.meta <> 'NIST' AND
        (mt.meta like 'video%' OR mt.meta like 'image%') ;


