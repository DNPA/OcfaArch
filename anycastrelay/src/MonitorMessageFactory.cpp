
#include "MonitorMessageFactory.hpp"
using namespace Anycast;
MonitorMessageFactory::MonitorMessageFactory(MessageBox *inMessageBox)
  : monitor("dntcr", "monitor", "default", "dntcr") {

  messageBox = inMessageBox;

}


Message *MonitorMessageFactory::createMonitorMessage(std::string inModuleName){

  Message *message = 0;
  messageBox->createMessage(&message, &monitor, Message::ANYCAST, Message::mtSystem,
		     "moduletypeadded", inModuleName);     
  return message;
}


