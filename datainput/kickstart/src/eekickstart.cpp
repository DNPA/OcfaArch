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
  cout << "Usage: ./kickstart caseid sourceid itemid path charset name" << endl;
  exit(1);
}

int main(int argc, char *argv[]){

  if (argc < 7) usage();
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
  string charSet = "LATIN1";
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
    
    //ks->waitForRouter();
    std::map<std::string,ocfa::misc::Scalar> attr;
    attr["charset"]=charSet;
    attr["mountpoint"]=argv[4];
    ks->KickstartEvidence(string(argv[1]),string(argv[2]),string(argv[3]),evidenceName,"EncaseExport",&attr,true);
    cerr << "Evidence has been entered" << endl;
    //    ks->run();
  } catch (ocfa::misc::OcfaException &e) {
    cerr << "OcfaException: " << e.what() << endl;
  } catch (exception &e) {
    cerr << "Exception: " << e.what() << endl;
  } catch (string e) {
    cerr << "Sting as exception: " << e << "\n";
  }catch (...) {
    cerr << "Unknown exception." << endl;
  }
  cerr << "Survived the try-catch block. Deleting kickstart object." << endl;
  delete ks;
  
  return 0;
}
