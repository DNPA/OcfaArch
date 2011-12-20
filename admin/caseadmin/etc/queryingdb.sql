alter  table metadatainfo add foreign key  (metadataid)  references metastoreentity(id) deferrable;
alter  table metadatainfo add foreign key  (dataid)  references evidencestoreentity(id) deferrable;
alter  table metadatainfo add foreign key  (itemid)  references item(itemid);
create index rowmimetype_xml_indx on rowmimetype(metadataid);
create index rowcharset_xml_indx on rowcharset(metadataid);
create index metadatainfo_dataid on metadatainfo(dataid);
create index metadatainfo_evidence_idx on metadatainfo(evidence);
