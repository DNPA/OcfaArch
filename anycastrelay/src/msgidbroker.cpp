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

#include"msgidbroker.hpp"

MsgIDBroker *MsgIDBroker::_instance = 0;

MsgIDBroker::~MsgIDBroker(){

}
 
MsgIDBroker::MsgIDBroker():_id(0){
}


unsigned int MsgIDBroker::getID(){
  return ++_id;
}
  
unsigned int MsgIDBroker::lastID(){
  return _id;
}

MsgIDBroker *MsgIDBroker::Instance(){
  if (_instance == 0){
    _instance = new MsgIDBroker();
  }
  return _instance;
}


