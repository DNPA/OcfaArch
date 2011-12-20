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


#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "SimpleExpression.hpp"

#include <list>
#include <string>


class Expression {
public:
	/* Empty expression is false */
	Expression();	
	/* Expression will handle deletion of the list! */	
	Expression(std::list<SimpleExpression>*);
	Expression(const Expression&);
	~Expression();

	Expression& operator=(const Expression&);

	void addSimpleExpression(SimpleExpression);
	void addSimpleExpression(std::list<SimpleExpression>);
	std::list<SimpleExpression>* getSimpleExpressions() { return expressions; }

	friend std::ostream& operator<<(std::ostream& out, Expression& expr);
private:
	bool metaNamesMatch(std::string rulemeta, std::string evidencemeta);
	void copy(const Expression&);
	void free();

	std::list<SimpleExpression>* expressions;

};


#endif /* EXPRESSION_H */
