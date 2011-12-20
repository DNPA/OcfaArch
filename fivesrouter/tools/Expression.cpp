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

// Written by Peter Hjärtquist for FIVES (fives.kau.se)

#include "Expression.hpp"

#include <iostream>


Expression::Expression(): expressions(0){
}

Expression::Expression(std::list<SimpleExpression>* expr): expressions(expr) {
}

Expression::Expression(const Expression& e):  expressions(0){
	copy(e);
}

Expression::~Expression(){
	delete expressions;
}

Expression& Expression::operator=(const Expression& e){
	if (&e != this){
		free();
		copy(e);
	}
	return *this;
}

void Expression::copy(const Expression& e){
	expressions = new std::list<SimpleExpression>(*e.expressions);
}

void Expression::free(){
	delete expressions;
	expressions = 0;
}

void Expression::addSimpleExpression(SimpleExpression se){
	if (expressions == 0)
		expressions = new std::list<SimpleExpression>();
	expressions->push_back(se);
}

void Expression::addSimpleExpression(std::list<SimpleExpression> se){
	if (expressions == 0)
		expressions = new std::list<SimpleExpression>();

	for (std::list<SimpleExpression>::iterator i = se.begin();
			i != se.end();
			++i)
		expressions->push_back(*i);
}

std::ostream& operator<<(std::ostream& out, Expression& expr){
	for (std::list<SimpleExpression>::iterator i = expr.expressions->begin(); i != expr.expressions->end(); ++i){
		
		out << (*i).getField();
		switch((*i).getOperator()){
		case OP_SMATCH:         out << " SMATCH "; 		break;
		case OP_NOSMATCH:       out << " NOSMATCH "; 		break;
		case OP_EQUAL:          out << " EQUAL "; 		break;
		case OP_NOTEQUAL:       out << " NOTEQUAL "; 		break;
		case OP_LESS:           out << " LESS "; 		break;
		case OP_MORE:           out << " MORE "; 		break;
		case OP_META:           out << " META "; 		break;
		default:                out << " BAD OPERATOR ";	break;
		}
		out << (*i).getValue();
		if (std::distance(i, expr.expressions->end()) > 1) /* more rules? */
			out << " AND ";
	}
	return out;
}


