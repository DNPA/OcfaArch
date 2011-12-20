#include "QueryTool.hpp"
#include <set>
#include <string.h>
#include <pwd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/CgiUtils.h"
#include "IndexResultGatherer.hpp"
using namespace cgicc;
using namespace std;
using namespace ocfa::misc;
static void printHeader(Cgicc &cgicc);
//static void getEvidenceInfo(string &evidenceInfo, EvidenceInfo &outEvidenceInfo);
static string replaceIf(string inVictim, const char *old, const char *replacement);


int main(int argc, char *argv[]){

  Cgicc cgi;
  printHeader(cgi);
  form_iterator searchIter = cgi.getElement("query");
  if (searchIter != cgi.getElements().end() 
      && searchIter->getValue() != ""){
    
    QueryTool tool;
    tool.setFromAndTo(cgi);
    std::string query = searchIter->getValue();
    tool.setQuery(query);
    //    tool.setFromAndTo(cgi);
    tool.showResults(cgi);    
  }
}

QueryTool::QueryTool() : OcfaObject("ui", "querytool"), mFrom(0){
}


void QueryTool::setFromAndTo(Cgicc &cgi){

  form_iterator searchIter = cgi.getElement("from");
  if (searchIter != cgi.getElements().end() && searchIter->getValue() != ""){

    mFrom = searchIter->getIntegerValue();
  }
}
/**
bool QueryTool::shouldPrint(string &docId){

  counter++;
  if (counter > from && counter <= from + maxDocCount){

    return true;
  }
  else {

    return false;
  }
}
*/
void QueryTool::showResults(Cgicc &cgi){

  
  openDatabase();
  PGresult *pgres;
  getLogStream(LOG_WARNING) << "query is " << query << endl;
  pgres = PQexec(mConnection, query.c_str());
  
  
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

    throwDatabaseException(query, pgres);
  }
  else {

    std::set<DocInfo, greater<DocInfo> > allDocInfos;
    int rowCount = PQntuples(pgres);
    DocInfo info;
    for (int x = 0; x < rowCount; x++){
      
      info.docid = PQgetvalue(pgres, x, 0);
      info.score = 1.0;
      allDocInfos.insert(info);
    }
    PQclear(pgres);
    IndexResultGatherer gatherer;
    cout << p() << allDocInfos.size() << "documenten gevonden " << p() ;
    std::set<EvidenceInfo *, EvidenceMostImportant > foundEvidenceSet;
    gatherer.getResults(allDocInfos, mFrom,mFrom + 30, foundEvidenceSet);
    cout << table() << endl;
    std::set<EvidenceInfo *, EvidenceMostImportant >::iterator iter;
     mPrintedEvidenceCounter = mFrom;
    for (iter = foundEvidenceSet.begin(); iter != foundEvidenceSet.end(); iter++){

      printResultInfo(**iter);
      mPrintedEvidenceCounter++;
    }
    printNavigation(allDocInfos.size()); 
    
  }  
}
  // open database and index. 
/**
 * retrieves information from config to open the database.
 *
 */
void QueryTool::openDatabase(){
  
  OcfaConfig *config = OcfaConfig::Instance();
  string dbName = config->getValue("storedbname");
  string dbhost = config->getValue("storedbhost");
  string dbuser = config->getValue("storedbuser");
  char connectionString[255];
  if (dbuser == ""){
    
    struct passwd *pwent = getpwuid(getuid());
    dbuser = pwent->pw_name;
  }
  string dbpasswd = config->getValue("storedbpasswd");
  sprintf(connectionString, "dbname=%s hostaddr=%s user=%s password=%s", 
	  dbName.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
  
  mConnection = PQconnectdb(connectionString);
  if (PQstatus(mConnection) != CONNECTION_OK) {
    
    string errstr = "PgRepository initialization failed: " 
      + string(PQerrorMessage(mConnection));
    printf("%s",errstr.c_str());
    throw ocfa::misc::OcfaException(errstr);  
  }
}

void QueryTool::printResultInfo(const EvidenceInfo &inEvidenceInfo){

  string refArguments = "case=" + inEvidenceInfo.ecase
    + "&src=" + inEvidenceInfo.source + "&item=" +  inEvidenceInfo.item
    + "&id=" + inEvidenceInfo.evidenceRef;
    
  string viewDataRef = "view.cgi?mime=" + inEvidenceInfo.mimetype 
    + "&" + refArguments + "&charset=" + inEvidenceInfo.encoding;
    
  string metaDataRef = "evidence.cgi?" + refArguments;
    
  cout << tr() << endl;
  cout << td() << mPrintedEvidenceCounter << td()<< endl;
  cout << td() << inEvidenceInfo.mScore << td() << endl;
  cout << td(inEvidenceInfo.source);
  cout << td(inEvidenceInfo.item);
  cout << td(a(inEvidenceInfo.location).set("href", viewDataRef));
  cout << td(a("meta").set("href", metaDataRef));

  cout << td("");
  cout << tr() << endl;
}




void printHeader(Cgicc &cgi){
  
  form_iterator searchIter = cgi.getElement("query");
  string searchText;
  if(searchIter != cgi.getElements().end()){

    searchText = searchIter->getValue();
  }
  else {

    searchText = "";
  }
  // get hostname for base href 
  cout << cgicc::HTTPHTMLHeader() << endl;
  cout << html();
  cout << meta().set("http-equiv", "content-type")
    .set("content", "text/html; charset=UTF-8");
  cout << head() << title("interface op de wasstraat v0.3") << endl;
  cout << body();
  cout << p().set("align", "center");
  
  cout << h1("SqlqueryTool in de wasstraat").set("align", "center");
  cout << p() << b("Warning:") << " This tool is a class example of how "
       << b("not") << " to present your database to an enduser over the web. "
       << "It is possible to completely corrupt, destroy, or hack this database "
       << "with this interface. " << p()
       << "We trust you. Please know what you're doing" << p() <<  endl;
  cout << form().set("accept-charset", "UTF-8")  << endl;   
  cout << p().set("align", "center") << endl;
  cout << "Voer een sqlQuery in";
  cout << textarea().set("name", "query").set("cols", "70");
  if (searchIter != cgi.getElements().end() 
      && searchIter->getValue() != ""){
    
    cout << searchIter->getValue();
  }

  cout <<  textarea() << endl; 



  // cout << input().set("type", "text").set("value", searchText)
  //  .set("name", "query").set("size", "40");
  cout << br() << input().set("name", "submit").set("type", "submit");
  cout << p();
  cout << form() << endl;  
}

void QueryTool::setQuery(string inQuery){

  query =inQuery;
}
string replaceIf(string inVictim, const char *old, const char *replacement){

  string theOutput = inVictim;
  string::size_type index;
  //cerr << "searching for " << old << endl;
  
  index = theOutput.find(old);
  //cerr << "index is " << index;

  while (index != string::npos){

    //cerr << "replacing " << endl; 
    theOutput = theOutput.replace(index, strlen(old), replacement);
    index = theOutput.find(old);
  }
  return theOutput;
  
}


void QueryTool::throwDatabaseException(string inQuery, PGresult *inResult){

  getLogStream(LOG_ERR) << "throwing up on " << inQuery << endl;

  //ocfaLog(LOG_ERR, string("primary description: ")
  //  + PQresultErrorField(inResult, PG_DIAG_MESSAGE_PRIMARY));	
  //ocfaLog(LOG_ERR, string((PQresultErrorField(inResult, PG_DIAG_SQLSTATE))));
  throw ocfa::misc::OcfaException(string("Error: ").append(PQresStatus(PQresultStatus(inResult))));
}    

void QueryTool::printNavigation(int inFoundEvidences){
  
  if (inFoundEvidences > 30){
    
    cout << tr() << td().set("colspan", "5") << endl;
    string basicQuery = "querytool.cgi?query=" + form_urlencode(getQuery())   
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
