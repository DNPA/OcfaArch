#include "LuceneSearcher.hpp"
#include <CLucene/analysis/standard/StandardAnalyzer.h>
#include <misc/syslog_level.hpp>
#include <stdio.h>
using namespace std;
//using ocfa::misc::OcfaConfig;
using namespace lucene::index;
using namespace lucene::util;
using namespace lucene::queryParser;
using namespace lucene::document;
using namespace lucene::search;
using lucene::util::Misc;
using namespace ocfa::misc;
CL_NS_USE(analysis::standard)

const string LuceneSearcher::default_index_dirkey = "indexdir";


LuceneSearcher::LuceneSearcher(string inIndexDirKey) 
  : OcfaObject("LuceneSearcher", "indexui"), 
    mIndexDirKey(inIndexDirKey), mLuceneSearcher(0){
  
  //openIndex();
}

LuceneSearcher::~LuceneSearcher(){

  if( mLuceneSearcher != 0){
    
    delete mLuceneSearcher;
    mLuceneSearcher = 0;
  }
}

/**
 * find matching terms.
 * @param inQuery the term with a wildcard for which the matching terms should be found.
 * @param 
 *
 */
void LuceneSearcher::findMatchingTerms(string inTerm, map<string, int> &ioTerms){

  if (mLuceneSearcher == 0){

    openIndex();
  }
  set<string> matchingTerms;
  TCHAR buffer[200];
  char buffer2[200];
  STRCPY_AtoT(buffer, inTerm.c_str(), 200);   
  Term *theTerm = new Term(_T("content"), buffer, false);
  lucene::analysis::Analyzer *theAnalyzer = 
    new lucene::analysis::standard::StandardAnalyzer();
  set<string>::iterator iter;
  STRCPY_TtoA(buffer2, theTerm->text(), 200);
  //cerr << "findMatchingTerms for the words: " << buffer2 << endl;
  //cerr << "findMatchingTerms getReader is " << mLuceneSearcher->getReader() << endl;
  //cerr << "getDirectgory is " << mLuceneSearcher->getReader()->getDirectory() << endl;
  TermEnum *tEnum = mLuceneSearcher->getReader()->terms();
  //cerr << "Termenum is " << tEnum << endl;
  //cerr << "numdocs is " << mLuceneSearcher->getReader()->numDocs() << endl;


  WildcardTermEnum termEnum(mLuceneSearcher->getReader(), theTerm); 
  //cerr << "Wildcardenum succeeded" << endl;
  while(termEnum.next()){
       
    Term *term = termEnum.term(false);
    STRCPY_TtoA(buffer2, term->text(), 200);
    //cerr << "adding term " << term->text() << endl;
    ioTerms[string(buffer2)] += termEnum.docFreq();
    } 
  //cerr << "done findMatchingTerms" << endl; 
  _CLDECDELETE(theTerm);
}



void LuceneSearcher::findDocumentAddresses(std::string inQuery,  set<DocInfo, greater<DocInfo> > &outDocAddresses){

  TCHAR tline[200];
  TCHAR * buf;
  if (mLuceneSearcher == 0){

    openIndex();
  }
  getLogStream(LOG_DEBUG) << "ASCI QUERY is " << inQuery << endl;
  STRCPY_AtoT(tline, inQuery.c_str(), 200);
  // getLogStream(LOG_DEBUG) << "tline is " << _wideToChar(tline) << endl;
  
  //fflush(0);
  lucene::analysis::Analyzer *theAnalyzer = new lucene::analysis::standard::StandardAnalyzer();
  //char test[100];
  //STRCPY_TtoA(test, tline, 100);
  //getLogStream(LOG_DEBUG) << "querytransformed is " << test << endl;
  lucene::search::Query *q = QueryParser::parse(tline, _T("content"), theAnalyzer);
  //FIXME assert(q != 0);
  buf = q->toString(_T("content"));
  _tprintf(_T("Searching for: %s\n\n"), buf);
  getLogStream(LOG_DEBUG) << "indexSearcher is " << mLuceneSearcher << endl;
  lucene::search::Hits *h = mLuceneSearcher->search(q);
  char dataID[20]; 
  char metadataID[20];
  getLogStream(LOG_DEBUG) << " found " << h->length() << "hits " << endl;
  for (int i = 0; i < h->length(); i++){

     DocInfo docInfo;
    getLogStream(LOG_DEBUG) << "found " << h->doc(i).get(_T("docid")) << endl; 
    Document *doc = &(h->doc(i));
    STRCPY_TtoA(dataID, doc->get(_T("docid")), 20);
    if (doc->get(_T("metadataid")) != 0){

      STRCPY_TtoA(metadataID, doc->get(_T("metadataid")), 20);
      
    }
    docInfo.docid = dataID;
    docInfo.score = h->score(i);
    docInfo.metadataid = metadataID;
    //cout << "Docinfo.docid is " << dataID << " docinfo.metadataid = " << metadataID << endl; 
    outDocAddresses.insert(docInfo);
  }
  //delete [] tline;
  delete theAnalyzer;
}

void LuceneSearcher::findDocumentsFuzzy(std::string ,  set< DocInfo, greater<DocInfo> > &, set<string> &){
  
  cerr << "fuzzy search NYI" << endl;
}

void LuceneSearcher::openIndex(){
  
  OcfaConfig *config = OcfaConfig::Instance();
  
  //return index   
  string indexDir = config->getValue(mIndexDirKey, this);
  getLogStream(LOG_DEBUG) << "opening index with " << indexDir << endl;
  mLuceneSearcher = new lucene::search::IndexSearcher(indexDir.c_str());
  getLogStream(LOG_DEBUG) << "opened index  " << indexDir << endl;
  
}


string LuceneSearcher::getIndexDirKey(){

  return mIndexDirKey;
}
