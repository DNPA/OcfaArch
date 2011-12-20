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
#include "basedir.hpp"
#include <grp.h>
#include <stdexcept>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/lexical_cast.hpp>
    void   StatusFile::sync() {
       rewind(statfile);
       fwrite(&rfindex,sizeof(off_t),4,statfile);
       fflush(statfile);
    }
    StatusFile::StatusFile(const char *basename,size_t fifoid,std::string ppqroot,bool readonly):
	    rfindex(0),
	    wfindex(0),
            rfskip(0),
	    fifosize(0),
	    statfile(0)
    {
       struct group *grp=getgrnam("ocfa");
       if (grp==0) {
	  throw std::runtime_error(std::string("Unable to find the group id of the 'ocfa' group."));
       }
       std::string basedir=ppqroot + "/" + basename;
       std::string statuspath=basedir + "/" + boost::lexical_cast<std::string>(fifoid) + ".status";
       if (readonly) {
         statfile=fopen(statuspath.c_str(),"r");
	 if (statfile==0) {
            throw std::runtime_error(std::string("Unable to load status file in read only mode:") + statuspath);
	 }
       } else {
         statfile=fopen(statuspath.c_str(),"r+");
         if (statfile==0) {
           mkdir(basedir.c_str(),0750);
	   chown(basedir.c_str(),static_cast<uid_t>(-1),grp->gr_gid);
           statfile=fopen(statuspath.c_str(),"w+");
           if (statfile==0) {
             throw std::runtime_error(std::string("Unable to load status file in r+ mode:")+ 
                                      statuspath);
           }
	   chown(statuspath.c_str(),static_cast<uid_t>(-1),grp->gr_gid);
	   chmod(statuspath.c_str(),0640);
           sync();
         }
       }
       rewind(statfile);
       fread(&rfindex,sizeof(off_t),4,statfile);
    }
    off_t StatusFile::getRFIndex(){ return rfindex;}
    off_t StatusFile::getWFIndex(){ return wfindex;}
    off_t StatusFile::getRFSkip() {return rfskip;}
    off_t StatusFile::getFifoSize(){return fifosize;}
    void   StatusFile::incRFIndex() {rfindex++; rfskip=0;sync();}
    void   StatusFile::incWFIndex() {wfindex++; sync();}
    void   StatusFile::incFifoSize(){fifosize++; sync();}
    void   StatusFile::decFifoSize(){fifosize--; sync();}
    void   StatusFile::incRFSkip() {rfskip++; sync();}
  
