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

#include<iostream>
#include<facade/KickstartAccessor.hpp>
using namespace std;

void usage(){
  cout << "Usage: ./kickstart caseid sourceid itemid path [ charset ]" << endl;
  exit(1);
}

int main(int argc, char *argv[]){

  if (argc < 5) usage();
  std::cout << "!!Warning!! : kickstart is depricated, please use kicktree instead" << std::endl;
  char *ocfacaseenv=getenv("OCFACASE");
  if (ocfacaseenv == 0) { 
    cerr << "WARNING: no OCFACASE specified in enviroment, using commandline argument instead\n";
    setenv("OCFACASE",argv[1],1);
  } else {
     if (strcmp(ocfacaseenv,argv[1])) {
         cerr << "ERROR: The OCFACASE enviroment variable is set to '" << ocfacaseenv << "' while the case specified is '" << argv[1] << "\n";
	 return 1;
     }
  }
  string charSet = "AUTO";
  string evidenceName = "";
  if (argc >= 6) {

    charSet = argv[5];    
  }
  if (argc ==7){

    evidenceName = argv[6];
  }
  ocfa::facade::KickstartAccessor *ks = 0;
  
  try {
    ks = new ocfa::facade::KickstartAccessor("kickstart", "kickstart");
    ks->KickstartEvidence(argv[1],argv[2],argv[3],argv[4], charSet, evidenceName);
    cerr << "Evidence has been entered" << endl;
    delete ks; 
  } catch (ocfa::misc::MessageConnectException &e) {
    cerr << "ERROR: no anycastrelay available" << endl;
    return 1;
  } catch (ocfa::misc::DbConnectException &e) {
    cerr << "ERROR: problem connecting to repository database server" << endl;
    return 1;
  } catch (ocfa::misc::ItemConstraintException &e) {
    cerr << "ERROR: Can not kickstart the same item twice" << endl;
  } catch (ocfa::misc::OcfaException &e) {
    cerr << "ERROR: unexpected type of OcfaException, consult /var/log/ocfa.log" << endl;
    //The folowing 3 lines should not ne needed.
    //ocfa::misc::DbConnectException *dbe=dynamic_cast<ocfa::misc::DbConnectException *>(&e);
    //ocfa::misc::MessageConnectException *msge=dynamic_cast<ocfa::misc::MessageConnectException *>(&e);
    //ocfa::misc::ItemConstraintException *ice=dynamic_cast<ocfa::misc::ItemConstraintException *>(&e);
    cerr << "OcfaException: " << e.what() << endl;
    return 1;
  } catch (exception &e) {
    cerr << "Exception: " << e.what() << endl;
    return 1;
  } catch (string &e) {
    cerr << "Sting as exception: " << e << "\n";
    return 1;
  }catch (...) {
    cerr << "Unknown exception." << endl;
    return 1;
  }
  return 0;
}
