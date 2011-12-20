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


#include "Lexer.hpp"
#include "Tokens.hpp"

#include <limits>
#include <strings.h>

//#include <misc/OcfaException.hpp>


Lexer::Lexer(const std::string &filename) : token(0), lexeme() {
	file.open(filename.c_str(), std::ifstream::in);
	if (!file){
		throw "Cannot open file";
	}
	linenumber = 1;
	next();
}

Lexer::~Lexer(){
	file.close();
}

int Lexer::getToken(){
	return token;
}

std::string Lexer::getLexeme(){
	return lexeme;
}

int Lexer::next(){
	char c;
	
	if (file.bad())
		return -1;

	lexeme.erase();
	
	
	/* discard spaces, newlines and other unwanted stuff */
	c = file.peek();
	while(c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '#' || c == EOF){
		if( c == ' ' || c == '\r' || c == '\t'){
			file.get(c);
		}else if (c == '\n'){
			linenumber++;
			file.get(c);
		}else if (c == '#'){ 
			file.ignore(std::numeric_limits<int>::max(),'\n');
			linenumber++;
		}else if (c == EOF){
			token = TOK_END_OF_FILE;
			return -1;
		}

		c = file.peek();
	}	
	
	/* read lexeme */
	if (c == ';'){	/* ';' is the only token consisting of one character and is not correctly read by the loop */
		file.get(c);
		lexeme.push_back(c);
	}else{
		do {
			file.get(c);
			lexeme.push_back(c);
			c = file.peek();
		}while( c != ' ' && c != ';' && c != '\n' && c != '\r' && c != '\t' && c != EOF);
	}

	const char* lxm = lexeme.c_str();

	if (strcasecmp(lxm, "smatch") == 0)			token = TOK_SMATCH;
	else if (strcasecmp(lxm, "nosmatch") == 0)		token = TOK_NOSMATCH;
	else if (strcasecmp(lxm, "equal") == 0)			token = TOK_EQUAL;
	else if (strcasecmp(lxm, "notequal") == 0)		token = TOK_NOTEQUAL;
	else if (strcasecmp(lxm, "less") == 0)			token = TOK_LESS;
	else if (strcasecmp(lxm, "more") == 0)			token = TOK_MORE;
	else if (strcasecmp(lxm, "meta") == 0)			token = TOK_META;
	else if (strcasecmp(lxm, "and") == 0)			token = TOK_AND;
	else if (strcasecmp(lxm, "act_jump") == 0)		token = TOK_ACT_JUMP;
	else if (strcasecmp(lxm, "act_commit") == 0)		token = TOK_ACT_COMMIT;
	else if (strcasecmp(lxm, "act_forward") == 0)		token = TOK_ACT_FORWARD;
	else if (strcasecmp(lxm, ";") == 0)			token = TOK_SEMICOLON;
//	else if (strcasecmp(lexeme, ":") == 0)			token = TOK_COLON;
	else if (strcasecmp(lxm, "version:") == 0)		token = TOK_VERSION;
	else if (isalnum(lxm[0]) &&	/* I actually don't remember if there is any constraint or why lxm[0] should be alphanumeric. */ 
			lexeme[lexeme.length()-1] == ':')	token = TOK_LABEL;
	else token = TOK_ID;
	
	return 0;
}

