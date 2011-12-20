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

#ifndef WRITEFILE_HPP
#define WRITEFILE_HPP
#include "basedir.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

class WriteFile {
  private:
    FILE *fp;
    off_t writepointer;
    off_t stringcount;
  protected:
    WriteFile(const WriteFile &):fp(0),writepointer(0),stringcount(0) {
        throw std::runtime_error(std::string("Not allowed to copy WriteFile"));
    }
    const WriteFile & operator=(const WriteFile&) {
	throw std::runtime_error(std::string("Not allowed to assign WriteFile"));
	return *this;
    }
  public:
    WriteFile(const char *basename,size_t fifoid,unsigned long fileseq,std::string ppqroot="/tmp/ppq");
  // Total nmbr records written to file 
    off_t getFileStringCount();
  // Used by persistent FIFO
    FILE *getFP();
    void addString(std::string pl);
    void close();
};
#endif
