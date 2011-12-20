#include "DbMethods.hpp"
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){

  if (argc != 4){

    cerr << "usage: hideItem <case> <source> <item> " << argc << endl;
    return 1;
  }
  PGconn *database = openDatabase();
  string command = string("select hideitem('") + argv[1] + "', '" + argv[2] + "', '" + argv[3] + "')";
  PGresult *result = PQexec(database, command.c_str());
  if (PQresultStatus(result) != PGRES_TUPLES_OK){

    cerr << "Error was " << PQerrorMessage(database) << endl;
    throwDatabaseException(command, result);
    return -1;
  } 
  PQclear(result);
  PQfinish(database); 
  return 0;
}

