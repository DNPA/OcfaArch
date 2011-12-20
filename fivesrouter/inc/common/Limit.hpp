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

#ifndef _ROUTER_LIMIT_HPP
#define _ROUTER_LIMIT_HPP
#define DONT_CARE             "DNTCR"  //RJM:CODEREVIEW this was in the monolyth header, not sure if this is the best place either
#include <string>
#include "ocfa.hpp"

namespace router {

/**
 * Interface class for all Limit classes
 */
class Limit : public ocfa::OcfaObject {
   public:
     enum limit_type {       //RJM:CODEREVIEW  Moved enum to within class definition.
	 LMT_SCALAR = 0,
	 LMT_ARRAY,
	 LMT_TABLE,
	 LMT_NAMESPACE,
	 LMT_MODULE,
	 LMT_HOST
     };
     enum match_type {      //RJM:CODEREVIEW  Moved enum to within class definition.
	 MT_EXACT=0,
	 MT_REGEX,
	 MT_GREATER,
	 MT_SUB		//RJM:CODEREVIEW any substring argument could be used as regex argument. MT_SUB is redundant.
     };
     Limit() : OcfaObject("Limit", "router") {};
     virtual ~Limit() {} ;
     /**
      * Pure virtual getLimittype in interface class Limit
      */
     virtual limit_type    getLimittype() = 0;
   private:
}; // Limit class

} //namespace router

#endif
