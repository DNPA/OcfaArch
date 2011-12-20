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

#ifndef PPQ_HPP
#define PPQ_HPP
// MAXPRIO = max prio of items
#define MAXPRIO 6 
#define NEVER MAXPRIO+1
#include "PersistentFifo.hpp"
#include <vector>
#include <string>
#include <stdexcept>

class PersistentPriorityQueue {
  private:
    std::vector<PersistentFifo *> fifos;
    off_t actprio;
    off_t size;
    std::string mppqroot;
  protected:
    PersistentPriorityQueue(const PersistentPriorityQueue&):fifos(),actprio(NEVER),size(0),mppqroot("") {
	           throw std::runtime_error(std::string("Not allowed to copy PersistentPriorityQueue"));
    }
    const PersistentPriorityQueue& operator=(const PersistentPriorityQueue&) {
		   throw std::runtime_error(std::string("Not allowed to assign PersistentPriorityQueue"));
		   return *this;			          
    }
	
  public:
    // constructor 
    PersistentPriorityQueue(std::string pqname,bool recover=false,std::string ppqroot="/tmp/ppq");
    ~PersistentPriorityQueue();
    // put item in queue with specified prio
    void put(std::string data,off_t prio);
    // prio of item which get() wil return
    off_t peek();
    // gets item with highest prio
    std::string get();
    // number of items in queue
    off_t  getSize();
    // recover a running queue 
    void recoverNow();
};
#endif
