# name is ambiguous can also mean evidencesource.
# metaclassification
create table srcofmeta (
  srcofmeta_id serial NOT NULL,
  PRIMARY KEY(srcofmeta_id),
  val varchar(255)
);
## nameofmeta
create table mtype (
  mtype_id serial NOT NULL,
  PRIMARY KEY(mtype_id),
  metaname varchar(255),
#should be metaclassification
  srcofmeta_id int references srcofmeta,
  scalartype int
);
create table category (
   category_id serial NOT NULL,
   PRIMARY KEY(category_id),
   name varchar(255),
   mtype_id int references mtype,
   metaval varchar(255)
);
create table evidencesource (
  evidencesource_id serial NOT NULL,
  PRIMARY KEY(evidencesource_id),
  val varchar(255)
);
create table digestsource (
   digestsource_id serial NOT NULL,
   PRIMARY KEY(digestsource_id),
   val varchar(255)
);
create table modulemetagroup (
   modulemetagroup_id serial NOT NULL,
   PRIMARY KEY(modulemetagroup_id),
   val varchar(255)
);
create table timesource (
   timesource_id serial NOT NULL,
   PRIMARY KEY(timesource_id),
   val varchar(255),
   error int
);
create table timeevents (
  id serial,
  # metaid int
  xml int,
  mtype_id int references mtype,
  digestsource_id int references digestsource,
  timesource_id int references timesource,
  time timestamp,
  evidencesource_id int references evidencesource
);
create table mdistinctcount (
  id serial,
  mtype_id int references mtype,
  digestsource_id int references digestsource,
#  modulemetagroup_id int references modulemetagroup,
  scalartype int,
  category_id int references category,
  cnt  int,
  evidencesource_id int references evidencesource
);
create table thumbnails (
  id serial,
#metaid
  xml int,
  digestsource_id int references digestsource,
  thumb oid,
  evidencesource_id int references evidencesource
);
create table parentpeer (
  id serial,
  xml int,
  parentpeer int,
  digestsource_id int references digestsource,
  evidencesource_id int references evidencesource,
  ppeerdigestsource_ int references digestsource(digestsource_id),
  ppeerevidencesource_id int references evidencesource(evidencesource_id)
);
create table bigmeta (
  id serial,
  xml int,
  minodetype varchar(255),
  menvelope_to varchar(255),
  mspecial_type varchar(255),
  moletype varchar(255),
  mcc varchar(255),
  mpubkeylist varchar(255),
  mrulelistissue varchar(255),
  mflash varchar(255),
  mhaspassword varchar(255),
  mmacromode varchar(255),
  msubject varchar(255),
  mexposuretime varchar(255),
  mnisttype varchar(255),
  mcreatime timestamp,
  mcharset varchar(255),
  mchangetime timestamp,
  mreturn_path varchar(255),
  msectionnames varchar(255),
  msignature varchar(255),
  mkeyid varchar(255),
  msender varchar(255),
  mfileinfo varchar(255),
  mcamerasoftware varchar(255),
  mmodificationtime timestamp,
  mfileextension varchar(255),
  mpacked varchar(255),
  mmessage_id varchar(255),
  mfullfrom varchar(255),
  mfnummer varchar(255),
  mstatus varchar(255),
  museragent varchar(255),
  mdigestsource varchar(255),
  mdatetimecreated timestamp,
  mto varchar(255),
  mreferences varchar(255),
  mfrom varchar(255),
  maccesstime timestamp,
  mcameramodel varchar(255),
  mcamerabrand varchar(255),
  minvalidheaders varchar(255),
  mfullsender varchar(255),
  mfullreply_to varchar(255),
  mfullcc varchar(255),
  mfullto varchar(255),
  mnistproduct varchar(255),
  min_reply_to varchar(255),
  msendtime timestamp,
  misorating varchar(255),
  mfocallength varchar(255),
  mmimeinfo varchar(255),
  mrecvtime timestamp,
  mreply_to varchar(255),
  mfsentity_type varchar(255),
  mmimetype varchar(255),
  mmimetop varchar(255),
  evidencesource_id int references evidencesource
);
create table distinctinodetype (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctenvelope_to (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctspecial_type (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctoletype (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctcc (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctpubkeylist (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctrulelistissue (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctflash (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distincthaspassword (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctmacromode (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctsubject (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctexposuretime (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctnisttype (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctcreatime (
  id serial,
  val timestamp,
  timesource_id int references timesource,
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctcharset (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctchangetime (
  id serial,
  val timestamp,
  timesource_id int references timesource,
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctreturn_path (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctsectionnames (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctsignature (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctkeyid (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctsender (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfileinfo (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctcamerasoftware (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctmodificationtime (
  id serial,
  val timestamp,
  timesource_id int references timesource,
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfileextension (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctpacked (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctmessage_id (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfullfrom (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfnummer (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctstatus (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctuseragent (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctdigestsource (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctdatetimecreated (
  id serial,
  val timestamp,
  timesource_id int references timesource,
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctto (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctreferences (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfrom (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctaccesstime (
  id serial,
  val timestamp,
  timesource_id int references timesource,
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctcameramodel (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctcamerabrand (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctinvalidheaders (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfullsender (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfullreply_to (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfullcc (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfullto (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctnistproduct (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctin_reply_to (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctsendtime (
  id serial,
  val timestamp,
  timesource_id int references timesource,
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctisorating (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfocallength (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctmimeinfo (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctrecvtime (
  id serial,
  val timestamp,
  timesource_id int references timesource,
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctreply_to (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctfsentity_type (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctmimetype (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
create table distinctmimetop (
  id serial,
  val varchar(255),
  digestsource_id int references digestsource,
  cnt int,
  evidencesource_id int references evidencesource
);
insert into srcofmeta (val) VALUES ('file module');
insert into srcofmeta (val) VALUES ('mailwash');
insert into srcofmeta (val) VALUES ('pkr');
insert into srcofmeta (val) VALUES ('objdump');
insert into srcofmeta (val) VALUES ('router');
insert into srcofmeta (val) VALUES ('pgp');
insert into srcofmeta (val) VALUES ('zip');
insert into srcofmeta (val) VALUES ('exif module');
insert into srcofmeta (val) VALUES ('libocfa');
insert into srcofmeta (val) VALUES ('GENERAL');
insert into mtype (metaname,scalartype,srcofmeta_id) select 'inodetype',0, srcofmeta_id from srcofmeta where val='libocfa';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'envelope_to',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'special_type',0, srcofmeta_id from srcofmeta where val='libocfa';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'oletype',0, srcofmeta_id from srcofmeta where val='file module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'cc',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'pubkeylist',0, srcofmeta_id from srcofmeta where val='pkr';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'rulelistissue',0, srcofmeta_id from srcofmeta where val='router';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'flash',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'haspassword',0, srcofmeta_id from srcofmeta where val='zip';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'macromode',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'subject',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'exposuretime',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'nisttype',0, srcofmeta_id from srcofmeta where val='file module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'creatime',1, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'charset',0, srcofmeta_id from srcofmeta where val='GENERAL';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'changetime',1, srcofmeta_id from srcofmeta where val='libocfa';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'return_path',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'sectionnames',0, srcofmeta_id from srcofmeta where val='objdump';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'signature',0, srcofmeta_id from srcofmeta where val='pgp';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'keyid',0, srcofmeta_id from srcofmeta where val='pgp';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'sender',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fileinfo',0, srcofmeta_id from srcofmeta where val='file module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'camerasoftware',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'modificationtime',1, srcofmeta_id from srcofmeta where val='libocfa';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fileextension',0, srcofmeta_id from srcofmeta where val='file module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'packed',0, srcofmeta_id from srcofmeta where val='objdump';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'message_id',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fullfrom',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fnummer',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'status',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'useragent',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'digestsource',0, srcofmeta_id from srcofmeta where val='file module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'datetimecreated',1, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'to',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'references',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'from',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'accesstime',1, srcofmeta_id from srcofmeta where val='libocfa';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'cameramodel',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'camerabrand',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'invalidheaders',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fullsender',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fullreply_to',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fullcc',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fullto',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'nistproduct',0, srcofmeta_id from srcofmeta where val='file module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'in_reply_to',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'sendtime',1, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'isorating',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'focallength',0, srcofmeta_id from srcofmeta where val='exif module';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'mimeinfo',0, srcofmeta_id from srcofmeta where val='GENERAL';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'recvtime',1, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'reply_to',0, srcofmeta_id from srcofmeta where val='mailwash';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'fsentity_type',0, srcofmeta_id from srcofmeta where val='libocfa';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'mimetype',0, srcofmeta_id from srcofmeta where val='GENERAL';
insert into mtype (metaname,scalartype,srcofmeta_id) select 'mimetop',0, srcofmeta_id from srcofmeta where val='GENERAL';
