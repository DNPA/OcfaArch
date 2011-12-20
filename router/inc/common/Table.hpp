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

#ifndef __TABLE_HPP__
#define __TABLE_HPP__
#include <ocfa.hpp>
#include <string>
#include <vector>
#include "packet_type.hpp"
#include "Rule.hpp"
#include "FinalAction.hpp"
#include <boost/lexical_cast.hpp>
namespace router {

class Table : public ocfa::OcfaObject {
  public:
    enum table_mode {
	TBLMD_NARROW=0,
	TBLMD_SHALOW,
	TBLMD_DEEP
    };
    //constructor
    Table() 
		: OcfaObject("Table", "router"), _Fascade(true), _Mode(TBLMD_NARROW), _RuleVector(),mMatchingRule(0) {
       //empty body
    } 
    Table(bool Fascade, table_mode Mode) 
		: OcfaObject("Table", "router"), _Fascade(Fascade), _Mode(Mode), _RuleVector(),mMatchingRule(0) {
       //empty body
    }
    ~Table();
    void               addRule(Rule* r) { _RuleVector.push_back(r); }
    FinalAction*       parsePacketVector(packet_vector_type &pv,std::list<SideAction> *sideactions,std::map<std::string,Table *> &tablemap,size_t depth); 
    std::string        getMatchPacketString() {
         if (mMatchingPacket.Value == "EMPTY") {
             return "NO MATCHING PACKET";
         }
         return std::string(//"tbl=" + mPacket.Table + ":" // RJM:CODEREVIEW
		 "ns=" + mMatchingPacket.NameSpace + 
		 ":type=" + mMatchingPacket.Type + 
		 ":val=" + mMatchingPacket.Value + 
		 ":lm=" + mMatchingPacket.LastMod); 
    }
    std::string        getMatchRuleString() {
        if (mMatchingRule == 0) {
           return "line=0:name=UNDEFINED";
        }
        return std::string("line=") + boost::lexical_cast<std::string>(mMatchingRule->getLineNumber()) + ":name=" + mMatchingRule->getName();
    }

    Rule *getMatchingRule() { return mMatchingRule;}
    
  private:
    bool                _Fascade;
    table_mode          _Mode;
    std::vector<Rule *>      _RuleVector;
    packet_type         mMatchingPacket;
    Rule                *mMatchingRule;
    // Private methods


}; //end class Table

} //namespace router

#endif
