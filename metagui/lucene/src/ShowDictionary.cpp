#include <sstream>
#include <CLucene.h>
#include <CLucene/analysis/standard/StandardAnalyzer.h>

#include <misc/OcfaConfig.hpp>
using namespace std;
CL_NS_USE(index)


void printIndexDictionary(string inKey, string inLuceneDir);

using namespace ocfa::misc;
int main(int argc, char *argv[]){

  OcfaConfig *config = OcfaConfig::Instance();;
  string luceneDir = config->getValue("indexdir");
  printIndexDictionary("indexdir", luceneDir);
  string extraIndexCountString = config->getValue("extraIndexCount");
  if (extraIndexCountString != ""){

    int extraIndexCount = atoi(extraIndexCountString.c_str());
    for (int x = 0; x < extraIndexCount; x++){

      ostringstream dir;
      dir << "indexdir" << x;
      luceneDir = config->getValue(dir.str());
      printIndexDictionary(dir.str(), luceneDir);
    }
  }
}

void printIndexDictionary(string inKey, string luceneDir){

  IndexReader *reader = IndexReader::open(luceneDir.c_str());
  TermEnum *termEnum = reader->terms();
  while (termEnum->next()){

    char buffer[1000];
    Term *theTerm = termEnum->term();
    int docFreq = termEnum->docFreq();
    STRCPY_TtoA(buffer, theTerm->text(), 1000);
    cout << inKey << " " << buffer << " " << docFreq << endl;
  }

}
