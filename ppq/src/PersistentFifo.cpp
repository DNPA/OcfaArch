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
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "basedir.hpp"
#include <boost/lexical_cast.hpp>
   PersistentFifo::PersistentFifo(const char *basename,size_t fifoid,std::string ppqroot):
	   status(0),
	   rf(0),
	   wf(0),
	   mbasename(0),
	   fifo_id(fifoid),
	   mppqroot(ppqroot)
    {
       mbasename=static_cast<char *>(malloc(strlen(basename)+1));
       strcpy(mbasename,basename);
       status=new StatusFile(basename,fifoid,ppqroot);
       status->incWFIndex();
       wf=new WriteFile(basename,fifoid,status->getWFIndex(),ppqroot);
       if (status->getRFIndex()==0) {
         status->incRFIndex();
       }
       FILE *fp=0;
       if (status->getRFIndex()==status->getWFIndex()) {
         fp=wf->getFP();
       }
       rf=new ReadFile(basename,fifoid,status->getRFIndex(),fp,ppqroot);
       rf->skip(status->getRFSkip());
    }

    PersistentFifo::~PersistentFifo(){
      bool dorm=false;
      if (status->getFifoSize() == 0) {
	      dorm=true;
      }
      delete wf;
      if (dorm) {
	      rf->remove();
	      delete rf;
	      if (status->getRFIndex() != status->getWFIndex()) {
		rf=new ReadFile(mbasename,fifo_id,status->getWFIndex(),0, mppqroot);
	        rf->remove();
		delete rf;
	      }
      } else {
         delete rf;
      }
      delete status;
      if (dorm) {
           std::string basedir=mppqroot + "/" + mbasename;
           std::string basepath= basedir + "/" + boost::lexical_cast<std::string>(fifo_id);
           std::string statuspath=basepath + ".status";
           std::string fifopath=basepath + ".fifo";
           unlink(statuspath.c_str());
           rmdir(fifopath.c_str()); 
	   //This will fail for everything but an empty pqueue dir, and that is what we want.
           rmdir(basedir.c_str());
      }
      free(mbasename);
    }

    off_t PersistentFifo::getSize(){
      return status-> getFifoSize();
    }

    void PersistentFifo::put(std::string data){
      if ((wf->getFileStringCount() >= BASE_DF_SIZE) && ((2* wf->getFileStringCount()) >= status->getFifoSize())) {
         /*The active file is rather big, lets make a new one */
         if (status->getRFIndex()!=status->getWFIndex()) {
           wf->close();
         }
         delete wf;
         status->incWFIndex();
         wf=new WriteFile(mbasename,fifo_id,status->getWFIndex(),mppqroot);
      }
      wf->addString(data);
      status->incFifoSize();
    }

    std::string PersistentFifo::get(){
      if (status->getFifoSize()==0) {
         throw std::runtime_error(std::string("Can not fetch data from an empty fifo"));
      }
      std::string data=rf->getnext();
      while ((status->getRFIndex()<status->getWFIndex())&& (rf->eof())) {
	status->incRFIndex();
        rf->remove();
        delete rf;
        FILE *fp=0;
        if (status->getRFIndex()==status->getWFIndex()) {
           fp=wf->getFP();
        }
        rf=new ReadFile(mbasename,fifo_id,status->getRFIndex(),fp,mppqroot);
	data=rf->getnext();
      }
      if (rf->eof()) {
        throw std::runtime_error(std::string("Unexpected eof in final segment of non empty persistent fifo"));
      }
      status->incRFSkip();
      status->decFifoSize();
      return data;
    }

