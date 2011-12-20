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

#include <message/AnycastMessageBox.hpp>
#include <misc/ModuleInstance.hpp>
#include <misc/OcfaLogger.hpp>
#include <misc/OcfaConfig.hpp>
#include"msgidbroker.hpp"
#include"anycast.hpp"
#include"ACE.h"

using namespace ocfa::message;
using namespace ocfa::misc;

// daemonize checks whether the logger needs stdio.
// if it does, the std file descriptors are not closed.
void daemonize(){
  pid_t pid;

  pid = fork();
  if (pid < 0){
    throw OcfaException ("fork failed");
  }
  else if (pid != 0) {
    exit(0);
  }

  setsid();
  // close std and stdout if the logger does not need them
  if (!OcfaLogger::Instance()->needsStdIO()){
    close(0);
    close(1);
    close(2);
  } else {
	  OcfaLogger::Instance()->syslog(LOG_NOTICE) << "Keeping std filedescriptors open because of Logger" << endl;
  }
  umask(022);
  chdir("/");
}


int main(int argc, char *argv[]){
  try {
    // JBS little trick. Baptize the logger with a nice module
    // instance so that the logging will be focused.
    
    ModuleInstance myself("localhost", "anycast", "core", "theonlyone");
    OcfaLogger::Instance()->baptize(&myself);
    OcfaGroup groupguard("ocfa");
    Anycast::AnyCast *a;
    
    if (OcfaConfig::Instance()->getValue("daemonize") != "false"){
      daemonize();
    }
    if (OcfaConfig::Instance()->getValue("recover") == "true"){
      a = new Anycast::AnyCast("anycast", "anycast", true); 
    } else {
      a = new Anycast::AnyCast("anycast", "anycast", false);
    }
    AnycastMessageBox *mb = dynamic_cast<AnycastMessageBox *>(MessageBox::createInstance("anycast", "anycast"));
    if (!mb){
      OcfaLogger::Instance()->syslog(LOG_ERR, "main ") << "MessageBox not expected type !" << endl;
      return 1;
    }
    Anycast::MonitorMessageFactory *factory = new Anycast::MonitorMessageFactory(mb);
    a->setMonitorMessageFactory(&factory);
    Message *msg = 0;
    bool haltreceived = false;
    while ((!haltreceived) || (a->getActiveModuleCount() > 0)){
      
      msg = mb->getNextMessage(100, 0);
      
      if (msg != 0){
	ModuleInstance *sender = msg->getSender();  
	string instanceuri = sender->getInstanceURI();
	string modulename = sender->getModuleName();
	a->getLogStream(LOG_INFO) << "Received message type: " << static_cast<int>(msg->getType()) 
				  << "and prio " << msg->getPriority() << endl;
	switch (msg->getType()){
	case Message::mtTaskProgress:{
	  a->getLogStream(LOG_INFO) << "Message::mtTaskProgress" << endl;
	  ModuleInstance *receiver = msg->getReceiver();
	  string instreceiver = receiver->getInstanceURI();
	  string instmodname = receiver->getModuleName();
	  unsigned int id = atoi(msg->getSubject().c_str());
	  a->dropMessage(instmodname, instreceiver, id);
	  if (haltreceived){
	    break; // stop sending new messages when halt is received
	  }
	  Message *tosend = a->getMessage(instmodname);
	  id = MsgIDBroker::Instance()->lastID();
	  if (tosend != 0){
	    mb->sendTask(tosend, id); // the message 'tosend' will be deleted if task is done
	  }
	  break;
	};		   
	case Message::mtModuleInstance:{
	  a->getLogStream(LOG_INFO) << "Message::mtModuleInstance" << endl;
          OcfaLogger::Instance()->syslog(ocfa::misc::LOG_INFO, "Anycast: ") << "Received mtModuleInstance: " << instanceuri << endl;
          // copy sender because it will be used and stored by anycast
          ModuleInstance *copysender = new ModuleInstance(sender);
	  a->addModuleInstance(modulename, instanceuri, copysender);
	  Message *tosend = 0;
	  while ((tosend = a->getMessage(modulename))!= 0){
	    unsigned int id = MsgIDBroker::Instance()->lastID();
	    mb->sendTask(tosend, id); // the message 'tosend' will be deleted if task is done
	  }
	  a->printQueues();
	  break;
	};
	case Message::mtModuleDisconnect:{
	  a->getLogStream(LOG_INFO) << "Message::mtModuleDisconnect" << endl;
	  a->removeModuleInstance(modulename, instanceuri);
	  a->printQueues();
	  break;
	}
	case Message::mtHalt: {
	  OcfaLogger::Instance()->syslog(ocfa::misc::LOG_NOTICE, "Anycast: ") << "Received HALT. Waiting for " << a->getActiveModuleCount() << " modules to go away ..." << endl;
       
	  haltreceived = true;
	  break;
	}
	case Message::mtRecover: {//FIXME: think we need to do something here.
	  a->getLogStream(LOG_INFO) << "Message::mtRecover" << endl;   
	  break;
	}
	case Message::mtEvidence:{
	  a->getLogStream(LOG_INFO) << "Message::mtEvidence" << endl;
	}
	case Message::mtSubscribe:
	case Message::mtUnsubscribe:
	case Message::mtEOC:
	case Message::mtHeartBeat:
	case Message::mtSystem:			 
	  // all other messages should probably be relayed
	  a->getLogStream(LOG_INFO) << "Processable message" << endl;
	  ModuleInstance *recepient = msg->getReceiver();
	  string rcptmodulename = recepient->getModuleName(); // modulename is used to denote moduletype
	  a->addMessage(rcptmodulename, msg);
	  if (haltreceived){
	    a->getLogStream(LOG_INFO) << "HALT STATE: dropping message" << endl;
	    break; // if we receive a halt, we stop sending new tasks
	  }
	  Message *tosend = a->getMessage(rcptmodulename);
	  unsigned int id = MsgIDBroker::Instance()->lastID();
	  a->printQueues();
	  if (tosend != 0){
	    a->getLogStream(LOG_INFO) << "Calling sendTask on messagebox" << endl;
	    mb->sendTask(tosend, id); // the message 'tosend' will be deleted if task is done
	  } else {
            a->getLogStream(LOG_INFO) << "Problem getting at message" << endl;
	  }
	  // Check whether there are any messages for the monitor. This is one of the times that the
	  // anycast cannot simple react on new messages. 
	  tosend = a->getMessage("monitor");
	  if (tosend != 0){
	    unsigned int id = MsgIDBroker::Instance()->lastID();
	    a->printQueues();
	    if (tosend != 0){
	      a->getLogStream(LOG_INFO) << "Calling sendTask on messagebox" << endl;
	      mb->sendTask(tosend, id); // the message 'tosend' will be deleted if task is done
	    } else {
	      a->getLogStream(LOG_INFO) << "Problem getting at message" << endl;
	    }
	  }
	}
	delete msg;
        msg = 0;
      }
    }

    a->Log( "Exiting the Anycast module on user request!");

    delete a;
    OcfaLogger::Instance()->syslog(LOG_DEBUG, "main: ") << "Deleted AnyCast object. Deleting MessageBox .." << endl;
    delete mb;
    OcfaLogger::Instance()->syslog(LOG_DEBUG, "main: ") << "MessageBox deleted. Anycast done !" << endl;
  } catch (OcfaException &e) {
    std::cerr << "Exception: " << e.what() << endl;
    //e.logWhat();
    return -1;
  } catch (exception &e) {
    OcfaLogger::Instance()->syslog(LOG_ERR, "main ") << "Exception: " << e.what() << endl;
    return -1; 
  }


  return 0;
}


