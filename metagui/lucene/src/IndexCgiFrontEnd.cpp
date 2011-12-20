#include "IndexCgiFrontEnd.hpp"
#include "LuceneSearcher.hpp"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
using namespace cgicc;
using namespace std;
string replaceIf(string inVictim, const char *old, const char *replacement);

/**
 * initializes the cgi, based upon the query string information.
 */
IndexCgiFrontEnd::IndexCgiFrontEnd(Cgicc &inCgicc) 
  : IndexFrontEnd(true), 
  mPrintedEvidenceCounter(0),
  mIndexSelectCgiArguments("")
{
  
  form_iterator searchIter = inCgicc.getElement("query");
  form_iterator categoryIter = inCgicc.getElement("category");
  form_iterator tblIter = inCgicc.getElement("tbl");
  form_iterator valIter = inCgicc.getElement("val");
  form_iterator likeIter = inCgicc.getElement("like");
  form_iterator digIter = inCgicc.getElement("dig");
  form_iterator srcIter = inCgicc.getElement("src");
  form_iterator itemIter = inCgicc.getElement("item");
  string query;
 
  selectIndexes(inCgicc);
  if (searchIter != inCgicc.getElements().end()){ 

    getIndexSearcher().setQuery(searchIter->getValue());    
  }
  if (categoryIter != inCgicc.getElements().end() && categoryIter->getValue() != ""){

    string sqlQuery = string("select metadataid from metacategories where categoryid = '") 
      + categoryIter->getValue() + "'";
    getIndexSearcher().setDbQuery(sqlQuery);
  }
  if ((tblIter != inCgicc.getElements().end()) &&(valIter != inCgicc.getElements().end()) ) {
    std::string digestsource="";
    std::string evidencesource="";
    std::string item="";
    std::string like="";
    if (digIter != inCgicc.getElements().end()) {
       digestsource=digIter->getValue();
    }
    if (srcIter != inCgicc.getElements().end()) {
       evidencesource=srcIter->getValue();
    }
    if (itemIter != inCgicc.getElements().end()) {
       item=itemIter->getValue();
    }
    if (likeIter != inCgicc.getElements().end()) {
       like=likeIter->getValue();
    }
    getDbMetaSearcher().setQuery(tblIter->getValue(),valIter->getValue(),like,digestsource,evidencesource,item);
  }
  form_iterator fromIter = inCgicc.getElement("from");

  if (fromIter != inCgicc.getElements().end() && fromIter->getValue() != ""){

    setFrom(fromIter->getIntegerValue());
    mPrintedEvidenceCounter = fromIter->getIntegerValue();
  }
  if (inCgicc.queryCheckbox("words")){

    setSearchType(IndexFrontEnd::WORDS);
  }
}
/**
 * @TODO very inefficient.
 *
 */
void IndexCgiFrontEnd::selectIndexes(Cgicc &inCgicc){

  // vector containing list of <indexkey>, <indexPath>
  vector<string> indexPaths;
  // vector containing all selected indexkeys
  vector<string> selectedIndexKeys;
  getIndexSearcher().getIndexPaths(indexPaths);
  for (unsigned int i = 0; i < indexPaths.size(); i+=2){

    if (inCgicc.queryCheckbox(indexPaths[i])){
      selectedIndexKeys.push_back(indexPaths[i]);
    }
  }
  if (selectedIndexKeys.size() > 0){

    getIndexSearcher().clearIndexSearchers();
    vector<string>::iterator iter;
    // create cgi argument for navigation purposes.
    for (iter = selectedIndexKeys.begin(); iter != selectedIndexKeys.end(); iter++){

      LuceneSearcher *searcher = new LuceneSearcher(*iter);
      getIndexSearcher().addIndexSearcher(&searcher);
      // add the selected indexdir to the set of cgiarguments that are used for printing navigation.
      mIndexSelectCgiArguments+="&";
      mIndexSelectCgiArguments+= *iter;
      mIndexSelectCgiArguments+="=on";
    }
  }
}


void IndexCgiFrontEnd::printHtmlDocumentStart(){
  
  cout << p().set("align", "center");
  cout << img().set("src", "../dungbeetle.jpg").set("align", "middle");
  cout << p();
  cout << h1("Keyword Search").set("align", "center");
  cout << form().set("accept-charset", "UTF-8")  << endl;   
  printIndexOptions();
  cout << p().set("align", "center") << endl;
  cout << "Keywords (" 
       << a("Help").set("href", "../helpsearch.html").set("target", "_blank") 
       << ")" << br() << endl;
  cout << input().set("type", "text").set("value", getIndexSearcher().getQuery())
  .set("name", "query").set("size", "40"); 
//  cout << " &nbsp; category:" ;
//  cout << input().set("type", "text").set("name", "category").set("size", "20");
  
  cout << "&nbsp; Word Search " << endl;
  cout << input().set("type", "checkbox").set("name", "words");
  cout << p();
  cout << input().set("type", "submit");
  cout << p();
  cout << form() << endl;  
  //cout << getIndexSearcher().getDbQuery() << endl;
}


void IndexCgiFrontEnd::printIndexOptions(){

  vector<string> indexPaths;
  getIndexSearcher().getIndexPaths(indexPaths);
  
  cout << table().set("align", "center").set("bgcolor", "white").set("cellpadding", "0").set("cellspacing", "0").set("width", "80%") << tr();
  unsigned int i;
  for (i = 0; i < indexPaths.size(); i+=2){

    string bgcolor = "white"; 
    if (i > 0 && (i % 8) == 0){

      cout << tr() << "\n" << tr();
    }
    
    if ((i % 4) == 2){

      bgcolor ="#d0f0d0";
    }
    cout << td().set("bgcolor", bgcolor);
    cout << input().set("type", "checkbox").set("name", indexPaths[i]).set("checked", "true");
    cout << td() << td().set("bgcolor", bgcolor);
    cout << indexPaths[i + 1] << td();
  }

  for (unsigned int j = (i % 8); j < 8; j+=2){

    string bgcolor = "white";
    if ((j % 4) == 2){

      bgcolor = "#d0f0d0";
    }
    cout << td().set("bgcolor", bgcolor);
    cout << td() << td().set("bgcolor", bgcolor);
    cout << td();
  }
  cout << tr() << table() << endl;  
  if (indexPaths.size() > 4){

    ostringstream jsstream("");
    jsstream << "allon(this.form, " << (indexPaths.size() / 2) <<  ")";
    cout << input().set("type", "button").set("value", "alles aan")
      .set("onClick", jsstream.str());
    jsstream.str("");
    jsstream << "alloff(this.form, " << (indexPaths.size() / 2) <<  ")";   
    cout << input().set("type", "button").set("value", "alles uit")
      .set("onClick", jsstream.str()) << endl;	 
    cout << "<script language=\"JavaScript1.2\" src=\"/allonoroff.js\"></script>" 
	 << endl; 
  } 
}

void IndexCgiFrontEnd::printEvidenceInfo(const EvidenceInfo &inEvidenceInfo){

  printEvidenceInfoColor(inEvidenceInfo, "");
  mPrintedEvidenceCounter++;

}

void IndexCgiFrontEnd::printEvidenceInfoColor(const EvidenceInfo &inEvidenceInfo,
					 string inStyle){

  string refArguments = "case=" + inEvidenceInfo.ecase
    + "&src=" + inEvidenceInfo.source + "&item=" +  inEvidenceInfo.item
    + "&id=" + inEvidenceInfo.evidenceRef;
    
  string viewDataRef = "view.cgi?mime=" + inEvidenceInfo.mimetype 
    + "&" + refArguments + "&charset=" + inEvidenceInfo.encoding;
    
  string metaDataRef = "evidence.cgi?" + refArguments;
  string viewNiceDataRef = "";
    
  if (inEvidenceInfo.mimetype.substr(0,4) == "text"){
    
      
    string queryWords = getIndexSearcher().getQuery();
    queryWords = replaceIf(queryWords, " and ", "+");

    queryWords = replaceIf(queryWords, " or ", "+");

    queryWords = replaceIf(queryWords, " AND ", "+");

    queryWords = replaceIf(queryWords, " OR ", "+");


    queryWords = replaceIf(queryWords, " ", "+");

    viewNiceDataRef =  "viewnice.cgi?mime=" + inEvidenceInfo.mimetype 
      + "&" + refArguments + "&charset=" + inEvidenceInfo.encoding 
      + string("&words=") + queryWords;
  }
    
  if (inEvidenceInfo.wasread){

    inStyle += " font-style: italic;";
  }
  cout << tr() << endl;
  cout << td().set("style", inStyle) << mPrintedEvidenceCounter << td()<< endl;
  cout << td().set("style", inStyle) << inEvidenceInfo.mScore << td() << endl;
  cout << td(inEvidenceInfo.source).set("style", inStyle );
  cout << td(inEvidenceInfo.item).set("style", inStyle);
  cout << td(a(inEvidenceInfo.location).set("href", viewDataRef));
  cout << td(a("meta").set("href", metaDataRef));
  if (viewNiceDataRef != ""){
    cout << td(a("view nice").set("href", viewNiceDataRef)); 
  }
  else {

    cout << td("");
  }
  cout << tr() << endl;
}


string replaceIf(string inVictim, const char *old, const char *replacement){

  string theOutput = inVictim;
  string::size_type index;
  
  index = theOutput.find(old);

  while (index != string::npos){

    theOutput = theOutput.replace(index, strlen(old), replacement);
    index = theOutput.find(old);
  }
  return theOutput;
  
}

void IndexCgiFrontEnd::printPeerEvidenceInfo(const EvidenceInfo &inEvidenceInfo){

  printEvidenceInfoColor(inEvidenceInfo, "color: gray");
}


void IndexCgiFrontEnd::printNavigation(int inFoundEvidences){
  
  if (inFoundEvidences > 30){
    
    cout << tr() << td().set("colspan", "5") << endl;
    string basicQuery = "lucene.cgi?query=" + getIndexSearcher().getQuery() + "&tbl=" + getDbMetaSearcher().getTable() + 
	    + "&like=" + getDbMetaSearcher().getLike() +
	    "&val=" + getDbMetaSearcher().getQuery() +  mIndexSelectCgiArguments  
      + "&from=";
    for (int x = 0; x < inFoundEvidences; x+=30){ 
      
      ostringstream href;
      href << basicQuery << x << endl;
      int lastNumber = inFoundEvidences > (x + 29) ? (x + 29) : inFoundEvidences;
      
      cout << a().set("href", href.str())
	   << string("(")  << x <<  " - " << lastNumber << ")" << a() ;
      cout << "&nbsp;";
    }
    cout << td();
    cout << tr()<< endl;
  }
  cout << table() << endl;

}

void IndexCgiFrontEnd::printHeader(int docFounds){

  cout << p() << "Found: " << docFounds << " hits" << p() << endl;
  cout << table() << endl;
  cout << tr() << td(b("nr")) << td(b("score")) << td(b("source"))
       << td(b("item")) << td(b("ref")) << td(b("metalink")) << td(b("nice view"))
       << tr() << endl;
}

  


void IndexCgiFrontEnd::printFoundWords(std::map<std::string, int> &foundWords){

  int colCount = 0;
  cout << "Words found: " << endl;
  cout << table();
  cout << tr();
  std::map<string, int>::iterator iter;
  for (iter = foundWords.begin(); iter != foundWords.end(); iter++){
    
    colCount++;
    if (colCount > 6){

      cout << tr() << tr() << endl;
      colCount = 0;
    }
    cout << td() << a().set("href", "lucene.cgi?query=" + iter->first + mIndexSelectCgiArguments) << iter->first 
	 << a() << iter->second  << td();
  }
  cout << tr() << table();

}

void IndexCgiFrontEnd::printHtmlDocumentClose(){
  
  cout << body() << html() << endl;

}


void IndexCgiFrontEnd::printErrorMessage(string inErrorMessage){

  
  cout << table();
  cout << tr();
  cout << td();
  cout << "<font color=\"#AA0000\">";
  cout << "An error has occurered. The error message is: " << endl;
  cout << "</font>" << endl;
  cout << td() << tr() << tr() << endl;
  cout << td() << inErrorMessage << td() << tr();
  cout << tr() << td() 
       << "<font color=\"#00AA00\">You can contact support to solve this problem.</font> " 
       << endl << td() << tr();  
}
