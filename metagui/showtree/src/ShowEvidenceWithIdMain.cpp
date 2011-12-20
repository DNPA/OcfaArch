#include "ShowEvidence.hpp"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/Cgicc.h"

using namespace cgicc;

int main (int , char *[]){

  Cgicc cgi;
  form_iterator metaIter = cgi.getElement("metaid");
  string errMessage("");
  if (metaIter == cgi.getElements().end() || metaIter->getValue() == "" ){

    errMessage += "Sorry, You did not provide a meta id<br>";
  }
  if (errMessage == ""){

    try {
      
      ShowEvidence showEv;
      showEv.showData(metaIter->getValue(), cout);
      return 0;
    } catch (ocfa::misc::OcfaException &e){
      e.logWhat();
      cerr << e.what(); 
    }
  }
}
