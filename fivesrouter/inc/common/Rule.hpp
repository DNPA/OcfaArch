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

#ifndef __RULE_HPP__
#define __RULE_HPP__
#include <ocfa.hpp>
#include <string>
#include <vector>
//#include <misc/syslog_level.hpp>
#include "Limit.hpp"
//#include "LimitHost.hpp"  //uncomment when implemented 
#include "LimitNamespace.hpp"  
//#include "LimitTable.hpp" //uncomment when implemented
//#include "LimitArray.hpp" //uncomment when implemented
#include "LimitModule.hpp"
#include "LimitScalar.hpp"
#include "SideAction.hpp"
//#include "SideActionAddJobArgument.hpp" //uncomment when implemented
#include "SideActionAddMeta.hpp"
//#include "SideActionELog.hpp" //uncomment when implemented
//#include "SideActionLog.hpp"  //uncomment when implemented
#include "FinalAction.hpp"
#include "FinalActionCommit.hpp"
#include "FinalActionForward.hpp"
#include "FinalActionJump.hpp"
#include "FinalActionReturn.hpp"
#include "FinalActionSuspend.hpp"
#include "Expression.hpp"
#include "common/packet_type.hpp"

namespace router {

/**
 * Class Rule
 * 
 */
class Rule : public ocfa::OcfaObject {
  protected:
    Rule(Rule&) 
	    //RJM:CODEREVIEW Removed DONT_CARE for implementation independance
//      : OcfaObject("Rule", "router"), _LimitVector(), _FinalAction(NULL), _Name(DONT_CARE), _Negate(false), _DataType(RDT_SCALARMETA) { 
	: OcfaObject("Rule", "router"), _LimitVector(), _FinalAction(NULL), _Name(), _Negate(false), _DataType(RDT_SCALARMETA) { 
	throw ocfa::OcfaException("No copy constructor allowded", this);
    }
    Rule operator=(Rule&) {
	throw ocfa::OcfaException("No copy constructor allowded", this);
    }
 

  public:
    enum rule_data_type {
	RDT_SCALARMETA=0,
	RDT_MODULEINSTANCE,
	RDT_ARRAYMETA,
	RDT_TABLEMETA
    };
    Rule() 
	    //RJM:CODEREVIEW: removed DONT_CARE for implementation independance
//: OcfaObject("Rule", "router"), _LimitVector(), _FinalAction(NULL), _Name(DONT_CARE), _Negate(false), _DataType(RDT_SCALARMETA) { };
      : OcfaObject("Rule", "router"), _LimitVector(), _FinalAction(NULL), _Name(), _Negate(false), _DataType(RDT_SCALARMETA) { };
    Rule(string Name) 
                            : OcfaObject("Rule", "router"), _LimitVector(), _FinalAction(NULL), _Name(Name), _Negate(false), _DataType(RDT_SCALARMETA) {};
    Rule(string Name, rule_data_type DataType)
			   : OcfaObject("Rule", "router"), _LimitVector(), _FinalAction(NULL), _Name(Name), _Negate(false), _DataType(DataType) {} ;
    
    /* FIVES-PH:*/
    Rule(Expression* expr, string finalaction, string target): OcfaObject("Rule", "router"), _LimitVector(), _FinalAction(NULL), _Name(), _Negate(false), _DataType(RDT_SCALARMETA), _Expression(expr) {
                setFinalAction(finalaction, target, "");
		getLogStream(ocfa::misc::LOG_DEBUG) << "Added rule: " << *this << "\n";
        }

     ~Rule();
			   
    void                   setName(string Name) { 
                               _Name = Name; 
			       _DataType = RDT_SCALARMETA;
                           }
    void                   setName(string Name, rule_data_type DataType) {
                               _Name = Name;
			       _DataType = DataType;  
                           }
    void                   setNegate(bool Negate) {
			      _Negate = Negate;
			   }
    void                   addLimit(Limit *limit);
    void                   setFinalAction(string Action, string Target, string Args);
    bool                   checkLimitNamespace(string Value);
    bool                   checkLimitScalar(string Value);
    bool                   checkLimitModule(string Value);
    string                 getName() { return _Name ; }
    rule_data_type         getType() { return _DataType ; }
    FinalAction*           getFinalAction() { return _FinalAction; }
    //RJM:CODEREVIEW we should add method for adding SideAction.
    //RJM:CODEREVIEW we should add methods for access to SideActions
    
    /* FIVES-PH:*/
    bool   evaluate(packet_vector_type &pv){ return _Expression->evaluate(pv); }
    friend std::ostream& operator<<(std::ostream& out, Rule& rule);

  private:
    //vector <SideAction *> _SideActionVector;  // RJM:CODEREVIEW
    vector<Limit *>         _LimitVector;
    FinalAction            *_FinalAction;
    string                 _Name;
    bool                   _Negate;
    rule_data_type         _DataType;

    /* FIVES-PH: expressions for new rulelist */
    Expression* _Expression;
    
}; //end class Rule

} //namespace router

#endif
