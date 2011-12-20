#ifndef LUCENESEARCHER_HPP
#define LUCENESEARCHER_HPP
#include <string>
#include <set>

#include "misc/OcfaConfig.hpp"
#include <misc/OcfaException.hpp>
#include <CLucene.h>
#include "DocInfo.hpp"

/**
 * class responsible for searching an particular index and retruning with the appropriate docids.
 *
 *
 */
class LuceneSearcher : public ocfa::OcfaObject {

public:
  
  LuceneSearcher(std::string inMidasDirKey = default_index_dirkey );  
  ~LuceneSearcher();
  /**
   * finds the document addresses. 
   */
  void findDocumentAddresses(const std::string inQuery, std::set<DocInfo, greater<DocInfo> > &outdocAddresses);
  
  /**
   * NYI and will probably be remove. Does a fuzzy search.
   */
  void findDocumentsFuzzy(std::string query,  std::set<DocInfo, greater<DocInfo> > &outDocAddresses, set<string> &outMatchinWords);
  
  
  
  std::string getIndexDirKey();
  /**
   * the standard key in the ocfaconfig file that is used to retrieve the directory where 
   * the indexes are stored.
   */
  const static std::string default_index_dirkey; 

  void findMatchingTerms(std::string inTerm, std::map<std::string, int> &ioTerms);

protected:
  void openIndex();
  
  LuceneSearcher(const LuceneSearcher& is):
	  ocfa::OcfaObject(is),
	  mLuceneSearcher(0),
	  mIndexDirKey("BOGUS")
  {
	  throw ocfa::misc::OcfaException("No copying allowed of LuceneSearcher",this);
  }
  const LuceneSearcher& operator=(const LuceneSearcher&) {
          throw ocfa::misc::OcfaException("No assignment allowed of LuceneSearcher",this);
	  return *this;
  }
private:
  lucene::search::IndexSearcher *mLuceneSearcher;
  std::string mIndexDirKey;

};
#endif
