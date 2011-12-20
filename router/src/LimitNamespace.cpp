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

#include "common/LimitNamespace.hpp" //RJM:CODEREVIEW Use the one file per vlass headers.
#include <iostream>
#include <fstream>

using namespace std;
using namespace router;

using namespace ocfa::misc;
using namespace ocfa::evidence;

bool LimitNamespace::checkValue(string Value) {

  bool rtnValue = false;

  //compatibiliteit met CSV rulelist opmaak
  if(_Value == DONT_CARE) 
      return true;
  
   if(_Matchsub == false) {
     rtnValue =  (_Value == Value) ;
   }
   else {
     //                  hooiberg   ,     naald
     rtnValue = strstr(Value.c_str(), _Value.c_str());
   }
   if(_Negate == true) 
     return !rtnValue;
   else
     return rtnValue;

}

