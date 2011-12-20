#ifndef PHP_OCFALUCENE_H
#define PHP_OCFALUCENE_H

#define PHP_OCFALUCENE_VERSION "0.1"
#define PHP_OCFALUCENE_EXTNAME "ocfalucene"


PHP_FUNCTION(doSearch);
PHP_FUNCTION(doDbSearch);
PHP_FUNCTION(testlib);
PHP_FUNCTION(dw_showIndexes);
PHP_FUNCTION(dw_getDbConnectionParams);
PHP_FUNCTION(dw_findMatchingTerms);
PHP_FUNCTION(dw_getOcfaParam);
//#int doSearch(const char *luceneQuery, char *outResults[], int size);

extern zend_module_entry ocfalucene_module_entry;

#define phpext_ocfalucene_ptr &ocfalucene_module_entry;



#endif
