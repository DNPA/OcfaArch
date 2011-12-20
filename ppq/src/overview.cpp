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
#include <sys/vfs.h>
#include <string.h>
#include <boost/lexical_cast.hpp>
int main(int argc,char **argv) {
   char *ppqroot=getenv("PPQROOT");
   std::cout << "Content-type: text/xml\n\n";
   std::cout << "<?xml version=\"1.0\"?>\n";
   std::cout << "<?xml-stylesheet type=\"text/xsl\" href=\"/ppqoverview.xsl\" ?>\n";
   std::cout << "<ppqset>\n";
   if (ppqroot == 0) {
     if (argc < 2) {
        std::cout << "   <error msg=\"No PPQROOT enviroment specified\"></error>\n";
        std::cout << "</ppqset>\n";
        return 1;
     }
   }

   int index;   
   for (index=1;index < argc;index++) {
      if ((argv[index][0] != '-') && (argv[index][0])) {
         ppqroot=argv[index];
      }
   }
   if ((ppqroot == 0)||(strlen(ppqroot) == 0)) {
      std::cout << "   <error msg=\"No PPQROOT enviroment specified\"></error>\n";
      std::cout << "</ppqset>\n";
      return 1;
   }
   DIR *rootdir=opendir(ppqroot);
   if (rootdir == 0) {
      std::cout << "   <error msg=\"Invalid or unreadable PPQROOT dir, unable to open for reading '" << ppqroot << "' " << strlen(ppqroot) << "\"></error>\n";
      std::cout << "</ppqset>\n";
      return 1;
   }
   struct statfs ppqfsstat;
   if (statfs(ppqroot, &ppqfsstat) == 0) {
      size_t megabytes=ppqfsstat.f_bavail*ppqfsstat.f_bsize/(1024*1024);
      if (megabytes >= 4096) {
         size_t gigabytes=megabytes/1024;
         std::cout << "   <diskavailable>" << boost::lexical_cast<std::string>(gigabytes) << "GB</diskavailable>\n";
      } else {
         std::cout << "   <diskavailable>" << boost::lexical_cast<std::string>(megabytes) << "MB</diskavailable>\n"; 
      }
   }
   struct dirent *dentry=0;
   try {
   while (dentry = readdir(rootdir)) {
     char *basename=dentry->d_name;
     if (basename && (basename[0] != '.')) {
       size_t priority;
       std::cout << "   <ppq module=\"" << basename << "\">\n";
       for (priority=0;priority <= NEVER;priority++) {
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
              wasactive = ((statusfile->getRFIndex()>1) || statusfile->getRFSkip() || fifosize) ;
	      delete statusfile;
	  }
	  std::cout << "      <fifo prio=\"" << priority << "\" used=\"" << wasactive << "\">" << fifosize << "</fifo>\n";  
       }
       std::cout << "   </ppq>\n";
     }
   }
   }
   catch (std::exception &e){
           std::cout << "   <error msg=\"" << e.what() << "\"></error>\n";
   }
   catch (...) {
           std::cout << "   <error msg=\"Unexpected exception\"></error>\n";
   }
   std::cout << "</ppqset>\n";
}
