#ifndef QUERYTOOL_HPP
#define QUERYTOOL_HPP


#include "cgicc/Cgicc.h"
#include "misc/OcfaConfig.hpp"
#include <libpq-fe.h>
#include "EvidenceInfo.hpp"

/**
 * Hacked tool that shows the results of a query. The first field of
 * the result should be a metadataid.
 *
 */
class QueryTool  : public ocfa::OcfaObject {
  
 public:
  QueryTool();
  void showResults(cgicc::Cgicc &cgi);
  void setFromAndTo(cgicc::Cgicc &cgi);
  void setQuery(std::string inQuery);
  std::string getQuery(){return query;}
  
protected:
  /**
   * opens the databaseconnection, which is stored in db.  
   *
   * @param config a washconfig. This is used to retrieve connection
   * information.
   */
  void openDatabase();
  void printResultInfo(const EvidenceInfo &inEvidenceInfo);
   //  void getDigestPeers(string &evidenceIdString, vector< string > ** peers);
  void throwDatabaseException(std::string command, PGresult *inResult);
  void printNavigation(int inFoundEvidences);
 private:
  int mFrom;
  int maxDocCount;
  std::string query;
  PGconn *mConnection;
  int mPrintedEvidenceCounter;
};
#endif
