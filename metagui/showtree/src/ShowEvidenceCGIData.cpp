
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
#include <store/AbstractRepository.hpp>
#include <sstream>
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
  form_iterator mimetypeIter = cgi.getElement("mime");
  form_iterator charsetIter = cgi.getElement("charset");
  string evidenceID;
  string errMessage("");
  if (caseIter == cgi.getElements().end() || caseIter->getValue() == ""){
    
    errMessage += "Sorry, you did not provide an investigation,<br>";
  }
  if (sourceIter == cgi.getElements().end() || sourceIter->getValue() == ""){
    
    errMessage += "Sorry, You did not provide an evidence source.<br>";
  }
  if (itemIter == cgi.getElements().end() || itemIter->getValue() == "" ){

    errMessage += "Sorry, You did not provide an item.</br>";
  }

  if (evidenceIter == cgi.getElements().end() || evidenceIter->getValue()==""){
    
    // if not child is given simply assume evidenceid is at the top.
    
    evidenceID = "0.j0e0";
    
  }
  else {

    evidenceID = evidenceIter->getValue();
  }
  if (errMessage == ""){
    std::string mimetype=mimetypeIter->getValue();
    if ((mimetype.substr(0,7) == "text/x-") ||(mimetype == "")) {
       mimetype="text/plain";
    }
    if (!(charsetIter == cgi.getElements().end() || charsetIter->getValue() == "")){
      std::string charset=charsetIter->getValue();
      mimetype += "; charset=" + charset + "";
    }
    cout << "Content-type: " << mimetype << "\n\n";
    ShowEvidence *showEv;    
    showEv = new ShowEvidence();
    showEv->showData(caseIter->getValue(), sourceIter->getValue(), 
		     itemIter->getValue(), evidenceID, cout); 
    delete showEv;
  }
  else {
    cout << cgicc::HTTPHTMLHeader() << endl;
    cout << html() << body();
    cout << h1("Error");
    cout << errMessage << endl;
    cout << body() << html() << endl;
  }
}
