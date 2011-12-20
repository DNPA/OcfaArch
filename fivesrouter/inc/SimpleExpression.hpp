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




#ifndef SIMPLEEXPRESSION_H
#define SIMPLEEXPRESSION_H

#include "Operator.hpp"
#include "common/packet_type.hpp"

#include <misc/OcfaException.hpp>

#include <string>

namespace router{

class SimpleExpression: public ocfa::OcfaObject{
public:
	SimpleExpression(std::string field, enum Operator op, std::string value): OcfaObject("simple expression", "router"), mField(field), mOperator(op), mValue(value){
	}
	
	enum Operator getOperator(){ return mOperator; }
	std::string getField(){ return mField; }
	std::string getValue(){ return mValue; }

private:
	std::string mField;
	enum Operator mOperator;
	std::string mValue;
};

}
#endif /* SIMPLEEXPRESSION_H */
