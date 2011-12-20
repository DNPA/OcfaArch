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
#include "evidence/Evidence.hpp"

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
		: OcfaObject("Table", "router"), _Fascade(true), _Mode(TBLMD_NARROW), _RuleVector(), mPacket(EmptyPacket) {
       //empty body
    } 
    Table(bool Fascade, table_mode Mode) 
		: OcfaObject("Table", "router"), _Fascade(Fascade), _Mode(Mode), _RuleVector(), mPacket(EmptyPacket) {
       //empty body
    }
    ~Table();
    void               addRule(Rule* r) { _RuleVector.push_back(r); }
    FinalAction*       parsePacketVector(packet_vector_type &pv,std::list<SideAction> *sideactions,std::map<std::string,Table *> &tablemap,size_t depth); 
    FinalAction*       parsePacketVector(packet_vector_type &pv, ocfa::evidence::Evidence*); /* FIVES-PH: needed the evidence, but not the other stuff */
    std::string        getMatchPacketString() {
         return std::string(//"tbl=" + mPacket.Table + ":" // RJM:CODEREVIEW
		 "ns=" + mPacket.NameSpace + 
		 ":type=" + mPacket.Type + 
		 ":val=" + mPacket.Value + 
		 ":lm=" + mPacket.LastMod); 
    }
    bool               has_rule(){ return !_RuleVector.empty(); }
  private:
    bool                _Fascade;
    table_mode          _Mode;
    std::vector<Rule *>      _RuleVector;
    packet_type         mPacket;
    // Private methods


}; //end class Table

} //namespace router

#endif
