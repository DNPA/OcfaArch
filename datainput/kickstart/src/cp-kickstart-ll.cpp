#include <string>
#include<iostream>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <ocfa.hpp>
#include <sys/types.h>
#include <pwd.h>
#include <libpq-fe.h>

using namespace std;

int main(int argc,char *argv[]) {
  int doforce=0;
  if (argc > 4) {
     fprintf(stderr,"to many arguments to %s\n",argv[0]);
     return 1;
  }
  if (argc < 3) {
     fprintf(stderr,"to litle arguments to %s\n",argv[0]);
     return 1;
  }
  if (argc == 4) {
     if (strcmp(argv[3],"-f")) {
        fprintf(stderr,"invalid 3th argument to %s '-f' expected\n",argv[0]);
        return 1;
     }
     doforce=1;
  }
  char *carvpath=(char *) malloc(strlen(argv[1])+1);
  sprintf(carvpath,"%s",argv[1]);
  char *basepath=(char *) malloc(strlen(argv[1]));
  sprintf(basepath,"%s/",dirname(argv[1]));
  char *copy=(char *) malloc(strlen(argv[1]));
  sprintf(copy,"%s",basepath);
  char *repository=(char *) malloc(strlen(argv[1]));
  sprintf(repository,"%s",dirname(dirname(copy)));
  char *xmlpath=(char *) malloc(strlen(basepath)+20);
  sprintf(xmlpath,"%socfa.xml",basepath);
  char *reppath=(char *) malloc(strlen(basepath)+20);
  sprintf(reppath,"%socfa.repository",basepath);
  char *cprelative=carvpath+strlen(repository);
  char *query1=(char *) malloc(4096);
  sprintf(query1,"insert into evidencestoreentity (repname) values('%s')",cprelative);
  char *query2=(char *) malloc(4096);
  sprintf(query2,"select id from evidencestoreentity where repname='%s'",cprelative);
  char *query3=(char *) malloc(4096);
  FILE *repfil=fopen(reppath,"w");
  if (repfil == 0) {
     fprintf(stderr,"ERROR: Unable top open %s for writing!!\n",reppath);
     return 1;
  }
  try {
  ocfa::message::MessageBox *mbox=ocfa::message::MessageBox::createInstance("cpkickstart","default");
  if (mbox == 0) {
     fprintf(stderr,"ERROR: Unable to create message box\n");
     return 1;
  }
  ocfa::misc::ModuleInstance *minst=mbox->getModuleInstance();
  if (minst == 0) {
     fprintf(stderr,"ERROR: unable to retreive module instance from messagebox\n");
  }
  ocfa::misc::OcfaLogger::Instance()->baptize(minst);
  ocfa::misc::OcfaConfig::Instance()->baptize(minst);
  ocfa::evidence::EvidenceFactory::Instance()->baptize(minst);
  string dbname = ocfa::misc::OcfaConfig::Instance()->getValue("storedbname");
  string dbhost = ocfa::misc::OcfaConfig::Instance()->getValue("storedbhost");
  string dbuser = ocfa::misc::OcfaConfig::Instance()->getValue("storedbuser");
  if (dbuser == "") {
       struct passwd *pwent=getpwuid(getuid());
       dbuser=pwent->pw_name;
  }
  string dbpasswd = ocfa::misc::OcfaConfig::Instance()->getValue("storedbpasswd"); 
  char conninfo[256];
  sprintf(conninfo, "dbname=%s hostaddr=%s user=%s password=%s", dbname.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
  PGconn *d_connection= PQconnectdb(conninfo);

  if (PQstatus(d_connection) != CONNECTION_OK) {  
     fprintf(stderr,"Unable to connect to database\n");
  }
  fprintf(stdout,"\nConnected\n");
  string estoreid="";
  PGresult *pgres = PQexec(d_connection, query1);
  if (PQresultStatus(pgres) == PGRES_COMMAND_OK) {
        PQclear(pgres);
        pgres = PQexec(d_connection,query2);
        if (PQresultStatus(pgres) == PGRES_TUPLES_OK) {
          estoreid = PQgetvalue(pgres, 0, 0);
          fprintf(stdout,"Inserted entry: %s\n",estoreid.c_str());
          fprintf(repfil,"%s\n",estoreid.c_str());
          fclose(repfil);
        }
        else {
          fprintf(stderr,"Problem with fething handle from just created evidence store entity\n");
          return 1;
        }
  } else {
     fprintf(stderr,"Problem executing query: %s\n",query1);
     return 1;
  }
  sprintf(query3,"delete from evidencestoreentity where id='%s'",estoreid.c_str());
  FILE *xmlfil=fopen(xmlpath,"r");
  pgres=0;
  if (xmlfil == 0) {
     fprintf(stderr,"Problem with carvpath library, xml not abailable after supplying store id\n");
     pgres = PQexec(d_connection, query3);
     return 1;
  } 
  fseek(xmlfil,0,SEEK_END);
  long xmlsize=ftell(xmlfil);
  fseek(xmlfil,0,SEEK_SET);
  unsigned char *xmldata=(unsigned char *) calloc(1,xmlsize+1);
  if (fread(xmldata, xmlsize, 1, xmlfil) < 1) {
     fprintf(stderr,"Problem with carvpath library, xml not readable in indicated size\n");
     pgres = PQexec(d_connection, query3);
     return 1;
  }  
  ocfa::misc::MemBuf *xmlmembuf=new ocfa::misc::MemBuf(xmldata,xmlsize);
  if (xmlmembuf == 0) {
      fprintf(stderr,"Problem with OCFA library: unable to create MemBuf from xml buffer\n");
      pgres = PQexec(d_connection, query3);
      return 1;
  } 
  ocfa::evidence::Evidence *evidence=0;
  ocfa::misc::OcfaHandle *handle=new ocfa::misc::OcfaHandle(estoreid);
  ocfa::evidence::EvidenceFactory::Instance()->createEvidence(&evidence,xmlmembuf,&handle);
  if (evidence == 0) {
    fprintf(stderr,"Problem creating evidence from xml membuffer\n");
    pgres = PQexec(d_connection, query3);
    return 1;
  }
  ocfa::misc::EvidenceIdentifier *eid= evidence->getEvidenceIdentifier();
  if (eid ==0) {
     fprintf(stderr,"Problem fetching evidence identifier from evidence\n");
     pgres = PQexec(d_connection, query3);
     return 1; 
  } 
  ocfa::misc::Item *item=0;
  try {
    ocfa::store::AbstractRepository::Instance()->createItem(&item,eid->getCaseID(),eid->getEvidenceSourceID(),eid->getItemID());
  } catch (ocfa::misc::OcfaException &ex) {
    fprintf(stderr,"Problem creating item: probably item already exists\n");
    pgres = PQexec(d_connection, query3);
    return 1;
  }
  ocfa::store::MetaStoreEntity *metaent;
  unsigned char *mbp=xmlmembuf->getPointer();
  size_t  mbs=xmlmembuf->getSize();
  ocfa::misc::OcfaHandle *handle2=new ocfa::misc::OcfaHandle(estoreid);
  ocfa::store::AbstractRepository::Instance()->createMetaStoreEntity(&metaent,mbp,mbs,*eid,handle2);
  if (metaent==0) {
     fprintf(stderr,"Problem creating meta store entity\n");
     pgres = PQexec(d_connection, query3);
     return 1;
  }
  ocfa::misc::ModuleInstance *router=new ocfa::misc::ModuleInstance("localhost", "router", "core", "DNTCR");
  ocfa::message::Message *message=0;
  mbox->createMessage(&message, router, ocfa::message::Message::ANYCAST,
                                     ocfa::message::Message::mtEvidence, string("newevidence"),
                                     string(metaent->getHandle()), MSG_EVIDENCE_LOWPRIO);
  //RJM:CODEREVIEW sugestion: instead of the abouve use:
  //mbox->createEvidenceMessage(&message,string(metaent->getHandle()));
  mbox->sendMessage(*message); 
  } catch (ocfa::misc::OcfaException &ex) {
    ex.logWhat(); 
  }
  return 0;
}
