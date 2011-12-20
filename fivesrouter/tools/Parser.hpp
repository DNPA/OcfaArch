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




#ifndef PARSER_HPP
#define PARSER_HPP

#include "Expression.hpp"
#include "SimpleExpression.hpp"
#include "Operator.hpp"
#include "Lexer.hpp"
#include "Rule.hpp"

#include <string>
#include <list>
#include <map>
#include <vector>


class Parser {
	public:
	Parser(std::string filename);
	~Parser();

	/* commence a parse of the file passed to constructor */
	int parse();

	/* will try to identify what version the rulelist is */
	/* Version 0 means unknown
	 * Version 1 is identified by not having VERSION: in beginning
	 * Version 2 and above has VERSION: <version> in beginning and 
		<version> is returned here
	 */
	static int identifyVersion(std::string filename);

	private:

	
	bool match(int token, const char* rulename);

	int rulelist();
	int rulelistline();	//perhaps all should be bool?
	int rule();
	bool label();
	Expression* expression();
	SimpleExpression simple_expression();
	std::list<SimpleExpression> simple_expression_list();
	enum Operator op();
	std::string action();

	/*
	 * Returns "" upon error, otherwise the ID
	 */
	std::string id();
	std::string id_list();

	Lexer* lexer;


	unsigned int mNextRuleNumber; /* the identifier for the next rule */
	std::vector<Rule *> mRules; 
	std::map<const std::string, unsigned int> mLabelMap;  /* labelname -> rulenumber */
	std::vector<std::string> mUsedLabels; /* labels for which there are ACT_JUMP to (used for sanity checking) */
	int mRuleListVersion;


};


#endif // PARSER_HPP
