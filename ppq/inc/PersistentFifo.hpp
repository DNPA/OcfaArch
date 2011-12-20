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

#ifndef PFOFO_HPP
#define PFIFO_HPP
#define BASE_DF_SIZE 1024
#include "StatusFile.hpp"
#include "ReadFile.hpp"
#include "WriteFile.hpp"
#include <string>
#include <stdexcept>

// Helper object for PersistentPriorityQueue

class PersistentFifo {
  private: 
    StatusFile *status;
    ReadFile   *rf;
    WriteFile  *wf;
    char       *mbasename;
    size_t      fifo_id;
    std::string mppqroot;
  protected:
    PersistentFifo(const PersistentFifo&):status(0),rf(0),wf(0),mbasename(0),fifo_id(0),mppqroot("") {
       throw std::runtime_error(std::string("Not allowed to copy PersistentFifo"));
    }
    const PersistentFifo& operator=(const PersistentFifo&) {
       throw std::runtime_error(std::string("Not allowed to assign PersistentFifo"));
       return *this;
    }
  public:
    PersistentFifo(const char *basename,size_t fifoid,std::string ppqroot="/tmp/ppq");
    ~PersistentFifo();
    off_t getSize();
    void put(std::string data);
    std::string get();
};
#endif
