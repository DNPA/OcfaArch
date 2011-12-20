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

#ifndef __RULELIST_HPP__
#define __RULELIST_HPP__

#include <ocfa.hpp>
#include <map>
#include <string>
#include "../common/FinalAction.hpp"
#include "../common/FinalActionJump.hpp"
#include "../common/Table.hpp"
#include "../common/SetMetaFacet.hpp"
#include "evidence/Evidence.hpp"

namespace router {
/**
 * A singleton class containing the rulelist 
 */
class RuleList : public ocfa::OcfaObject {  //RJM:CODEREVIEW it might be apropriate to see how both the CSV and the XML version
	                                    //can be defined to use a comon baseclass, and maybe even dynamic loading of either
					    //the csv or xml based rulelist module.
  
  /**************************************************************************/
  public:
    /**
     * Destructor
     */
    ~RuleList() ;
     
    /**
     * getInstance Method
     * The call to this method tries to load a new Rulelist from a predefined 
     * location, defined in the configuration file. On success, the new rulelist
     * gets active, otherwise, the old one is still valid.
     * @return RuleList* Pointer to the RuleList singleton
     */
    static RuleList*    getInstance();
    void                initSetMetaFacet(SetMetaFacet &smf) {
         mSetMetaFacet = smf;
    };
    FinalAction*        parsePacketVector(packet_vector_type &pv, std::string TableName,std::list<SideAction> *sideactions=0); 
                                                                                          //instead of a FinalAction !!
    std::string        getMatchPacketString(std::string TableName) {
	    return mTableMap[TableName]->getMatchPacketString();
    }

    /* FIVES-PH:  some needed stuff */
    FinalAction* parsePacketVector(packet_vector_type &pv, ocfa::evidence::Evidence* ev);
    
    int getRuleListVersion(){ return mRuleListVersion; }

    void addLabel(const string labelname){
       mLabelMap[labelname] = mNextRuleNumber;
    }

    void addRule(Rule* rule){
       mRules.addRule(rule);
       mNextRuleNumber++;
       if (rule->getFinalAction()->getType() == FinalAction::FACT_JUMP){
          string target = dynamic_cast<FinalActionJump *>(rule->getFinalAction())->getValue();
          if (find(mUsedLabels.begin(), mUsedLabels.end(), target) == mUsedLabels.end())
             mUsedLabels.push_back(target);
       }
    }

    unsigned int getRuleNumber(const string labelname) {
       if( mLabelMap.find(labelname) != mLabelMap.end()){
         return mLabelMap.find(labelname)->second;
       }else{
         getLogStream(ocfa::misc::LOG_EMERG) << "Requesting a non-existant label!(" << labelname << ")" << endl;
         return 0;
       }
    }

    bool has_rule(){
        return mRules.has_rule();
    }

  /**************************************************************************/
  protected:  
    /**
     * Constructor
     */
    RuleList()                  
   : OcfaObject("RuleList", "router"), mFaDefault(NULL), mSetMetaFacet(NULL), mTableMap(),mUseFacades(true), mNextRuleNumber(0), mRuleListVersion(0) {};
    RuleList(RuleList&)   
   : OcfaObject("RuleList", "router"), mFaDefault(NULL), mSetMetaFacet(NULL), mTableMap(),mUseFacades(true), mNextRuleNumber(0), mRuleListVersion(0) {
       throw ocfa::OcfaException("No copy constructor allowded", this);
    }
    RuleList operator=(RuleList&) {
       throw ocfa::OcfaException("No copy constructor allowded", this);
    }
      
  /**** Private parts of class **********************************************/
  private:
    FinalAction          *mFaDefault;
    SetMetaFacet         mSetMetaFacet;
    std::map<std::string,Table *>   mTableMap;
    bool mUseFacades;			//RJM:CODEREVIEW its in the DTD
    /**
     * Static rulelist instance
     */
    static RuleList* _instance;
    /**
     * Private functions
     * May be solved in future with Pattern Generalisation
     */
    /* Load the rulelist from a file. Filelocation is retrieved from the 
     * ocfa configuration file */
    void           Load()throw (ocfa::OcfaException);
                   /* Read the rulefile and store rules in memory */
    void           ReadRuleFile(std::string FileName) throw (ocfa::OcfaException);
    void           processRuleLine(std::string *ruleLine) throw(ocfa::OcfaException);
    
    /* FIVES-PH: */ 
    void           ReadVersion2RuleFile(std::string FileName) throw (ocfa::OcfaException);
   
    unsigned int mNextRuleNumber; /* the identifier for the next rule */
    Table mRules; /* one table is enough for new-format-rules */
    std::map<const std::string, unsigned int> mLabelMap;  /* labelname -> rulenumber */
    std::vector<std::string> mUsedLabels; /* labels for which there are ACT_JUMP to (used for sanity checking) */
    int mRuleListVersion;
    

    
}; //end class Rulelist
} //namespace router

#endif
