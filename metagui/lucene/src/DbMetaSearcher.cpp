
#include <sstream>
#include <misc/OcfaException.hpp>
#include "DbMetaSearcher.hpp"
#include <pwd.h>
#include <sys/types.h>

using ocfa::misc::OcfaConfig;
using ocfa::misc::OcfaException;
/**
 *
 *
 */
DbMetaSearcher::DbMetaSearcher(): 
	OcfaObject("DbMetaSearcher", "misc"),
	mTable(""),
	mQuery(""),
	mConnection(0)
{
  string dbName = OcfaConfig::Instance()->getValue("storedbname", this);
  string dbhost = OcfaConfig::Instance()->getValue("storedbhost", this);
  string dbuser = OcfaConfig::Instance()->getValue("storedbuser", this);
  char connectionString[255];
  if (dbuser == ""){
		      struct passwd *pwent = getpwuid(getuid());
		      dbuser = pwent->pw_name;
		   }
  string dbpasswd = OcfaConfig::Instance()->getValue("storedbpasswd");
  sprintf(connectionString, "dbname=%s hostaddr=%s user=%s password=%s",
            dbName.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
            mConnection = PQconnectdb(connectionString);
  if (PQstatus(mConnection) != CONNECTION_OK) {
			      string errstr = "PgRepository initialization failed: "
				            + string(PQerrorMessage(mConnection));
			      printf("%s",errstr.c_str());
			      throw OcfaException(errstr, this);
  }
}


int DbMetaSearcher::searchDocuments(){
  std::string query;
  if (mLike == "") {
	  query=string("select count(xml) from meta") 
	                 + mTable 
			 + string(" where meta = '") 
			 + mQuery
			 + "'";
	  if ((mDig!="") || (mSrc!="")) {
             query += string(" and ");
	  }
  } else {
          query=string("select count(xml) from meta")
		  + mTable
		  + string(" where substr(meta,1,")
		  + mLike
		  + string(") = '")
		  + mQuery
		  + "'";
	  if ((mDig!="") || (mSrc!="")) {
            query += string(" and ");
	  }
  }
  if (mDig !="") {
     if (mDig == "unprocessed") {
       query += string(" xml not in (select xml from metadigestsource) ");
     } else {
       query += string(" xml in (select xml from metadigestsource where meta='") +
	      mDig +
	      string("') ");
     }
     if (mSrc != "") {
        query += " and ";
     }
  }
  if (mSrc != "") {
    query += string("xml in (select metadataid from metadatainfo where itemid in (select itemid from item where evidencesource='") +
             mSrc + string("' ");
    if (mItem != "") {
        query += string("and item='") + mItem + string("' ");
    }
    query += "))";
  }
  PGresult *pgres;
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK) {
     throw OcfaException(string("Problem with query:'")+query+string("'"),this);
  }
  if (PQgetvalue(pgres, 0, 0) == 0){
     throw OcfaException(string("No record returned for query:'")+query+string("'"),this); 
  }
  return atoi(PQgetvalue(pgres, 0, 0));
}

int DbMetaSearcher::retrieveEvidences(std::set<EvidenceInfo *, greater<EvidenceInfo *> > &outResults, int inFrom, int inAmount){
    char buffer[40];
    PGresult *pgres;
    sprintf(buffer,"%d",inFrom+inAmount);
    std::string query;
    if (mLike == "") {
       query=std::string("select xml from meta") + 
	                mTable + 
			string(" where meta = '") + 
			mQuery +
			string("' ");
       
       if ((mDig!="") || (mSrc!="")) {
           query += string(" and ");
       }
    } else {
      query=std::string("select xml from meta") +
	      mTable +
	      string(" where substr(meta,1,")
	      + mLike
	      + string(") = '")
	      + mQuery
	      + string("' ");
      if ((mDig!="") || (mSrc!="")) {
	                   query += string(" and ");
      }
    }

    if (mDig !="") {
     if (mDig == "unprocessed") {
       query += string(" xml not in (select xml from metadigestsource) ");
     } else {
     query += string(" xml in (select xml from metadigestsource where meta='") +
	      mDig +
	      string("') ");
     }
     if (mSrc != "") {
        query += " and ";
     }
    }
    if (mSrc != "") {
      query += string("xml in (select metadataid from metadatainfo where itemid in (select itemid from item where evidencesource='") +
             mSrc + string("' ");
      if (mItem != "") {
        query += string("and item='") + mItem + string("' ");
      }
      query += "))";
    }

    if (mLike == "") {
       query += string(" order by xml limit ") + string(buffer);
    } else {
       query += string(" order by meta limit ") + string(buffer);
    }

    
    pgres = PQexec(mConnection, query.c_str());
    if (PQresultStatus(pgres) != PGRES_TUPLES_OK) {
         throw OcfaException(string("Problem with query:'")+query+string("'"),this);
    }
    int count=0;
    for (int index=inFrom;index < PQntuples(pgres);index++) {
         string xmlid=(PQgetvalue(pgres, index, 0));
	 EvidenceInfo *newInfo = new EvidenceInfo();
	 getEvidenceInfo(xmlid, *newInfo,string("metadataid"));
         outResults.insert(newInfo);
	 count++;
    }
    return count;    
}
//FIXME: the folowing is shamelessly copied from midasResultGatherer, duplicate code requires a elegant alternative.
void DbMetaSearcher::getEvidenceInfo(string &evidenceIdString, 
				  EvidenceInfo &outEvidenceInfo, string inEvidenceField){


  string query = 
    string("select item.casename, item.evidencesource, item.item, metadatainfo.metadataid, metadatainfo.dataid, ")
    + "metadatainfo.evidence, metadatainfo.location from Metadatainfo inner join item on metadatainfo.itemid = item.itemid where "
    + inEvidenceField + " = '";
    query += evidenceIdString;

  query += "'";
  PGresult *pgres;
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){
    throw OcfaException(string("Problem with query:'")+query+string("'"),this);
  }
  if (PQntuples(pgres) > 0){
    outEvidenceInfo.ecase = PQgetvalue(pgres, 0, 0);
    outEvidenceInfo.source = PQgetvalue(pgres, 0, 1);
    outEvidenceInfo.item = PQgetvalue(pgres, 0, 2);
    outEvidenceInfo.xmlRef = PQgetvalue(pgres, 0, 3 );
    outEvidenceInfo.dataRef = PQgetvalue(pgres, 0, 4);
    outEvidenceInfo.evidenceRef = PQgetvalue(pgres, 0, 5);
    outEvidenceInfo.location = PQgetvalue(pgres, 0, 6);
    outEvidenceInfo.mimetype = getMimeType(outEvidenceInfo.xmlRef);
    outEvidenceInfo.encoding = getEncoding(outEvidenceInfo.xmlRef);
  }
  else {
    // bogus values in case the database is corrupt.
    outEvidenceInfo.xmlRef = -1;
    outEvidenceInfo.ecase = "";
    outEvidenceInfo.source = "";
    outEvidenceInfo.item = "";
    outEvidenceInfo.evidenceRef = "";
    outEvidenceInfo.dataRef = evidenceIdString;
    outEvidenceInfo.location = string("no Evidence found for ") 
      + evidenceIdString;
  }
}

string DbMetaSearcher::getMimeType(string xmlRef){

  return getMetaTableValue(xmlRef, "metamimetype", "text/plain");
}

string DbMetaSearcher::getEncoding(string xmlRef){

  return getMetaTableValue(xmlRef, "metacharset", "");
}

string DbMetaSearcher::getMetaTableValue(string xmlRef, string inTable, string inDefault){

  string query = "select meta from " + inTable + " where xml = \'" 
    + xmlRef  +"\'";
  PGresult *pgres;
  pgres = PQexec(mConnection, query.c_str());
  if (PQresultStatus(pgres) != PGRES_TUPLES_OK){
    throw OcfaException(string("Problem with query:'")+query+string("'"),this);
  } 

  if (PQntuples(pgres) > 0){

    return PQgetvalue(pgres, 0, 0);
  }
  else {

    return inDefault;
  }
}

