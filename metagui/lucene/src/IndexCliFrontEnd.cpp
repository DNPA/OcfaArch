#include "IndexCliFrontEnd.hpp"
#include <iostream>

IndexCliFrontEnd::IndexCliFrontEnd(int argc, char *argv[])
  : IndexFrontEnd(true), mPrintedEvidenceCounter(0) {

  string query = "";
  if (argc > 2 && (strcmp(argv[1], "-f") == 0)){

    setSearchType(WORDS);
  }
  else {

    query += argv[1];
  }
  for (int x = 2; x < argc; x++){

    query +=argv[x];
  }
  setFrom(0);
  setAmount(100000);
  getIndexSearcher().setQuery(query);
}


void IndexCliFrontEnd::printEvidenceInfo(const EvidenceInfo &evidenceInfo){
  mPrintedEvidenceCounter++;
  printEvidenceInfoLine(evidenceInfo);
  cout << endl;
}

void IndexCliFrontEnd::printEvidenceInfoLine(const EvidenceInfo &evidenceInfo){

  cout << mPrintedEvidenceCounter << " ";
  cout << evidenceInfo.ecase;
  cout << " " << evidenceInfo.source;
  cout << " " << evidenceInfo.item; 
  cout  << " " << evidenceInfo.location;
}
void IndexCliFrontEnd::printPeerEvidenceInfo(const EvidenceInfo &inEvidenceInfo){

  printEvidenceInfoLine(inEvidenceInfo);
  cout << " DUP" << endl;
}

void IndexCliFrontEnd::printHeader(int inDocFounds){

  cout << "Found " << inDocFounds << " occurences" << endl;
  cout << "count case source item location" << endl;
}

void IndexCliFrontEnd::printNavigation(int foundEvidence){
  cout << "Totally found evidences " << foundEvidence << endl;
}

void IndexCliFrontEnd::printFoundWords(map<string, int> &foundWords){

  cout << "The following words were in the dictionary " << endl;
  map<string, int>::iterator iter;
  int x = 0;
  for (iter = foundWords.begin(); iter != foundWords.end(); iter++){

    x++;
    cout << iter->first;
    cout << "\t";
    cout << iter->second;
    cout << endl;
  }
  
}
