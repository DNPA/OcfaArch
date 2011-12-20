
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
  form_iterator metaIter = cgi.getElement("metaid");
  form_iterator xslIter = cgi.getElement("xsl");
  string metaId;
  string errMessage("");
  string xslSheet;
  if (metaIter == cgi.getElements().end() || metaIter->getValue() == "" ){

    errMessage += "Sorry, You did not provide a meta id.</br>";
  }
  if (xslIter == cgi.getElements().end() || xslIter->getValue() == ""){

    xslSheet = "/ocfa.xsl";
  }
  else {
    
    xslSheet = xslIter->getValue();
  }
  if (errMessage == ""){
    ShowEvidence *showEv;    
    cout << "Content-type: text/xml\n\n";
    cout << "<?xml version=\"1.0\"?>\n";
    cout << "<?xml-stylesheet type=\"text/xsl\" href=\"" << xslSheet <<  "\" ?>\n";
    try {

      showEv = new ShowEvidence();
      showEv->showXml(metaIter->getValue(), cout); 
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
