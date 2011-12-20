#ifndef EVIDENCEINFO_HPP
#define EVIDENCEINFO_HPP
#include <string>
#include <algorithm>

/**
 * describes the information about an evidence as it was found in the ocfa database.
 *
 */
struct EvidenceInfo {
  EvidenceInfo():
	  xmlRef("BOGUS"),
	  dataRef("BOGUS"),
	  ecase("BOGUS"),
	  source("BOGUS"),
	  item("BOGUS"),
	  evidenceRef("BOGUS"),
	  location("BOGUS"),
	  mimetype("BOGUS"),
	  encoding("BOGUS"),
	  mScore(1.0)
  {
  }
  bool operator>(const EvidenceInfo &other) const;
	  
  /**
   * id of the handle to the metadata about this document. 
   */
  std::string xmlRef; 
  /**
   * id of the handle to the data of this document.
   *
   */
  std::string dataRef;
  /**
   * case name in which the document was found.
   */
  std::string ecase;
  /**
   * evidence sourc in which the document was found.
   */
  std::string source;
  /**
   * item name in which the document was found.
   */
  std::string item;
  /**
   * reference name of the document e.g. e1.j2e0
   */
  std::string evidenceRef;
  /**
   * user readable representation of the location of the document.
   *
   */
  std::string location;
  /**
   * mimetype of the evidenc.e
   *
   */
  std::string mimetype;

  /**
   * encoding of the evidence
   *
   */
  std::string encoding;

  /**
   * whether the evidence was already seen;
   */
  bool wasread;
  /**
   * the relevance of the evidence with regard to the query.
   *
   */
  float mScore;
};

struct EvidenceMostImportant : public std::binary_function <EvidenceInfo *, EvidenceInfo *, bool> {

  bool operator()(const EvidenceInfo *x, EvidenceInfo *y) const {
    
    if (x->mScore != y->mScore){

      return x->mScore > y->mScore;
    }
    else {

      return x->xmlRef >y->xmlRef;
    }
  }
};

#endif
