#include "ShowEvidence.hpp"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/Cgicc.h"

using namespace cgicc;

int main (int , char *[]){

  Cgicc cgi;
  form_iterator metaIter = cgi.getElement("metaid");
  form_iterator mimetypeIter = cgi.getElement("mime");
  string errMessage("");
  if (metaIter == cgi.getElements().end() || metaIter->getValue() == "" ){

    errMessage += "Sorry, you did not provide a metaId.</br>";
  }
  if (errMessage == ""){

    try {

     cout << "Content-type: " << mimetypeIter->getValue() << "\n\n";     
      ShowEvidence showEv;
      showEv.showData(metaIter->getValue(), cout);
      return 0;
    } catch (ocfa::misc::OcfaException &e){
      e.logWhat();
      cerr << e.what(); 
    }
  }
}
