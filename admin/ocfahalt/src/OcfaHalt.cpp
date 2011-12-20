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

#include "message/MessageBox.hpp"
using namespace ocfa::message;
using namespace ocfa::misc;

void usage(){
  cout << "Usage: ./ocfahalt <caseid>" << endl;
  exit(1);
}
int main(int argc, char *argv[]){

  try {
    if (argc < 2) usage();
    
    char *ocfacaseenv=getenv("OCFACASE");
    if (ocfacaseenv == 0){

      setenv("OCFACASE", argv[1], 1);
    }
    else {

      if (strcmp(ocfacaseenv, argv[1])){
        
	cerr << "ERROR: The OCFACASE enviroment variable is set to '" << ocfacaseenv << "' while the case specified is '" << argv[1] << "\n";
	return 1;
       
      }
    }
    MessageBox *box = MessageBox::createInstance("stopwash", "admin");
    OcfaConfig::Instance()->baptize(box->getModuleInstance());
    OcfaLogger::Instance()->baptize(box->getModuleInstance());

    if (box == 0){
      
      cerr << "cannot initialize messagebox" << endl;
    } else {
      
      Message *message = 0;
      ocfa::misc::ModuleInstance receiver("all", "all", "all", "all");
      // TODO set priority.
      cerr << "Messagetype is mtHalt" << endl;
      box->createMessage(&message, 0, Message::BROADCAST, Message::mtHalt, "subject",  "blahbalh", 0);
      // RJM:CODEREVIEW sugestion: instead of the abouve use :
      // box->createHaltMessage(&message);
      //cerr << "Message->getType() is " << message->getType() << endl;
      box->sendMessage(*message);
      cerr << "message sent" << endl;
      delete box;
    }
  } catch(ocfa::misc::OcfaException e){
    cerr << "Exception" << e.what() << endl;
  }
  
}
