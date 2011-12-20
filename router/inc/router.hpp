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
#ifndef __ROUTER_HPP__
#define __ROUTER_HPP__


/**
 * Router class header file
 */
//#include "rulelist.hpp"  // RJM:CODEREVIEW dont use the old single file headers
#include "csv/RuleList.hpp" // RJM:CODEREVIEW instead use the one file per class headers.

using namespace ocfa::evidence;
using namespace ocfa::module;
using namespace ocfa::misc;
using ocfa::message::Message;

  
  
namespace router {  
/**
 * Class Router
 * The main class for routing evidence around in the digital washing machine.
 * The router class implements the pure virual processEvidence and responds 
 * to messages, to handle new rulelists and module instance messages.
 */
class Router : public ocfa::facade::TargetAccessor {
  protected:
    Router(Router&)
             :  TargetAccessor("router", "core"), mRuleList(NULL), mGlobalNamespace("klpd") {
       throw OcfaException("No copy constructor allowded", this);
    }
    Router operator=(Router&) {
       throw OcfaException("No copy constructor allowded", this);
    }
  
  public:
    /**
     * constructor
     */
    Router();
    /** 
      * destructor 
      */
    ~Router();

    /**
     * Implementing pure virutal processEvidence
     */
    void processEvidence();
    //void processMessage(ocfa::message::Message &msg) {};
    /**
     * disptachMessage 
     */
//    void dispatchMessage(const ocfa::message::Message &mesg); //RJM:CODEREVIEW This is not needed for anything

  /**** The private parts of the router class *******************************/
  private:
    RuleList *mRuleList;  //RJM:CODEREVIEW naming conventions.
    void PerformRouting(FinalAction *fa);
    string mGlobalNamespace; //RJM:CODEREVIEW naming conventions.
    bool mDoIntegerNames;
    bool mDoFloatNames;
    bool mDoDateNames;
    bool mDoIntegerVals;
    bool mDoFloatVals;
    bool mDoDateVals;
    bool mDoArrayNames;
    bool mDoTableNames;
    bool mDoExtendedEvidence;
}; //end routerclass

} //namespace router

#endif

