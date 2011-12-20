#include "MetaFilter.hpp"
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "misc/ScalarMetaValue.hpp"
#include "misc/ArrayMetaValue.hpp"
#include "misc/TableMetaValue.hpp"
MetaFilter::MetaFilter(PGconn *con,std::string config):ocfa::OcfaObject("MetaFilter","dsm"),mConnection(con) {
   ifstream dsmconf(config.c_str());
   if (! dsmconf) {
      
   }
   std::string line;
   while(dsmconf) {
      getline(dsmconf,line);
      if ((line.size() > 0) && (line[0] != '#')) {
         boost::char_separator<char> sep(" ");
	 boost::tokenizer< boost::char_separator<char> > tok(line,sep);
	 boost::tokenizer< boost::char_separator<char> >::iterator iter=tok.begin();
	 if (iter != tok.end()) {
            std::string metaname=*iter;
	    std::transform(metaname.begin(),metaname.end(),metaname.begin(),::tolower);
	    std::replace(metaname.begin(),metaname.end(),'-','_');
	    mTableInfo[metaname]=new std::pair<ocfa::misc::meta_type,ocfa::misc::Scalar::scalar_type >(
	       ocfa::misc::META_SCALAR,
	       ocfa::misc::Scalar::SCL_INVALID
	    );
	 }
      }
   }
}
bool MetaFilter::operator()(std::string metaname,ocfa::misc::MetaValue *val) {
   std::pair<ocfa::misc::meta_type,ocfa::misc::Scalar::scalar_type > *typeinfo=mTableInfo[metaname];
   if (typeinfo == 0)
      return false;
   if (typeinfo->second == ocfa::misc::Scalar::SCL_INVALID) { 
      getLogStream(ocfa::misc::LOG_INFO) << "row" << metaname << " not seen before, trying to create\n";
      createTableFromMeta(metaname,val); 
      return true;
   }  else {
      //getLogStream(ocfa::misc::LOG_WARNING) << "row" << metaname << " already added\n"; 
      //we have seen this one before, lets assume everything is fine
      return true;
   }
   return false;
}
void MetaFilter::createScalarTableFromMeta(std::string name, ocfa::misc::Scalar val) {

  stringstream strstrValtype;
  strstrValtype << "varchar(" << DEFAULT_ROW_VARCHAR_SIZE << ")";
  std::string valtype = strstrValtype.str();

  bool timesource=false;
  switch (val.getType()) {
     case ocfa::misc::Scalar::SCL_INT: 
         getLogStream(ocfa::misc::LOG_INFO) << "row" << name << " is to be an bigint table\n";
         valtype="bigint"; 
	 break;
     case ocfa::misc::Scalar::SCL_FLOAT:
         getLogStream(ocfa::misc::LOG_INFO) << "row" << name << " is to be a float table\n";
         valtype="double";
	 break;
     case ocfa::misc::Scalar::SCL_DATETIME:
         getLogStream(ocfa::misc::LOG_INFO) << "row" << name << " is to be a timestamp table\n";
         valtype="timestamp";
         timesource=true;
	 break;
     case ocfa::misc::Scalar::SCL_STRING:
         getLogStream(ocfa::misc::LOG_INFO) << "row" << name << " is to be a string table\n";
	 valtype="varchar(" + boost::lexical_cast<std::string>((int) DEFAULT_ROW_VARCHAR_SIZE)  + ")";
	 break;
     case ocfa::misc::Scalar::SCL_INVALID:
     default:
         getLogStream(ocfa::misc::LOG_INFO) << "row" << name << " is to be a default (string) table\n";
         valtype="varchar(" + boost::lexical_cast<std::string>((int) DEFAULT_ROW_VARCHAR_SIZE) + ")";
         break;
  }
  mTableInfo[name]->second=val.getType();
  std::string sub1="";
  if (timesource) {
      sub1=",timesource varchar("+ boost::lexical_cast<std::string>((int) DEFAULT_ROW_VARCHAR_SIZE) +")";
  }
  std::string query="create table row" + name + " (id serial,metadataid int,meta " + valtype + sub1 +  ");";
  PGresult *res=PQexec(mConnection,query.c_str());
  PQclear(res);
  return;
}

void MetaFilter::createTableTableFromMeta(std::string name,ocfa::misc::TableMetaValue *val) {
   if (val->size() > 0) {
      ocfa::misc::ArrayMetaValue *arval=dynamic_cast<ocfa::misc::ArrayMetaValue *>(val->getValueAt(0));
      std::string query="create table tbl" + name + "(id serial, metadataid int";
      for (unsigned int ColNo=0;ColNo < val->getColCount();ColNo++) {
         query += ",m" + val->getColName(ColNo) + " "; 
         ocfa::misc::ScalarMetaValue *sclval=dynamic_cast<ocfa::misc::ScalarMetaValue *>(arval->getValueAt(ColNo));
	 switch (sclval->asScalar().getType()) {
           case ocfa::misc::Scalar::SCL_INT:
	      query += "int";
	      break;
	   case ocfa::misc::Scalar::SCL_FLOAT:
	      query += "double";
	      break;
	   case ocfa::misc::Scalar::SCL_DATETIME:
	      query += "timestamp,timesrc" + val->getColName(ColNo) + " varchar(" + boost::lexical_cast<std::string>((int) DEFAULT_ROW_VARCHAR_SIZE) + "255)";
	      break;
	   case ocfa::misc::Scalar::SCL_STRING:
	      query += "varchar(" + boost::lexical_cast<std::string>((int) DEFAULT_ROW_VARCHAR_SIZE) + ")";
	      break;
	   case ocfa::misc::Scalar::SCL_INVALID:
	   default:
	      query += "varchar(" + boost::lexical_cast<std::string>((int) DEFAULT_ROW_VARCHAR_SIZE) + ")";
	      break;
	 }
      }
      query += ");";
      PGresult *res=PQexec(mConnection,query.c_str());
      PQclear(res);
      mTableInfo[name]->second=ocfa::misc::Scalar::SCL_STRING;
   } 
}

void MetaFilter::createTableFromMeta(std::string name, ocfa::misc::MetaValue *val) {
   switch (val->getType()) {
     case ocfa::misc::META_SCALAR:
           getLogStream(ocfa::misc::LOG_INFO) << "row" << name << " is to be a scalar table\n";
           createScalarTableFromMeta(name,dynamic_cast<ocfa::misc::ScalarMetaValue *>(val)->asScalar());
	   break;
     case ocfa::misc::META_ARRAY:
           getLogStream(ocfa::misc::LOG_INFO) << "row" << name << " is to be an array table\n";
           createScalarTableFromMeta(name,dynamic_cast<ocfa::misc::ScalarMetaValue *>(val->getValueAt(0))->asScalar());
	   break;
     case ocfa::misc::META_TABLE:
           getLogStream(ocfa::misc::LOG_INFO) << "tbl" << name << " is to be a table table\n";
           createTableTableFromMeta(name,dynamic_cast<ocfa::misc::TableMetaValue *>(val));
           break;
   }
   mTableInfo[name]->first=val->getType();
}
