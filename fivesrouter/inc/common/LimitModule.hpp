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

#ifndef _ROUTER_LIMIT_MODULE_HPP
#define _ROUTER_LIMIT_MODULE_HPP
#include <string>
#include "Limit.hpp"
namespace router {
class LimitModule : public Limit {
  public:
    LimitModule() 
                          : _Value("EMPTY"), _Negate(false) {};
    LimitModule(string Value)
                          : _Value(Value), _Negate(false) {};
    ~LimitModule() {};
    limit_type            getLimittype() { return LMT_MODULE; }
    bool                  checkValue(string Value) ;
    void                  setNegate(bool Negate) { _Negate = Negate; }
  private:
    string                _Value;
    bool                  _Negate;
}; //end class LimitModule

} //namespace router

#endif
