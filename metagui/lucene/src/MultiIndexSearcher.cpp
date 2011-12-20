
#include <sstream>
#include <misc/OcfaException.hpp>
#include "MultiIndexSearcher.hpp"
#include "DocInfo.hpp"
#include <misc/syslog_level.hpp>
using ocfa::misc::OcfaConfig;
using ocfa::misc::OcfaException;
using namespace ocfa::misc;
/**
 *
 *
 */
MultiIndexSearcher::MultiIndexSearcher(bool initIndex): 
	OcfaObject("MultiIndexSearcher", "misc"),
	mIndexSearchers(),
	mFoundDocIds(),
	mQuery("")
{
  if (initIndex){

    int extraIndexCount = 0;
    
    OcfaConfig *config = OcfaConfig::Instance();
    string countString = config->getValue("extraIndexCount");
    if (countString != ""){

      extraIndexCount = atoi(countString.c_str());
    }
    mIndexSearchers.push_back(new LuceneSearcher());
    for (int x = 0; x < extraIndexCount; x++){

      ostringstream  indexKey;
      indexKey << LuceneSearcher::default_index_dirkey << x;
      mIndexSearchers.push_back(new LuceneSearcher(indexKey.str()));
    }
  }
  getLogStream(LOG_DEBUG) << "end of constructor" << endl;
}


void MultiIndexSearcher::getIndexPaths(vector<string> &outPaths){

  int extraIndexCount = 0;
  
  OcfaConfig *config = OcfaConfig::Instance();
  string countString = config->getValue("extraIndexCount", this);
  if (countString != ""){

    extraIndexCount = atoi(countString.c_str());
  }
  outPaths.push_back(LuceneSearcher::default_index_dirkey);
  outPaths.push_back(config->getValue(LuceneSearcher::default_index_dirkey));
  for(int x = 0; x < extraIndexCount; x++){

    ostringstream  indexKey;
    indexKey << LuceneSearcher::default_index_dirkey << x;
    outPaths.push_back(indexKey.str());
    outPaths.push_back(config->getValue(indexKey.str()));
  }

}

void MultiIndexSearcher::findMatchingTerms(std::string inTerm, map<string, int> &outMatchingTerms){

  vector<LuceneSearcher *>::iterator iter;
  for (iter = mIndexSearchers.begin(); iter != mIndexSearchers.end(); iter++){

    (*iter)->findMatchingTerms(inTerm, outMatchingTerms);
  }
}

int MultiIndexSearcher::searchDocuments(){

  vector<LuceneSearcher *>::iterator iter;
  for (iter = mIndexSearchers.begin(); iter != mIndexSearchers.end(); iter++){

    (*iter)->findDocumentAddresses(mQuery, mFoundDocIds);
    //(*iter)->closeIndex();
  }
  return mFoundDocIds.size();
}

void MultiIndexSearcher::setQuery(std::string inQuery){

  mQuery = inQuery;
}

int MultiIndexSearcher::retrieveEvidences(std::set<EvidenceInfo *, EvidenceMostImportant > &outEvidences, int inFrom, int inAmount){

  return gatherer.getResults(mFoundDocIds, inFrom, inAmount, outEvidences);

}

int  MultiIndexSearcher::searchFuzzyDocuments(set<string> &outMatchingWords){

  vector<LuceneSearcher *>::iterator iter;
  
  for (iter = mIndexSearchers.begin(); iter != mIndexSearchers.end(); iter++){

    (*iter)->findDocumentsFuzzy(mQuery, mFoundDocIds, outMatchingWords);
  }
  return mFoundDocIds.size();
}

void MultiIndexSearcher::clearIndexSearchers(){

  vector<LuceneSearcher *>::iterator iter;
  for (iter = mIndexSearchers.begin(); iter != mIndexSearchers.end(); iter++){

    delete (*iter);    
  }
  mIndexSearchers.clear();
}

/**
 * 
 */
void MultiIndexSearcher::removeIndexSearcher(int indexSearcherNr){

  LuceneSearcher *toBeRemoved = mIndexSearchers[indexSearcherNr];
  mIndexSearchers.erase(mIndexSearchers.begin() + indexSearcherNr);
  delete toBeRemoved;
}

void MultiIndexSearcher::addIndexSearcher(LuceneSearcher **newIndexSearcher){

  if (newIndexSearcher == 0 || *newIndexSearcher == 0){

    throw OcfaException("empty indexSearcher added", this);
  }

  mIndexSearchers.push_back(*newIndexSearcher);
}



string MultiIndexSearcher::getQuery(){

  return mQuery;
}

IndexResultGatherer &MultiIndexSearcher::getResultGatherer(){

  return gatherer;
}

std::set<DocInfo, greater<DocInfo> > &MultiIndexSearcher::getFoundDocIds(){

  return mFoundDocIds;
}

