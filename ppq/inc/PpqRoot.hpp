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

#ifndef PPQROOT_HPP
#define PPQROOT_HPP
#include "PersistentPriorityQueue.hpp"
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

class PpqRoot {
  private:
    bool mRecover;
    std::string mPpqRoot;
    std::map<std::string , PersistentPriorityQueue * > mQueuesByName;
  protected:
    PpqRoot(const PpqRoot&):mRecover(0),mPpqRoot("") {
       throw std::runtime_error(std::string("Not allowed to copy PpqRoot"));
    }
    const PpqRoot& operator=(const PpqRoot&) {
       throw std::runtime_error(std::string("Not allowed to assign PpqRoot"));
       return *this;
    }
  public:
    PpqRoot(bool recover=false,std::string ppqroot="/tmp/ppq");
    ~PpqRoot();
    std::vector< std::string > getQueueNames();
    PersistentPriorityQueue & operator[](std::string queuename);
    void recoverNow();
};
#endif
