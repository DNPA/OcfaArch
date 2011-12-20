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

#ifndef INCLUDED_ANYCAST_HPP
#define INCLUDED_ANYCAST_HPP
#include<map>
#include<vector>
#include<string>
#include <OcfaObject.hpp>
#include <misc/OcfaException.hpp>
#include <message/MessageBox.hpp>
#include <message/Serialize.hpp>
#include <misc/ModuleInstance.hpp>
#include "../../ppq/inc/PersistentPriorityQueue.hpp"
#include "MonitorMessageFactory.hpp"
namespace Anycast {
  using namespace std; 
  using namespace ocfa::message;

  class ModuleInstance: public ocfa::OcfaObject {
  public:
    ModuleInstance(string instname, ocfa::misc::ModuleInstance *minst);
    Message *assignMessage(Message *m);
    Message *getNACKMessages();
    Message *getOldestNACK();
    bool isFull();
    unsigned int queueSize();
    void dropMessage(unsigned int id);
  protected:
    ModuleInstance(const ModuleInstance& mi):
	    ocfa::OcfaObject(mi),
	    _max_outstanding(0),
	    _msgids(),
	    _instname(""),
	    _minst(0)
    {
	throw ocfa::misc::OcfaException("No copying of Anycast::ModuleInstance allowed",this);
    }
    const ModuleInstance& operator=(const ModuleInstance&) {
        throw ocfa::misc::OcfaException("No assignment of Anycast::ModuleInstance allowed",this);
	return *this;
    }
  private:
    static unsigned int _currmsgid;
    unsigned int _max_outstanding;
    map<unsigned int, Message *> _msgids;
    string _instname;
    ocfa::misc::ModuleInstance *_minst;
  };

  class ModuleType: public ocfa::OcfaObject {
  public:
    ModuleType(string mtname, bool recover = false);
    virtual ~ModuleType();
    void printQueues();
    void addMessage(Message *msg);
    Message *getMessage();
    bool addInstance(string instname, ocfa::misc::ModuleInstance *minst);
    bool removeInstance(string instname);
    void dropMessage(string instanceuri, unsigned int id);
    void freezeMessage(Message *msg);
    unsigned int getActiveModuleCount();
  protected:
    ModuleType(ModuleType& mt):
	    OcfaObject(mt),
	    _lastix(),
	    _instances(),
	    _instancevec(),
	    _lastinstanceix(0),
	    _mtname("BOGUS"),
	    _ppq(0),
	    serializeMessage(getMessageConverter()),
	    deserializeMessage(getMessageConverter())
    {
        throw ocfa::misc::OcfaException("No copying of Anycast::ModuleType allowed",this);
    }
    const ModuleType& operator=(const ModuleType&) {
	throw ocfa::misc::OcfaException("No assignment of Anycast::ModuleType allowed",this);
	return *this;
    }
  private:
    map<string, ModuleInstance *>::iterator _lastix;
    map<string, ModuleInstance *> _instances;
    vector<ModuleInstance *>_instancevec;
    unsigned int _lastinstanceix; 
    string _mtname;
    PersistentPriorityQueue *_ppq;
    XMLMessageConverter &serializeMessage;
    XMLMessageConverter &deserializeMessage;
  };

  class AnyCast: public ocfa::OcfaObject {
  public:
    AnyCast(string name = "Anycast", string mnamespace = "Anycast", bool recover = false);
    virtual ~AnyCast();
    bool addModuleType(string modtype);
    bool addModuleInstance(string modtype, string instance, ocfa::misc::ModuleInstance *minst);
    bool removeModuleInstance(string modtype, string instance);
    string getModuleInstance(string modtype, Message *msg);
    void dropMessage(string modulename, string instanceuri, unsigned int id);
    void addMessage(string modtype, Message *m);
    //    Message *getMessage(string modtype, string instance);
    Message *getMessage(string modtype);
    void printQueues();
    unsigned int getActiveModuleCount();
    void Log(string msg);
    void setMonitorMessageFactory(MonitorMessageFactory **monitorMessageFactory);
    MonitorMessageFactory *getMonitorMessageFactory();
  protected:
    MonitorMessageFactory *monitorMessageFactory;

  private:
    bool _recover;
    map<string, ModuleType * > _modules;
  };
}
#endif
