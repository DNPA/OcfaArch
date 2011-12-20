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

#include "common/Table.hpp" //RJM:CODEREVIEW Use the one file per vlass headers.
#include <iostream>
#include <fstream>
#include "evidence/Evidence.hpp"
#include "csv/RuleList.hpp"

using namespace std;
using namespace router;

using namespace ocfa::misc;
using namespace ocfa::evidence;

/****************************************************************************/
/**** Table class implementation ********************************************/
/****************************************************************************/
FinalAction* Table::parsePacketVector(packet_vector_type &pv,std::list<SideAction> *sideactions,std::map<std::string,Table *> &tablemap,size_t depth){ 
  // Locals
  Rule                           *tmpRule;
  vector<Rule *>::iterator       piRule; 
  vector<packet_type>::iterator  piPacket;
 
  //foreach Rule in rulevector from given Table
  for(piRule = _RuleVector.begin() ; piRule != _RuleVector.end() ; ++piRule) {
     tmpRule = *piRule;
     //foreach packet in packetvector to check
     //RJM:CODEREVIEW instead of using a vector and iterator we should use a PacketContainer and ask it
     //if our rule matches within our current table's view.
     for(piPacket = pv.begin() ; piPacket != pv.end() ; ++piPacket) {
        FinalAction *fa=0;
        switch (tmpRule->getType()) {
          case Rule::RDT_MODULEINSTANCE :
             if( (tmpRule->getName() == piPacket->LastMod) || 
                  (tmpRule->getName() == DONT_CARE) ) {
	         if(tmpRule->checkLimitNamespace(piPacket->NameSpace) == true) {
		      mPacket = *piPacket;
		      fa=tmpRule->getFinalAction();
	         }
	   }
	   break;
          case Rule::RDT_SCALARMETA :
	  case Rule::RDT_ARRAYMETA :
	  case Rule::RDT_TABLEMETA :
           if(tmpRule->getName() == piPacket->Type) {
	     //all possible limits from limit vector must check OK.
	      if(  (tmpRule->checkLimitNamespace(piPacket->NameSpace) ==true) &&
	           (tmpRule->checkLimitScalar(piPacket->Value) == true ) &&
		   (tmpRule->checkLimitModule(piPacket->LastMod) == true) ) {
		  mPacket = *piPacket;
		  fa=tmpRule->getFinalAction();
	      }
	   }
	   break;
        }
	if (fa) {
           switch (fa->getType()) {
               case FinalAction::FACT_SUSPEND :
	       case FinalAction::FACT_COMMIT  :
	       case FinalAction::FACT_FORWARD :
	           return fa;
	       case FinalAction::FACT_DONOTHING :
	           throw OcfaException("FACT_DONOTHING is not currently allowed as final action",this);
	       case FinalAction::FACT_RETURN :
	           if (depth == 0) 
		       throw OcfaException("FACT_RETURN can not be called from depth=0",this);
		   else 
		       return NULL;
               case FinalAction::FACT_JUMP :
	           if (depth > 16) 
		       throw OcfaException("Aparent low level JUMP loop in rulelist, depth would exceed the treshold of 16.",this);
		   FinalActionJump *faj=dynamic_cast<FinalActionJump *>(fa);
		   if (faj == 0) {
                        throw OcfaException("Problem casting FACT_JUMP back to its orn class",this);
		   }
		   Table *jumptable=tablemap[faj->getValue()];
		   if (jumptable == 0) {
                        throw OcfaException("Attempt to jump to non existing table",this);
		   }
                   FinalAction *fa2=jumptable->parsePacketVector(pv,sideactions,tablemap,depth+1);
		   if (fa2) {
		       return fa2;
		   }
	   }
	}
     } //end foreach packet in packet vector 
  } //end foreach rule in vector
  
  //No FinalAction Found, so return NULL
  //Make it possible to react on, instead of throwing up exception
  mPacket = EmptyPacket;
  return NULL;
  
} //RuleList::parsePacketVector


/* FIVES-PH: rulelist v2 packet vector parsing */
FinalAction* Table::parsePacketVector(packet_vector_type &pv, Evidence* ev){

  ExtendableEvidence *eev = dynamic_cast<ExtendableEvidence *>(ev);

  for(unsigned int rulenum=eev->getRuleNumber(); rulenum < _RuleVector.size(); ++rulenum){
      if (_RuleVector[rulenum]->evaluate(pv)){
	getLogStream(ocfa::misc::LOG_DEBUG) << ev->getEvidencePath().asUTF8() << "/" << ev->getEvidenceName().asUTF8() << " Matched " << *_RuleVector[rulenum] << "\n";


        eev->setRuleNumber(rulenum+1); /* next time, continue at next rule */
        // all rules have valid and implemented final actions, if configured otherwise, 
        //      the parser will notify the user
        FinalAction* fa = _RuleVector[rulenum]->getFinalAction();
        switch(fa->getType()){
        case FinalAction::FACT_JUMP :
            {
                 FinalActionJump *faj = 0;
                 if((faj = dynamic_cast<FinalActionJump *>(fa)) == 0) {
                        throw OcfaException("Dynamic cast exception on FinalAction", this);
                 }

                 /* FIVES-PH: I didnt find any string to unsigned int in Scalar */
               getLogStream(ocfa::misc::LOG_DEBUG) << "Jumping to " << faj->getValue() << " (" 
                       << RuleList::getInstance()->getRuleNumber(faj->getValue()) << ")" << endl;

                 rulenum = RuleList::getInstance()->getRuleNumber(faj->getValue())-1; /* -1 compensates ++rulenum in loop */
		/* no return here but instead continue looping */
            }
            break;
        case FinalAction::FACT_COMMIT :
        case FinalAction::FACT_FORWARD :
        default:
            return fa;
        }
      }
  }
  return 0; /* no rule matched. This should NOT happen, the rulelist should be written such that at least the last rule matches */
}



Table::~Table() {
  //garbage collection
  vector<Rule *>::iterator piRule;
  for(piRule = _RuleVector.begin() ; piRule != _RuleVector.end() ; ++piRule) {
    if(*piRule != 0)
      delete *piRule;
  }

}

