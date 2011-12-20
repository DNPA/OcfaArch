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





#include "Parser.hpp"

#include <misc/syslog_level.hpp>
#include <OcfaObject.hpp>
#include <csv/RuleList.hpp>
#include <common/Rule.hpp>
#include <misc/Scalar.hpp>

#include "Expression.hpp"
#include "SimpleExpression.hpp"
#include "Operator.hpp"

#include "Tokens.hpp"

#include <cstdarg>
#include <stdio.h>
#include <string>

namespace router{

Parser::Parser(std::string filename): OcfaObject("Parser", "router"), lexer(0) {
	lexer = new Lexer(filename);
}

Parser::~Parser(){
	delete lexer; 
	lexer = 0;
}

int Parser::parse(){
	if (rulelist())
		return 1;

	if (lexer->next() == 0){
		getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ": symbols after end of parse, " <<  lexer->getLexeme().c_str() << std::endl;
		return 1;
	}

	if (!router::RuleList::getInstance()->has_rule()){
		getLogStream(ocfa::misc::LOG_EMERG) << "No rules in rulelist!\n";
		return 1;
	}

	return 0;
}

int Parser::identifyVersion(std::string filename){
	Lexer l(filename);
	int version = 0;
	if (l.getToken() == TOK_VERSION){
		l.next();
		if (l.getToken() == TOK_ID)
			version = ocfa::misc::Scalar(l.getLexeme()).asInt();
	}else
		version = 1;

	return version;
}

/*
 * Returns true if good match, otherwise false
 */
bool Parser::match(int token, const char* rulename){
	if (lexer->getToken() != token){
		getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ": Error: parse error in " << rulename << ", unexpected '" << lexer->getLexeme() << "'\n";
		exit(1);
	}
	getLogStream(ocfa::misc::LOG_DEBUG) <<lexer->getLinenumber() << ": " << lexer->getLexeme() << " in " << rulename << " OK\n";
	lexer->next();
	return true;
}

int Parser::rulelist(){
	int ret = 0;

	if (lexer->getToken() == TOK_VERSION){
		match(TOK_VERSION, "rulelist");
		match(TOK_ID, "rulelist");	/* this should be strictly numerical */
	}
	
	ret += rulelistline();	
	while (lexer->getToken() == TOK_ID || lexer->getToken() == TOK_LABEL)
		ret += rulelistline();
	return ret;
}

int Parser::rulelistline(){

	switch(lexer->getToken()){
	case TOK_ID:		return rule();		break;
	case TOK_LABEL:		return !label();	break; /* label returns true upon match */
	default:
		getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ": Error: parse error in rulelistline, unexpected '" << lexer->getLexeme() << "'\n";
		return 1;
		break;
	}
	
	return 0;
}

int Parser::rule(){
	Expression *expr;
	std::string act;
	std::string target;
	
	expr = expression();
	match(TOK_SEMICOLON, "rule, expr part");
	act = action();
	match(TOK_SEMICOLON, "rule, action part");
	target = id();	
	match(TOK_SEMICOLON, "rule, target part");
	
	if ( act == "" || target == "" || expr == 0){
		getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ": Bad rule " << act.c_str() << " " << target.c_str() << std::endl;
		return 1;
	}
	
	RuleList::getInstance()->addRule( 
            new router::Rule(expr,
                             act,
                             target));
	return 0;
}

bool Parser::label(){
        if (lexer->getToken() == TOK_LABEL){
            char* lbl = strdup(lexer->getLexeme().c_str());
            lbl[strlen(lbl)-1] = 0;  /* get rid of ':' */
            router::RuleList::getInstance()->addLabel(lbl);
	}
	return match(TOK_LABEL, "label"); 
}

Expression* Parser::expression(){
	Expression* e = new Expression();
	e->addSimpleExpression( simple_expression() );
	e->addSimpleExpression( simple_expression_list());
	return e;
}

std::list<SimpleExpression> Parser::simple_expression_list(){
	std::list<SimpleExpression> se;
	if (lexer->getToken() == TOK_AND){
		match(TOK_AND, "expression_list_aux");
		se.push_back(simple_expression());
		std::list<SimpleExpression> exprlist (simple_expression_list());
		
		/* copy exprlist into se as merge requires operator< or something */
		for (std::list<SimpleExpression>::iterator i = exprlist.begin();
				i != exprlist.end();
				++i)
			se.push_back(*i);
	}
	return se;
}

SimpleExpression Parser::simple_expression(){
	std::string field = id();
	enum Operator oper = op();
	std::string value = id_list();
	if (field == "" || oper == OP_BAD_OPERATOR || value == ""){
		getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ": Broken expression '" << field << "' '" << oper << "' '" << value <<"'\n"; 
		exit(1);
	}
	return SimpleExpression(field, oper, value);
}

enum Operator Parser::op(){
	switch(lexer->getToken()){
	case TOK_SMATCH:	match(TOK_SMATCH, "operator");	return OP_SMATCH;
	case TOK_NOSMATCH:	match(TOK_NOSMATCH, "operator");return OP_NOSMATCH;
	case TOK_EQUAL:		match(TOK_EQUAL, "operator");	return OP_EQUAL;
	case TOK_NOTEQUAL:	match(TOK_NOTEQUAL, "operator");return OP_NOTEQUAL;
	case TOK_LESS:		match(TOK_LESS, "operator");	return OP_LESS;
	case TOK_MORE:		match(TOK_MORE, "operator");	return OP_MORE;
	case TOK_META:		
		match(TOK_META, "operator");	
		if (lexer->getLexeme() != "exists" && lexer->getLexeme() != "not_exists")
			getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ":  Operator META used but without exists/not_exists as operand which is required for the META operator. \n";
			
		return OP_META;
	default:
		getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ": unexpected token '" <<lexer->getLexeme() << "'\n";
		return OP_BAD_OPERATOR;
		break;
	}
}

/*
 * returns the final action or empty string upon error
 */
std::string Parser::action(){
	switch(lexer->getToken()){
	case TOK_ACT_JUMP:	match(TOK_ACT_JUMP, "action");		return "ACT_JUMP";
	case TOK_ACT_FORWARD:	match(TOK_ACT_FORWARD, "action");	return "ACT_FORWARD";
	case TOK_ACT_COMMIT:	match(TOK_ACT_COMMIT, "action");	return "ACT_COMMIT";
	default:
		getLogStream(ocfa::misc::LOG_EMERG) << lexer->getLinenumber() << ": unexpected token '" <<lexer->getLexeme() << "'\n";
		return "";
		break;
	}
}


/* returns empty string upon mismatch */
std::string Parser::id(){
	std::string idstring(lexer->getLexeme());
	if (match(TOK_ID, "id"))
		return idstring;
	else
		return "";
}

/* XXX: ugly hack to allow "Microsoft Office Document" (without ") */
std::string Parser::id_list(){
	bool is_multiword = false;
	std::string idstring( id());
	while(lexer->getToken() == TOK_ID){
		idstring += " " + id();
		is_multiword = true;
	}
	if (is_multiword)
		getLogStream(ocfa::misc::LOG_WARNING) << lexer->getLinenumber() << ": Error prone multiword identifier ("<< idstring << ")\n";
	return idstring;
}

} /* namespace router */
