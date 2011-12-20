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

#include "ReadFile.hpp"
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <boost/lexical_cast.hpp>

    ReadFile::ReadFile(const char *basename,size_t fifoid,unsigned long fileseq,FILE *fptr,std::string ppqroot):
	    fp(fptr),
	    readpointer(0)
    {
      std::string basedir=ppqroot + "/" + basename;
      std::string fifodir=basedir + "/" + boost::lexical_cast<std::string>(fifoid) + ".fifo";
      mDatafilepath = fifodir + "/" + boost::lexical_cast<std::string>(fileseq) + ".dat";
      if (fp ==0) {
        fp=fopen(mDatafilepath.c_str(),"r");
      } else {
      }
      if (fp==0) {
         throw std::runtime_error(std::string("Unable to open fifo read file " + mDatafilepath));
      }      
    }
    bool ReadFile::eof(){
       return (feof(fp)!=0);
    }
    std::string ReadFile::getnext(){
      off_t strsz=0;
      fseeko(fp,readpointer,SEEK_SET);
      readpointer+=fread(&strsz,1,sizeof(off_t),fp);
      if (strsz == 1){
	      throw std::runtime_error(std::string("strsize == 1"));
      }
      if (strsz == 0){
	      return std::string("");
      }
      char *tmp=static_cast<char *>(malloc(strsz));
      readpointer+=fread(tmp,1,strsz,fp);
      std::string rval(tmp);
      free(tmp);
      return rval;
    }
    void ReadFile::skip(off_t count){
      off_t cnt;
      for (cnt=count;cnt>0;cnt--) {
        std::string dummy=getnext();
      }
    }
    void ReadFile::remove() {
      fclose(fp);
      if (unlink(mDatafilepath.c_str()) == -1) {
        usleep(100);
        if (unlink(mDatafilepath.c_str()) == -1) {
        std::string problem="undefined error";
          switch (errno) {
            case EACCES: problem="EACCES: directory access to containing directory not allowed"; break;
            case EFAULT: problem="EFAULT: path points outside your accessible address space"; break;   
            case EIO:    problem="EIO: An I/O error occurred."; break;
            case EISDIR: problem="EISDIR: path refers to a directory, not a file.";break;
            case ELOOP:  problem="ELOOP: Too many symbolic links were encountered  in  translating path";break;
            case ENAMETOOLONG: problem="ENAMETOOLONG: pathname is to long";break;
            case ENOENT: problem="ENOENT: path does not point to an existing entity";break;
            case ENOMEM: problem="ENOMEM: Insufficient kernel memory was available.";break;
            case ENOTDIR:problem="ENOTDIR: A component used as a directory in the path is not, in fact a directory";break;
            case EPERM: problem="The file system does not allow unlinking of files.";break;
            case EROFS: problem="Path refers to a file on a read only filesystem";break;
          }
          std::string errormsg=std::string("Unable to remove the finished readfile '") + mDatafilepath + "' " + problem + "\n";
          //FILE *logdata=fopen("/var/ocfa/ppq.log", "a");
          //if (logdata == 0) {
            throw std::runtime_error(errormsg);
          //}
          //fwrite( errormsg.c_str(), 1, errormsg.size(), logdata);
          //fclose(logdata);
        }   
      }
    }
