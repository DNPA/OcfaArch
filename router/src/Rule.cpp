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

#include "common/Rule.hpp" //RJM:CODEREVIEW Use the one file per vlass headers.
#include <iostream>
#include <fstream>

using namespace std;
using namespace router;

using namespace ocfa::misc;
using namespace ocfa::evidence;

/****************************************************************************/
/**** Rule  class implementation ********************************************/
/****************************************************************************/
bool Rule::checkLimitNamespace(string Value){
  
  vector<Limit *>::iterator   piLimit;
  Limit                      *tmpLimit;
  LimitNamespace             *limitNamespace; 
  
  for(piLimit = _LimitVector.begin() ; piLimit != _LimitVector.end() ; ++piLimit) {
    tmpLimit = *piLimit;
    if(tmpLimit->getLimittype() == Limit::LMT_NAMESPACE) { //RJM:CODEREVIEW Moved enum to Limit class
	if((limitNamespace = dynamic_cast<LimitNamespace *>(tmpLimit)) == 0) {
	   throw OcfaException("Dynamic cast exception on Limit", this);
	}
        if(limitNamespace->checkValue(Value) == true)
	  return true;
	else
	  return false;
    }
  }
  //No limit namespace found to disqualify check
  return true;

} //end checkLimitNamespace

bool Rule::checkLimitScalar(string Value){
  
  vector<Limit *>::iterator   piLimit;
  Limit                      *tmpLimit;
  LimitScalar                *limitScalar;
  
  for(piLimit = _LimitVector.begin() ; piLimit != _LimitVector.end() ; ++piLimit) {
    tmpLimit = *piLimit;
    if(tmpLimit->getLimittype() == Limit::LMT_SCALAR) { //Moved enum to Limit class
	if((limitScalar = dynamic_cast<LimitScalar *>(tmpLimit)) == 0) {
	   throw OcfaException("Dynamic cast exception on Limit", this);
	}
        if(limitScalar->checkValue(Value) == true)
	  return true;
	else
	  return false;
    }
  }
  //No limit scalar found to disqualify check
  return true;

} //end checkLimitScalar

bool Rule::checkLimitModule(string Value){
  
  vector<Limit *>::iterator   piLimit;
  Limit                      *tmpLimit;
  LimitModule                *limitModule;
  
  for(piLimit = _LimitVector.begin() ; piLimit != _LimitVector.end() ; ++piLimit) {
    tmpLimit = *piLimit;
    if(tmpLimit->getLimittype() == Limit::LMT_MODULE) { //RJM:CODEREVIEW moved enum to Limit class
	if((limitModule = dynamic_cast<LimitModule *>(tmpLimit)) == 0) {
	   throw OcfaException("Dynamic cast exception on Limit", this);
	}
        if(limitModule->checkValue(Value) == true)
	  return true;
	else
	  return false;
    }
  }
  //No limit scalar found to disqualify check
  return true;

} //end checkLimitModule

void Rule::addLimit(Limit *limit) {
    _LimitVector.push_back(limit);
}

Rule::~Rule() {
   if(_FinalAction) delete _FinalAction;
   //emptyLimitvector
   vector<Limit *>::iterator piLimit;
   for(piLimit = _LimitVector.begin() ; piLimit != _LimitVector.end(); ++piLimit) {
     if(*piLimit != 0) {
       delete *piLimit;
     }  
   }
   
}

void  Rule::setFinalAction(string Action, string Target, string Args){
     if (Action ==  "ACT_FORWARD") {
        _FinalAction = new FinalActionForward(Target);
     } else if (Action == "ACT_COMMIT") {
        _FinalAction = new FinalActionCommit(Target, Args);
     } else if (Action == "ACT_SUSPEND") {
        _FinalAction = new FinalActionSuspend(Target);
     } else if (Action == "ACT_JUMP") {
        _FinalAction = new FinalActionJump(Target);
     } else {
        _FinalAction = new FinalActionReturn();
     }
}

