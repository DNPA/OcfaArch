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

#include "misc/Scalar.hpp"

#include <boost/lexical_cast.hpp>
#include <list>
#include <iostream>

namespace router{

Expression::Expression(): OcfaObject("Expression", "router"), expressions(0){
}

Expression::Expression(std::list<SimpleExpression>* expr):OcfaObject("Expression", "router"), expressions(expr) {
}

Expression::Expression(const Expression& e): OcfaObject("Expression", "router"), expressions(0){
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

bool Expression::metaNamesMatch(std::string rulemeta, std::string evidencemeta){
	/* 
    * if last char is * , then the meta names match if the content up to the * is the same 
    */
	if (rulemeta.at(rulemeta.length()-1) == '*'){
		return rulemeta.substr(0, rulemeta.length()-1) == evidencemeta.substr(0, rulemeta.length()-1);
	} else {
		return rulemeta == evidencemeta;
	}
}

bool Expression::evaluate(packet_vector_type& pv){
	if (expressions == 0)
		return false;
	
	std::string lastModule = "";	/* last visited module */
	std::string parentModule = "";	/* module that derived evidence, assumed to be first ModuleInstance */

	/* find lastModule  */
	/* perhaps the (*piPacket).LastMod can be used (but still requires a loop?)*/
	std::vector<packet_type>::iterator  piPacket = pv.end();
	for(; piPacket != pv.begin(); ){ 
		--piPacket;	/* here due to strange vector behaviour */
		if ((*piPacket).Type.compare("ModuleInstance") == 0 && (*piPacket).Value.compare("router") != 0){ /* router doesnt count for last_module */
			lastModule = (*piPacket).Value;
			break; /* we're done */
		}
	}

	/* find parentModule */
	// Assumed to always be the first entry
	piPacket = pv.begin();
	parentModule = (*piPacket).Value;
	

	/* for each part of the expression in the rule:
	 *  check meta data according to operator in rule (grouped as in first switch statement)
	 *  1. the META operator simply loops through all expressions looking at meta data
	 *  2. The negating operators (NOSMATCH and NOTEQUAL) requires all meta data of right types to match rule
	 *  3. The other operators requires at least one meta data to match rule
	 * 
	 * The SimpleExpression evaluation is not used because there must be quite much operator and value specific information 
	 *  here (in Expression) so the SimpleExpression evaluation function contains almost the same code. 
	 */
	std::list<SimpleExpression>::iterator expr = expressions->begin();
	for(; expr != expressions->end(); ++expr){
		
		if ((*expr).getField() == "DNTCR")
			continue;	/* skip this expression as it is always true */

		/* Operator specific evaluation */
		switch ((*expr).getOperator()){
		case OP_META: { 
			/* META is handled by itself because handling is dependant on <exists/not_exists> 
			 * and cannot be put in just one of the different handlers below */
			bool has_meta_type = false;
			if ((*expr).getField() == "previous_module" ||	/* these 'specials' always exist */
					(*expr).getField() == "parent_module" || 
					(*expr).getField() == "last_module" ){
				return ((*expr).getValue() == "exists");
			}
			
			for(piPacket = pv.begin(); piPacket != pv.end() && has_meta_type == false; ++piPacket) {
//				std::cerr << (*expr) << " -1 " << (*piPacket).Type << " " << (*piPacket).Value << "\n";
//				if ((*expr).getField() == (*piPacket).Type ){
				if (metaNamesMatch((*expr).getField(), (*piPacket).Type)){
//					std::cerr << "\t type match\n";
					has_meta_type = true;
				}
			}

			if (! (((*expr).getValue() == "exists" && has_meta_type) || ((*expr).getValue() == "not_exists" && !has_meta_type)))
				return false; 
		}
			break;
		case OP_NOSMATCH:
		case OP_NOTEQUAL: /* ALL rules with the same type of meta data MUST evaluate to true */ 
			/* first check special expressions */
			if ((*expr).getField() == "parent_module"){
				if (parentModule == (*expr).getValue())
					return false;
			} else if ((*expr).getField() == "last_module"){
				if (lastModule == (*expr).getValue())
					return false;
			}else{	
				/* and then check the other kinds of expressions */
				bool simple_expr_match = true;
				for(piPacket = pv.begin(); piPacket != pv.end() && simple_expr_match == true; ++piPacket) {
//					std::cerr << (*expr) << " -2 " << (*piPacket).Type << " " << (*piPacket).Value << "\n";
					if (metaNamesMatch((*expr).getField(), (*piPacket).Type) || ((*expr).getField() == "previous_module"  && (*piPacket).Type == "ModuleInstance")){
//						std::cerr << "\t type match\n";
						
						switch ((*expr).getOperator()){
						case OP_NOSMATCH: 
							if (!((*piPacket).Value.find((*expr).getValue()) == std::string::npos))
								simple_expr_match = false;
							break;
						case OP_NOTEQUAL: /* Oscar: Perhaps this should be integer comparison? */
							if (!(boost::lexical_cast<float>((*piPacket).Value) != boost::lexical_cast<float>((*expr).getValue())))
								simple_expr_match = false;
							break;
						}
					}
				}
			
				if (simple_expr_match == false)
					return false;	// One simple expr failed. rule does not match (lazy evaluation)
			}
			break;
		case OP_SMATCH:
		case OP_EQUAL:
		case OP_LESS:
		case OP_MORE: /* At least ONE meta data must evaluate to true */
			/* first check special expressions */
			if ((*expr).getField() == "parent_module"){
				if (parentModule != (*expr).getValue())
					return false;
			} else if ((*expr).getField() == "last_module"){
				if (lastModule != (*expr).getValue())
					return false;
			}else{
				/* and then check the other kinds of expressions */
				bool simple_expr_match = false;
				for(piPacket = pv.begin(); piPacket != pv.end() && simple_expr_match == false; ++piPacket) {
//					std::cerr << (*expr) << " -3 " << (*piPacket).Type << " " << (*piPacket).Value << "\n";
					if (metaNamesMatch((*expr).getField(), (*piPacket).Type) || ((*expr).getField() == "previous_module"  && (*piPacket).Type == "ModuleInstance")) {
//						std::cerr << "\t type match\n";
						switch((*expr).getOperator()){
						case OP_SMATCH:
							if (((*piPacket).Value.find((*expr).getValue()) != std::string::npos))
								simple_expr_match = true;
							break;
						case OP_EQUAL:	/* Oscar: Perhaps this should be integer comparison? */
							if (boost::lexical_cast<float>((*piPacket).Value) == boost::lexical_cast<float>((*expr).getValue()))
								simple_expr_match = true;
							break;
						case OP_LESS:
							if (boost::lexical_cast<float>((*piPacket).Value) < boost::lexical_cast<float>((*expr).getValue()))
								simple_expr_match = true;
							
							break;
						case OP_MORE: 
							if (boost::lexical_cast<float>((*piPacket).Value) > boost::lexical_cast<float>((*expr).getValue()))
								simple_expr_match = true;
							break;
						}
					}
				}
			
				if (simple_expr_match == false)
					return false;	// all simple expr has failed
			}
			break;
		default:
			getLogStream(ocfa::misc::LOG_EMERG) << "Bad operator\n";
			return false;
		}
	}
	return true;
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


}
