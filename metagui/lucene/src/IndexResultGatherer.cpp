#include "IndexResultGatherer.hpp"
#include <pwd.h>
#include <sys/types.h>
#include <misc/OcfaException.hpp>
#include "EvidenceInfo.hpp"
#include "DocInfo.hpp"
//using namespace ocfa::misc;
using namespace std;
//using ocfa::misc::OcfaConfig;
using namespace ocfa::misc;


IndexResultGatherer::IndexResultGatherer(): 
	OcfaObject("IndexResultGatherer", "luceneui"),
        mConnection(0)
{

  OcfaConfig *config = OcfaConfig::Instance();

  // open database and index. 
  openDatabase(config);
  setMetaTableNames();
    
}


IndexResultGatherer::~IndexResultGatherer(){

  //  PGclose(mConnection);
}

/**
 * determines whether the mimetype aand char set shoudl be
 * retrieved using metamimetype and metacharset or rowmimetype and
 * rowcharset.
 *
 */
void IndexResultGatherer::setMetaTableNames(){

  if (tableExists("metamimetype")){

    mMimeTypeTable = "metamimetype";
    mXmlField = "xml";

  } else if (tableExists("rowmimetype")){

    mMimeTypeTable = "rowmimetype";
    mXmlField = "metadataid";
  }
  else {

    getLogStream(LOG_WARNING) << "No mimetype table found " << endl;
    mMimeTypeTable = "";
  }
  if (tableExists("metaencoding")){
    
    mEncodingTable = "metacharset";
  } 
  else if (tableExists("rowcharset")){
    
    mEncodingTable = "rowcharset";
  }
  else{
    //    throw new OcfaException("no metaencoding table found", this);
    getLogStream(LOG_WARNING) << "no metcaencodign table found " << endl;
  }
  getLogStream(LOG_DEBUG) << "mMimeTypeTable is " << mMimeTypeTable << endl;
  getLogStream(LOG_DEBUG) << "mEncodingTable is " << mEncodingTable << endl;
}

/**
 * does a query on pg_tables to check wherher ein TableName exists.
 * 
 *
 */
bool IndexResultGatherer::tableExists(string inTableName){

  string query = "select * from pg_tables where tablename = '" + 
    inTableName + "'";
  PGresult *pgres;
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    throwDatabaseException(query, pgres);
  } 

  getLogStream(LOG_DEBUG) << "tuples for " << query  << " is " << PQntuples(pgres) << endl;
  return PQntuples(pgres) > 0;
}

int IndexResultGatherer::getResults(set<DocInfo, greater<DocInfo> > &inDataIds, 
				    int inFrom, int resultAmount, 
				    std::set <EvidenceInfo *, EvidenceMostImportant > &outResults){

  set<DocInfo, greater<DocInfo> >::const_iterator iter;
  int i;
  string lastEvidenceId;
  for (iter = inDataIds.begin(), i = 0; 
       (iter != inDataIds.end() && i < (inFrom + 1000)); 
	iter++){

    // Compatibility reasons. Some docinfos are with dataid others with metadataid
    string evidenceId = iter->docid == "" ? iter->metadataid : iter->docid;;   
    string evidenceField = iter->docid == "" ? "metadataid" : "dataid";
    //string metadataid = iter->metadataid;
    getLogStream(LOG_DEBUG) << "docid found " << evidenceId << " score " << iter->score << endl;
    if (evidenceId != lastEvidenceId){
      
     
      if (i >= inFrom && (i < inFrom + resultAmount)){
	
	EvidenceInfo *newInfo = new EvidenceInfo();
	getEvidenceInfo(evidenceId, *newInfo, evidenceField);
	newInfo->mScore = iter->score;
	outResults.insert(newInfo);
	
      }
      i++;
      lastEvidenceId = evidenceId;
      
    }
    getLogStream(LOG_DEBUG) << "end of loop" << endl;
  }
  return i;
}

void IndexResultGatherer::getEvidenceInfo(string &evidenceIdString, 
				  EvidenceInfo &outEvidenceInfo, string inEvidenceField){


  string query = 
    string("select item.casename, item.evidencesource, item.item, metadatainfo.metadataid, metadatainfo.dataid, ")
    + "metadatainfo.evidence, metadatainfo.location from Metadatainfo inner join item on metadatainfo.itemid = item.itemid where "
    + inEvidenceField + " = '";
    query += evidenceIdString;

  query += "'";
  
  PGresult *pgres;
  getLogStream(LOG_DEBUG) << "executing " << query << endl;
  pgres = PQexec(mConnection, query.c_str());
  getLogStream(LOG_DEBUG) << "executed " << endl;
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    throwDatabaseException(query, pgres);
  }
  if (PQntuples(pgres) > 0){

    outEvidenceInfo.ecase = PQgetvalue(pgres, 0, 0);
    outEvidenceInfo.source = PQgetvalue(pgres, 0, 1);
    outEvidenceInfo.item = PQgetvalue(pgres, 0, 2);
    outEvidenceInfo.xmlRef = PQgetvalue(pgres, 0, 3 );
    outEvidenceInfo.dataRef = PQgetvalue(pgres, 0, 4);
    outEvidenceInfo.evidenceRef = PQgetvalue(pgres, 0, 5);
    outEvidenceInfo.location = PQgetvalue(pgres, 0, 6);
    outEvidenceInfo.mimetype = getMimeType(outEvidenceInfo.xmlRef);
    outEvidenceInfo.encoding = getEncoding(outEvidenceInfo.xmlRef);
    //    cerr <<  PQgetvalue(pgres, 0, 7) << endl;
    //    outEvidenceInfo.wasread = (strncmp(PQgetvalue(pgres, 0, 7), "t", 1) == 0) ? true : false;
  }
  else {
    // bogus values in case the database is corrupt.
    outEvidenceInfo.xmlRef = -1;
    outEvidenceInfo.ecase = "";
    outEvidenceInfo.source = "";
    outEvidenceInfo.item = "";
    outEvidenceInfo.evidenceRef = "";
    outEvidenceInfo.dataRef = evidenceIdString;
    outEvidenceInfo.location = string("no Evidence found for ") 
      + evidenceIdString;
  }
  PQclear(pgres);
}

void IndexResultGatherer::openDatabase(OcfaConfig *config){
  
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
    throw ocfa::misc::OcfaException(errstr);
  }  
}

void IndexResultGatherer::throwDatabaseException(string inQuery, PGresult *inResult){
  
  getLogStream(LOG_ERR) << "Query was : " << inQuery << endl;
  ocfaLog(LOG_ERR, string("primary description: ")
    + PQresultErrorField(inResult, PG_DIAG_MESSAGE_PRIMARY));	
  ocfaLog(LOG_ERR, string((PQresultErrorField(inResult, PG_DIAG_SQLSTATE))));
  throw ocfa::misc::OcfaException(string("Error: ").append(PQresStatus(PQresultStatus(inResult))));
}    



string IndexResultGatherer::getMimeType(string xmlRef){

  return getMetaTableValue(xmlRef, mMimeTypeTable, "text/plain");
}

string IndexResultGatherer::getEncoding(string xmlRef){

  return getMetaTableValue(xmlRef, mEncodingTable , "");
}

string IndexResultGatherer::getMetaTableValue(string xmlRef, string inTable, string inDefault){

  getLogStream(LOG_DEBUG) << "xmlref is " << xmlRef << endl;
  getLogStream(LOG_DEBUG) << "inTable is " << inTable << endl;
  if (inTable == ""){

    return inDefault;
  }
  else {
    string query = "select meta from " + inTable + " where " + mXmlField + " = \'" 
      + xmlRef  +"\'";
    PGresult *pgres;
    pgres = PQexec(mConnection, query.c_str());
    if (PQresultStatus(pgres) != PGRES_TUPLES_OK){
      
      throwDatabaseException(query, pgres);
    } 
    
    if (PQntuples(pgres) > 0){
      
      return PQgetvalue(pgres, 0, 0);
    }
    else {
      
      return inDefault;
    }
  }
}

PGconn *IndexResultGatherer::getConnection(){

  return mConnection;
}
