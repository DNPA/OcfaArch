#include "MultiIndexDbSearcher.hpp"
#include <functional>
#include <libpq-fe.h>
#include <misc/syslog_level.hpp>

using namespace std;
using namespace ocfa::misc;

class MemberOf : public unary_function<string &, bool> {
public:
  MemberOf(const std::set<string> &inCollection) : mCollection(inCollection){

  }
  bool operator()(const string &inItem){


    set<string>::const_iterator iterator;
    //for (iterator = mCollection.begin(); iterator != mCollection.end();iterator++){

    //cout << *iterator << " ";
    //}
    //cout << endl;

    return binary_search(mCollection.begin(), mCollection.end(), inItem);
  }
private:
  const std::set<string> &mCollection;
};

class FillFilteredSet {

  set<DocInfo, greater<DocInfo> > &mFilteredSet;
  MemberOf &mFilter;
  int mCount;
public:
  FillFilteredSet(std::set<DocInfo, greater <DocInfo> > &inFilteredSet, MemberOf &inFilter) 
    : mFilteredSet(inFilteredSet), mFilter(inFilter), mCount(0) {

  }
  void operator()(const  DocInfo &inInfo){

    if (mFilter(inInfo.metadataid) && mCount < 1000){

      //cout << "inserting " << inInfo.metadataid << endl;
      mFilteredSet.insert(inInfo);
      mCount++;
    }

  }
};

MultiIndexDbSearcher::MultiIndexDbSearcher(bool initIndexes)
  : MultiIndexSearcher(initIndexes) {

}


int MultiIndexDbSearcher::searchDocuments(){

  int foundByQuery  = 0;
  int foundByIndex = 0;

  if (getQuery() != ""){

    foundByIndex = MultiIndexSearcher::searchDocuments();
  }
  if (mDbQuery != ""){

    foundByQuery = searchDatabase();
  }
  if (mDbQuery == ""){

    return foundByIndex;
  }
  else if (getQuery() == ""){

    /**
     * fill ddocuments with database info
     */
    
    fillDocInfoWithDb();
    return foundByQuery;
  }
  else {

    filterIndexResultsWithDb();
    return getFoundDocIds().size();
  }
}


/**
 * when no query was given the database results are simply converted to 
 * the set of found DocIds
 *
 */
void MultiIndexDbSearcher::fillDocInfoWithDb(){

  set<std::string>::iterator iter;
  for (iter = mdbResults.begin(); iter != mdbResults.end(); iter++){
    
    DocInfo doc;
    doc.metadataid = *iter;
    doc.score = 1.0;
    getFoundDocIds().insert(doc);
  }
  
}

int MultiIndexDbSearcher::searchDatabase(){

  PGconn *theConnection = getResultGatherer().getConnection();
  PGresult *pgres;
  pgres = PQexec(theConnection, mDbQuery.c_str());
  
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){
    
    getResultGatherer().throwDatabaseException(mDbQuery, pgres);
  }
  else {
    
    int rowCount = PQntuples(pgres);
    
    getLogStream(LOG_DEBUG) << "MultiIndexDbSearcher::searchDatabase RowCount is " << rowCount << endl;
    for (int x = 0; x < rowCount; x++){

      string metaDocId = PQgetvalue(pgres, x, 0);      
      
      mdbResults.insert(metaDocId);
    }
    PQclear(pgres);
    return rowCount;
  }
}


void MultiIndexDbSearcher::filterIndexResultsWithDb(){

  set<DocInfo, greater<DocInfo> > mFilteredResults;
  MemberOf theMemberOf(mdbResults);
  FillFilteredSet theFillFilteredSet(mFilteredResults, theMemberOf);
  set<DocInfo, greater<DocInfo> >::iterator iterbegin;
  set<DocInfo, greater<DocInfo> >::iterator iterend;
  iterbegin = getFoundDocIds().begin();
  iterend = getFoundDocIds().end();
  


  for_each(iterbegin, iterend, theFillFilteredSet);
  getFoundDocIds().clear();
  set<DocInfo, greater<DocInfo> >::iterator iter;
  for (iter = mFilteredResults.begin(); iter != mFilteredResults.end(); iter++){

    //DocInfo info = *iter;
    getFoundDocIds().insert(*iter);
  }
	     
}


void MultiIndexDbSearcher::setDbQuery(string inDbQuery){

  mDbQuery = inDbQuery;
}

std::string MultiIndexDbSearcher::getDbQuery(){

  return mDbQuery;
}
