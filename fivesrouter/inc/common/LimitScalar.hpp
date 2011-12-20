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

#ifndef _ROUTER_LIMIT_SCALAR_HPP
#define _ROUTER_LIMIT_SCALAR_HPP
#include <string>
#include "Limit.hpp"
namespace router {

class LimitScalar : public Limit {
  public:
    
    LimitScalar()
                         : _szValue(""), _iValue(0), _bValue(false), _fValue(0),
			   _Negate(false), _Matchtype(MT_EXACT)  {} ;
    LimitScalar(string Value)
                         : _szValue(Value), _iValue(0), _bValue(false), _fValue(0), _Negate(false), _Matchtype(MT_EXACT) {} ;
    LimitScalar(string Value, match_type Mt) 
                         : _szValue(Value), _iValue(0), _bValue(false), _fValue(0), _Negate(false), _Matchtype(Mt){ };
    
    ~LimitScalar() {} ;
    /**
     * Public methods
     */
    limit_type            getLimittype() { return LMT_SCALAR; }
    void                  setValue(string Value) { _szValue = Value; }  //RJM:CODEREVIEW We should use a Scalar here.
    bool                  checkValue(string Value) ; //A single checkValue(Scalar) should replace the 3 methods defined
    bool                  checkValue(float Value) ;  //here.
    bool                  checkValue(int Value) ;
     
  
  private:
    string                 _szValue;  //RJM:CODEREVIEW A single Scalar should replace these four
    int                    _iValue;   //               member vars. We may need to add some operator
    bool                   _bValue;   //               overloading to the ocfa::misc::Scalar class
    float                  _fValue;   //               to alow for proper checks. Further there are
                                      //	       no boolan Scalars, but there are DateTimes.
				      //	       The usage of a Scalar would also take care of the 
				      //	       type field defined in the DTD.
    bool                   _Negate;
    match_type             _Matchtype;
}; //end LimitScalar class

} //namespace router

#endif
