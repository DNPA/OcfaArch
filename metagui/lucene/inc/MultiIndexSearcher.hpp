#ifndef INDEXSEARCHER_HPP
#define INDEXSEARCHER_HPP
#include <set>
#include <map>
#include "misc/OcfaConfig.hpp"

#include "EvidenceInfo.hpp"
#include "LuceneSearcher.hpp"
#include "IndexResultGatherer.hpp"

/**
 * class that combines IndexSearcher and ResultGatherers to searhc an index.
 * @TODO use smart pointers to handle memory allocation.
 *
 * general idea is to initialize it, then call searchDocuments and then retrieveEvidences.
 */
class MultiIndexSearcher : public ocfa::OcfaObject {

public:
  MultiIndexSearcher(bool initIndexes);
  void findMatchingTerms(std::string inTerm, std::map<string, int> &outMatchingTerms);
  int searchDocuments();
  int searchFuzzyDocuments(set<string> &outMatchingWords);
  /**
   * retrieved evidence belonging to the found document ids.
   *
   */
  int  retrieveEvidences(std::set<EvidenceInfo *, EvidenceMostImportant  > &outEvidences, int inFrom, int inAmount);
  //void searchFuzzy(set<EvidenceInfo *> &outEvidences, int inFrom, int inAmount);
  
  void clearIndexSearchers();
  void removeIndexSearcher(int indexSearcher);
  /**
   * add indexsearcher. IndexSearcher assumes ownership of the indexSearcher.
   */
  void addIndexSearcher(LuceneSearcher **indexSearcher);
  void getIndexPaths(std::vector<std::string> &outIndexPaths);
  void setQuery(std::string query);
  std::string getQuery();
  std::set<DocInfo, greater<DocInfo> > &getFoundDocIds();
  IndexResultGatherer &getResultGatherer();
  std::set<DocInfo, greater<DocInfo> > mFoundDocIds;
  
private:
  IndexResultGatherer gatherer;
  vector<LuceneSearcher *> mIndexSearchers;
  std::string mQuery;
  const static std::string default_index_dirkey; 
};
#endif
