#include <string>
#include <sstream>
#include <libpq-fe.h>
#include <iostream>
#include <vector>
#include <boost/tokenizer.hpp>
#include <pwd.h>
#include <fstream>
#include <misc/OcfaConfig.hpp>
using namespace ocfa::misc;
using namespace std;
using namespace boost;
void throwDatabaseException(string command, PGresult *inResult);
void removeMetaDataRecords(PGconn *connection, string &metadataid, vector<string> &inTableNames);
void getMetaTableNames(vector<string> &outTableNames);
PGconn *openDatabase();
void removeMetaStoreReferences(PGconn *, string &inItemId, 
			       vector<string> &inTableNames){

  ostringstream query;
  PGresult *pgres;
  query << "select metastoreentity.repname, MetaDataInfo.metadataid" 
	<< " from MetaDataInfo inner join "
	<< " metastoreentity on MetaDataInfo.metadataid = metastoreentity.id where itemid = '" 
	<< inItemId << "'";
  //PQresetStart(connection);
  PGconn *connection;
  connection= openDatabase();
  cerr << "executing " << query.str() << "on connection " << connection << endl;
  pgres = PQexec(connection, query.str().c_str());
  cerr << "executed" << endl;
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    cerr << "Error was " << PQerrorMessage(connection) << endl;
  
    throwDatabaseException(query.str(), pgres);
  }
  int rowCount =PQntuples(pgres);
  cerr << "found " << rowCount  << " records" << endl;
  string repository = OcfaConfig::Instance()->getValue("repository");
  for (int x = 0; x < rowCount; x++){

    string file = repository + PQgetvalue(pgres, x, 0);
    string metaDataId = PQgetvalue(pgres, x, 1);

    cerr << "unlinking " << file << endl;
    unlink(file.c_str());
    removeMetaDataRecords(connection, metaDataId, inTableNames);
  }
  PQclear(pgres);
}
    

void removeMetaDataRecords(PGconn *connection, string & metadataid, vector<string> &inTableNames){

  vector<string>::iterator iter;
  for (iter = inTableNames.begin(); iter < inTableNames.end(); iter++){

    ostringstream deleteCommand;
    deleteCommand << "delete from " << *iter << " where xml = '" 
		  << metadataid << "'";
    //    cerr << "deletecommand is " << deleteCommand.str() << endl;
    PGresult *pgres = PQexec(connection, deleteCommand.str().c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){

      throwDatabaseException(deleteCommand.str(), pgres);
    }
    PQclear(pgres);
  }
}

void getMetaTableNames(vector<string> &outTableNames){

  vector<string> metatables;
  string file = OcfaConfig::Instance()->getValue("metatabledescription");

  if (file == ""){ 

    cerr << "warning: no metatabledescripitonfile found " << endl;
    cerr << "Going without it " << endl;
    return;
  }
  
  
  cerr << "using table description file " << endl; 
  ifstream tableDescriptionStream(file.c_str());
  if (! tableDescriptionStream){

      cerr << "cannot read from " << file;
      cerr <<  "Going without it ";    
      return;
  }
  string metaDescription;
  while (tableDescriptionStream){
    
    getline(tableDescriptionStream, metaDescription);
    if (metaDescription[0] != '#'){
    
      //(LOG_DEBUG) << "metaDescription is " << metaDescription << endl;
      boost::char_separator<char> sep (" ");
      tokenizer<char_separator<char> > tok(metaDescription, sep);

      tokenizer<char_separator<char> >::iterator iter = tok.begin();
      if (iter != tok.end()){

	string metaTableName = string("meta") + *iter;
	replace(metaTableName.begin(), metaTableName.end(), '-', '_');
	outTableNames.push_back(metaTableName);
      }
    }
  }
  //PQfinish(connection);
}

void throwDatabaseException(string command, PGresult *pgres){
  
  cerr << "error executing " << command << endl;
  cerr << "Error was " << PQresultErrorMessage(pgres) << endl;
  cerr << "Error code was " << (long)  PQresultStatus(pgres) << endl;
  cerr << "Error status was " << PQresStatus(PQresultStatus(pgres)) << endl;
  exit(-1);
}    
PGconn *openDatabase(){
 
  string dbName = OcfaConfig::Instance()->getValue("storedbname");
  string dbhost = OcfaConfig::Instance()->getValue("storedbhost");
  string dbuser = OcfaConfig::Instance()->getValue("storedbuser");
  char connectionString[255];
  if (dbuser == ""){
    
    struct passwd *pwent = getpwuid(getuid());
    dbuser = pwent->pw_name;
  }
  string dbpasswd = OcfaConfig::Instance()->getValue("storedbpasswd");
  sprintf(connectionString, "dbname=%s hostaddr=%s user=%s password=%s", 
	  dbName.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
  cerr << "connectionString is " << connectionString << endl;
  PGconn *connection = PQconnectdb(connectionString);
  if (PQstatus(connection) != CONNECTION_OK) {
    
    string errstr = "PgRepository initialization failed: " 
      + string(PQerrorMessage(connection));
    //printf("%s",errstr.c_str());
    cerr << "error "<< errstr;
    exit(-1);
  } 
  return connection;
}

string getItemId(PGconn *connection, string inCase, string inSource, string inItem){

  ostringstream sqlQuery;
  sqlQuery << "select itemid from item where casename = '" << inCase 
	   << "' and evidencesource = '" << inSource << "' and item = '" << inItem << "'";
  PGresult *pgres = PQexec(connection, sqlQuery.str().c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    cerr << "Error executing " << sqlQuery.str() << endl;
    exit (-1);
  }
  if (PQntuples(pgres) == 1){

    string value = PQgetvalue(pgres, 0, 0);
    PQclear(pgres);
    cerr << "returning " << value << endl;
    string feedback;
    switch(PQstatus(connection))
      {
      case CONNECTION_STARTED:
        feedback = "Connecting...";
        break;
	
      case CONNECTION_MADE:
        feedback = "Connected to server...";
        break;
      case CONNECTION_AWAITING_RESPONSE:
	feedback = "awaiting response";
	break;
      case CONNECTION_OK:
	feedback = "Connection ok";
	break;
      case CONNECTION_BAD:
	feedback = "COnnection bad";
	break;
      case CONNECTION_AUTH_OK:
      case CONNECTION_SETENV:
      case CONNECTION_SSL_STARTUP:
      case CONNECTION_NEEDED:
       
	feedback = "other";
	break;
      }

    cerr << "feedback of conneciton " << connection << "is " << feedback << endl;
    return value;
  }
  else {
    return "";
  }
}

void removeMetaRecords(PGconn *connection, string &inItemId){

  ostringstream sqlCommand;
  sqlCommand << "select deleteitemdata('" << inItemId << "')";
  cerr << "Executing " << sqlCommand.str() << endl;
  PGresult *pgres = PQexec(connection, sqlCommand.str().c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    throwDatabaseException(sqlCommand.str(), pgres);
    exit(-1);
  }
  PQclear(pgres);

}



int main (int argc, char *argv[]){

  if (argc < 4){

    cout << "usage: removeitem <case> <source> <item>" << endl;
    return 2;
  }
  else {
    string theCase = argv[1];
    string theSource = argv[2];
    string theItem = argv[3];
    PGconn *connection = openDatabase();
    string itemId = getItemId(connection, theCase, theSource, theItem);
    //PQfinish(connection);
    //connection = 0;
    if (itemId == ""){

      cout << "could not find item" << endl;
     
    }
    else {

      vector<string> metaTableNames;
      getMetaTableNames(metaTableNames);
      //     prepareMetaTables(metaTableNames);
      removeMetaStoreReferences(connection, itemId, metaTableNames);
      removeMetaRecords(connection, itemId);
    }
    PQfinish(connection);
  }
}
