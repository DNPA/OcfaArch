#include <vector>
#include <facade/TargetAccessor.hpp>
#include <libpq-fe.h>


class Resubmitter : public ocfa::facade::TargetAccessor {

public:
  Resubmitter();
  void resubmitEvidence(ocfa::misc::OcfaHandle inHandle);
  void addGeneralMetadata(std::string inName, std::string inValue);
  virtual void processEvidence();
  virtual PGconn *openDatabase();
  void removeMetaData(ocfa::misc::OcfaHandle &inHandle);
private:
  std::vector< std::pair<std::string, std::string > > mGeneralMetaData;
  std::vector<string> mMetaTables;
  PGconn *mConnection;
};

