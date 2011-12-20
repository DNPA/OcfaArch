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

//#include "rulelist.hpp" //RJM:CODEREVIEW Dont use the monolyth
#include "csv/RuleList.hpp" //RJM:CODEREVIEW Use the one file per vlass headers.
#include <boost/tokenizer.hpp> //RJM:CODEREVIEW this was in the monolyth header
#include "Parser.hpp"
typedef boost::tokenizer<boost::escaped_list_separator<char> > escTokenizer; //RJM:CODEREVIEW this was in the monolyth header
#define MAX_RULELIST_COLLS 8	//RJM:CODEREVIEW this was in the monolyth header
//#define MAX_RULELIST_ROWS 254   //RJM:CODEREVIEW this was in the monolyth header, not used may be removed
#define IDX_TABLENAME         0 //RJM:CODEREVIEW this was in the monolyth header
#define IDX_NAMESPACE         1 //RJM:CODEREVIEW this was in the monolyth header
#define IDX_RULETYPE          2 //RJM:CODEREVIEW this was in the monolyth header
#define IDX_RULEVALUE         3 //RJM:CODEREVIEW this was in the monolyth header
#define IDX_PROCESSMODULE     4 //RJM:CODEREVIEW this was in the monolyth header
#define IDX_ACTION            5 //RJM:CODEREVIEW this was in the monolyth header
#define IDX_TARGET            6 //RJM:CODEREVIEW this was in the monolyth header
#define IDX_ARGUMENTS         7 //RJM:CODEREVIEW this was in the monolyth header
#include <iostream>
#include <fstream>

using namespace std;
using namespace router;

using namespace ocfa::misc;
using namespace ocfa::evidence;

/****************************************************************************/
/**** RuleList class implementation *****************************************/
/****************************************************************************/

/*Stuff used to make Rulelist singleton work */
RuleList* RuleList::_instance = 0;

RuleList* RuleList::getInstance(){
   if(_instance == 0) {
     _instance  = new RuleList;
     try {
     _instance->Load();
     } catch(OcfaException &e) {
       e.logWhat();
       throw OcfaException("Failed to load rulelist", NULL);
     }
   }
   

   return _instance;
}

RuleList::~RuleList() {
  Table *tmpTable;
  map<string, Table *>::iterator piTablemap;
  for(piTablemap = mTableMap.begin() ; piTablemap != mTableMap.end() ; ++piTablemap) {
    tmpTable = piTablemap->second;
    if(tmpTable != NULL) delete tmpTable;
  }
  mTableMap.clear();
  delete mFaDefault;
}


void RuleList::Load() throw (ocfa::OcfaException) {
  //get filename of rulelist from config file
  string fileName = OcfaConfig::Instance()->getValue("rulelist");
  if (fileName == ""){
    throw OcfaException("No initial rulelist found in config under name router.rulelist", this);
  }
  if (fileName[0] != '/'){

    string ocfaRoot = OcfaConfig::Instance()->getValue("ocfaroot");
    if (ocfaRoot == ""){

      throw OcfaException("No ocfaroot value found", this);
    }
    fileName = ocfaRoot +  "/" + fileName;
  }
  
  switch (Parser::identifyVersion(fileName)){
    case 1:
       ReadRuleFile(fileName);
       mRuleListVersion = 1;
       break;
    case 2:
       ReadVersion2RuleFile(fileName);
       mRuleListVersion = 2;
       break;
    default:
       /* what is appropriate action here? */
       ReadRuleFile(fileName);
       mRuleListVersion = 1;
       break;
  }
  
  string faType = OcfaConfig::Instance()->getValue("defaultfatype");
  string faValue = OcfaConfig::Instance()->getValue("defaultfavalue");
  if(faType == "" || faValue == "") {
    throw OcfaException("No default final action type and value found", this);
  }
  else {
     if(faType == "FACT_COMMIT")
       mFaDefault = new FinalActionCommit(faValue);
     else
       if(faType == "FACT_FORWARD")
         mFaDefault = new FinalActionForward(faValue);
       else
         if(faType == "FACT_SUSPEND")
	   mFaDefault = new FinalActionSuspend(faValue);
	 else
	   throw OcfaException("Invalid Final Action Type specified in conf file", this);
	 
  }
}

void RuleList::ReadRuleFile(string fileName) throw(ocfa::OcfaException) {
   //the escape character is a backslash
   //the separation string: use a "," ";" or "|"
   boost::escaped_list_separator<char> sepField("\\",",;|","\"\'");
   
   std::string szLine="";
   std::string RuleLine[MAX_RULELIST_COLLS];
   
   //try to open Rulelistfile as inputstream
   std::ifstream fstrRuleFile(fileName.c_str());
   if(!fstrRuleFile)
      throw OcfaException("Cannot open Rulelist file", this);
      
   int columCount=0;
   int rowCount=0;   
   //itterate trought all lines from the rulefile
   while(!fstrRuleFile.eof()) {
      getline(fstrRuleFile, szLine);
      rowCount++;
      if(szLine[0] != 0x23) { //if first char is not a "#" (not a comment line)
         escTokenizer tokField(szLine,sepField);
	 columCount=0;
         for (escTokenizer::iterator tkitrFields = tokField.begin();tkitrFields != tokField.end(); ++tkitrFields) {
	     if(columCount++ > MAX_RULELIST_COLLS) {
	        getLogStream(LOG_ERR) << "Rulelist violation on line " << rowCount << endl;
	        throw OcfaException("Illegal Rulelist Layout: Maximum number of collums violation", this);
	     }	
	     else {
	        RuleLine[columCount-1] = *tkitrFields;
	     }
         } // end for each token in Rulefileline
	 if(columCount < MAX_RULELIST_COLLS && columCount != 0) {
	    getLogStream(LOG_ERR) << "Rulelist violation on line " << rowCount << endl;
	    throw OcfaException("Illegal Rulelist Layout: Missing Collums violation",this);
	 }
	 if(columCount != 0)
	   processRuleLine(RuleLine);
      } // if not a comment line
   } //end while all lines from RuleFile

}

void RuleList::processRuleLine(string *ruleLine) throw(ocfa::OcfaException) {

Rule *tmpRule;
Table *tbl;
map<string, Table *>::iterator piTableMap = mTableMap.find(ruleLine[IDX_TABLENAME]);
     getLogStream(LOG_INFO) << "Processing rule: table=" << ruleLine[IDX_TABLENAME] << " ruletype=" << ruleLine[IDX_RULETYPE] << " namespace=" << ruleLine[IDX_NAMESPACE] <<
                           " module=" << ruleLine[IDX_PROCESSMODULE] << " val=" << ruleLine[IDX_RULEVALUE] << "target=" << ruleLine[IDX_TARGET] << endl;
     if(piTableMap == mTableMap.end()) {
       //not existing table, so create
       tbl = new Table();
       mTableMap[ruleLine[IDX_TABLENAME]] = tbl;
     }
     else {
     }  
     if(ruleLine[IDX_RULETYPE] != DONT_CARE) {
        tmpRule = new Rule(ruleLine[IDX_RULETYPE]);
	tmpRule->addLimit(new LimitNamespace(ruleLine[IDX_NAMESPACE]));
        tmpRule->addLimit(new LimitModule(ruleLine[IDX_PROCESSMODULE]));
	tmpRule->addLimit(new LimitScalar(ruleLine[IDX_RULEVALUE], Limit::MT_SUB)); //RJM:CODEREVIEW moved enum to Limit class
     }
     else {
          tmpRule = new Rule(ruleLine[IDX_PROCESSMODULE], Rule::RDT_MODULEINSTANCE); //Moved enum to Rule class
	  tmpRule->addLimit(new LimitNamespace(ruleLine[IDX_NAMESPACE]));
          /* do nothing with rule type and value fields, because rule 
          type not defined */
     }   //  action_type, flags, arguments )
     tmpRule->setFinalAction(ruleLine[IDX_ACTION],ruleLine[IDX_TARGET],ruleLine[IDX_ARGUMENTS]);
     tbl = NULL;
     tbl = mTableMap[ruleLine[IDX_TABLENAME]];
     if(tbl != NULL) 
       tbl->addRule(tmpRule);
     else
       throw OcfaException("No table found to add rule to");
}

FinalAction* RuleList::parsePacketVector(packet_vector_type &pv, string TableName,std::list<SideAction> *sideactions) {
  
  Table   *tmpTable = mTableMap[TableName];
  
  FinalAction *faResult=0; 
  faResult=tmpTable->parsePacketVector(pv,sideactions,mTableMap,0);  //RJM:CODEREVIEW we should also pass mTableMap so the table can do jumps if needed.
  if(faResult == NULL) {
       getLogStream(LOG_NOTICE) << "No legal final action found, use default final action from conf file" << endl;
       mSetMetaFacet("rulelistissue", ocfa::misc::Scalar("nofinalactionfound","LATIN1"));
       return mFaDefault;
  }
  else {      
     return faResult;
  }          
 
} //end RuleList::processRuleLine

//RJM::CODEREVIEW Moved the other classes into their own files.



/* FIVES-PH: parse packet vector for rulelist v2 */
FinalAction* RuleList::parsePacketVector(packet_vector_type &pv, Evidence* ev){
  FinalAction *faResult=0; 
  faResult=mRules.parsePacketVector(pv, ev);     /* FIVES-PH: */

  if(faResult == 0) {
       getLogStream(LOG_NOTICE) << "No legal final action found, use default final action from conf file" << endl;
       mSetMetaFacet("rulelistissue", ocfa::misc::Scalar("nofinalactionfound","LATIN1"));
       return mFaDefault;
  } else {      
     return faResult;
  }
}

/* FIVES-PH: read version 2 rule list */
void RuleList::ReadVersion2RuleFile(string fileName) throw(ocfa::OcfaException) {
       getLogStream(ocfa::misc::LOG_DEBUG) << "Reading rulelist " << fileName << endl;

       // check access for the file
       if(access(fileName.c_str(), R_OK))
               throw OcfaException("Cannot open Rulelist file", this);

       Parser p(fileName);
       if (p.parse())
               throw OcfaException("Parse failed", this);

       bool labels_ok = true;
       for (unsigned int i=0; i<mUsedLabels.size(); i++){
               if (mLabelMap.find(mUsedLabels[i]) == mLabelMap.end()){
                       labels_ok = false;
                       getLogStream(LOG_EMERG) << "Jump to undefined label: " << mUsedLabels[i] << endl;
               }
       }
       if (!labels_ok)
               throw ocfa::OcfaException("Jump to undefined label(s)", this);
}

