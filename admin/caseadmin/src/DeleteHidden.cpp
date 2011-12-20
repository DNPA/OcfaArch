#include "DbMethods.hpp"
#include <misc/OcfaConfig.hpp>
#include <string>
#include <iostream>
using namespace std;
void unlinkStoreEntities(PGconn *database);

int main(int argc, char *argv[]){

  PGconn *database = openDatabase();
  string command = "select preparetodeleteitem()";
  PGresult *pgres = PQexec(database, command.c_str());
  if (PQresultStatus(pgres) !=  PGRES_TUPLES_OK){

    cerr << "preparetodelete: Error was " << PQerrorMessage(database) << endl;
  
    throwDatabaseException(command, pgres);
  }
  command = "select setHiddenForDelete()";
  pgres = PQexec(database, command.c_str());
  if (PQresultStatus(pgres) !=  PGRES_TUPLES_OK){

    cerr << "sethiddenfordelet: Error was " << PQerrorMessage(database) << endl;
  
    throwDatabaseException(command, pgres);
  }
  unlinkStoreEntities(database);
  command = string("select deletehidden()");
  pgres = PQexec(database, command.c_str());
  if (PQresultStatus(pgres) !=  PGRES_TUPLES_OK){

    cerr << "purging Error was " << PQerrorMessage(database) << endl;
  
    throwDatabaseException(command, pgres);
  }

  command = string("select cleanupafterdelete()");
  pgres = PQexec(database, command.c_str());
  cerr << "executing " << command << endl;
  if (PQresultStatus(pgres) !=  PGRES_TUPLES_OK){

    cerr << "purging Error was " << PQerrorMessage(database) << endl;
  
    throwDatabaseException(command, pgres);
  }

}



void unlinkStoreEntities(PGconn *database){

  string query = "select metastoreentity.repname from metastoreentity where willdelete = true";
  PGresult *pgres = PQexec(database, query.c_str());
  cerr << "executed" << endl;
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    cerr << "Error was " << PQerrorMessage(database) << endl;
  
    throwDatabaseException(query, pgres);
  }  
  int rowCount =PQntuples(pgres);
  cerr << "found " << rowCount  << " records" << endl;
  string repository = ocfa::misc::OcfaConfig::Instance()->getValue("repository");
  for (int x = 0; x < rowCount; x++){

    string file = repository + PQgetvalue(pgres, x, 0);
    string metaDataId = PQgetvalue(pgres, x, 0);

    cerr << "unlinking " << file << endl;
    unlink(file.c_str());
    // removeMetaDataRecords(connection, metaDataId, inTableNames);
  }
  
}
