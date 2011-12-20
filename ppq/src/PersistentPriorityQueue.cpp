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

#include "PersistentPriorityQueue.hpp"
#include <iostream>
    PersistentPriorityQueue::PersistentPriorityQueue(std::string pqname,bool recover,std::string ppqroot):
	    fifos(),
	    actprio(MAXPRIO),
	    size(0),
	    mppqroot(ppqroot)
    {
      off_t index;
      //Make one extra Fifo for the purpose of storing failed messages
      for (index=0;index<=(MAXPRIO+1);index++) {
	 PersistentFifo *fifo=new PersistentFifo(pqname.c_str(),index,mppqroot);
	 fifos.push_back(fifo);
         if (index < (MAXPRIO+1) ) {size += fifo->getSize();}
         if ((index < actprio) && (fifo->getSize())) {
           actprio=index;
         }
      }
      if (recover) {
        recoverNow();
      }
    }
    PersistentPriorityQueue::~PersistentPriorityQueue() {
       off_t index;
       for (index=0;index<=(MAXPRIO+1);index++) {
          delete fifos[index];
       }
    }
    void PersistentPriorityQueue::recoverNow() {
       while(fifos[NEVER]->getSize()) {
             fifos[MAXPRIO]->put(fifos[NEVER]->get());
             size++;
        }
    }
    void PersistentPriorityQueue::put(std::string data,off_t prio) {
      fifos[prio]->put(data);
      if (prio < actprio) {
        actprio=prio;
      }
      //Only count data with a priority up to MAXPRIO.
      if (prio <= MAXPRIO) {
        size++;
      }
    }

    off_t PersistentPriorityQueue::peek(){
       return actprio;
    }

    std::string PersistentPriorityQueue::get(){
       std::string rval=fifos[actprio]->get();
       size--;
       if ((fifos[actprio]->getSize()==0)) {
          off_t index;
          for (index=actprio;index<=MAXPRIO;index++) {
             if (fifos[index]->getSize()) {
               actprio=index;
               return rval;
             }
          }
	  actprio=MAXPRIO;
       }
       return rval;
    }

    off_t  PersistentPriorityQueue::getSize(){
      return size;
    }
