#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "../inc/lucenewrapper.h"
#include "../../inc/MultiIndexDbSearcher.hpp"
#include "../../inc/DocInfo.hpp"

using namespace ocfa::misc;
#define MAX_BUFFER_LENGTH 1024
static function_entry ocfalucene_functions[] = {
  PHP_FE(doSearch, NULL)
  PHP_FE(doDbSearch, NULL)
  PHP_FE(testlib, NULL)
  PHP_FE(dw_showIndexes, NULL)
  PHP_FE(dw_getDbConnectionParams, NULL)
  PHP_FE(dw_findMatchingTerms, NULL)
  PHP_FE(dw_getOcfaParam, NULL)
  {NULL, NULL, NULL}
};

zend_module_entry ocfalucene_module_entry = {
  #if ZEND_MODULE_API_NO >= 20010901
      STANDARD_MODULE_HEADER,
  #endif
      PHP_OCFALUCENE_EXTNAME,
      ocfalucene_functions,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
  #if ZEND_MODULE_API_NO >= 20010901
      PHP_OCFALUCENE_VERSION,
  #endif
      STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_OCFALUCENE
ZEND_GET_MODULE(ocfalucene)
#endif
     
void initializeIndexer(MultiIndexDbSearcher &indexer, HashTable *srcHash);
  
PHP_FUNCTION(doSearch)
{
  char *query;

   long maxResults;
   long queryLength;
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl",
			     &query, &queryLength,  &maxResults) 
       == FAILURE) {
   }
   
   MultiIndexSearcher multiIndexSearcher(true);
   string indexQuery(query);
   multiIndexSearcher.setQuery(indexQuery);
   int resultCount = multiIndexSearcher.searchDocuments();
   array_init(return_value);
    
   int maxIndex = maxResults;
   int x = 0;
   const set<DocInfo, greater<DocInfo > > foundDocs = multiIndexSearcher.getFoundDocIds();
   set<DocInfo, greater<DocInfo> >::const_iterator iter;
   for (x = 0, iter = foundDocs.begin(); 
	x < maxIndex && iter != foundDocs.end(); 
	x++, iter++){
     
     string docid = iter->docid;
     int score = (int)((iter->score) * 100);
     add_next_index_long(return_value,  score);
     add_next_index_string(return_value, (char *)docid.c_str(), 1);
     
   }
   return;
 }



PHP_FUNCTION(dw_findMatchingTerms){

  char buffer[MAX_BUFFER_LENGTH];
  char *beginWord;
  char *theCase;
  long beginWordLength;
  long theCaseLength;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
			    &beginWord, &beginWordLength,  &theCase, &theCaseLength) 
      == FAILURE) {
  }  
  snprintf(buffer, MAX_BUFFER_LENGTH, "OCFACASE=%s", theCase);
  putenv(buffer);

  MultiIndexSearcher multiIndexSearcher(true);
  array_init(return_value);
  map<std::string, int> result;
  multiIndexSearcher.findMatchingTerms(beginWord, result);
  map<std::string, int>::iterator iter;
  for (iter = result.begin(); iter != result.end(); iter++){
    
    zval *tempVal;
    ALLOC_INIT_ZVAL(tempVal);
    array_init(tempVal);
    strncpy(buffer, iter->first.c_str(), MAX_BUFFER_LENGTH); 
    add_assoc_string(tempVal, "word", buffer, 1);
    add_assoc_long(tempVal, "cnt", iter->second);
    add_next_index_zval(return_value, tempVal);
  }


}


PHP_FUNCTION(doDbSearch)
{
  char *query;
  char *dbQuery;
  char *theCase;
  long maxResults;
  long fromResult;
  long queryLength;
  long dbQueryLength;
  long theCaseLength;
  zval *sources;
  //  putenv("OCFACASE=joep");
  //  cout << "going to parse" << endl;
  //  cout <<  " tOCFACASE= " << getenv("OCFACASE") << "\n" <<  endl;
  //cout <<  " OCFAROOT= " << getenv("OCFAROOT") << "\n" << endl;;
  //cerr <<  " cOCFACASE= " << getenv("OCFACASE") << "\n" <<  endl;
  //err <<  " cOCFAROOT= " << getenv("OCFAROOT") << "\n" << endl;;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsa",
			    &query, &queryLength, &dbQuery, &dbQueryLength, 
			    &fromResult, &maxResults, &theCase, &theCaseLength, &sources) 
      == FAILURE) {
  
    RETURN_NULL();
  }
  //cout << "querylength is "  << queryLength << endl;
  //cout << "dbQueryLength " << dbQueryLength << endl;
  char buffer[MAX_BUFFER_LENGTH];
  //cerr << "thecase is " << theCase << endl;
  snprintf(buffer, MAX_BUFFER_LENGTH, "OCFACASE=%s", theCase);
  putenv(buffer);
  try {
    
    HashTable *srcHash = Z_ARRVAL_P(sources);
    
    MultiIndexDbSearcher multiIndexDbSearcher(true);
    if (zend_hash_num_elements(srcHash) > 0){

      initializeIndexer(multiIndexDbSearcher, srcHash);
    }
    string indexQuery(query);
    string databaseQuery(dbQuery);
    multiIndexDbSearcher.setQuery(indexQuery);
    multiIndexDbSearcher.setDbQuery(databaseQuery);
    int resultCount = multiIndexDbSearcher.searchDocuments();
    array_init(return_value);
    add_next_index_long(return_value, (long)resultCount); 
    set<EvidenceInfo *, EvidenceMostImportant > outEvidences;
    multiIndexDbSearcher.retrieveEvidences(outEvidences, (int) fromResult, (int) maxResults);
    set<EvidenceInfo *, EvidenceMostImportant >::iterator iter;
    //set<DocInfo, greater<DocInfo> >::const_iterator iter;
    //  ALLOC_INIT_ZVAL(resultVal);
    for (iter = outEvidences.begin(); iter != outEvidences.end(); iter++){
      
      EvidenceInfo *info  = *iter;
      zval *tempVal;
      ALLOC_INIT_ZVAL(tempVal);
      array_init(tempVal);
      long score = (long)(info->mScore * 100);
      //zend_print("score is %ld\n",score);
      add_assoc_long(tempVal, "score", score);
      
      strncpy(buffer, info->xmlRef.c_str(), MAX_BUFFER_LENGTH);
      
      add_assoc_string(tempVal, "id", buffer, 1);
      
      strncpy(buffer, info->dataRef.c_str(), MAX_BUFFER_LENGTH);    
      
      add_assoc_string(tempVal, "dataref", buffer, 1);
      
      strncpy(buffer, info->location.c_str(), MAX_BUFFER_LENGTH); 
      add_assoc_string(tempVal, "path", buffer, 1);
      strncpy(buffer, info->ecase.c_str(), MAX_BUFFER_LENGTH);
      add_assoc_string(tempVal, "case", buffer, 1);
      strncpy(buffer, info->source.c_str(), MAX_BUFFER_LENGTH);
      add_assoc_string(tempVal, "source", buffer, 1);
      strncpy(buffer, info->xmlRef.c_str(), MAX_BUFFER_LENGTH);
      add_assoc_string(tempVal, "item", buffer, 1);
      string showArguments = "mime=" + info->mimetype + "&case=" + info->ecase
	+ "&src=" + info->source + "&item=" + info->item + "&id=" 
	+ info->evidenceRef + "&encoding=" + info->encoding;
      snprintf(buffer, MAX_BUFFER_LENGTH, "/cgi-bin/view.cgi?%s", showArguments.c_str());
      add_assoc_string(tempVal, "datalink", buffer, 1);
      snprintf(buffer, MAX_BUFFER_LENGTH, "/cgi-bin/evidence.cgi?%s", showArguments.c_str());
      add_assoc_string(tempVal, "metalink", buffer, 1);
      snprintf(buffer, MAX_BUFFER_LENGTH, "/cgi-bin/viewnice.cgi?%s&words=%s", showArguments.c_str(), 
	       query);
      add_assoc_string(tempVal, "nicelink", buffer, 1);

      strncpy(buffer, info->mimetype.c_str(), MAX_BUFFER_LENGTH);
      add_assoc_string(tempVal, "type", buffer, 1);
      add_assoc_bool(tempVal, "isread", info->wasread);

      add_next_index_zval(return_value,  tempVal);     
      
      delete info;
    }
  } catch (...){

    cerr << "an exceptiono has occurred " <<endl;
  }
  return;
}

void initializeIndexer(MultiIndexDbSearcher &indexer, HashTable *srcHash){

  zval **data;
  indexer.clearIndexSearchers();
  HashPosition pointer;
  for (zend_hash_internal_pointer_reset_ex(srcHash, &pointer);
       zend_hash_get_current_data_ex(srcHash, (void **) &data, &pointer) == SUCCESS;
       zend_hash_move_forward_ex(srcHash, &pointer)){

    if (Z_TYPE_PP(data) == IS_STRING){

      string indexKey(Z_STRVAL_PP(data));
      LuceneSearcher *searcher = new LuceneSearcher(indexKey);
      indexer.addIndexSearcher(&searcher);
    }
  }
}
    

  
  




PHP_FUNCTION(testlib){

  printf("ocfacaseis %s\n", getenv("OCFACASE"));
  printf("ocfarootis %s\n", getenv("OCFAROOT"));
  printf("user is %s\n", getenv("USER"));
  fprintf(stderr, "ocfacaseis %s\n", getenv("OCFACASE"));
  fprintf(stderr, "ocfarootis %s\n", getenv("OCFAROOT"));
  fprintf(stderr, "user is %s\n", getenv("USER"));
  fprintf(stderr, "querystring is %s\n", getenv("QUERY_STRING"));
  //printf("ocfacaseis %s\n", getenv("OCFACASE"));
  
  FILE *test = fopen("/tmp/test", "w");
  fprintf(test, "blahba");
  fclose(test);

  RETURN_STRING(getenv("USER"), 1);
  


/*   cerr << "ocfacase is " << getenv("OCFACASE") << endl; */
/*   cerr << "ocfaroot is " << getenv("OCFAROOT") << endl; */
/*   cout << "ocfacase is " << getenv("OCFACASE") << endl; */
/*   cout << "ocfaroot is " << getenv("OCFAROOT") << endl; */
/*   cerr << "user = " << getenv("USER") << endl; */
/*   cout << "user is " << getenv("USER") << endl; */
}

PHP_FUNCTION(dw_showIndexes){

  char buffer[MAX_BUFFER_LENGTH];
  char *theCase;
  long theCaseLength;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
			    &theCase, &theCaseLength) 
      == FAILURE) {
  
    RETURN_NULL();
  }

  snprintf(buffer, MAX_BUFFER_LENGTH, "OCFACASE=%s", theCase);
  putenv(buffer);
  OcfaConfig *config = OcfaConfig::Instance();
  array_init(return_value);
  vector<string> outPaths;
  MultiIndexSearcher searcher(true);
  searcher.getIndexPaths(outPaths);

  for (unsigned int x = 0; x < outPaths.size(); x+=2){

    zval *tempVal;
    ALLOC_INIT_ZVAL(tempVal);
    array_init(tempVal);

    strncpy(buffer, outPaths[x].c_str(), MAX_BUFFER_LENGTH);
    add_assoc_string(tempVal, "sourceref", buffer, 1);
    strncpy(buffer, outPaths[x + 1].c_str(), MAX_BUFFER_LENGTH);
    add_assoc_string(tempVal, "source", buffer, 1);

    add_next_index_zval(return_value, tempVal);
  }
  
}

PHP_FUNCTION(dw_getDbConnectionParams){

  char buffer[MAX_BUFFER_LENGTH];
  char *theCase;
  long theCaseLength;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
			    &theCase, &theCaseLength) 
      == FAILURE) {
  
    RETURN_NULL();
  }
  cerr << "the case is " << theCase << endl;
  snprintf(buffer, MAX_BUFFER_LENGTH, "OCFACASE=%s", theCase);
  putenv(buffer);

  try {
    OcfaConfig *config = OcfaConfig::Instance();
    array_init(return_value);
    strncpy(buffer, config->getValue("storedbhost").c_str(), MAX_BUFFER_LENGTH);
    add_assoc_string(return_value, "dbhost", buffer, 1);
    
    strncpy(buffer, config->getValue("storedbname").c_str(), MAX_BUFFER_LENGTH);
    add_assoc_string(return_value, "dbname", buffer, 1);
    
    strncpy(buffer, config->getValue("storedbuser").c_str(), MAX_BUFFER_LENGTH);
    add_assoc_string(return_value, "dbuser", buffer, 1);
    
    strncpy(buffer, config->getValue("storedbpasswd").c_str(), MAX_BUFFER_LENGTH);
    add_assoc_string(return_value, "dbpasswd", buffer, 1);
  } catch (OcfaException &e){
  
    cerr << e.what() << endl;
    RETURN_NULL();
  }
}


PHP_FUNCTION(dw_getOcfaParam){

  char buffer[MAX_BUFFER_LENGTH];
  char *theCase;
  long theCaseLength;
  char *paramName;
  long paramLength;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
			    &paramName, &paramLength, &theCase,
			    &theCaseLength) 
      == FAILURE) {
  
    RETURN_NULL();
  }
  // cerr << "the case is " << theCase << endl;
  snprintf(buffer, MAX_BUFFER_LENGTH, "OCFACASE=%s", theCase);
  putenv(buffer);

  try {
    OcfaConfig *config = OcfaConfig::Instance();
    strncpy(buffer, config->getValue(paramName).c_str(), MAX_BUFFER_LENGTH);
    RETURN_STRING(buffer, 1);
  } catch(OcfaException e){

    cerr << e.what() << endl;
    RETURN_NULL();
  }
}
