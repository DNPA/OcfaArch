#include "Resubmitter.hpp"
#include <store/AbstractRepository.hpp>
#include <boost/tokenizer.hpp>
#include <pwd.h>

using namespace ocfa::facade;
using namespace ocfa::misc;
using namespace ocfa::store;
using namespace ocfa::message;
using namespace std;
using namespace boost;
Resubmitter::Resubmitter() : TargetAccessor("resubmitter", "default", true){
  
    string file = getConfEntry("metatabledescription");
  logModule(LOG_INFO, string("using table description file ") + file); 
  ifstream tableDescriptionStream(file.c_str());
  if (! tableDescriptionStream){

    logModule(LOG_ERR, string("cannot read from ") + file);
    logModule(LOG_ERR, "Going without it ");
    return;
  }
  string metaDescription;
  //char buffer[100];
  while (tableDescriptionStream){

    getline(tableDescriptionStream, metaDescription);
    if (metaDescription[0] != '#'){
      
      
      boost::char_separator<char> sep (" ");
      tokenizer<char_separator<char> > tok(metaDescription, sep);
      
      tokenizer<char_separator<char> >::iterator iter = tok.begin();
      if (iter != tok.end()){
	
	string metaName = *iter;
	getLogStream(LOG_DEBUG) << "metaName is " << metaName << endl;
	
	replace(metaName.begin(), metaName.end(), '-', '_');
	mMetaTables.push_back(metaName);
      }
    }
  } 
  mConnection = openDatabase();
}


PGconn *Resubmitter::openDatabase(){
 
  string dbName = getConfEntry("storedbname");
  string dbhost = getConfEntry("storedbhost");
  string dbuser = getConfEntry("storedbuser");
  char connectionString[255];
  if (dbuser == ""){
    
    struct passwd *pwent = getpwuid(getuid());
    dbuser = pwent->pw_name;
  }
  string dbpasswd = getConfEntry("storedbpasswd");
  sprintf(connectionString, "dbname=%s hostaddr=%s user=%s password=%s", 
	  dbName.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
  
  PGconn *connection = PQconnectdb(connectionString);
  if (PQstatus(connection) != CONNECTION_OK) {
    
    string errstr = "PgRepository initialization failed: " 
      + string(PQerrorMessage(connection));
    //printf("%s",errstr.c_str());
    ocfaLog(LOG_ERR, errstr);
    throw OcfaException(errstr, this);
  } 
  //  PQsetNoticeProcessor(connection, DSMlogNotices, 0);
  return connection;
}



void Resubmitter::addGeneralMetadata(string inName, string inValue){

  pair<string, string> thePair;
  thePair.first = inName;
  thePair.second = inValue;
  mGeneralMetaData.push_back(thePair);
}


void Resubmitter::resubmitEvidence(OcfaHandle inHandle){

  MetaStoreEntity *metaEntity;
  getLogStream(LOG_DEBUG) << "resubmitting " << inHandle << endl;
  AbstractRepository::Instance()->createMetaStoreEntity(&metaEntity, inHandle);
  if (metaEntity == 0){

      throw OcfaException(string("cannot create metastoreentity from ") + inHandle, this);
  }
  setEvidence(createEvidenceFromMeta(metaEntity));
  // JCW: By default don't remove metadata
  // Add dsm:moduletwice=skipjobs
  // removeMetaData(inHandle);
  addNewJob(0);
  getEvidence()->setMutable();
  vector<pair<string, string > >::iterator iter;
  for (iter = mGeneralMetaData.begin(); iter != mGeneralMetaData.end(); iter++){

    string name = iter->first;
    ScalarMetaValue value(iter->second);
    setMeta(name, value);
  }
  getEvidence()->getActiveJob()->close();
  updateMetaWithEvidence(*metaEntity, *getEvidence());
  Message *newMessage;
  ModuleInstance router("localhost", "router", "core", "theonlyone");
  getMessageBox()->createMessage(&newMessage, &router , Message::ANYCAST, Message::mtEvidence, 
				     "request", metaEntity->getHandle(), MSG_EVIDENCE_LOWPRIO);
  //RJM:CODEREVIEW sugestion: instead of the abouve use:
  //getMessageBox()->createEvidenceMessage(&newMessage,metaEntity->getHandle());
  if (newMessage == 0){
    
     throw OcfaException("Could not create a newMessage", this); 
  }
  
  getMessageBox()->sendMessage(*newMessage);
  delete metaEntity;
  delete getEvidence();
  delete newMessage;
  setEvidence(0);
}

void Resubmitter::removeMetaData(OcfaHandle &inHandle){

  vector<string>::iterator iter;
  for (iter = mMetaTables.begin(); iter != mMetaTables.end(); iter++){

    string query = string("delete from meta") + *iter + " where xml = '" + inHandle + "'";
    
  
    PGresult *pgres;
    pgres = PQexec(mConnection, query.c_str());
    if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
      
      getLogStream(LOG_ERR) << "cannot execute " << query << endl;
    }
    PQclear(pgres);
  }
}

void Resubmitter::processEvidence(){
}

int main(int argc, char *argv[]){

  if (argc < 2|| (argc % 2) == 1){

    cout << endl << "Resubmit Evidence into OCFA" << endl;
    cout << "Provide a file with one metadataid per line" << endl;
    cout << "This metadataid will be resubmitted into ocfa" << endl;
    cout << "using the metaname,metavalue paire provided after" << endl;
    cout << "the filename with metadataid's" << endl << endl;
    cout << "Usage:\n\t resubmitter <filename> [<metaname> <metavalue>]*" << endl << endl;

  }
  else {

    cout << "argv[0] is " << argv[0] << endl;
    cout << "argc is " << argc <<  endl;
    try {
      Resubmitter submitter;
      cout << endl;
      for (int x = 2; x < argc; x+=2){
	
	string name = argv[x];
	string value = argv[x+1];
	cout << " adding " << name << ":" << value << endl;
	submitter.addGeneralMetadata(name, value);
      }
      ifstream ocfaHandleStream(argv[1]);
      while(!ocfaHandleStream.eof()){
	
	string metaid;
	ocfaHandleStream >> metaid;
	if (metaid != ""){
	  submitter.resubmitEvidence(metaid);
	}
      }
      
      cout << "done" << endl;
    } catch(OcfaException &e){
      
      cout << "OcfaException: " << e.what() << endl;
    }
  }
}
