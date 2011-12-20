#ifndef MIDASFE_HPP
#define MIDASFE_HPP
#include "MultiIndexDbSearcher.hpp"
#include "DbMetaSearcher.hpp"
#include <map>
//#include "EvidenceInfo.hpp"
/**
 * class that configures a lucene search depending on its input and 
 * prints out the results.
 *
 */
class IndexFrontEnd : public ocfa::OcfaObject {
public:
  
  /**
   * relict from the past. For now, only standard is
   * supported. HOwever, because the query language already supports
   * it, it is not a problem here.
   */
  enum SearchType  {STANDARD, WORDS};
  IndexFrontEnd(bool shouldInitialize);
  /**
   * virtual methods to be implemented by descendents.
   */
  virtual void printEvidenceInfo(const EvidenceInfo &inEvidenceInfo) = 0;
  virtual void printPeerEvidenceInfo(const EvidenceInfo &inPeerEvidenceInfo) = 0;
  //  virtual void initialize() = 0;
  /**
   * does the actual searching of the index and the retrieving of
   * results from the database.
   */
  virtual void doSearch();
  virtual void doDbMetaSearch();

  virtual void setFrom(int inFrom);
  virtual void setAmount(int inAmount);
  virtual void setSearchType(SearchType );
  virtual void printNavigation(int ){};
  virtual void printHeader(int ) {};
  /**
   * NYI should print the words that matched on a fuzzy query.
   */
  virtual void printFoundWords(std::map<std::string, int> &foundWords) = 0;
  MultiIndexDbSearcher &getIndexSearcher();
  DbMetaSearcher &getDbMetaSearcher() { return mDbMetaSearcher;}

private:

  MultiIndexDbSearcher mIndexSearcher;
  DbMetaSearcher mDbMetaSearcher;
  int mFrom;
  int mAmount;
  
 
  SearchType mSearchType;
};
#endif
