#ifndef _METAFILTER_HPP_
#define _METAFILTER_HPP_
#include <libpq-fe.h>
#include <map>
#include <string>
#include "misc.hpp"

#define DEFAULT_ROW_VARCHAR_SIZE      255

class MetaFilter: public ocfa::OcfaObject {
    PGconn *mConnection;
    std::map< std::string, std::pair<ocfa::misc::meta_type, ocfa::misc::Scalar::scalar_type > * > mTableInfo;
    void createTableFromMeta(std::string name,ocfa::misc::MetaValue *val);
    void createScalarTableFromMeta(std::string name,ocfa::misc::Scalar val);
    void createArrayTableFromMeta(std::string name,ocfa::misc::Scalar val);
    void createTableTableFromMeta(std::string name,ocfa::misc::TableMetaValue *val);
 public:
    MetaFilter(PGconn *con,std::string config);
    bool operator()(std::string metaname,ocfa::misc::MetaValue *val);
};

#endif
