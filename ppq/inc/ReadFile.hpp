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

#ifndef READFILE_HPP
#define READFILE_HPP
#include "basedir.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

class ReadFile {
  private:
    FILE *fp;
    off_t readpointer;
    std::string mDatafilepath;
  protected:
    ReadFile(const ReadFile&):
	    fp(0),
	    readpointer(0) 
    {
         throw std::runtime_error(std::string("Not allowed to copy ReadFile"));
    }
    const ReadFile& operator=(const ReadFile&) {
         throw std::runtime_error(std::string("Not allowed to assign ReadFile"));
    }
  public:
    ReadFile(const char *basename,size_t fifoid,unsigned long fileseq,FILE *fptr=0,std::string ppqroot="/tmp/ppq");
    bool eof();
    std::string getnext();
  // used when recovering from previous run
    void skip(off_t count);
    void remove();
};
#endif
