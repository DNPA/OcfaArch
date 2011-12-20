#ifndef MULTIINDEX_DBSEARCHER_HPP
#define MULTIINDEX_DBSEARCHER_HPP
#include "MultiIndexSearcher.hpp"

/**
 * class that searches several indexes for a query and filters the results by intersecting them with the 
 * results from a given query that returns a set of 
 *
 *
 */
class MultiIndexDbSearcher : public MultiIndexSearcher {
  
public:
  MultiIndexDbSearcher(bool inInitIndexes);
  void setDbQuery(string inDbQuery);
  std::string getDbQuery();
  std::set<DocInfo, greater<DocInfo> > &getDbResults();
  int searchDocuments();
  int searchDatabase();
protected:
  void fillDocInfoWithDb();
  void filterIndexResultsWithDb();
  
private:
  std::string mDbQuery;
  std::set<std::string> mdbResults;

};
#endif
