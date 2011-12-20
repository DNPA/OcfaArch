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

#include "Dsm.hpp"
#include <boost/tokenizer.hpp>
#include <store/AbstractRepository.hpp>
#include <evidence/JobIterator.hpp>
#include <evidence/MetaIterator.hpp>
#include <pwd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
using namespace ocfa::message;
using namespace boost;
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::store;
using namespace std;
Dsm::Dsm() : 
	XMLAccessor("dsm", "core"),
	mInterestingMeta(),
        mConnection(0),
        mTransaction(false)
{
  getLogStream(LOG_DEBUG) << "Dsm::Dsm in constructor" << endl;
  createMetaTables();
  if (getConfEntry("transactions")== "true")
       mTransaction=true;
  getLogStream(LOG_DEBUG) << "Dsm::Dsm connection is " << mConnection << endl;
}

Dsm::~Dsm(){

  if (mConnection != 0){

    PQfinish(mConnection);
    mConnection = 0;
  
  }
}
/*
void Dsm::initialize(string inName, string inNameSpace){

   getLogStream(LOG_DEBUG) << "Dsm::initialize entering initialize" << endl;
  BaseAccessor::initialize(inName, inNameSpace);
}
*/
static void DSMlogNotices(void *, const char *msg){
	   OcfaLogger::Instance()->syslog(LOG_NOTICE, string("DSM database notice:")+string(msg)+string("\n"));
}
std::string Dsm::escape(std::string in) {
    char *escaped=(char *)calloc((in.size()+2)*4,1);
    PQescapeString(escaped, in.c_str(), in.length()); 
    std::string rval(escaped);
    free(escaped);
    return rval;
}
PGconn *Dsm::openDatabase(){
 
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




void Dsm::createMetaTables(){

  string file = getConfEntry("metatabledescription");
  mConnection = openDatabase();
  ensureLocationTable();
  ensureColumn("wasread", "bool", "false");
  
  if (file == ""){

    logModule(LOG_WARNING, "no metatabledescripitonfile found ");
    logModule(LOG_WARNING, "Going without it ");
    return;
  }
  logModule(LOG_INFO, string("using table description file ") + file); 
  ifstream tableDescriptionStream(file.c_str());
  if (! tableDescriptionStream){

    logModule(LOG_ERR, string("cannot read from ") + file);
    logModule(LOG_ERR, "Going without it ");
    return;
  }
  string metaDescription;
  //char buffer[100];
  while (tableDescriptionStream){
    
    getline(tableDescriptionStream, metaDescription);
    if (metaDescription[0] != '#'){
    
      getLogStream(LOG_DEBUG) << "metaDescription is " << metaDescription << endl;
      boost::char_separator<char> sep (" ");
      tokenizer<char_separator<char> > tok(metaDescription, sep);

      tokenizer<char_separator<char> >::iterator iter = tok.begin();
      if (iter != tok.end()){
	string metaName = *iter;
	getLogStream(LOG_DEBUG) << "metaName is " << metaName << endl;
	mInterestingMeta[metaName] = true;
	replace(metaName.begin(), metaName.end(), '-', '_');
	iter++;
	if (iter != tok.end()){
	  
	  vector<string> headers;
	  do {

	    string header = *iter;
	    replace(header.begin(), header.end(), '-', '_');
	    headers.push_back(header);
	    iter++;
	  } while (iter != tok.end());	
	  ensureMetaTableTable(metaName, headers);
	}
	else {
	  
	  ensureMetaNameTable(metaName);
	}
      }
    }
  }
}
    

void Dsm::ensureLocationTable(){

  // check if the column exists.
  string command = "select table_name, column_name from information_schema.columns where table_name = 'metadatainfo' and column_name = 'location';";
  PGresult *pgres;
  pgres = PQexec(mConnection, command.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    getLogStream(LOG_ERR) << "cannot determing whether location field exists" << endl;
    throwDatabaseException(command, pgres);
  }
  int rows = PQntuples(pgres);
  PQclear(pgres);
  if (rows == 0){
    
    ostringstream queryStream;
    // command = "alter table metadatainfo add column  location varchar(1024) default '[being processed]'";    
    command = "alter table metadatainfo add column location varchar(1024);";
    getLogStream(LOG_DEBUG) << "executing command " << command << endl;
    pgres = PQexec(mConnection, command.c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      
      getLogStream(LOG_ERR) << "problem while adding location column " << endl;
      throwDatabaseException(queryStream.str(), pgres);
    }
    command = "alter table metadatainfo alter column location set default '[being processed]'";
    getLogStream(LOG_DEBUG) << "executing command " << command << endl;
    PQclear(pgres);
    pgres = PQexec(mConnection, command.c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      getLogStream(LOG_ERR) << "problem while altering location column " << endl;
      throwDatabaseException(queryStream.str(), pgres);
    }
    PQclear(pgres);
    //alter table metadatainfo add column  location varchar(255);
    //alter table metadatainfo alter column location set default 'wordt nog gewassen';
		  
  }

}

void Dsm::ensureColumn(string inColumnName, string inType, string inDefault){

  // check if the column exists.
  string command = "select table_name, column_name from information_schema.columns where table_name = 'metadatainfo' and column_name = '" + escape(inColumnName) + "';";
  PGresult *pgres;
  pgres = PQexec(mConnection, command.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    getLogStream(LOG_ERR) << "cannot determing whether location field exists" << endl;
    throwDatabaseException(command, pgres);
  }
  int rows = PQntuples(pgres);
  PQclear(pgres);
  if (rows == 0){
    
    //ostringstream queryStream;
    // command = "alter table metadatainfo add column  location varchar(1024) default 'wordt nog gewassen'";    
    command = "alter table metadatainfo add column " + escape(inColumnName) + " " + escape(inType) + ";";
    getLogStream(LOG_DEBUG) << "executing command " << command << endl;
    pgres = PQexec(mConnection, command.c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      
      getLogStream(LOG_ERR) << "throwing while adding " + inColumnName+ " column " << endl;
      throwDatabaseException(command, pgres);
    }
    command = "alter table metadatainfo alter column " + escape(inColumnName) + " set default '" + escape(inDefault) + "'";
    getLogStream(LOG_DEBUG) << "executing command " << command << endl;
    PQclear(pgres);
    pgres = PQexec(mConnection, command.c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      
      getLogStream(LOG_ERR) << "throwing while adding " << inColumnName << " column " << endl;
      throwDatabaseException(command, pgres);
    }
    PQclear(pgres);
    //alter table metadatainfo add column  location varchar(255);
    //alter table metadatainfo alter column location set default 'wordt nog gewassen';
		  
  }



}
void Dsm::ensureMetaNameTable(string metaName){
    
  if (! tableExists(string("meta") + metaName)){
	
    ostringstream queryStream;
    PGresult *pgres;
    queryStream.str("");
    queryStream << "create table meta" << escape(metaName) 
		<< "(id serial, xml int,meta varchar(255));";    
    
    pgres = PQexec(mConnection, queryStream.str().c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      getLogStream(LOG_ERR) << "throwing up here " << endl;
      throwDatabaseException(queryStream.str(), pgres);
    }
    PQclear(pgres);
  }
}

bool Dsm::tableExists(string metaName){

  ostringstream queryStream;
  transform(metaName.begin(), metaName.end(), metaName.begin(), ::tolower);

  queryStream << "select tablename from pg_tables " 
	      << "where tablename = '" << escape(metaName) << "'"
	      << "and schemaname = 'public'";
  PGresult *pgres;
  logModule(LOG_DEBUG, string("ensureMetaNameTable: executing..") + queryStream.str()); 
  pgres = PQexec(mConnection, queryStream.str().c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    getLogStream(LOG_ERR) <<  string ("throwinf database exception on ") <<  PQresStatus(PQresultStatus(pgres)) << endl;
    throwDatabaseException(queryStream.str(), pgres);
  }
  bool returnValue = (PQntuples(pgres) != 0);
  PQclear(pgres);
  return returnValue;
}

void Dsm::ensureMetaTableTable(string metaName, vector<string> headers){
  
  if (! tableExists("meta" + metaName)){

    ostringstream queryStream;
    PGresult *pgres;
    vector<string>::iterator iter;
    queryStream << "create table meta" << escape(metaName) << " (xml int";
    for (iter = headers.begin(); iter != headers.end(); iter++){

      queryStream << ", " << escape(*iter) << " varchar(255)";
    }
    queryStream << ")";
    pgres = PQexec(mConnection, queryStream.str().c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){

      getLogStream(LOG_ERR) << "throwing up here " << endl;
      throwDatabaseException(queryStream.str(), pgres);
    }
    PQclear(pgres);
  }
    
}

void Dsm::throwDatabaseException(string command, PGresult *inResult){
  
  ocfaLog(LOG_ERR, string("error executing ").append(command));
  ocfaLog(LOG_ERR, string("error was " ).append(PQresStatus(PQresultStatus(inResult))));
  ocfaLog(LOG_ERR, string("result status was ", PQresultStatus(inResult)));
  //  ocfaLog(LOG_ERR, string("primary description: ")
  //  + PQresultErrorField(inResult, PG_DIAG_MESSAGE_PRIMARY));	
  //ocfaLog(LOG_ERR, string((PQresultErrorField(inResult, PG_DIAG_SQLSTATE))));
  throw OcfaException(string("Error: ").append(PQresStatus(PQresultStatus(inResult))), this);
}    


void Dsm::processEvidenceMessage(const Message &message){
   if (mTransaction) {
      PQexec(mConnection,"BEGIN;");
   }
   try {
	//create an evidence from the message.
	MetaStoreEntity *metaEntity;
	ModuleInstance sender(message.getSender());
	getLogStream(LOG_INFO) << " received message with content " << 
	  message.getContent() << endl;
	AbstractRepository::Instance()
	  ->createMetaStoreEntity(&metaEntity, OcfaHandle(message.getContent()));
	if (metaEntity == 0){

	  throw OcfaException(string("no Meta could be created from ") + message.getContent());
	}
	ocfaLog(LOG_DEBUG, "setting evidence");

	setEvidence(createEvidenceFromMeta(metaEntity));

	// set evidence mutable.
	getEvidence()->setMutable();

	// call the pure virtual process message now that the active evidence is set.
	ocfaLog(LOG_DEBUG, "Calling processEvidence.");
	try {
	  processEvidence();
	} catch (OcfaException &e){

	  getLogStream(LOG_ERR) << e.what() << endl;
	  logEvidence(LOG_ERR, e.what());
	}
	//
	// close the activeJob
	ocfaLog(LOG_DEBUG, "closing active JOb");
	getEvidence()->getActiveJob()->close();
	
	ocfaLog(LOG_DEBUG, "closed active Job");
	// update the metastoreentity with the changed evidence.
	updateMetaWithEvidence(*metaEntity, *getEvidence());
	delete metaEntity;
	delete getEvidence();
	setEvidence(0);
	//OcfaObject::PrintObjCount();
        
   } catch (OcfaException &e){
     if (mTransaction) {
        PQexec(mConnection,"ROLLBACK;"); 
     }
     ocfaLog(LOG_ERR, "ocfaexception ");
     throw e;
   }
   if (mTransaction) {
      PQexec(mConnection,"COMMIT;");
   }
}

void Dsm::processEvidence(){
  getLogStream(LOG_DEBUG) << "entering processEvidence()" << endl;
  // JBS todo: the behavior of this part should be dependent on the commit
  // Flag. This behavior is ACT_COMMIT behavior
 
  insertLocation();
  JobIterator *jobIterator = getEvidence()->getJobIterator();
  
  if (jobIterator == 0){

    throw OcfaException("no job iterator found", this);
  }
  do {

    MetaIterator *metaIterator = jobIterator->getMetaIterator();
    if (metaIterator != 0){

      do {
	
	string metaName  =metaIterator->getName();
	transform(metaName.begin(), metaName.end(), metaName.begin(), ::tolower);
	
	if (mInterestingMeta[metaName]){
	  	  
	  ostringstream queryStream;
	  ;
	  // replace '-' by '_' to avoid db problems.
	  replace(metaName.begin(), metaName.end(), '-', '_');
	  //	  queryStream << "insert into meta" << metaName;
	  //queryStream << " (xml, id";
	  if (metaIterator->getMetaVal()->getType() == META_TABLE){
	    
 	    insertMetaTableValue( dynamic_cast<TableMetaValue*> 
				  (metaIterator->getMetaVal()), metaIterator->getName());
	  }
	  else if (metaIterator->getMetaVal()->getType() == META_ARRAY){

	    insertArrayMetaValue(  dynamic_cast<ArrayMetaValue *>(metaIterator->getMetaVal()), metaIterator->getName());
	  }
	  else {
	    insertStandardMetaValue(metaIterator->getMetaVal(), 
				    metaIterator->getName());
	    
	  } 
	}
      } while (metaIterator->next());
    }
  } while (jobIterator->next());
}


void Dsm::insertArrayMetaValue(ArrayMetaValue *inMetaValue, string inName){

  if (inMetaValue == 0){

    throw OcfaException("meta value is not an array or 0", this);
  }
  size_t metaCount = inMetaValue->size();
  for (size_t x = 0; x < metaCount; x++){

    ScalarMetaValue *metaValue = dynamic_cast<ScalarMetaValue *>(inMetaValue->getValueAt(x));
    insertStandardMetaValue(metaValue, inName);
					      		 
  }

}

void Dsm::insertLocation(){

  ostringstream commandStream;
  // Create escaped version
  string location = getEvidenceLocation().asUTF8();
  char *escapedLocation = new char[(2 * location.length()) + 1];
  // PQescapeStringConn woule be better but for some reason I cannot find it in the headers
  PQescapeString(escapedLocation, location.c_str(), location.length());

  commandStream << "update metadatainfo set location = '" << escapedLocation 
		<< "' where metadataid = '" + getMetaXMLStoreHandle()->getHandle() << "';";
  getLogStream(LOG_DEBUG) << "insert location command is " << commandStream.str() << endl;
  PGresult *pgres = PQexec(mConnection, commandStream.str().c_str());
  if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
    
    getLogStream(LOG_ERR) << "Error inserting location of  " << getEvidenceID() << endl;
    throwDatabaseException(commandStream.str(), pgres);
  }  
  PQclear(pgres);
  delete [] escapedLocation;
}


void Dsm::insertMetaTableValue(TableMetaValue *inTableValue,
			       string inName){ 
  // precondition checking.
  if  (inTableValue == 0){
    
    string message = "Dsm::insertMetaTableValue received  0 inTableValue";
    logModule(LOG_ERR, 
	      message);
    throw OcfaException(message, this);
  }
  if (inTableValue->getColCount() == 0){

    string message = "Dsm::insertMetaTableValue received table value with 0 columns";
    logModule(LOG_ERR, message);
    throw OcfaException(message, this);
  }

  // repace '-' ny '_' 
  replace(inName.begin(), inName.end(), '-', '_');
  
  // retrieve columnnames
  //vector <string> columnNames;
  ostringstream queryHeaderStream;
  queryHeaderStream << "insert into meta" << escape(inName);
  queryHeaderStream << " (xml ";
  unsigned int colNr;
  for (colNr = 0;  colNr < inTableValue->getColCount(); colNr++){
    
    string header = inTableValue->getColName(colNr);
    // replace '-' by '_' to avoid db problems
    replace(header.begin(), header.end(), '-', '_');

    queryHeaderStream << ", " << escape(header);
  
    //columnNames.push_back(inTableValue->getColName());
  }

  queryHeaderStream << ") VALUES (" << getMetaXMLStoreHandle()->getHandle();
  //queryHeaderStream << ", '" << getEvidenceID() << "'";
  unsigned int rowNr;
  for (rowNr = 0; rowNr < inTableValue->size(); rowNr++){

    ArrayMetaValue *rowValue = dynamic_cast<ArrayMetaValue *>(inTableValue->getValueAt(rowNr));
    if (rowValue == 0){

      string message = "Dsm::insertMetaTableValue Bad formatted rowValue in a TableMetaValue";
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
	char *escapedValue = new char[utfvalue.length() * 2];
	unsigned int sizeEscaped  = PQescapeString(escapedValue, utfvalue.c_str(), utfvalue.length());
	if (sizeEscaped > utfvalue.length() * 2){

	  getLogStream(LOG_CRIT) << "too big escape sequence. should exit now." << endl;
	  throw OcfaException( "too big escape sequence. memory overwritten. Please terminate this process", this);
	}
	else {

	  queryStream  << ", '" << escapedValue << "' ";  
	}
	delete [] escapedValue;
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
 



void Dsm::insertStandardMetaValue(MetaValue *value, string inName){

  if (value == 0){

    string message = "Dsm::insertStandardMetaValue no value given"; 
    logModule(LOG_ERR, message);
    throw OcfaException(message, this);
  }
  replace(inName.begin(), inName.end(), '-', '_');

  ostringstream queryStream;
  queryStream << "insert into meta" << escape(inName);
  queryStream << " (xml, meta) VALUES (";
  queryStream << getMetaXMLStoreHandle()->getHandle() << ", '" 
	      << escape(getStringValue(*value)) << "')";
  
  
  PGresult *pgres = PQexec(mConnection, queryStream.str().c_str());
 if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
    
   throwDatabaseException(queryStream.str(), pgres);
    // if insertion fails try an update.
   /*    getLogStream(LOG_NOTICE) << "Cannot insert " << inName << " with value: " << getStringValue(*value)
	 << " for xmlMetaHandle " << getMetaXMLStoreHandle()->getHandle() << endl;
	 getLogStream(LOG_NOTICE) << "Trying update" << endl;
	 updateStandardMetaValue(value, inName);
   */
  }
  PQclear(pgres);
}
 
void Dsm::updateStandardMetaValue(MetaValue *value, string inName){

  if (value == 0){

    string message = "Dsm::insertStandardMetaValue no value given"; 
    logModule(LOG_ERR, message);
    throw OcfaException(message, this);
  }
  replace(inName.begin(), inName.end(), '-', '_');
  ostringstream queryStream;
  queryStream << "update meta" << escape(inName);
  queryStream << " set meta = '" << escape(getStringValue(*value)) << "' ";
  queryStream << "where xml = '" << getMetaXMLStoreHandle()->getHandle() << "'";
  PGresult *pgres = PQexec(mConnection, queryStream.str().c_str());
  if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
    
    throwDatabaseException(queryStream.str(), pgres);
  }
  PQclear(pgres);

}
string Dsm::getStringValue(MetaValue &inValue){

  string returnValue;
  if (inValue.getType() == META_ARRAY){

    ArrayMetaValue &arrayMeta = dynamic_cast<ArrayMetaValue &>(inValue);
    ScalarMetaValue *scalarValue;
    scalarValue = dynamic_cast<ScalarMetaValue *>(inValue.getValueAt(0));
    if (scalarValue == 0){
      
      string message = "Dsm::getStringValue retrieved a nonscalar value from a ArrayMetaValue";
      logModule(LOG_ERR, message);
      throw OcfaException(message, this);
    }
    returnValue += scalarValue->asScalar().asUTF8();
    for (unsigned int x = 1; x < arrayMeta.size(); x++){
      
      scalarValue = dynamic_cast<ScalarMetaValue *>(arrayMeta.getValueAt(x));
      if (scalarValue == 0){

	string message = "Dsm::getStringValue retrieved a nonscalar value from a ArrayMetaValue";
	logModule(LOG_ERR, message);
	throw OcfaException(message, this);
      }
      returnValue += " ";
      returnValue += scalarValue->asScalar().asUTF8();
    }
  }
  else {

    ScalarMetaValue &scalarValue = dynamic_cast<ScalarMetaValue &>(inValue);
    returnValue += scalarValue.asScalar().asUTF8();
  }
  returnValue = returnValue.substr(0,254);
  return returnValue;
}

int main(int , char *[]){
  
  Dsm dsm;  
  try {
    dsm.run();
  } catch (ocfa::OcfaException e){

    dsm.getLogStream(ocfa::misc::LOG_ERR) << "OcfaException: " 
					  << e.what() << endl;
  }
}
