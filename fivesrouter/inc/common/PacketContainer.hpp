//RJM:CODEREVIEW: this file is a proposal for a replacement for the
//packet vector that apears to be rather inefficient.
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

#ifndef __PACKET_CONTAINER_HPP__
#define __PACKET_CONTAINER_HPP__
#include <ocfa.hpp>
#include "Rule.hpp"

namespace router { 
 #define DISTINCT_PACKET_CONTAINER_VIEWS 3
 #define DISTINCT_RULE_DATA_TYPES 4
 class PacketContainer : public ocfa::OcfaObject {  
     public:
	enum PacketContainerView { //The views on the container.
            VIEW_NARROW=0,
	    VIEW_SHALOW,
	    VIEW_DEEP
	}; 
	class PacketContainerRow { //A helper class to allow both for pc[rule][view] and
		                   //for pc[rule] lookups in the container.
	   public: 
             bool operator[](PacketContainerView pv) {  //[] operator for the second set of [] braces
                  return mContainer->doesMatch(mRule,pv);
	     }
	     operator bool() {  //Cast operator for when only one set of [] braces is used.
                 return (*this)[VIEW_NARROW];
	     }
	     PacketContainerRow(PacketContainer *cont,Rule& r):mContainer(cont),mRule(r) {}
	   private:
	     PacketContainer *mContainer;
	     Rule& mRule; 
	};
	//We add two booleans to the constructor that should be set dependant on if the
	//rule list has any array or table match lines.
        PacketContainer(bool ruleListHasArrays=false,bool ruleListHasTables=false);
	~PacketContainer();
	//The first set of [] braces results in a helper class PacketContainerRow, see abouve.
	PacketContainerRow  operator[](Rule& r) {return PacketContainerRow(this,r);}
	//The container by default will have only a narrow view, but other views can be added
	//on table jumps. In order to avoid adding packets twice, the folowing two methods tell
	//if a particular view was added to at least ones.
	bool hasDeepView();
	bool hasShalowView();
	//Instead of worying about packets at a high level, we hould be able to add ModuleInstances and
	//meta key/value pairs.
	void add(ocfa::misc::ModuleInstance *minst,
			PacketContainerView view=VIEW_NARROW);
	void add(ocfa::misc::ModuleInstance *minst,
			std::string metaname, 
			ocfa::misc::MetaValue *metaval,
			PacketContainerView view=VIEW_NARROW);
	//This is the basic doesMatch method that the overloaded [] braces build upon.
	bool doesMatch(Rule& r,PacketContainerView pv);
     private:
	//It is up to jochen to come up with the best storage structure here. Currently the router
	//uses a single vector of packets. I would sugest that using the folowing setup may be better:
	std::map< std::string,std::vector < ocfa::ModuleInstance * > > mMinstContainer[DISTINCT_PACKET_CONTAINER_VIEWS];
	std::map< std::string,std::vector < ocfa::MetaValue * > > mMetaContainer[DISTINCT_PACKET_CONTAINER_VIEWS][DISTINCT_RULE_DATA_TYPES];
 };
 
} //namespace router

#endif
