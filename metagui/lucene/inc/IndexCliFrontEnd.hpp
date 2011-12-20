#ifndef MIDASCLIFE_HPP
#define MIDASCLIFE_HPP
#include "IndexFrontEnd.hpp"
/**
 * class that configures a lucene search depending on its input and 
 * prints out the results.
 *
 */
class IndexCliFrontEnd : public IndexFrontEnd {
public:
  
  
  IndexCliFrontEnd(int argc, char *argv[]);
  virtual void printEvidenceInfo(const EvidenceInfo &inEvidenceInfo);
  //virtual void printEvidenceInfoColor(const EvidenceInfo &inEvidenceInfo, string style = "");
  
  virtual void printPeerEvidenceInfo(const EvidenceInfo &inPeerEvidenceInfo);
  virtual void printNavigation(int inFoundEvidences); 
  virtual void printHeader(int inDocFounds);
  /**
   * NYI originally this should give all words that have a fuzzy resemblance to the original query.
   */
  virtual void printFoundWords(std::map<std::string, int> &foundWords) ;
  
protected:
  virtual void printEvidenceInfoLine(const EvidenceInfo &inEvidenceInfo);  
private:
  int mPrintedEvidenceCounter;
};
#endif
