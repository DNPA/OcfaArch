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

#ifndef __RULE_HPP__
#define __RULE_HPP__

#include "Expression.hpp"

#include <string>
#include <iostream>

class Rule  {
  protected:
 

  public:
    
    /* FIVES-PH:*/
    Rule(Expression* expr, std::string finalaction, std::string target) : _Expression(expr) {
					 _FinalAction = finalaction;
					 _FinalActionValue = target;
//					std::cout << "Added rule: " << *this << "\n";
        }

    std::string            getFinalAction() { return _FinalAction; }
    std::string            getFinalActionValue() { return _FinalActionValue; }
    Expression*				getExpression() {return _Expression; }

    /* FIVES-PH:*/
    friend std::ostream& operator<<(std::ostream& out, Rule& rule){
	 	out << *(rule._Expression) << " " << rule._FinalAction << "; " << rule._FinalActionValue << ";";
		return out;
	}

  private:
	std::string		_FinalAction;
	std::string		_FinalActionValue;
	Expression* _Expression;
    
}; 


#endif
