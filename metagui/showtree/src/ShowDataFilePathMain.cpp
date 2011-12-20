
//  The Open Computer Forensics Architecture.
//  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
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

#include "ShowEvidence.hpp"


int main(int argc, char *argv[]){

  if (argc < 4){

    cout << "usage: showevidence <case> <source> <item> <evidenceid>" << endl;
    exit(-1);
  }
  else {
    
    try {
      ShowEvidence showEv;
      showEv.showDataFilePath(argv[1], argv[2], argv[3], argv[4], cout);
      return 0;
    } catch (ocfa::misc::OcfaException &e){
      e.logWhat();
      cerr << e.what(); 
    }
  }
}
