#ifndef DBMETASEARCHER_HPP
#define DBMETASEARCHER_HPP
#include <set>

#include "misc/OcfaConfig.hpp"

#include "EvidenceInfo.hpp"
#include "MultiIndexSearcher.hpp"
#include <libpq-fe.h>

/**
 * class that combines IndexSearcher and ResultGatherers to searhc an index.
 * @TODO use smart pointers to handle memory allocation.
 *
 * general idea is to initialize it, then call searchDocuments and then retrieveEvidences.
 */
class DbMetaSearcher : public ocfa::OcfaObject {

public:
  DbMetaSearcher();
  int searchDocuments();
  int  retrieveEvidences(std::set<EvidenceInfo *, greater<EvidenceInfo *> > &outEvidences, int inFrom, int inAmount);
  void setQuery(std::string tbl,std::string query,std::string like,std::string dsrc,std::string esrc,std::string item) {
	  mTable=tbl; mQuery=query;mLike=like;mDig=dsrc;mSrc=esrc;mItem=item;
  }
  std::string getQuery() {return mQuery;}
  std::string getTable() {return mTable;}
  std::string getLike() {return mLike;}
private:
  void getEvidenceInfo(std::string &evidenceIdString,
		          EvidenceInfo &outEvidenceInfo, std::string inEvidenceField = "dataid");
  std::string getMimeType(std::string xmlRef);
  std::string getEncoding(std::string xmlRef); 
  std::string getMetaTableValue(std::string xmlRef, std::string inTable, std::string inDefault);
  std::string mTable;
  std::string mQuery;
  std::string mLike;
  std::string mDig;
  std::string mSrc;
  std::string mItem;
  PGconn *mConnection;
};
#endif
