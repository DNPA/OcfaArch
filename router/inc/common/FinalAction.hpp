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

#ifndef _ROUTER_FINALACTION_HPP_
#define _ROUTER_FINALACTION_HPP_
#include <string>
namespace router {

/**
 * Interface Class FinalAction
 */
class FinalAction {
  public:
    enum final_action_type {
	FACT_COMMIT=0,
	FACT_FORWARD,
	FACT_JUMP,
	FACT_SUSPEND,
	FACT_RETURN,
	FACT_DONOTHING
    };
    FinalAction()  { } ;
    virtual ~FinalAction() {} ;
    virtual final_action_type  getType() = 0 ;
  
  private:
    
}; //end class FinalAction

} //namespace router

#endif
