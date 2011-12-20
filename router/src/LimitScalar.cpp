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
#include <boost/regex.hpp>
#include "common/LimitScalar.hpp" //RJM:CODEREVIEW Use the one file per vlass headers.
#include <iostream>
#include <fstream>
using namespace std;
using namespace router;

using namespace ocfa::misc;
using namespace ocfa::evidence;
/****************************************************************************/
/**** Limit class implementation ********************************************/
/****************************************************************************/
//RJM:CODEREVIEW Instead of using differently typed checks, we should simply use
//Scalars and move the check logic to operator overloaded methods within Scalar.
//
bool LimitScalar::checkValue(string Value){ 

  bool rtnValue = false;
  
   //compatibiliteit met CSV rulelist opmaak
   if(_szValue == DONT_CARE) return true;
   
   switch(_Matchtype) {
     case  MT_EXACT:
        rtnValue = (_szValue == Value) ; 
        break;
     case  MT_REGEX:
        {
	  boost::regex expression(_szValue.c_str());
	  rtnValue =  boost::regex_match(Value.c_str(), expression);
	}
        break;
     case  MT_GREATER:
	if(_szValue.compare(Value) > 0)
	   rtnValue = true;
	else
	   rtnValue = false;   
        break;
     case MT_SUB: //RJM:CODEREVIEW the substring can be implemented as regex.
        /*The substring match was originally the default in the old router*/
	//                   hooiberg  ,      naald
	rtnValue = strstr(Value.c_str(), _szValue.c_str());
        break;
   } //end switch Matchtype
   if(_Negate == true) 
     return !rtnValue;
   else
     return rtnValue;
}

bool LimitScalar::checkValue(int Value){ 

  bool rtnValue = false;

   switch(_Matchtype) {
     case  MT_EXACT:
        rtnValue = (_iValue == Value); 
        break;
     case  MT_GREATER:
	if(_iValue < Value)
	   rtnValue = true;
	else
	   rtnValue = false;   
        break;
     case  MT_SUB:
     case  MT_REGEX:
        getLogStream(LOG_NOTICE) << "REGEX and SUB comparison for integer not suported\n";
        rtnValue = false;
        break;
   } //end switch Matchtype
   
   if(_Negate == true) 
     return !rtnValue;
   else
     return rtnValue;
}

bool LimitScalar::checkValue(float Value){ 

  bool rtnValue = false;

   switch(_Matchtype) {
     case  MT_EXACT:
        getLogStream(LOG_NOTICE) << "EXACT comparison for floats not suported\n";
        rtnValue = false; //Don't match exact floats: (_fValue == Value) ; 
        break;
     case  MT_GREATER:
	if(_fValue < Value)
	   rtnValue = true;
	else
	   rtnValue = false;   
        break;
     case  MT_SUB:
     case  MT_REGEX:
        getLogStream(LOG_NOTICE) << "REGEX and SUB comparison for floats not suported\n";
        rtnValue = false;
        break;
   } //end switch Matchtype
   
   if(_Negate == true) 
     return !rtnValue;
   else
     return rtnValue;
}

