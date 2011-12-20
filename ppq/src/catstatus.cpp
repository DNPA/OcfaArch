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

#include "StatusFile.hpp"
#include <iostream>

int main(int argc,char **argv) {
   if (argc < 3) {
      std::cerr << "Please specify the ppqroot the ppqname and the priority\n";
      return 1;
   }
   char *ppqroot=argv[1];
   char *basename=argv[2];
   size_t priority=atoi(argv[3]);
   StatusFile *statusfile=new StatusFile(basename,priority,ppqroot);
   std::cout << "Size=" << statusfile->getFifoSize() << "\n";
   std::cout << "Writefile=" << statusfile->getWFIndex() << "\n";
   std::cout << "Readfile=" << statusfile->getRFIndex() << "\n";
   std::cout << "RF-Skip=" << statusfile->getRFSkip() << "\n";
   delete statusfile;
}
