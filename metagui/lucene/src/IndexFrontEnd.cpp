#include "IndexFrontEnd.hpp"

IndexFrontEnd::IndexFrontEnd(bool shouldInitialize)
  : OcfaObject("IndexFrontEnd", "indexui"), mIndexSearcher(shouldInitialize), mFrom(0), mAmount(30)
  , mSearchType(STANDARD){

}

void IndexFrontEnd::doDbMetaSearch() {
  set<EvidenceInfo *, greater<EvidenceInfo *> > foundEvidenceSet;
  int foundEvidences = getDbMetaSearcher().searchDocuments();
  printHeader(foundEvidences);
  getDbMetaSearcher().retrieveEvidences(foundEvidenceSet, mFrom, mAmount);
  set<EvidenceInfo *>::iterator iter;
  for (iter = foundEvidenceSet.begin(); iter != foundEvidenceSet.end(); iter++){
     printEvidenceInfo(**iter);
  }
  printNavigation(foundEvidences);
  for (iter = foundEvidenceSet.begin(); iter != foundEvidenceSet.end(); iter++){
    delete *iter;
  }
  return;
}

/**
 * does the general template of searching. Assumes everythnig has been set alreadu.
 *
 */

void IndexFrontEnd::doSearch(){
  
  int docIdsFound;
  set<EvidenceInfo *, EvidenceMostImportant > foundEvidenceSet;
  map<std::string, int> foundWords;
  if (mSearchType == WORDS){

    getIndexSearcher().findMatchingTerms(mIndexSearcher.getQuery(), foundWords);
    printFoundWords(foundWords);
    
  } else {

    
    docIdsFound = getIndexSearcher().searchDocuments();
    printHeader(docIdsFound);
    
    int foundEvidences = getIndexSearcher().retrieveEvidences(foundEvidenceSet, mFrom, mAmount);
    
    set<EvidenceInfo *>::iterator iter;
    for (iter = foundEvidenceSet.begin(); iter != foundEvidenceSet.end(); iter++){
      
      printEvidenceInfo(**iter);
    }
    printNavigation(foundEvidences); 
    for (iter = foundEvidenceSet.begin(); iter != foundEvidenceSet.end(); iter++){

      delete *iter;
      //   *iter = 0;
    }
  }

}


void IndexFrontEnd::setFrom(int inFrom){

  mFrom = inFrom;
}

void IndexFrontEnd::setAmount(int inAmount){

  mAmount = inAmount;
}

MultiIndexDbSearcher &IndexFrontEnd::getIndexSearcher(){

  return mIndexSearcher;
}

void IndexFrontEnd::setSearchType(SearchType inType){

  mSearchType = inType;
}
