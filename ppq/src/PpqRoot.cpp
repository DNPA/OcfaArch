//  Persistent Priority Queue Library
//  Copyright (C) 2005 Rob J Meijer <rmeijer@xs4all.nl>
//  Copyright (C) 2006 KLPD <ocfa@dnpa.nl>
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
#include "PpqRoot.hpp"
#include <sys/types.h>
#include <dirent.h>

//    std::map<std::string , PersistentPriorityQueue > mQueuesByName;
PpqRoot::PpqRoot(bool recover,std::string ppqroot):mRecover(recover),mPpqRoot(ppqroot) {
      DIR *rootdir=opendir(mPpqRoot.c_str());  
      if (rootdir == 0) {
         throw std::runtime_error(std::string("Problem opening top level dir for PPQ"));
      }        
      struct dirent *entity=0;
      while(entity=readdir(rootdir)) {
         std::string queuename(entity->d_name);
         if (queuename.c_str()[0] != '.') {
            mQueuesByName[queuename]=new PersistentPriorityQueue(queuename,mRecover,mPpqRoot);
         }
      }    
}
PpqRoot::~PpqRoot(){
     for (std::map<std::string , PersistentPriorityQueue * >::iterator i=mQueuesByName.begin();i!=mQueuesByName.end();++i) {
        delete(i->second);
     }
}
std::vector< std::string > PpqRoot::getQueueNames(){
     std::vector< std::string > rval;
     for (std::map<std::string , PersistentPriorityQueue *>::iterator i=mQueuesByName.begin();i!=mQueuesByName.end();++i) {
        rval.push_back(i->first);
     }
     return rval;
}
PersistentPriorityQueue & PpqRoot::operator[](std::string queuename){
    bool needscreate=true;
    for (std::map<std::string , PersistentPriorityQueue *>::iterator i=mQueuesByName.begin();i!=mQueuesByName.end();++i) {
      if(i->first == queuename) {
         needscreate=false;
      }
    }
    if (needscreate) {
       mQueuesByName[queuename]=new PersistentPriorityQueue(queuename,mRecover,mPpqRoot);
    }
    return *(mQueuesByName[queuename]);
}
void PpqRoot::recoverNow(){
   for (std::map<std::string , PersistentPriorityQueue * >::iterator i=mQueuesByName.begin();i!=mQueuesByName.end();++i) {
       i->second->recoverNow();
   }
}
