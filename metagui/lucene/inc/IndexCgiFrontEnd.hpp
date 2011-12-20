#ifndef MIDASCGIFRONTEND_HPP
#define MIDASCGIFRONTEND_HPP
#include "IndexFrontEnd.hpp"
#include "cgicc/Cgicc.h"
//#include "EvidenceInfo.hpp"
/**
 * class that configures a Lucene search depending on its input and 
 * prints out the results.
 *
 */
class IndexCgiFrontEnd : public IndexFrontEnd {
public:

  //enum SearchType  {STANDARD, FUZZY, WORDS};
  IndexCgiFrontEnd(cgicc::Cgicc &cgicc);

  /**
   * prints out one returned results to the browser.
   */
  virtual void printEvidenceInfo(const EvidenceInfo &inEvidenceInfo);
  //virtual void printEvidenceInfoColor(const EvidenceInfo &inEvidenceInfo, string style = "");

  virtual void printPeerEvidenceInfo(const EvidenceInfo &inPeerEvidenceInfo);
  virtual void printNavigation(int inFoundEvidences); 
  virtual void printHeader(int inDocFounds);
  /**
   * NYI originally this should print the words that have a
   * resemblance to a original fuzzy query.
   */
  virtual void printFoundWords(std::map<std::string, int> &foundWords) ;
  virtual void printHtmlDocumentStart();
  virtual void printHtmlDocumentClose();
  static void printErrorMessage(std::string error); 
protected:
  virtual void printEvidenceInfoColor(const EvidenceInfo &inEvidenceInfo,
			 string inStyle);

  /**
   * selects indexes based upon information contained in the cgi variables.
   *
   */
  virtual void selectIndexes(cgicc::Cgicc &inCgicc);
  /**
   * prints the different index options that can be used.
   *
   */
  virtual void printIndexOptions();

private:
  int mPrintedEvidenceCounter;
  /**
   * arguments that are used to select
   */
  std::string mIndexSelectCgiArguments;
};
#endif
