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
#include <facade/KickstartAccessor.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPCookie.h"
#include "Poco/Net/HTTPStream.h"
#include "Poco/InflatingStream.h"
#include <unistd.h>
#include <sstream>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/types.h>
using namespace std;

class KaasHttpRequestHandler: public Poco::Net::HTTPRequestHandler
{
    public:
        KaasHttpRequestHandler(std::string inv,std::string name):mCase(inv),mModule(name) {}

        void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
        {
	   while (waitpid(-1,NULL,WNOHANG) > 0) {;}
	   std::string body;
	   request.stream() >> body;
           std::stringstream ss;
	   ss << body;
           boost::property_tree::ptree pt;
	   boost::property_tree::json_parser::read_json(ss,pt);
	   std::string givenCaseName = pt.get("case", "");
	   std::string sourceName = pt.get("source", "");
           std::string itemName=pt.get("item", "");
	   std::string evidenceName="kaasitem";
           std::string path=pt.get("uri", "");
           if ( givenCaseName != mCase) {
	      response.setStatus(Poco::Net::HTTPServerResponse::HTTP_FORBIDDEN);
	      response.setReason("Incompativle investigation name");
              response.setContentType("text/plain");
	      std::ostream &out=response.send();
	      //out << "Invalid case" << std::endl;
	      return;
	   }
           if ((sourceName == "") || ( itemName == "") || (path == "")) {
	      response.setStatus(Poco::Net::HTTPServerResponse::HTTP_BAD_REQUEST);
	      response.setReason("Invalid or incomplete json command");
              response.setContentType("text/plain");
	      std::ostream &out=response.send();
	      //out << "Problem processing json command" << std::endl; 
	      return;
	   } else { 
             std::map<std::string,ocfa::misc::Scalar> attr;
             response.setContentType("text/plain");
             std::ostream &out=response.send();
             out << "<H1>Starting kickstart into background process</H1>" << std::endl;
	     out << "Please watch the ocfa logging to see if kickstarting succeeded." << std::endl;
             pid_t pid=fork();
             if (pid == 0) {    
               try {   
                 ocfa::facade::KickstartAccessor kickstart(mModule,"kaas");
		 if (mModule == "kickstart") {
                   kickstart.KickstartEvidence(mCase,sourceName,itemName,path,"LATIN1", evidenceName);
		 } else {
                   kickstart.TreeGraphKickstart(mCase,sourceName,itemName,evidenceName,mModule,&attr,path); 
		 } 
               } catch (ocfa::misc::OcfaException &e) {
                 e.logWhat();
               }
	       exit(0); 
	     }
           }
        }
    private:
        std::string mCase;
        std::string mModule;
};

class KaasHttpRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
  public:
        KaasHttpRequestHandlerFactory(std::string inv,std::string name):mCase(inv),mModule(name) {}

        Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
        {
                return new KaasHttpRequestHandler(mCase,mModule);
        }
  private:
        std::string mCase;
        std::string mModule;
};


class KaasService: public Poco::Util::ServerApplication
{
public:
        KaasService(int port,std::string inv,std::string name):mServerPort(port),mCase(inv),mModule(name) {}
        ~KaasService() {}
protected:
        void initialize(Application& self)
        {
                ServerApplication::initialize(self);
        }

        void uninitialize()
        {
                ServerApplication::uninitialize();
        }

        int main(const std::vector<std::string>& args)
        {
                Poco::Net::ServerSocket svs( mServerPort );
                Poco::Net::HTTPServer srv(new KaasHttpRequestHandlerFactory(mCase,mModule), svs, new Poco::Net::HTTPServerParams);
                srv.start();
                waitForTerminationRequest();
                srv.stop();
                return Application::EXIT_OK;
        }
private:
        int mServerPort;
        std::string mCase;
        std::string mModule;
};

void usage(){
  cout << "The kaas tool requires multiple options to be set, and has some aditional options: " << std::endl;
  cout << "  -M --module <modname> : Specify the treegraph module to load. (optional, default=kickstart) " << std::endl;
  cout << "  -P --port <port>      : Specify the port to listen on. (optional, default=17488) " << std::endl;
  cout << "  -C --case <casename>  : Specify the name of the active investigation. (optional, default=$OCFACASE) " << std::endl;
  cout << "  -a --attribute <k>=<v>: Specify a module specific attribute. You can specify multiple attributes" << std::endl;
  cout << "                          this way by repeating the -a/--attribute multiple times. (optional)" << std::endl;
  cout << std::endl;
  cout << "Examples ::" << std::endl;
  cout << "  kaas -M sl -P 17489" << std::endl;
  cout << "  kaas" << std::endl;
  exit(1);
}

int main(int argc, char *argv[]){
  string caseName="";
  int  serverport = 17488;
  string moduleName="kickstart";
  char *ocfacaseenv=getenv("OCFACASE");
  if (ocfacaseenv != 0) { 
    caseName=ocfacaseenv; 
  }
  static struct option long_options[] = {
      {"module",1,NULL,'M'},
      {"case",1,NULL,'C'},
      {"port",1,NULL,'P'},
      {"help",1,NULL,'h'},
      {"attribute",1,NULL,'a'},
      {"daemon",1,NULL,'d'},
      {0,0,0,0}
  };
  int c;
  std::map<std::string,ocfa::misc::Scalar> attr;

  while ((c=getopt_long(argc,argv,"M:C:P:a:h:",long_options,NULL)) != -1) {
     switch(c) {
       case ('M'): moduleName=optarg;
                   break;
       case ('C'): caseName=optarg;
                   break;
       case ('P'): serverport=boost::lexical_cast<int>(optarg);
                   if (serverport < 1024) {
                      std::cerr << "Invalid server port specified: " << serverport << "Reverting to default" << std::endl;
                      serverport = 17488;
                   }
                   break;
       case ('a'): { 
                     std::string keyVal=optarg; 
                     int l = keyVal.length();
                     int i = keyVal.find_first_of("=");
                     if ((i > 0) && (i < l - 1)) { 
                      std::string key = keyVal.substr(0, i);
                      std::string value = keyVal.substr(i + 1, keyVal.length() - 1);
                      attr[key]=value;
                     }
                   }
                   break;
       case ('h'): usage();
                   exit(0);
                   break;
    }
  }
  bool allok=true;
  if (caseName=="") {
     std::cerr << "casename name not specified" << std::endl;
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
  cerr << "Starting Kaas service on port " << serverport << " casenmame=" << caseName << " modulename=" << moduleName << std::endl;
  KaasService kaas(serverport,caseName,moduleName);
  char* myargv[2];
  myargv[0]=argv[0];
  char daemon[40];
  strcpy(daemon,"--daemon");
  myargv[1]=daemon;
  return kaas.run(2,myargv);
}
