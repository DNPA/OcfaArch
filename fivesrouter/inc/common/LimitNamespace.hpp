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

#ifndef _ROUTER_LIMIT_NAMESPACE_HPP
#define _ROUTER_LIMIT_NAMESPACE_HPP
#include <string>
#include "Limit.hpp"
namespace router {
class LimitNamespace : public Limit {
  public:
    LimitNamespace() 
                          : _Value(""), _Negate(false), _Matchsub(false) {};
    LimitNamespace(string Value)
                          : _Value(Value), _Negate(false), _Matchsub(false) {};
    ~LimitNamespace() {};
    limit_type            getLimittype() { return LMT_NAMESPACE; }
    void                  setValue(string Value) { _Value = Value; }
    bool                  checkValue(string Value) ;
    void                  setNegate(bool Negate) { _Negate = Negate; }
    void                  setMatchsub(bool Matchsub) { _Matchsub = Matchsub ; }
  private:
    string                _Value;
    bool                  _Negate;
    bool                  _Matchsub;

}; //end class LimitNamespace
} //namespace router

#endif
