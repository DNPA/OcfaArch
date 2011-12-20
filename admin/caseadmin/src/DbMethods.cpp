
#include <libpq-fe.h>
#include <misc/OcfaConfig.hpp>
#include <pwd.h>

#include <iostream>
#include "DbMethods.hpp"
using namespace ocfa::misc;
using namespace std;

void throwDatabaseException(string command, PGresult *pgres){
  
  cerr << "error executing " << command << endl;
  cerr << "Error was " << PQresultErrorMessage(pgres) << endl;
  cerr << "Error code was " << (long)  PQresultStatus(pgres) << endl;
  cerr << "Error status was " << PQresStatus(PQresultStatus(pgres)) << endl;
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
    
  } 
  return connection;
}
