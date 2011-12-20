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
#include <unistd.h>
#include <getopt.h>
using namespace std;

void usage(){
  cout << "The kicktree tool requires multiple options to be set, and has some aditional options: " << std::endl;
  cout << "  -M --module <modname> : Specify the treegraph module to load. (optional) " << std::endl;
  cout << "  -C --case <casename>  : Specify the name of the active investigation. (optional) " << std::endl;
  cout << "  -S --source <srcname> : Specify the formal source of the evidence. Typically this will be the" << std::endl;
  cout << "                          identifying name specified on a label or tag attached to the item or" << std::endl;
  cout << "                          box or computer containing the item. (required)" << std::endl;
  cout << "  -I --item <itemname>  : Specify the name of the item. If the label or tag specified by -S or -source" << std::endl;
  cout << "                          refers to more than one actual items, the itemname specifies the individual" << std::endl;
  cout << "                          item its individual name. (optional)" << std::endl;
  cout << "  -n --name <nickname>  : Specify an optional non formal nickname for the item. (optinal)" << std::endl; 
  cout << "  -p --path <path>      : Specify the path of the item to kickstart. (required)." << std::endl;
  cout << "  -a --attribute <k>=<v>: Specify a module specific attribute. You can specify multiple attributes" << std::endl;
  cout << "                          this way by repeating the -a/--attribute multiple times. (optional)" << std::endl;
  cout << std::endl;
  cout << "Examples ::" << std::endl;
  cout << "  kicktree -S testset -p /var/testset" << std::endl;
  cout << "  kicktree -M mmls -S mrplstrt42nasdisk1 -p /mnt/images/mrplstrt42nasdisk.dd" << std::endl;
  cout << "  kicktree -M mmls -S mrplstrt42nasdisk1 -p /mnt/carvfs/51b0003ce788b128b61cbd4e6ffa5e77/CarvFs.crv" << std::endl;
  cout << "  kicktree -M e01 -S mrplstrt42nasdisk1 -p /mnt/images/mrplstrt42nasdisk.E01" << std::endl;
  cout << "  kicktree -M e01 -S mrplstrt42server1 -I satadisk1 -p /mnt/images/mrplstrt42server1-satadisk1.E01" << std::endl;
  exit(1);
}

int main(int argc, char *argv[]){
  string caseName="";
  char *ocfacaseenv=getenv("OCFACASE");
  
  if (ocfacaseenv != 0) { 
    caseName=ocfacaseenv; 
  }

  string moduleName="";
  string sourceName="";
  string itemName="NotSpecified";
  string path="";
  string charSet = "LATIN1";
  string evidenceName = "NotSpecified";
  static struct option long_options[] = {
      {"module",1,NULL,'M'},
      {"case",1,NULL,'C'},
      {"source",1,NULL,'S'},
      {"item",1,NULL,'I'},
      {"name",1,NULL,'n'},
      {"path",1,NULL,'p'},
      {"attribute",1,NULL,'a'},
      {0,0,0,0}
  };
  int c;
  std::map<std::string,ocfa::misc::Scalar> attr;
  bool nSwitchUsed = false;

  while ((c=getopt_long(argc,argv,"M:C:S:I:n:p:a:",long_options,NULL)) != -1) {
     switch(c) {
       case ('M'): moduleName=optarg;
                   
                   break;
       case ('C'): caseName=optarg;
                   break;
       case ('S'): sourceName=optarg;
                   break;
       case ('I'): itemName=optarg;
                   break;
       case ('n'): evidenceName=optarg;
		   nSwitchUsed = true;
                   break;
       case ('p'): path=optarg; 
                   break;
       case ('a'): std::string keyVal=optarg; 
                   int l = keyVal.length();
                   int i = keyVal.find_first_of("=");
                   if ((i > 0) && (i < l - 1)) { 
                      std::string key = keyVal.substr(0, i);
                      std::string value = keyVal.substr(i + 1, keyVal.length() - 1);
                      attr[key]=value;
                   }
                   break;
    }
  }
  bool allok=true;

  if(nSwitchUsed == false) {
	evidenceName = path;
  }



  if (moduleName == "") {
     if (attr["charset"] == "") {
        attr["charset"] == "LATIN1";
     }
  }
  if (caseName=="") {
     std::cerr << "casename name not specified" << std::endl;
     allok=false;
  }
  if (sourceName == "") {
     std::cerr << "source name not specified" << std::endl;
     allok=false;
  }
  if (itemName == "") {
     std::cerr << "item name not specified" << std::endl;
     allok=false;
  }
  if (path == "") {
     std::cerr << "item path not specified" << std::endl;
     allok=false;
  }
  if (allok == false) {
     usage();
  }
  if (ocfacaseenv) {
    if (strcmp(ocfacaseenv,caseName.c_str())) {
      cerr << "ERROR: The OCFACASE enviroment variable is set to '" << ocfacaseenv << "' while the case specified is '" << caseName << "'!\n";
      return 1;
    }
  }


  ocfa::facade::KickstartAccessor *ks = 0;
  
  try {
    if (moduleName == "") {
       //Falling back to old kickstart behaviour.
       cerr << "Falling back to old kickstart behaviour." <<std::endl;
       ks = new ocfa::facade::KickstartAccessor("kickstart", "kickstart");
       ks->KickstartEvidence(caseName,sourceName,itemName,path,"LATIN1", evidenceName);
       cerr << "Evidence has been entered" << endl;
       delete ks;
    } else {
       cerr << "Using new modular behaviour" << std::endl;
       ks = new ocfa::facade::KickstartAccessor(moduleName, "kickstart");
       ks->TreeGraphKickstart(caseName,sourceName,itemName,evidenceName,moduleName,&attr,path);
       cerr << "Evidence has been entered" << endl;
       delete ks;
    }
  } catch (ocfa::misc::OcfaException &e) {
    cerr << "OcfaException: " << e.what() << endl;
  } catch (exception &e) {
    cerr << "Exception: " << e.what() << endl;
  } catch (string e) {
    cerr << "Sting as exception: " << e << "\n";
  }catch (...) {
    cerr << "Unknown exception." << endl;
  }
  return 0;
}
