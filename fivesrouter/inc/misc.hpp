//RJM:CODEREVIEW this file is not yet used for the CSV implementation.
//It should be used however asap.
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

#include <string>
#include <ocfa.hpp> //RJM:CODEREVIEW
#include <facade/TargetAccessor.hpp>


#ifndef __MISC_HPP__
#define __MISC_HPP__

//using namespace std;   //RJM:CODEREVIEW no using in header files
//using namespace ocfa;  //RJM:CODEREVIEW no using in header files

/**
 * The misc header file
 * A header file containing all kind of support classes and definitions for 
 * the router:
 * - Match class
 * - Packet class
 */
 
namespace router { 

//extern const string Match::StandardTarget;   /**< Standard const class parameter */
//extern const string Match::StandardAction;   /**< Standard const class parameter */
//extern const string Match::StandardArgument; /**< Standard const class parameter */

 
 /**
  * The Match Class
  * The Match contains the result from the match with a defined condition
  * Pattern Creation and Not Mutable: After creating, the class is not mutable
  * anymore.
  */
 class Match : public ocfa::OcfaObject {
    public:
      //declaration
      static const string StandardTarget;
      static const string StandardAction;
      static const string StandardArgument;
      Match() 
         :OcfaObject("Match", "router"), _ModuleType(StandardTarget), _Action(StandardAction), _Arguments(StandardArgument) 
      {};
      Match(string mType, string Action, string Arguments)
         :ocfa::OcfaObject("Match", "router"), _ModuleType(mType), _Action(Action), _Arguments(Arguments) 
      {};
      /**
       * copy constructor
       * Deep copy of Match
       */
      Match(Match &m) 
         :ocfa::OcfaObject("Match", "router"), _ModuleType(m.getType()), _Action(m.getAction()), _Arguments(m.getArguments())
      {};
      //destructor
      ~Match() {};
      string getType() { return _ModuleType; }
      string getAction() { return _Action; }
      string getArguments() { return _Arguments; }
      
    private:
      string _ModuleType;
      string _Action;
      string _Arguments;
 
 }; //end class Match


  
 /**
  * The Packet Class
  * The packet contains the condition to be matched with the rulelist
  */
 class Packet : public ocfa::OcfaObject {
    public:
       // constructor
       Packet() :OcfaObject("Packet", "router") {}; //RJM:CODEREVIEW Instead of this constructor:
       Packet(ocfa::misc::ModuleInstance *minst);   //RJM:CODEREVIEW one with a module instance
       Packet(std::string metaname, ocfa::misc::MetaValue *metaval); //RJM:CODEREVIEW and one with a MetaValue
       // destructor
       ~Packet() {};
       
       void getMetaType() {} ;
       void getModInst() {};
       void getMetaValue() {} ;
       void getMetaName() {} ;
    
    private:
       string _MetaType;
       string _MetaValue;
       string _LastModule;
       string _NameSpace;
 
 }; // end class Packet

 class PacketContainer : public ocfa::OcfaObject {  //RJM:CODEREVIEW
     public:
        PacketContainer(bool ruleListHasArrays=false,bool ruleListHasTables=false):mRuleListHasArrays(ruleListHasArrays),mRuleListHasTables(ruleListHasTables) {}
	~PacketContainer();
	Packet * operator[](Rule& r);
	void add(ocfa::misc::ModuleInstance *minst);
	void add(std::string metaname, ocfa::misc::MetaValue *metaval);
     private:
        //RJM:CODEREVIEW need to add some private members, the folowing is just a sugestion of
	//some direction we could go with this.
	// 
	//bool mRuleListHasArrays;
	//bool mRuleListHasTables;
	//std::map<std::string,ocfa::misc::ModuleInstance *> mModInstMap;
	//std::map<std::string,ocfa::misc::ScalarMetaValue *> mScalarMetaMap;
	//std::map<std::string,ocfa::misc::ArrayMetaValue *>
 }
 
} //namespace router

#endif
