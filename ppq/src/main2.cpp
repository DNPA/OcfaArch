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
#include <boost/lexical_cast.hpp>
int main() {
  std::cout << "Creating priority queue\n";
  PersistentPriorityQueue *pqueue=new PersistentPriorityQueue("testpqueue",0,"/tmp/hohoho");
  std::cout << "Priority queue created\n";
  off_t index;
  std::string datastring="";
  for (index=0;index<40;index++) {
    datastring = "data id=" + boost::lexical_cast<std::string>(index);
    if ((index % 10) ==0) {
      pqueue->put(datastring,NEVER);
    } else {
      pqueue->put(datastring,(index % 2));
    }
  }
  while(pqueue->getSize()) {
     std::cout << pqueue->get() << "\n";
  }
  //delete pqueue;
}
