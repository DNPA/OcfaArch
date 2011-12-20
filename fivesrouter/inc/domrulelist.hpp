//RJM:CODEREVIEW Review of this file has been delayed due to the fact that it is currently
//not yet used or complete.
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

/**
 * The domrulelist class header file
 * 
 **/
 #include "misc.hpp"
 #include "rulelist.hpp"
 #include <facade/Accessor/TargetAccessor.hpp>
 
 #ifndef __DOMRULELIST_HPP__
 #define __DOMRULELIST_HPP__
 
 using namespace ocfa::evidence;
 
namespace router { 
 class DomRuleList : public RuleList {
  /**************************************************************************/
  public:
    /**
     * Destructor
     */
    ~DomRuleList();
    
    
    /**
     * method parseEvidence overloading derived class from rulelist
     */
    Match* parseEvidence(JobIterator *ji, string TableName);
    
  /**************************************************************************/
  protected:  
    /**
     * Constructor
     */
    DomRuleList();

      
  /**** Private parts of class **********************************************/
  private:
 
 
 }; //end class DomRuleList
 
} //end namespace router
 
 
 #endif
 
