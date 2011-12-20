
//  The Open Computer Forensics Architecture.
//  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <string>
#include <vector>
#include <iostream>
#include <pwd.h>
#include <sys/types.h>
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/Cgicc.h"
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ShowTree.hpp"

#include <misc/OcfaException.hpp>
#include <sstream>
using namespace cgicc;
using namespace ocfa::misc;
using namespace std;
ShowTree::ShowTree() : 
	OcfaObject("showtree", "metagui"),
	mConnection(0)
{

    OcfaConfig *config = OcfaConfig::Instance();
    openDatabase(config);
}

/**
 * gets all stuff from the config and uses it to connect
 * to the databse.
 *
 */
void ShowTree::openDatabase(OcfaConfig *config){
  
  string dbName = config->getValue("storedbname", this);
  string dbhost = config->getValue("storedbhost", this);
  string dbuser = config->getValue("storedbuser", this);
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
    
    string errstr = "PgRepository initialization failed: " + string(PQerrorMessage(mConnection));
    printf("%s",errstr.c_str());
    throw OcfaException(errstr, this);
  }
  
}

// TODO this should be better.
void ShowTree::showCases(){

  vector<string>  caseLinks;
  string query("select distinct casename from item");
  PGresult *pgres;
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK) {
    
    throwDatabaseException(query, pgres);
  }
  int count = PQntuples(pgres);
  for (int x = 0; x < count; x++){
    
    if (PQgetvalue(pgres, x, 0) == 0){
      
      ostringstream errMessage;
      errMessage << "getSuspendedMetaEntities: cannot find result at " << x;
      ocfaLog(LOG_ERR, errMessage.str());
      throw OcfaException("cannot find enough results with getSuspended metaentities", this);
    }
    string link= PQgetvalue(pgres, x, 0);
    link = string("<a href=\"showtree.cgi?casename=") + link + "\">" + link + "</a> ";
    caseLinks.push_back(link);
  } 
  cout << h1("Showing all cases");
  showList(caseLinks);
}

void ShowTree::showEvidenceSources(string &inCase){
  
  vector<string>  sourceLinks;
  string query("select distinct evidencesource from item");
  query += " where casename = \'" + inCase + "\' ";
  
  PGresult *pgres;
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK) {
    
    throwDatabaseException(query, pgres);
  }


  int count = PQntuples(pgres);
  for (int x = 0; x < count; x++){
    
    if (PQgetvalue(pgres, x, 0) == 0){
      
      ostringstream errMessage;
      errMessage << "PQgalue: cannot find result at " << x;
      ocfaLog(LOG_ERR, errMessage.str());
      throw OcfaException("cannot find enough results with getSuspended metaentities", this);
    }
    string link= PQgetvalue(pgres, x, 0);
    link = string("<a href=\"showtree.cgi?casename=") + inCase + "&evidencesource=" + link + "\">" + link + "</a>";
    sourceLinks.push_back(link);
  } 
  cout << h1("Showing all evidencesources");
  showList(sourceLinks);
 }

void ShowTree::showList(vector<std::string> links){

  vector<string>::iterator iter;

  cout << ul();
  for (iter = links.begin(); iter != links.end(); iter++){

    cout <<  li(*iter)<< endl;;

  }
  cout << ul();

}


void ShowTree::showItems(string inCase, string inSource){

  string query = 
    "select item from item where casename='" + inCase + "' and evidencesource ='"
    + inSource + "'";
  string done1 = "select item.item,evidence from MetaDataInfo inner join item on item.itemid = MetaDataInfo.itemid where casename ='" + inCase +"' and evidencesource='" + inSource +"' and length(evidence)=1";
  string bussy1 = "select item.item,evidence from MetaDataInfo inner join item on item.itemid = MetaDataInfo.itemid where casename ='" + inCase +"' and evidencesource='" + inSource +"' and length(evidence) < 11 and length(evidence)>1";
  string bussy2 = "select item.item,evidence from MetaDataInfo inner join item on item.itemid = MetaDataInfo.itemid where casename ='" + inCase +"' and evidencesource='" + inSource +"' and length(evidence) < 16 and length(evidence)>1";
  string bussy3 = "select item.item,evidence from MetaDataInfo inner join item on item.itemid = MetaDataInfo.itemid where casename ='" + inCase +"' and evidencesource='" + inSource +"' and length(evidence) < 21 and length(evidence)>1";
  vector<string> itemLinks;
  PGresult *pgres;
  int count = 0;
  cout << h1("Items for evidence source " + inSource + " in investigation " + inCase);
  for (int tryno=0; ((tryno < 4) && (count ==0));tryno++) { 
    switch (tryno) {
	    case 0: query=done1; break;
	    case 1: cout << h2("Incomplete run");
		    cout << h3("looking at 2nd level");
		    query=bussy1;
		    break;
	    case 2: cout << h3("looking at 3nd level");
		    query=bussy2;
		    break;
	    case 3: cout << h3("looking at 4nd level");
		    query=bussy3;
		    break;
    }
    pgres = PQexec(mConnection, query.c_str());
    if (PQresultStatus(pgres) != PGRES_TUPLES_OK) {
    
      throwDatabaseException(query, pgres);
    }
    count = PQntuples(pgres);
  }
  for (int x = 0; x < count; x++){
    
    if (PQgetvalue(pgres, x, 0) == 0){
      ostringstream errMessage;
      errMessage << "PQgalue: cannot find result at " << x;
      ocfaLog(LOG_ERR, errMessage.str());
      throw OcfaException("cannot find enough results with getSuspended metaentities", this);
    }
    string item= PQgetvalue(pgres, x, 0);
    string eid = PQgetvalue(pgres, x, 1);
    string link = string("<a href=\"/cgi-bin/evidence.cgi?case=") + inCase + "&src=" + inSource 
      + "&item=" + item + "&id=" +eid +"\">" + item + "(eid=" + eid + ")</a> ";
    itemLinks.push_back(link);
  }
  showList(itemLinks);
}


void ShowTree::throwDatabaseException(string command, PGresult *inResult){
  
  ocfaLog(LOG_ERR, string("error executing ").append(command));
  ocfaLog(LOG_ERR, string("error was " ).append(PQresStatus(PQresultStatus(inResult))));
  //ocfaLog(LOG_ERR, string("primary description: ")
  //  + PQresultErrorField(inResult, PG_DIAG_MESSAGE_PRIMARY));	
  //ocfaLog(LOG_ERR, string((PQresultErrorField(inResult, PG_DIAG_SQLSTATE))));
  throw OcfaException(string("Error: ").append(PQresStatus(PQresultStatus(inResult))), this);
}    

int main(int , char *[]){

  Cgicc cgi;
  cout << cgicc::HTTPHTMLHeader() << endl;
  cout << html() << body();
  ShowTree showTree;

  form_iterator caseIter = cgi.getElement("casename");
  if (caseIter != cgi.getElements().end() && caseIter->getValue() != ""){

    string caseId = caseIter->getValue();
    form_iterator sourceIter = cgi.getElement("evidencesource");
    if (sourceIter != cgi.getElements().end() && sourceIter->getValue() != ""){

      showTree.showItems(caseId, sourceIter->getValue());
    }
    else {
      
      showTree.showEvidenceSources(caseId);
    } 
  }
  else { 
    showTree.showCases();
  }
  cout << body() << html()<< endl;
}
