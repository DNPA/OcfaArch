alter table metadatainfo drop constraint metadatainfo_metadataid_fkey;
alter table metadatainfo drop constraint metadatainfo_dataid_fkey;
alter table metadatainfo drop constraint metadatainfo_itemid_fkey;
drop index metamimetype_xml_indx;
drop index metacharset_xml_indx;
drop index metadatainfo_dataid; 
