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

#include"anycast.hpp"
#include"msgidbroker.hpp"
#include<sstream>
#include <message/Serialize.hpp>
#include <misc/syslog_level.hpp>
#include <misc/OcfaConfig.hpp>
#include <sys/types.h>
#include <dirent.h>

namespace Anycast {

  using namespace std;
  using namespace ocfa::misc;

  unsigned int ModuleInstance::_currmsgid = 0;


  ModuleInstance::ModuleInstance(string instname, ocfa::misc::ModuleInstance *minst):
	  OcfaObject("AnycastModInstance", instname),
	  _max_outstanding(10),
	  _msgids(),
	  _instname(instname), 
	  _minst(minst)
  {
    ocfaLog(LOG_DEBUG, "Constructed");
  }

  bool ModuleInstance::isFull(){
    return _msgids.size() >= _max_outstanding;
  }

  void ModuleInstance::dropMessage(unsigned int id){
    map<unsigned int, Message *>::iterator toremove = _msgids.find(id);
    if (toremove != _msgids.end()){
      getLogStream(LOG_DEBUG) << "Found ID to remove" << endl;
      delete(_msgids[id]);
      _msgids.erase(toremove);
    } else {
      getLogStream(LOG_ERR) << "ID not found " << endl;
    }
  }


  // This method returns the 'oldest' (the one with the smallest id)
  // from the nack queue. 
  // If a module crashes we suspect that one to contain the offending evidence
  Message *ModuleInstance::getOldestNACK(){
    return getNACKMessages(); 
  }


  Message *ModuleInstance::getNACKMessages(){
    if (_msgids.size() == 0)
      return 0;
     map<unsigned int, Message *>::iterator toremove = _msgids.begin();
     Message *tmp = toremove->second;
     _msgids.erase(toremove);
     return tmp;
  }

  // Message is rewritten and tagged as being sent by this instance
  Message *ModuleInstance::assignMessage(Message *msg){
    msg->setReceiver(_minst); 
    msg->setCastType(Message::UNICAST);
    _msgids[MsgIDBroker::Instance()->getID()] = msg;
    return msg;
  }

  unsigned int ModuleInstance::queueSize(){
    //return _msgqueue.size();
    return _msgids.size();
  }


  ModuleType::ModuleType(string mtname, bool recover): 
	  OcfaObject("AnyCastModType", mtname),
	  _lastix(),
	  _instances(),
	  _instancevec(),
	  _lastinstanceix(0),
	  _mtname(mtname),
	  _ppq(0),
	  serializeMessage(getMessageConverter()),
	  deserializeMessage(getMessageConverter())

  {
    ocfaLog(LOG_DEBUG, "Constructor");
    string ppqRoot = OcfaConfig::Instance()->getValue("ppqrootdir");
    if (ppqRoot == "" || ppqRoot[0] != '/' || ppqRoot.size() < 3){

      getLogStream(ocfa::misc::LOG_WARNING) << "ppqRoot was empty or invalid, now storing stuff in /tmp/ppq" 
					    << endl;
      ppqRoot = "/tmp/ppq";
    }
    DIR *testdir = 0;
    if ((testdir = opendir(ppqRoot.c_str())) == 0){

      getLogStream(ocfa::misc::LOG_CRIT) << "Cannot open ppqRoot " << ppqRoot << endl;
      throw ocfa::misc::OcfaException(string("Cannot open ") + ppqRoot);      
      
    } else {

      closedir(testdir);
    }
    if (recover){
      getLogStream(ocfa::misc::LOG_NOTICE) << mtname <<  " recovering queue" << endl;
    }
    _ppq = new PersistentPriorityQueue(mtname, recover, ppqRoot);
    _lastix = _instances.end();
  }

  ModuleType::~ModuleType(){
    ocfaLog(ocfa::misc::LOG_DEBUG, "ModuleType destructor");
    if (_instances.size() > 0){
       ocfaLog(ocfa::misc::LOG_WARNING, "Instances vector not empty in ModuleType destructor !");
    }
    ocfaLog(ocfa::misc::LOG_DEBUG,"Deleting PPQ");
    delete _ppq;
    ocfaLog(ocfa::misc::LOG_DEBUG,"Destructor end.");
  }


  void ModuleType::printQueues(){  
    for (map<string, ModuleInstance *>::iterator itr = _instances.begin(); itr != _instances.end(); itr++ ){
      getLogStream(ocfa::misc::LOG_INFO) << "Instance: " << itr->first << " {" << itr->second->queueSize() << "}" << endl;
    }
    getLogStream(ocfa::misc::LOG_INFO) << "Overflow queue: " << _ppq->getSize() << endl; 
  }


  void ModuleType::dropMessage(string instanceuri, unsigned int id){
    if (_instances.find(instanceuri) != _instances.end()){
      _instances[instanceuri]->dropMessage(id);
    }
  }

  void ModuleType::addMessage(Message *msg){
    getLogStream(ocfa::misc::LOG_DEBUG) << "Adding message" << endl;
    getLogStream(ocfa::misc::LOG_DEBUG) << "Storing msg into backup queue !" << endl;
    string serialized = serializeMessage(msg);
    if (serialized == ""){
      getLogStream(ocfa::misc::LOG_ERR) << "Serialize message failed" << endl;
    }
    _ppq->put(serialized, msg->getPriority());
   
    // The caller of this function is responsible for deleting msg
  }

  void ModuleType::freezeMessage(Message *msg){
    getLogStream(ocfa::misc::LOG_NOTICE) << "Storing msg into NEVER queue !" << endl;
    string serialized = serializeMessage(msg);
    if (serialized == ""){
	throw ocfa::misc::OcfaException("Empty put in freezeMessage");
    }
    _ppq->put(serialized, NEVER);
   // The caller of this function is responsible for deleting msg
  }


  bool ModuleType::addInstance(string instname, ocfa::misc::ModuleInstance *minst){
   
    if (_instances.find(instname) == _instances.end()){
      _instances[instname] = new ModuleInstance(instname, minst);
      _instancevec.push_back(_instances[instname]);
      getLogStream(ocfa::misc::LOG_DEBUG) << instname << " added." << endl;
      // invalidate the _lastix iterator because of change
      _lastix = _instances.begin();
      _lastinstanceix = 0;
    } else {
      getLogStream(ocfa::misc::LOG_ERR) << instname << " already registered" << endl;
      return true; 
    }
    return true;
  }

  bool ModuleType::removeInstance(string instname){
    // remove instance but first redistribute messages over other instances
    getLogStream(ocfa::misc::LOG_DEBUG) << "Removing instance " << instname << endl;
    if (_instances.find(instname) != _instances.end()){
      ModuleInstance *toremove = _instances[instname];
      _instances.erase(instname);
      Message *msg;
      // find the message which presumably made the module crash
      if ((msg = toremove->getOldestNACK()) != 0){
        freezeMessage(msg);
        delete msg;
      }
	// move the rest of the messages back to the modultype queue
      while ((msg = toremove->getNACKMessages())!=0){
        addMessage(msg);
        delete msg;
      }
      // invalidate the _lastix iterator because of change
      _lastix = _instances.begin();
      if (_instances.size() == 0){
	// Oops, this was the last instance, not much to redistribute to
	// then.
	// Move all messages from this instance to the message-queue 
	getLogStream(ocfa::misc::LOG_NOTICE)<< "All module instances gone !" << endl;
	return true;
      }
  }
    return true;
  }

  unsigned int ModuleType::getActiveModuleCount(){
    return _instances.size();
  }

  AnyCast::AnyCast(string name, string mnamespace, bool recover): 
	  OcfaObject(name, mnamespace), 
	  _recover(recover),
          _modules(),
	  monitorMessageFactory(0)
  {
    ocfaLog(ocfa::misc::LOG_DEBUG, "Constructor");
  } 

  AnyCast::~AnyCast(){
    ocfaLog(ocfa::misc::LOG_DEBUG, "Destructor");
    for (map<string, ModuleType * >::iterator itr = _modules.begin(); itr != _modules.end(); itr++){
      getLogStream(ocfa::misc::LOG_DEBUG) << "Deleting moduletype " << itr->first << endl; 
      delete (itr->second);
    }
    
  }


  void AnyCast::printQueues(){
    for (map<string, ModuleType * >::iterator itr = _modules.begin(); itr != _modules.end(); itr++){
      getLogStream(ocfa::misc::LOG_INFO) << "Printing queues for moduletype " << itr->first << endl; 
      itr->second->printQueues();
    }
  }

  bool AnyCast::addModuleType(string modtype){
    if (_modules.find(modtype) == _modules.end()){
      _modules[modtype] = new ModuleType(modtype);
     
      getLogStream(ocfa::misc::LOG_DEBUG)<<modtype << " added." << endl;
    } else {
      getLogStream(ocfa::misc::LOG_DEBUG)<< modtype << " already present." << endl;
    }
    return true;
  }

  bool AnyCast::addModuleInstance(string modtype, string instance, ocfa::misc::ModuleInstance *minst){
    if (_modules.find(modtype) == _modules.end())
      _modules[modtype] = new ModuleType(modtype, _recover);
    _modules[modtype]->addInstance(instance, minst);
    return true;
  }

  bool AnyCast::removeModuleInstance(string modtype, string instance){
    if (_modules.find(modtype)!=_modules.end()){
      return _modules[modtype]->removeInstance(instance);
    } else {
      getLogStream(ocfa::misc::LOG_ERR) << modtype << " not found to remove" << endl;
      return false;
    }
  }

  string AnyCast::getModuleInstance(string , Message *){
     return "";
  }

  void AnyCast::dropMessage(string modtype, string instance, unsigned int msgid){
    if (_modules.find(modtype) != _modules.end()){
      _modules[modtype]->dropMessage(instance, msgid);
    }
  }

  void AnyCast::addMessage(string modtype, Message *m){
    if (_modules.find(modtype) != _modules.end()){
      getLogStream(ocfa::misc::LOG_DEBUG)<< "Message dispatched to moduletype " << modtype << endl;
      _modules[modtype]->addMessage(m); 
    } else {
      getLogStream(ocfa::misc::LOG_WARNING) << "No moduletype '" << modtype<< "' found to dispatch message to !" << endl;
      getLogStream(ocfa::misc::LOG_WARNING) << "Creating backup queue for future use" << endl;
      addModuleType(modtype);
      _modules[modtype]->addMessage(m);

      // Send a monitormessage if possible.
      if (getMonitorMessageFactory() != 0){
	Message *monitorMessage = getMonitorMessageFactory()->createMonitorMessage(modtype);
	getLogStream(ocfa::misc::LOG_WARNING) << "subject is :" << monitorMessage->getSubject() << " content is " << monitorMessage->getContent() << endl;
	addMessage("monitor", monitorMessage);
	//delete monitorMessage;
      }
      // create notify message

      // _modules["monitor"]->addMessage(nm)
      // of addMessage("monitor". nm)   
    }
   
  }


  Message *ModuleType::getMessage(){
    // maybe rewrite this to vector so that we can use modulo arithmic
    if (_ppq->getSize() == 0 || (_instances.size() == 0)){
      return 0;
    }
    map<string, ModuleInstance *>::iterator tmp = ++_lastix;
    if (_lastix == _instances.end()){
      _lastix =_instances.begin();
    }

    for(unsigned int i = 0; i < _instances.size(); i++){
      if (tmp == _instances.end()){
	getLogStream(ocfa::misc::LOG_DEBUG)<< "Setting tmp to begin." << endl;
	tmp = _instances.begin();
      }
      if (!(tmp->second->isFull())){
	getLogStream(ocfa::misc::LOG_DEBUG) << "Found empty moduleinstance" << endl;
	string serialized = _ppq->get();
        Message *msg = deserializeMessage(serialized);
        _lastix = tmp;
	return _lastix->second->assignMessage(msg);
      } else {
        getLogStream(ocfa::misc::LOG_DEBUG)<< "Skipping full moduleinstance" << endl;
      }
      tmp++;
    }
    return 0;
  }

  Message *AnyCast::getMessage(string modtype){
    getLogStream(ocfa::misc::LOG_DEBUG) << "GETMESSAGE:" << modtype << endl;
    if (_modules.find(modtype) != _modules.end()){
      return _modules[modtype]->getMessage();
    } else return 0;
  }

  unsigned int AnyCast::getActiveModuleCount(){
    unsigned int total = 0;
    for (map<string, ModuleType * >::iterator itr = _modules.begin(); itr != _modules.end(); itr++){
      getLogStream(ocfa::misc::LOG_NOTICE) << "Active moduleinstances for moduletype " << itr->first << ": " << itr->second->getActiveModuleCount() << endl;
      total += itr->second->getActiveModuleCount();
    }
    getLogStream(ocfa::misc::LOG_NOTICE) << "Total active modules now: "  << total << endl;
    return total;
  }


  void AnyCast::Log(string msg){
    getLogStream(ocfa::misc::LOG_NOTICE) << msg << endl;
  }


  //JBS
  MonitorMessageFactory *AnyCast::getMonitorMessageFactory(){

    return monitorMessageFactory;
  }

  void AnyCast::setMonitorMessageFactory(MonitorMessageFactory **factory){

    if (monitorMessageFactory != 0){

      delete monitorMessageFactory;
    } 
    monitorMessageFactory = *factory;
  }
}

