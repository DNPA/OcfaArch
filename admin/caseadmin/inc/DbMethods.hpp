#include <libpq-fe.h>
#include <string>
PGconn *openDatabase();
void throwDatabaseException(std::string command, PGresult *inResult);
