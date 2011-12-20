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



#ifndef OPERATOR_HPP
#define OPERATOR_HPP

enum Operator {
	OP_SMATCH = 0,
	OP_NOSMATCH,
	OP_EQUAL,
	OP_NOTEQUAL,
	OP_LESS,
	OP_MORE,
	OP_META,
	OP_BAD_OPERATOR  /* used only for signalling error in operator parsing */
};

#endif /* OPERATOR_HPP */
