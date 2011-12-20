--###### Part 1: metadata make overview tables ###########;
--the names and types used for metadata;
create table metaname (
  metaname_id serial NOT NULL,
  PRIMARY KEY(metaname_id),
  name varchar(255),
  scalartype int,
  CONSTRAINT metatypename UNIQUE(name)
);
--The clasification of the source of the metadata,;
--please note that module will mostly be a module ;
--name, but may also be things like 'generic' or;
--'library';
create table clasification (
  module varchar(255),
  metaname_id int references metaname,
  PRIMARY KEY(module,metaname_id)
);
--Table with all distinct metadata values and their occurences.
create table distinctstringmeta (
  id serial NOT NULL,
  metaname_id int references metaname,
  item_id int references item,
  digestsource varchar(255),
  val varchar(255),
  cnt  int,
  --FIXME: digestsource NULL is also OK !!
  PRIMARY KEY(metaname_id,item_id,digestsource,val)
);
--The distinct year part of datetime metadata values.
create table distinctyearmeta (
  id serial NOT NULL,
  metaname_id int references metaname,
  item_id int references item,
  digestsource varchar(255),
  val varchar(255),
  cnt  int,
  PRIMARY KEY(metaname_id,item_id,digestsource,val)
);

--#### Part 2: thumbnail image overviews ##;
--Table containing the tumbnails of picture type files.
create table thumbnails (
  id serial,
  metadataid int references metadatainfo,
  digestsource varchar(255),
  item_id int references item,
  repname varchar(255),
  PRIMARY KEY(metadataid)
);


--##### Part 3: ancestors, these are needed both for sticky categories
--and for solving the SELF UI problems.
--Table containing all ancestory relationships for 'non-dir' nodes.
create table ancestor (
  id serial,
  metadataid int references metadatainfo(metadataid),
  ancestorid  int references metadatainfo(metadataid),
  PRIMARY KEY(metadataid,ancestorid)
);

--### Part 4: The sticky categories.
--Sticky categories that should be inherited; 
--by evidences derived from those that match;
--their metadata against these records.;
--create table category (
--   category_id serial NOT NULL,
--   PRIMARY KEY(category_id),
--   name varchar(255),
--   metaname_id int references metaname,
--   metaval varchar(255)
--);
--Coupling tables for marking evidences;
--as being part of a category;
--create table categorized (
--    categorized_id serial NOT NULL,
--    PRIMARY KEY(categorized_id),
--    category_id int references category,
--    metadataid int references metadatainfo
--);



--### Part 5: combined same subtype metadata
--Table with all the evidence sources and their; 
--offset with respect to the true time.;
--create table timesource (
--   timesource_id serial NOT NULL,
--   PRIMARY KEY(timesource_id),
--   val varchar(255),
--   error int,
--   bool set
--);
--All the time events from different sources of metadata;
--create table timeevents (
--  id serial NOT NULL,
--  metadataid int references metadatainfo,
--  timesource_id int references timesource,
--  time timestamp
--);
--Table containing all e-mail address metadata from different sources;
--create table emailaddresses (
--   id serial,
--   metadataid int,
--   metaname_id int references metaname,
--   digestsource_id int references digestsource,
--   address varchar(255),
--   evidencesource_id int references evidencesource
--);
--Table containing all message id metadata from different sources;
--create table messageid (
--   id serial,
--   metadataid int,
--   metaname_id int references metaname,
--   digestsource_id int references digestsource,
--   msgid varchar(255),
--   evidencesource_id int references evidencesource
--);



--#### Part 6: The big but empty all in one meta table;
--The big metadata table containing all metadata;
--create table bigmeta (
--  id serial,
--  metadataid int,
--  evidencesource_id int references evidencesource
--);
