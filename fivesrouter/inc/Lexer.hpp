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



#ifndef LEXER_HPP
#define LEXER_HPP

#include <OcfaObject.hpp>

#include <misc/OcfaException.hpp>

#include <fstream>
#include <string>

namespace router{

class Lexer : public ocfa::OcfaObject {

	public:
	Lexer(const std::string &filename) throw (ocfa::misc::OcfaException);
	virtual ~Lexer();

	/* getToken() and getLexeme() may be called multiple times and 
	 * will return the same token/lexeme as long as next() is not called
	 */

	// returns the numeric identifier for the next token
	int getToken();

	// returns the text that corresponds to the token
	std::string getLexeme();

	// steps forward to the next token/lexeme
	// returns -1 upon error (such as EOF), otherwise 0
	int next();
	
	unsigned int getLinenumber(){	return linenumber; }

	private:
	
	int token;
	std::string lexeme;
	unsigned int linenumber;

	std::ifstream file;
};

}; /* namespace router */

#endif /* LEXER_HPP */
