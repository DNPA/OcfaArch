
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

#include "ShowEvidence.hpp"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/Cgicc.h"
using namespace cgicc;
int main (int , char *[]){

  Cgicc cgi;

  form_iterator caseIter = cgi.getElement("case");
  form_iterator sourceIter = cgi.getElement("src");
  form_iterator itemIter = cgi.getElement("item");
  form_iterator evidenceIter = cgi.getElement("id");
  form_iterator childIter = cgi.getElement("child");
  string evidenceID;
  string errMessage("");
  if (caseIter == cgi.getElements().end() || caseIter->getValue() == ""){
    
    errMessage += "Sorry, You did not provide an investigation.<br>";
  }
  if (sourceIter == cgi.getElements().end() || sourceIter->getValue() == ""){
    
    errMessage += "Sorry, You did not provide an evidence source.<br>";
  }
  if (itemIter == cgi.getElements().end() || itemIter->getValue() == "" ){

    errMessage += "Sorry, You did not provide an itam.<br>";
  }


  if (evidenceIter == cgi.getElements().end() || evidenceIter->getValue()==""){
    // if not child is given simply assume evidenceid is at the top.
    if (childIter == cgi.getElements().end() || childIter->getValue()== ""){
      
      // cout << "<!--  no child was given -->" << endl;
      evidenceID = "0";
    }
    else {

      // cout << "<!-- finding lastdotloc -->" << endl;
      // remove informatino after last dot of child to get to the father.
      string::size_type theLastDotLoc;
      evidenceID = childIter->getValue();
      theLastDotLoc = evidenceID.find_last_of('.');
      if (theLastDotLoc == string::npos){

	errMessage+="do not undestand  " + evidenceID + "<br/>";
      }
      else {
	
	evidenceID = evidenceID.substr(0, theLastDotLoc);
      }
      //cout << "evidenceID = " << evidenceID << endl; 
    }
   
    
  } else {
    
    evidenceID = evidenceIter->getValue();
  }
  if (errMessage == ""){
    ShowEvidence *showEv;    
    cout << "Content-type: text/xml\n\n";
    cout <<"<?xml version=\"1.0\"?>\n";
    cout << "<?xml-stylesheet type=\"text/xsl\" href=\"/ocfa.xsl\" ?>\n";
    try {
      showEv = new ShowEvidence();
      showEv->showXml(caseIter->getValue(), sourceIter->getValue(), 
		      itemIter->getValue(), evidenceID, cout); 
      delete showEv;
    } catch (ocfa::misc::OcfaException &e){

      e.logWhat();
    }
  }
  else {
    
    cout << cgicc::HTTPHTMLHeader() << endl;
    cout << html() << body();
    cout << h1("Error");
    cout << errMessage << endl;
    cout << body() << html() << endl;
  }
}
