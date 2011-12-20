
//  The Open Computer Forensics Architecture.
//  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "ShowEvidence.hpp"
#include <store/AbstractRepository.hpp>
#include <pwd.h>
#include <sys/types.h>
#include <sstream>
using namespace ocfa::misc;
using namespace ocfa::store;
ShowEvidence::ShowEvidence(): 
	OcfaObject("ShowEvidence", "ui"),
        mConnection(0) 
{

    OcfaConfig *config = OcfaConfig::Instance();   
    openDatabase(config);  
}




void ShowEvidence::openDatabase(OcfaConfig *config){
  
  string dbName = config->getValue("storedbname", this);
  string dbhost = config->getValue("storedbhost", this);
  string dbuser = config->getValue("storedbuser", this);
  char connectionString[255];
  if (dbuser == ""){
    
    struct passwd *pwent = getpwuid(getuid());
    dbuser = pwent->pw_name;
  }
  string dbpasswd = config->getValue("storedbpasswd");
  sprintf(connectionString, "dbname=%s hostaddr=%s user=%s password=%s", 
	  dbName.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
  
  mConnection = PQconnectdb(connectionString);
  if (PQstatus(mConnection) != CONNECTION_OK) {
    
    string errstr = "PgRepository initialization failed: " 
      + string(PQerrorMessage(mConnection));
    printf("%s",errstr.c_str());
    throw OcfaException(errstr, this);
  }  
}


void ShowEvidence::showXml(string inCase, string inSource, string inItem, string inId, ostream &out){

  string query = 
    string("select metadataid from MetaDataInfo inner join item on item.itemid = MetaDataInfo.itemid")
    + " where casename = '" + inCase + "' and evidencesource='" + inSource 
    + "' and item.Item = '" + inItem + "' and evidence = '" + inId + "'";
  PGresult *pgres;
  ocfaLog(LOG_INFO, string("query is ")  + query); 
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK) {
    
    throwDatabaseException(query, pgres);
  }
  if (PQgetvalue(pgres, 0, 0) == 0){
    
    ostringstream errMessage;
    ocfaLog(LOG_ERR, errMessage.str());
    throw OcfaException(string("cannot find enough results with query") + query, this);
  }
  string xmlHandle = PQgetvalue(pgres, 0, 0);
  MetaStoreEntity *entity;
  //cout << "xmlhandle is " << xmlHandle << endl;
  AbstractRepository::Instance()->createMetaStoreEntity(&entity, xmlHandle);
  

  istream *inStream = entity->getAsIstream();
  if (inStream == 0){

    throw OcfaException(string("cannot get istream from handle ") + entity->getHandle());
  }
  char buffer[1024];
  while (!inStream->eof()){

    inStream->read(buffer,1024);
    out.write(buffer, inStream->gcount());
  }
  
  delete inStream;
  //  cout << "metacontent is " << metaContent << endl;
  delete entity;  
}

void ShowEvidence::showXml(string inMetaDataId, ostream &out){

  //  string xmlHandle = PQgetvalue(pgres, 0, 0);
  MetaStoreEntity *entity;
  //cout << "xmlhandle is " << xmlHandle << endl;
  AbstractRepository::Instance()->createMetaStoreEntity(&entity, inMetaDataId);
    
  istream *inStream = entity->getAsIstream();
  if (inStream == 0){

    throw OcfaException(string("cannot get istream from handle ") + entity->getHandle());
  }
  char buffer[1024];
  while (!inStream->eof()){

    inStream->read(buffer,1024);
    out.write(buffer, inStream->gcount());
  }
  
  delete inStream;
  //  cout << "metacontent is " << metaContent << endl;
  delete entity;  
}

void ShowEvidence::showDataFilePath(string inCase, string inSource, string inItem, string inId, ostream &out){

  EvidenceStoreEntity *entity;
  OcfaHandle handle = getEvidenceStoreHandle(inCase, inSource, inItem, inId);
  AbstractRepository::Instance()->createEvidenceStoreEntity(&entity, handle);  
  out << entity->getAsFilePath();
  out.flush();
  delete entity;
}
/**
 *
 *
 */
void ShowEvidence::showData(string inCase, string inSource, string inItem, string inId, ostream &out){

  EvidenceStoreEntity *entity;
  OcfaHandle handle = getEvidenceStoreHandle(inCase, inSource, inItem, inId);
  AbstractRepository::Instance()->createEvidenceStoreEntity(&entity, handle);  

  istream *inStream = entity->getAsIstream();
  if (inStream == 0){

    throw OcfaException(string("cannot get istream from handle ") + entity->getHandle());
  }
  
  out << inStream->rdbuf();

  /*char buffer[1024]; 
  while (!inStream->eof()){
     inStream->read(buffer,1024)) {
     out.write(buffer, inStream->gcount());
  } */
  
  delete inStream;
  //  cout << "metacontent is " << metaContent << endl;
  delete entity;
}

void ShowEvidence::showData(string inDataId, ostream &out){
  
  EvidenceStoreEntity *entity;
  AbstractRepository::Instance()->createEvidenceStoreEntity(&entity, OcfaHandle(inDataId));
  istream *inStream = entity->getAsIstream();
  if (inStream == 0){

    throw OcfaException(string("cannot get istream from handle ") + entity->getHandle());
  }

  out << inStream->rdbuf();
  /*char buffer[1024];
  while (!inStream->eof()){

    inStream->read(buffer,1024);
    out.write(buffer, inStream->gcount());
  } */
  
  delete inStream;
  //  cout << "metacontent is " << metaContent << endl;
  delete entity;
}  




string ShowEvidence::getEvidenceStoreHandle(string inCase, string inSource, string inItem, string inId){

 string query = 
    string("select dataid from MetaDataInfo inner join item on item.itemid = MetaDataInfo.itemid")
    + " where casename = '" + inCase + "' and evidencesource='" + inSource 
    + "' and item.Item = '" + inItem + "' and evidence = '" + inId + "'";
  PGresult *pgres;
  getLogStream(LOG_DEBUG) << "Query is " << query << endl;
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK) {
    
    throwDatabaseException(query, pgres);
  }
  if (PQgetvalue(pgres, 0, 0) == 0){
    
    ostringstream errMessage;
    ocfaLog(LOG_ERR, errMessage.str());
    throw OcfaException(string("cannot find enough results with query") + query, this);
  }
  OcfaHandle dataHandle(PQgetvalue(pgres, 0, 0));
  //cout << "xmlhandle is " << xmlHandle << endl;
  //AbstractRepository::Instance()->createEvidenceStoreEntity(outEntity, dataHandle);
  return dataHandle;
}



void ShowEvidence::throwDatabaseException(string command, PGresult *inResult){
  
  ocfaLog(LOG_ERR, string("error executing ").append(command));
  ocfaLog(LOG_ERR, string("error was " ).append(PQresStatus(PQresultStatus(inResult))));
  //ocfaLog(LOG_ERR, string("primary description: ")
  //  + PQresultErrorField(inResult, PG_DIAG_MESSAGE_PRIMARY));	
  //ocfaLog(LOG_ERR, string((PQresultErrorField(inResult, PG_DIAG_SQLSTATE))));
  throw OcfaException(string("Error: ").append(PQresStatus(PQresultStatus(inResult))), this);
}    
