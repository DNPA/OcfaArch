#include "DocInfo.hpp"

/**
void DocInfo::operator=(const  DocInfo &inInfo){

  docid = inInfo.docid;
  metadataid = inInfo.metadataid;
  score = inInfo.score;

}
*/

DocInfo::DocInfo() : docid(""), metadataid(""), score(0){
}

/**
 * some operators are defined to ensure correct ordering in a set.
 *
 */
bool DocInfo::operator<(const  DocInfo &info2) const {

  if (score != info2.score){

    return (score < info2.score);
  } else if (docid != info2.docid){

    return docid < info2.docid;

  } else {

    return metadataid < info2.metadataid;
  }
}

bool DocInfo::operator>(const  DocInfo &info2) const {

  if (score != info2.score){

    return (score > info2.score);
  } else if (docid != info2.docid){

    return docid > info2.docid;
  } else {

    return metadataid > info2.metadataid;
  }
}
