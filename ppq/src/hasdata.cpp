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
#include "PersistentPriorityQueue.hpp"
#include <iostream>
#include <dirent.h>
#include <stdlib.h>
int main(int argc,char **argv) {
   char *ppqroot=getenv("PPQROOT");
   if (!(ppqroot)) {
      std::cerr << "Please specify the ppqroot in your enviroment settings.\n";
      return 1;
   }
   DIR *rootdir=opendir(ppqroot);
   if (rootdir == 0) {
      std::cerr << "Invalid or unreadable ppqroot dir, unable to open for reading\"" << ppqroot << "\"\n";
      return 2;
   }
   struct dirent *dentry=0;
   while (dentry = readdir(rootdir)) {
     char *basename=dentry->d_name;
     if (basename && (basename[0] != '.')) {
       size_t priority;
       bool hasdata=false;
       for (priority=0;priority < NEVER;priority++) {
          bool statusfileok=true;
	  StatusFile *statusfile=0;
          try {
	       statusfile=new StatusFile(basename,priority,ppqroot,true);
	  } 
	  catch (std::exception &e){
             statusfileok=false;
	  }
	  size_t fifosize=0;
	  bool wasactive=false;
	  if (statusfileok) { 
	      fifosize=statusfile->getFifoSize();
	      delete statusfile;
	      if (fifosize)
	         hasdata=true;
	  }
       }
       if (hasdata) {
          std::cout << basename << std::endl;
       }
     }
   }
   return 0;
}
