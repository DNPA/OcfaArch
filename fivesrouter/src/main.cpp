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

#include "router.hpp"

using namespace ocfa;
using namespace router;

/**
 * Main
 */
int main(int argc, char *argv[]) {
    char *dummy;
    if (argc>0) {
        dummy=argv[0];
    }
//Globals
Router *rc=NULL;

   try {
      //create router component
      rc = new Router();
      if(rc!=NULL) 
        rc->run();
      
   } catch(OcfaException &e) {
	e.logWhat();
   } catch (exception &e) {
	cerr << "Exception: " << e.what() << endl;
   } catch (...) {
	cerr << "Unknown exception." << endl;
   }
   //delete rouer component, if any present
   if(rc!=NULL)  delete rc;
   return 0;
} //end main
