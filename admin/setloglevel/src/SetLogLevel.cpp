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
using namespace std;
int main(int argc, char *argv[]){

  if (argc < 4){

    cerr << "Usage: <namespace> <modulename> <prefix> <debug | info | warning | err | emerg>" << endl;
  }
  else {
    
    string moduleNameSpace = argv[0];
    string moduleName = argv[1];
    string prefix = argv[2];
    string logLevel = argv[3];

    try {
      MessageBox *box = MessageBox::createInstance("setloglevel", "admin");
      
      
      if (box == 0){
	
	cerr << "cannot initialize messagebox" << endl;
      } else {
	
	string content = prefix + " " + logLevel;
	OcfaConfig::Instance()->baptize(box->getModuleInstance());
	Message *message = 0;
	ocfa::misc::ModuleInstance receiver("localhost",moduleName, moduleNameSpace, "all");
	// TODO set priority.
	cerr << "Messagetype is " << Message::mtHalt << endl;
	box->createMessage(&message, &receiver, Message::ANYCAST, Message::mtSystem, "SetLogLevel",  content, 0);
	// RJM:CODEREVIEW sugestion: instead of the abouve use:
	// box->createSetLogLevelMessage(&message,prefix,logLevel,moduleName)
	cerr << "Message->getType() is " << message->getType() << endl;
	box->sendMessage(*message);
	cerr << "message sent" << endl;
	delete box;
      }
    } catch(ocfa::misc::OcfaException e){
      cerr << "Exception" << e.what() << endl;
    }
  }
}
