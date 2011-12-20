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

#include "PersistentFifo.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>
int main() {

  try {
  PersistentFifo *fifo=new PersistentFifo("testfifo",1,"/tmp/hohoho");
  off_t index;
  off_t readcount=0;
  std::string datastring="";
  for (index=0;index<1000000;index++) {
    datastring="data id=" + boost::lexical_cast<std::string>(index);
    fifo->put(datastring));
    if ((index % 7)==0) {
       std::string rval=fifo->get();
       readcount++;
       std::cout << "DATA: '" << rval << "'\n";
    }
  }
  while(fifo->getSize()) {
     std::string rval2=fifo->get();
     readcount++;
     std::cout << "More data: '" << rval2 << "'\n";
  }
  delete fifo;
  } catch (std::exception &e){
	  std::cerr << e.what() << "\n";
  }
}
