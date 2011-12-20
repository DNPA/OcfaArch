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

#include "Dsm2.hpp"
#include <boost/tokenizer.hpp>
#include <store/AbstractRepository.hpp>
#include <evidence/JobIterator.hpp>
#include <evidence/MetaIterator.hpp>
#include <pwd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <boost/lexical_cast.hpp>
using namespace ocfa::message;
using namespace boost;
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::store;
using namespace std;
Dsm2::Dsm2() : 
	DataStoreModule("dsm", "core"),
        mConnection(0),
        mLog2Db(false),
        mModules2Db(false)
{
  getLogStream(LOG_DEBUG) << "Dsm2::Dsm2 in constructor" << endl;
  if (getConfEntry("log2db") == "true") {mLog2Db=true;}
  if (getConfEntry("modules2db") == "true") {mModules2Db=true;}
  string file = getConfEntry("metatabledescription");
  mConnection = openDatabase();
  logModule(LOG_INFO, string("using table description file ") + file); 
  mFilter=new MetaFilter(mConnection,file);
  if (mLog2Db) {
    getLogStream(LOG_NOTICE) << "Creating a loglines table (if non excists) for debugging purposes" << std::endl;
    std::string query="create table loglines (metadataid int,jobseq int,module varchar(64),line varchar(1024))";
    PGresult *res=PQexec(mConnection,query.c_str());
    PQclear(res);
  } else {
    getLogStream(LOG_NOTICE) << "Skipping creation of loglines table." << std::endl;
  }
  if (mModules2Db) {
    getLogStream(LOG_NOTICE) << "Creating a joblist table (if non excists) for debugging purposes" << std::endl;
    std::string query="create table joblist (metadataid int,jobseq int,module varchar(64))";
    PGresult *res=PQexec(mConnection,query.c_str());
    PQclear(res);
  } else {
    getLogStream(LOG_NOTICE) << "Skipping creation of joblist table." << std::endl;
  }
  getLogStream(LOG_DEBUG) << "Dsm2::Dsm2 connection is " << mConnection << endl;
}

Dsm2::~Dsm2(){

  if (mConnection != 0){

    PQfinish(mConnection);
    mConnection = 0;
  }
  if (mFilter) {
    delete mFilter;
  }
}

static void DSMlogNotices(void *, const char *msg){
	   OcfaLogger::Instance()->syslog(LOG_NOTICE, string("DSM database notice:")+string(msg)+string("\n"));
}

std::string Dsm2::escape(std::string in) {
    char *escaped=(char *)calloc((in.size()+2)*4,1);
    PQescapeString(escaped, in.c_str(), in.length()); 
    std::string rval(escaped);
    free(escaped);
    return rval;
}

PGconn *Dsm2::openDatabase(){
 
  string dbName = getConfEntry("storedbname");
  string dbhost = getConfEntry("storedbhost");
  string dbuser = getConfEntry("storedbuser");
  char connectionString[255];
  if (dbuser == ""){
    
    struct passwd *pwent = getpwuid(getuid());
    dbuser = pwent->pw_name;
  }
  string dbpasswd = getConfEntry("storedbpasswd");
  sprintf(connectionString, "dbname=%s hostaddr=%s user=%s password=%s", 
	  dbName.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
  
  PGconn *connection = PQconnectdb(connectionString);
  if (PQstatus(connection) != CONNECTION_OK) {
    
    string errstr = "PgRepository initialization failed: " 
      + string(PQerrorMessage(connection));
    //printf("%s",errstr.c_str());
    ocfaLog(LOG_ERR, errstr);
    throw OcfaException(errstr, this);
  } 
  PQsetNoticeProcessor(connection, DSMlogNotices, 0);
  return connection;
}

void Dsm2::throwDatabaseException(string command, PGresult *inResult){  
  ocfaLog(LOG_ERR, string("error executing ").append(command));
  ocfaLog(LOG_ERR, string("error was " ).append(PQresStatus(PQresultStatus(inResult))));
  ocfaLog(LOG_ERR, string("result status was ", PQresultStatus(inResult)));
  //  ocfaLog(LOG_ERR, string("primary description: ")
  //  + PQresultErrorField(inResult, PG_DIAG_MESSAGE_PRIMARY));	
  //ocfaLog(LOG_ERR, string((PQresultErrorField(inResult, PG_DIAG_SQLSTATE))));
  throw OcfaException(string("Error: ").append(PQresStatus(PQresultStatus(inResult))), this);
}    


void Dsm2::processEvidence(){
  getLogStream(LOG_DEBUG) << "entering processEvidence()" << endl;
  // JBS todo: the behavior of this part should be dependent on the commit
  // Flag. This behavior is ACT_COMMIT behavior
 
  JobIterator *jobIterator = getEvidence()->getJobIterator();
  
  if (jobIterator == 0){

    throw OcfaException("no job iterator found", this);
  }

  //Special handling of jobs with resubmit active.
  bool skipEvidenceLevelMeta=false;
  size_t skippedcount=0;
  if (ocfa::misc::OcfaConfig::Instance()->getValue("moduletwice",this) == "skipjobs") {
      size_t skipcount=0;         
      size_t jobcount=0; 
      do {
         jobcount++;
         ocfa::misc::ModuleInstance *minst=jobIterator->getModuleInstance();
         if ((minst->getModuleName() == "dsm") && (jobcount < getEvidence()->getJobCount())) {
              getLogStream(LOG_WARNING) << "Job " << jobcount << " was found to be made by a dsm, this means we shall skip all jobs up to and including job " << jobcount << ". Likely this is a resubmit." << std::endl;
              skipcount=jobcount;
              skipEvidenceLevelMeta=true;
         }
      } while (jobIterator->next()); 
      jobIterator = getEvidence()->getJobIterator();
      while (skipcount > 0) {
         skipcount--;
         skippedcount++;
         getLogStream(LOG_WARNING) << "Skipping job " << skippedcount << "created by " << jobIterator->getModuleInstance()->getModuleName() << "." << std::endl;
         jobIterator->next();
      }   
  }
  if (skipEvidenceLevelMeta == true) {
    getLogStream(LOG_WARNING) << "We skip adding top level meta, likely this is a resubmit." << std::endl;
  } else {
    //getLogStream(LOG_WARNING) << "Normal mode or no dsm job found, adding all top level meta (remove this logline from Dsm2.cpp)." << std::endl;
    ScalarMetaValue *gmv=new ScalarMetaValue(getDigestSHA1());
    if ((*mFilter)("sha1",gmv)) {
      insertStandardMetaValue(gmv,"sha1");
    }
    delete gmv;
    gmv=new ScalarMetaValue(getDigestMD5());
    if ((*mFilter)("md5",gmv)) {
      insertStandardMetaValue(gmv,"md5");
    }
    delete gmv;
    gmv=new ScalarMetaValue(getEvidenceName());
    if ((*mFilter)("name",gmv)) {
      insertStandardMetaValue(gmv,"name");
    }
    delete gmv;
    //FIXME: the facade library should make getEvidencePath() from Evidence available here !!
    //gmv=new ScalarMetaValue(getEvidencePath());
    //insertStandardMetaValue(gmv,"path");
    //delete gmv;
    //FIXME: the facade library should make getJobCount() and getJobCount() from Evidence available here !!
    //gmv=new ScalarMetaValue(Scalar(getJobCount()));
    //insertStandardMetaValue(gmv,"jobcount");
    //delete gmv;
    //gmv=new ScalarMetaValue(Scalar(getParentCount()));
    //insertStandardMetaValue(gmv,"parentcount");
    //delete gmv;
    insertLocation();
  }
  size_t jobno=skippedcount;
  std::string modulename="UNDEFINED";
  do {
    std::string previousmodulename=modulename;
    if (mModules2Db || mLog2Db) {
       ocfa::misc::ModuleInstance *jobmoduleinstance=jobIterator->getModuleInstance();
       modulename=jobmoduleinstance->getModuleName();          
    }
    if (mModules2Db) {
       insertModuleName(modulename,jobno);
    }
    if (mLog2Db) {
       LogIterator *logiter=jobIterator->getLogIterator(); 
       if (logiter != 0) {
          do {
             std::string logline=logiter->getLine();
             insertLogLine(modulename,jobno,logline);
          } while (logiter->next());
       }
    }
    MetaIterator *metaIterator = jobIterator->getMetaIterator();
    if (metaIterator != 0){
      do {
	string metaName  =metaIterator->getName();
	transform(metaName.begin(), metaName.end(), metaName.begin(), ::tolower);
	replace(metaName.begin(), metaName.end(), '-', '_');
	if ((*mFilter)(metaName,metaIterator->getMetaVal())) {
	  if (metaIterator->getMetaVal()->getType() == META_TABLE){
	    
 	    insertMetaTableValue( dynamic_cast<TableMetaValue*> 
				  (metaIterator->getMetaVal()), metaName);
	  }
	  else if (metaIterator->getMetaVal()->getType() == META_ARRAY){

	    insertArrayMetaValue(  dynamic_cast<ArrayMetaValue *>(metaIterator->getMetaVal()), metaName);
	  }
	  else {
	    insertStandardMetaValue(metaIterator->getMetaVal(), 
				    metaName);
	    
	  } 
	}
      } while (metaIterator->next());
    }
    jobno++;
  } while (jobIterator->next());
}


void Dsm2::insertArrayMetaValue(ArrayMetaValue *inMetaValue, string inName){

  if (inMetaValue == 0){

    throw OcfaException("meta value is not an array or 0", this);
  }
  size_t metaCount = inMetaValue->size();
  for (size_t x = 0; x < metaCount; x++){
    ScalarMetaValue *metaValue = dynamic_cast<ScalarMetaValue *>(inMetaValue->getValueAt(x));
    insertStandardMetaValue(metaValue, inName);
  }

}

void Dsm2::insertMetaTableValue(TableMetaValue *inTableValue,
			       string inName){ 
  // precondition checking.
  if  (inTableValue == 0){
    
    string message = "Dsm2::insertMetaTableValue received  0 inTableValue";
    logModule(LOG_ERR, 
	      message);
    throw OcfaException(message, this);
  }
  if (inTableValue->getColCount() == 0){

    string message = "Dsm2::insertMetaTableValue received table value with 0 columns";
    logModule(LOG_ERR, message);
    throw OcfaException(message, this);
  }

  // repace '-' ny '_' 
  replace(inName.begin(), inName.end(), '-', '_');
  
  // retrieve columnnames
  //vector <string> columnNames;
  ostringstream queryHeaderStream;
  queryHeaderStream << "insert into tbl" << escape(inName);
  queryHeaderStream << " (metadataid ";
  unsigned int colNr;
  for (colNr = 0;  colNr < inTableValue->getColCount(); colNr++){
    
    string header = inTableValue->getColName(colNr);
    // replace '-' by '_' to avoid db problems
    replace(header.begin(), header.end(), '-', '_');

    queryHeaderStream << ", m" << escape(header);
  
    //columnNames.push_back(inTableValue->getColName());
  }

  queryHeaderStream << ") VALUES (" << getMetaXMLStoreHandle()->getHandle();
  //queryHeaderStream << ", '" << getEvidenceID() << "'";
  unsigned int rowNr;
  for (rowNr = 0; rowNr < inTableValue->size(); rowNr++){

    ArrayMetaValue *rowValue = dynamic_cast<ArrayMetaValue *>(inTableValue->getValueAt(rowNr));
    if (rowValue == 0){

      string message = "Dsm2::insertMetaTableValue Bad formatted rowValue in a TableMetaValue";
      logModule(LOG_ERR, message);
      throw OcfaException(message, this);
    }
    ostringstream queryStream;
   
    queryStream << queryHeaderStream.str();
    for (colNr = 0; colNr < rowValue->size(); colNr++){
      ScalarMetaValue *value = dynamic_cast<ScalarMetaValue *>(rowValue->getValueAt(colNr));
      if (value == 0){
	throw OcfaException( "a table cell consisted of non scalar cells", this);
      }
      else {
	// escape stuff, to be sure.
	Scalar scalarHelp = value->asScalar();
	string utfvalue = scalarHelp.asUTF8();
        std::string escapedValue =escape(utfvalue);
        std::string trimmedScalarVal;
        if(escapedValue.length() > DEFAULT_ROW_VARCHAR_SIZE) {
             trimmedScalarVal = escapedValue.substr(0,DEFAULT_ROW_VARCHAR_SIZE);
             trimmedScalarVal.replace(DEFAULT_ROW_VARCHAR_SIZE-3,3,"...");
             getLogStream(LOG_WARNING) << "Scalar Value to long for database column width, trimmed!" << endl;
        } else {
              trimmedScalarVal=escapedValue;
        }
	queryStream  << ", '" << trimmedScalarVal << "' ";  
      }
    }
    queryStream << ")";
    getLogStream(LOG_INFO) << "insertint metavalue query:" << queryStream.str() << endl;
    PGresult *pgres = PQexec(mConnection, queryStream.str().c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){

      getLogStream(LOG_ERR) << "Database connection exeption occurred! " << endl;
      throwDatabaseException(queryStream.str(), pgres);
    }
    PQclear(pgres);
  }
} 
 

void Dsm2::insertLocation(){
  ostringstream commandStream;
  // Create escaped version
  string location = getEvidenceLocation().asUTF8();
  string escapedLocation = escape(location); 
  commandStream << "update metadatainfo set location = '" << escapedLocation 
		<< "' where metadataid = '" + getMetaXMLStoreHandle()->getHandle() << "';";
  getLogStream(LOG_DEBUG) << "insert location command is " << commandStream.str() << endl;
  PGresult *pgres = PQexec(mConnection, commandStream.str().c_str());
  if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
    getLogStream(LOG_ERR) << "Error inserting location of  " << getEvidenceID() << endl;
    throwDatabaseException(commandStream.str(), pgres);
  }  
  PQclear(pgres);

}

void Dsm2::insertModuleName(std::string modulename,size_t jobno){
   std::string query= std::string("insert into joblist (metadataid,module,jobseq) VALUES (") + getMetaXMLStoreHandle()->getHandle() + ",'" + modulename + "'," + boost::lexical_cast<std::string>(jobno) + ")";
   PGresult *pgres = PQexec(mConnection, query.c_str());
   if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      getLogStream(LOG_ERR) << "Error adding job " << jobno << " to the database for evidence " << getEvidenceID() << endl;
      throwDatabaseException(query, pgres);
   }
   PQclear(pgres);   
}

void Dsm2::insertLogLine(std::string modulename,size_t jobno,std::string logline) {
   std::string query= std::string("insert into loglines (metadataid,module,jobseq,line) VALUES (") + getMetaXMLStoreHandle()->getHandle() + ",'" + modulename + "'," + boost::lexical_cast<std::string>(jobno) + 
       ",'" + escape(logline) +  "')"; 
   PGresult *pgres = PQexec(mConnection, query.c_str());              
   if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      getLogStream(LOG_ERR) << "Error adding logline job " << jobno << " to the database for evidence " << getEvidenceID() << endl;
      throwDatabaseException(query, pgres);
   }
   PQclear(pgres);
}

void Dsm2::insertStandardMetaValue(MetaValue *value, string inName){
  if (value == 0){
    string message = "Dsm2::insertStandardMetaValue no value given"; 
    logModule(LOG_ERR, message);
    throw OcfaException(message, this);
  }
  replace(inName.begin(), inName.end(), '-', '_');
  std::string query="insert into row" + escape(inName) + " (metadataid,meta";
  ScalarMetaValue &scalarvalue= dynamic_cast<ScalarMetaValue &>(*value);
  if (scalarvalue.asScalar().getType() == ocfa::misc::Scalar::SCL_DATETIME) 
      query += ",timesource";
  query += ") VALUES (" + getMetaXMLStoreHandle()->getHandle() + ", '";
  if (scalarvalue.asScalar().getType() == ocfa::misc::Scalar::SCL_DATETIME) {
     ocfa::misc::Scalar dtscal=scalarvalue.asScalar();
     std::string datestr=dtscal.asUTF8();
     const ocfa::misc::DateTime *dt=dtscal.asDateTime();
     long long time=dt->getTime();
     std::string timestr=ocfa::misc::DateTime::translate(time);
     std::string timesource=dt->getTimeSourceRef();
     query += timestr + "','" + escape(scalarvalue.asScalar().asDateTime()->getTimeSourceRef()) + "')";
  } else {
     // JCW Check for string length
     // DEFAULT_ROW_VARCHAR_SIZE
     std::string escapedScalarVal = escape(scalarvalue.asScalar().asUTF8());
     std::string trimmedScalarVal;
     if(escapedScalarVal.length() > DEFAULT_ROW_VARCHAR_SIZE) {
        //trim value to max length and genereate warning,
	//also replace last three characters by "..."
	trimmedScalarVal = escapedScalarVal.substr(0,DEFAULT_ROW_VARCHAR_SIZE);
	trimmedScalarVal.replace(DEFAULT_ROW_VARCHAR_SIZE-3,3,"...");
	getLogStream(LOG_WARNING) << "Scalar Value to long for database column width, trimmed!" << endl;
     } else {
        trimmedScalarVal = escapedScalarVal;
     }

     query += trimmedScalarVal + "')";
  }
 PGresult *pgres = PQexec(mConnection, query.c_str());
 if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
    
   throwDatabaseException(query, pgres);
  }
  PQclear(pgres);
}




 
int main(int , char *[]){
  Dsm2 dsm;  
  try {
    dsm.run();
  } catch (ocfa::OcfaException &e){

    dsm.getLogStream(ocfa::misc::LOG_ERR) << "OcfaException: " 
					  << e.what() << endl;
  }
}
