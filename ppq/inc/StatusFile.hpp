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

#ifndef STATUSFIL_HPP
#define STATUSFIL_HPP
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

class StatusFile {
  private:
    // index of file currently being read by Persistent FIFO	  
    off_t rfindex;
  // index of file currently being written by Persistent FIFO
    off_t wfindex;
  // number of items being already read in readfile
    off_t rfskip;
  // number of items nonread in FIFO
    off_t fifosize;
    FILE   *statfile;
    void   sync();
  protected:
    StatusFile(const StatusFile&):
	    rfindex(0),
	    wfindex(0),
	    rfskip(0),
	    fifosize(0),
	    statfile(0)
    {
       throw std::runtime_error(std::string("Not allowed to copy StatusFile"));
    }
    const StatusFile& operator=(const StatusFile&) {
       throw std::runtime_error(std::string("Not allowed to assign StatusFile"));
       return *this;
    }
  public:
    StatusFile(const char *basename,size_t fifoid,std::string ppqroot="/tmp/ppq",bool readonly=false);
    off_t getRFIndex();
    off_t getWFIndex();
    off_t getRFSkip();
    off_t getFifoSize();
    void   incRFIndex();
    void   incWFIndex();
    void   incFifoSize();
    void   decFifoSize();
    void   incRFSkip();
};
#endif
