//#include <misc/OcfaException.hpp>
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include <misc/OcfaException.hpp>

#include "IndexCgiFrontEnd.hpp"
using namespace cgicc;
using namespace ocfa;
int main(int , char *[]){

  cout << cgicc::HTTPHTMLHeader() << endl;
  cout << html();
  cout << meta().set("http-equiv", "content-type")
     .set("content", "text/html; charset=UTF-8");
  //  cout << head() << base().set("href", baseRef) << title("interface op de wasstraat v1.0") << endl;
  cout << body().set("style", "font-family:arial").set("bgcolor", "#d0f0d0");

  try {
    Cgicc cgi;

    IndexCgiFrontEnd lucene(cgi);
    lucene.printHtmlDocumentStart();
    if (lucene.getIndexSearcher().getQuery() != "" || lucene.getIndexSearcher().getDbQuery() != ""){
      lucene.doSearch();
    } else if (lucene.getDbMetaSearcher().getQuery() != ""){
      lucene.doDbMetaSearch();
    } 
    lucene.printHtmlDocumentClose();

      
  } catch (ocfa::misc::OcfaException &e){
    e.logWhat();
    IndexCgiFrontEnd::printErrorMessage(e.what());
  } catch (string e){

    IndexCgiFrontEnd::printErrorMessage(e);
  }
  


}
