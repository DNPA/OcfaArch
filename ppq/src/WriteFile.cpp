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

#include "WriteFile.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <stdexcept>
#include <grp.h>
#include <boost/lexical_cast.hpp>
   WriteFile::WriteFile(const char *basename,size_t fifoid,unsigned long fileseq,std::string ppqroot):
	    fp(0),
	    writepointer(0),
	    stringcount(0)
    {
      struct group *grp=getgrnam("ocfa");
      if (grp==0) {
	throw std::runtime_error(std::string("Unable to find the group id of the 'ocfa' group."));
      }
      std::string basedir=ppqroot + "/" + basename;
      std::string fifodir = basedir + "/" + boost::lexical_cast<std::string>(fifoid) + ".fifo";
      std::string datafilepath=fifodir + "/" + boost::lexical_cast<std::string>(fileseq) + ".dat"; 
      fp=fopen(datafilepath.c_str(),"w+");
      if (fp==0) {
        mkdir(fifodir.c_str(),0750);
	chown(fifodir.c_str(),static_cast<uid_t>(-1),grp->gr_gid);
        fp=fopen(datafilepath.c_str(),"w+");
        if (fp==0) {
           throw std::runtime_error(std::string("Unable to open fifo write file" + datafilepath));
        }
      }
      chown(datafilepath.c_str(),static_cast<uid_t>(-1),grp->gr_gid);
      chmod(datafilepath.c_str(),0640);  
    }
    off_t WriteFile::getFileStringCount(){
      return stringcount;
    }
    FILE *WriteFile::getFP(){
      return fp;
    }
    void WriteFile::addString(std::string pl){
      off_t slen=pl.size()+1;
      if (slen == 1){
	throw std::runtime_error(std::string("slen == 1"));
      }
      off_t etsize=0;
      fseeko(fp,0,SEEK_END);
      etsize=fseeko(fp,0,SEEK_CUR);
      if (fwrite(&slen,sizeof(off_t),1,fp)!=1) {
         ftruncate(fileno(fp),etsize);
         throw std::runtime_error(std::string("Problem writing to open fifo writefile"));
      }
      if (fwrite(pl.c_str(),slen,1,fp)!=1) {
         ftruncate(fileno(fp),etsize);
         throw std::runtime_error(std::string("Problem writing to open fifo writefile"));
      }
      writepointer=fseeko(fp,0,SEEK_CUR);
      stringcount++;
    }
    void WriteFile::close(){
      fclose(fp);
    }
