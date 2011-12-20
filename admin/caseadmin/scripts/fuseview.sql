CREATE VIEW view_fuse_node (metadataid, itemid, mimetype, filename, location, fileext, repname)
AS
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
        (mt.meta like 'video%' OR mt.meta like 'image%');
