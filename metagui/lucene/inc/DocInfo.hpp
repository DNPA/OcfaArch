
#ifndef DOCINFO_HPP
#define DOCINFO_HPP
#include <string>
/**
 * class that represents the one returned result from the lucene
 * indexer.
 *
 */
class DocInfo {
public:
  DocInfo();
  //  DocInfo &operator=(const  DocInfo &inOther);
  bool operator<(const DocInfo &other) const;
  bool operator>(const DocInfo &other) const;
  std::string docid;
  std::string metadataid;
  float score;
};
#endif
