#ifndef MIDASRESULTGATHER_HPP
#define MIDASRESULTGATHER_HPP
//#include "IndexSearcher.hpp"
#include <set>

#include <string>
#include <libpq-fe.h>
#include <misc/OcfaConfig.hpp>
#include <misc/OcfaException.hpp>
#include "DocInfo.hpp"
#include "EvidenceInfo.hpp"

/**
 * class that collects more information about results given a set of dataid's and scores.
 *
 *
 */

class IndexResultGatherer : public ocfa::OcfaObject {
public:
  IndexResultGatherer();
  ~IndexResultGatherer();
  /**
   * Fills the set of evidences belonging to 
   * @param inDataIds the set of docinfos for which the further information must be retrieved. 
   * 
   * @param inFrom the number from which the further information must be retrieved.
   * @param inResultAmount the amount of docinfo's for which the information must be retrieved. 
   * @param inEvidenceField the field in the metadatainfo table which is 
   *   reference by the doid in the docinfo.
   */
  int getResults(std::set<DocInfo, std::greater<DocInfo> > &inDataIds, int inFrom, 
		 int resultAmount, 
		 std::set <EvidenceInfo *, EvidenceMostImportant > &outResults);

  /**
   * returns the connection the IndexResultGatherer still has responsibility over the connection.
   *
   */
  PGconn *getConnection();
  void throwDatabaseException(std::string command, PGresult *inResult);

protected:

  void setMetaTableNames();
  bool tableExists(std::string inTableName);
  

  /**
   * gets one evidence info.
   */
  void getEvidenceInfo(std::string &evidenceIdString, 
		       EvidenceInfo &outEvidenceInfo, std::string inEvidenceField = "dataid");
  void openDatabase(ocfa::misc::OcfaConfig *config);

  std::string getMimeType(std::string xmlRef);
  std::string getEncoding(std::string xmlRef);
  std::string getMetaTableValue(std::string xmlRef, std::string inTable, std::string inDefault);
  IndexResultGatherer(const IndexResultGatherer& mrg):
	  OcfaObject(mrg),
	  mConnection(0)
  {
	  throw ocfa::misc::OcfaException("No copying allowed for IndexResultGatherer",this);
  }
  const IndexResultGatherer& operator=(const IndexResultGatherer&) {
	  throw ocfa::misc::OcfaException("No assignment allowed for IndexResultGatherer",this);
	  return *this;
  }
private:
  /** 
   * connection to the ocfa datavase.
   */
  PGconn *mConnection;
  std::string mXmlField;
  std::string mMimeTypeTable;
  std::string mEncodingTable;
};
 




#endif
