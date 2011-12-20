#include <message/MessageBox.hpp>

#include <misc/OcfaLogger.hpp>

using namespace ocfa::message;
using namespace ocfa::misc;

/**
 *
 * Class that can create a monitorMessage. This can then be used by the anycastrelay 
 * to send a message to the monitor module. 
 *
 *
 *
 */

namespace Anycast {
class MonitorMessageFactory {

public:
  MonitorMessageFactory(MessageBox * inMessageBox);
  Message *createMonitorMessage(std::string inModuleName);

private:
  MessageBox *messageBox;
  ModuleInstance monitor;
};
}
