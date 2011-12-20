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
#include <iostream>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>
int main(int argc,char **argv) {
   char *ppqroot=getenv("PPQROOT");
   if (ppqroot == 0) {
      std::cerr << "ERROR: PPQROOT not set\n";
      return 1;
   }
   PpqRoot myPpqRoot(false,ppqroot);
   std::vector< std::string > queuenames=myPpqRoot.getQueueNames();
   for (std::vector< std::string >::iterator i=queuenames.begin();i!=queuenames.end();++i) {
      std::string queuename=*i;
      std::cout << "queue '" << queuename << "'\n";
      off_t items=myPpqRoot[queuename].getSize();
      std::cout << "\tItems=" << boost::lexical_cast< std::string >(items) << std::endl;            
   }
   std::cout << "queue 'bogus'\n";
   myPpqRoot["bogus"].put("bogus string",4);
   off_t items2=myPpqRoot["bogus"].getSize();
   std::cout << "\tItems=" << boost::lexical_cast< std::string >(items2) << std::endl;
 
}
