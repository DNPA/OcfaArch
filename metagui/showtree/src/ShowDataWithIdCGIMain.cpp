#include "ShowEvidence.hpp"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/Cgicc.h"

using namespace cgicc;

int main (int , char *[]){

  Cgicc cgi;
  form_iterator dataIter = cgi.getElement("dataid");
  form_iterator mimetypeIter = cgi.getElement("mime");
  string errMessage("");
  
  if (dataIter == cgi.getElements().end() || dataIter->getValue() == "" ){

    errMessage += "Sorry, You did not provide a dataId.</br>";
  }
  if (errMessage == ""){

    try {

     cout << "Content-type: " << mimetypeIter->getValue() << "\n\n";     
      ShowEvidence showEv;
      showEv.showData(dataIter->getValue(), cout);
      return 0;
    } catch (ocfa::misc::OcfaException &e){
      e.logWhat();
      cerr << e.what(); 
    }
  } else {

    cout << "Content-type: text/html" << endl;
    cout << "<html><body><h1>" << errMessage << "</h1></body></html>";
    cout << endl;
  }
}
