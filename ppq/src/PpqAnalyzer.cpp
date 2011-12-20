#include <stdlib.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <boost/lexical_cast.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

bool fileexists(std::string filepath) {
   struct stat buf;
   int statok=stat(filepath.c_str(),&buf);
   return (statok == 0);
}

off_t getdatafilereccount(std::string datafilepath,int verbosity){
  off_t datafilereccount=0;
  off_t messagesize=0;
  FILE *datafile=fopen(datafilepath.c_str(),"r");
  if (datafile == 0) {
    if (verbosity > 1)
       std::cerr << "ERROR: Unable to open " << datafilepath << std::endl;
    return 0;
  }
  while (fread(&messagesize,sizeof(off_t),1,datafile) == 1) {
    char buffer[4096000];
    if (fread(buffer,1,messagesize,datafile) != messagesize) {
       if (verbosity > 1)
          std::cerr << "ERROR: Broken final record in " << datafilepath << std::endl; 
    }  else {
       datafilereccount++; 
    }
  }
  return datafilereccount; 
}

void repairbogusreadfile(std::string statusfilepath,std::string fifodirpath,off_t rfindex,off_t filecount) {
   off_t newfifosize=0;
   off_t newrfindex=rfindex+1;
   off_t newskip=0;
   off_t newwfindex=rfindex+filecount-1;
   off_t dataindex;
   for (dataindex=rfindex+1;dataindex<(rfindex+filecount);dataindex++) {
     std::string datafile=fifodirpath + "/" + boost::lexical_cast<std::string>(dataindex) + ".dat";
     newfifosize+=getdatafilereccount(datafile,false); 
   }
   FILE *statfile=fopen(statusfilepath.c_str(),"w");
   fwrite(&newrfindex,sizeof(off_t),1,statfile); 
   fwrite(&newwfindex,sizeof(off_t),1,statfile);
   fwrite(&newskip,sizeof(off_t),1,statfile);
   fwrite(&newfifosize,sizeof(off_t),1,statfile);
   fclose(statfile);
}

bool dumpdatainfo(std::string fifodirpath,std::string statusfilepath,off_t rfindex,off_t filecount,off_t rfskip,off_t fifosize,int verbosity,bool fix) {
  off_t dataindex;
  off_t realfifosize=0;
  for (dataindex=rfindex;dataindex<(rfindex+filecount);dataindex++) {
    std::string datafile=fifodirpath + "/" + boost::lexical_cast<std::string>(dataindex) + ".dat";
    off_t filereccount=getdatafilereccount(datafile,verbosity);
    if (dataindex == rfindex) {
       if (fifosize && (filereccount <= rfskip)) {
         if (verbosity > 1)
            if ((fix) && (filecount > 1)) {
              std::cout << "Trying to fix problem with " << datafile << std::endl;
              repairbogusreadfile(statusfilepath,fifodirpath,rfindex,filecount);
            } else {
              std::cerr << "ERROR: the read file " << datafile << " of non empty fifo contains only " << filereccount << " entries, while the readfile skipcount is set to " << rfskip << std::endl;
              if (filecount > 1) {
                std::cerr << "I think I know how to fix this, use 'do' to ask me to perform this act.\n";
                std::cerr << "Please note that there are no guarantees here !!" << std::endl;
              }
            }
         return false;
       }
       filereccount -= rfskip;
    }
    realfifosize+=filereccount;
  }
  if (realfifosize != fifosize) {
     if (verbosity > 1)
         std::cerr << "ERROR: The real fifo size seems to be " << realfifosize << " while the status file defines " << fifosize << "as the size of the fifo." << std::endl;
     return false; 
  } 
  if (verbosity > 1)
     std::cout << fifodirpath << " fifo seems to be OK\n";  
  return true;
}

bool dumpstatusinfo(std::string statusfilepath,std::string fifodirpath,int verbosity,bool fix) {
  off_t rfindex=0;
  off_t wfindex=0;
  off_t rfskip=0;
  off_t fifosize=0;
  FILE *statfile=fopen(statusfilepath.c_str(),"r");    
  if (statfile == 0) {
    if (verbosity > 1)
       std::cerr << "Problem opening status file " << statusfilepath << std::endl;
    return false;
  }
  if (fread(&rfindex,sizeof(off_t),1,statfile) != 1) {
    if (verbosity > 1)
       std::cerr << "Problem reading 1st size_t of 4 from " << statusfilepath  << std::endl;
    return false;
  }
  if (fread(&wfindex,sizeof(off_t),1,statfile) != 1) {
    if (verbosity > 1)
       std::cerr << "Problem reading 2nd size_t of 4 from " << statusfilepath  << std::endl;
    return false;
  }
  if (fread(&rfskip,sizeof(off_t),1,statfile) != 1) {
    if (verbosity > 1)
       std::cerr << "Problem reading 3rd size_t of 4 from " << statusfilepath  << std::endl;
    return false;
  }
  if (fread(&fifosize,sizeof(off_t),1,statfile) != 1) {
    if (verbosity > 1)
       std::cerr << "Problem reading 4th size_t of 4 from " << statusfilepath  << std::endl;
    return false;
  }
  off_t filecount = wfindex +1 - rfindex;
  if (verbosity > 1) {
    std::cout << "  fifosize=" << fifosize  << " , stored in " << filecount << " data files\n";
    std::cout << "  writefile=" << wfindex << ".dat" << std::endl;
    std::cout << "  readfile=" << rfindex << ".dat , skip = " << rfskip << std::endl;
  }
  return dumpdatainfo(fifodirpath,statusfilepath,rfindex,filecount,rfskip,fifosize,verbosity,fix);
}

bool processfifo(std::string statusfilepath,std::string fifodirpath,int verbosity,bool fix=false) {
  return dumpstatusinfo(statusfilepath,fifodirpath,verbosity,fix);
}



bool processppqdir(std::string ppqdirpath,int verbosity) {
  int x;
  bool rval=true;
  for (x=0;x<8;x++) {
     std::string statusfile = ppqdirpath + "/" + boost::lexical_cast<std::string>(x) + ".status";
     std::string fifodir= ppqdirpath + "/" + boost::lexical_cast<std::string>(x) + ".fifo";
     if (fileexists(statusfile)) {
        bool thisok=processfifo(statusfile,fifodir,verbosity);
        rval &= thisok;
        if (verbosity > 0) {
          if (thisok) {std::cout << statusfile << ": OK " << std::endl;}
          else { std::cout << statusfile << ": BROKEN " << std::endl;}
        }
     } else {
        if (verbosity > 0) {
           std::cout << statusfile << ": MISSING " << std::endl;
        }
     }
  } 
  return rval;
}

bool processallppq(std::string ppqdirpath) {
  DIR *ppqdir=opendir(ppqdirpath.c_str());  
  if (ppqdir == 0) {
    std::cerr << "Can not open the top ppq dir: " << ppqdirpath << std::endl;
    return false; 
  }
  struct dirent *subent;
  bool rval=true;
  while(subent = readdir(ppqdir) ) {
    if ((subent->d_name[0] >= 'a') && (subent->d_name[0] >= 'Z')) {
        std::string ppqmoduledir = ppqdirpath + "/" + subent->d_name;
        bool queuok=processppqdir(ppqmoduledir,0);
        rval &= queuok; 
        std::cout << "Module " << subent->d_name << " queue :";
        if (queuok)  { std::cout << "OK" << std::endl;}
        else { std::cout << "BAD" << std::endl;}
    }         
  } 
  return rval;
}

int main(int argc,char **argv) {
  char *ocfacase=getenv("OCFACASE"); 
  if (ocfacase == 0) {
    std::cerr << "No OCFACASE set" << std::endl;
    exit(1);
  }
  std::string ppqtopdir=std::string("/var/ocfa/queues/") + ocfacase;  
  if (argc == 1) {
    if (processallppq(ppqtopdir) == false) {
        std::cerr << "ERR: One or more ppq is not OK\n";
        exit(1);
    }
    exit(0);
  } 
  std::string ppqmoduledir=ppqtopdir + "/" + argv[1];
  if (argc == 2) {
    if (processppqdir(ppqmoduledir,1) == false) {
      std::cerr << "ERR: One or more fifo is not OK\n";
      exit(1);
    }
    exit(0);
  }
  std::string statusfile= ppqmoduledir + "/" + argv[2] + ".status";
  std::string fifodir=ppqmoduledir + "/" + argv[2] + ".fifo";
  bool fix=false;
  if (argc  > 3) {
     if (strcmp(argv[3],"do")==0) {
       fix=true;
     }
  }
  if (fileexists(statusfile)) {
    if (processfifo(statusfile,fifodir,2,fix)== false) {
       std::cerr << "ERR: The fifo is not OK" << std::endl;
    }
  } else {
    std::cerr << "ERR: statusfile does not exist" << std::endl;
  } 
}
